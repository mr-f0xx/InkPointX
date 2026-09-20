// Mutation fuzzer for the readers that consume untrusted files from the SD
// card. These parse attacker-shaped binary structures - ZIP central
// directories, XTC page tables - and size their heap allocations from fields
// inside the file, which is exactly the shape of bug the unit tests do not
// reach because they only ever feed well-formed input.
//
// Build it with -fsanitize=address,undefined; the sanitizers are the oracle.
// A clean exit means no iteration tripped one.
//
// Usage: format_fuzz_harness <corpus-dir> <work-dir> [iterations] [seed]

#include <HalStorage.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include "XtcParser.h"
#include "ZipFile.h"

namespace {

struct NullSink : Print {
  size_t write(const uint8_t*, const size_t size) override { return size; }
};

std::vector<uint8_t> readFile(const std::filesystem::path& path) {
  std::ifstream in(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

// Structure-aware enough to matter: pure byte flips almost never survive the
// magic check, so also target the length and offset fields that drive
// allocation, and truncate - a short file is the cheapest way to walk a parser
// off the end of its own buffer.
void mutate(std::vector<uint8_t>& data, std::mt19937& rng) {
  if (data.empty()) return;
  std::uniform_int_distribution<int> pick(0, 99);
  const int roll = pick(rng);

  if (roll < 45) {
    std::uniform_int_distribution<size_t> pos(0, data.size() - 1);
    const int flips = 1 + (pick(rng) % 8);
    for (int i = 0; i < flips; ++i) data[pos(rng)] ^= static_cast<uint8_t>(1u << (pick(rng) % 8));
    return;
  }
  if (roll < 70) {
    // Overwrite a 32-bit field with a boundary value. Sizes and offsets read
    // straight out of the file are what reach malloc().
    static constexpr uint32_t interesting[] = {0u,          1u,      0xFFFFFFFFu, 0x7FFFFFFFu,
                                               0x80000000u, 0xFFFFu, 0x10000u,    0xFFFFFFFEu};
    if (data.size() < 4) return;
    std::uniform_int_distribution<size_t> pos(0, data.size() - 4);
    const uint32_t value = interesting[pick(rng) % (sizeof(interesting) / sizeof(*interesting))];
    std::memcpy(data.data() + pos(rng), &value, sizeof(value));
    return;
  }
  if (roll < 85) {
    std::uniform_int_distribution<size_t> cut(1, data.size());
    data.resize(cut(rng));
    return;
  }
  // Splice a block over itself: cheap way to produce duplicate/overlapping
  // records without losing the header the parser needs to get started.
  std::uniform_int_distribution<size_t> pos(0, data.size() - 1);
  const size_t from = pos(rng);
  const size_t to = pos(rng);
  const size_t len = std::min(data.size() - std::max(from, to), size_t{64});
  if (len) std::memmove(data.data() + to, data.data() + from, len);
}

void driveZip(const std::string& logicalPath) {
  ZipFile zip(logicalPath);
  if (!zip.open()) return;
  zip.loadAllFileStatSlims();

  std::vector<std::string> names;
  zip.enumerateFilePaths([&names](const std::string_view name) {
    if (names.size() < 64) names.emplace_back(name);
  });

  for (const auto& name : names) {
    size_t inflated = 0;
    zip.getInflatedFileSize(name.c_str(), &inflated);

    size_t produced = 0;
    if (uint8_t* bytes = zip.readFileToMemory(name.c_str(), &produced, true)) {
      // Touch both ends so a short allocation is caught at the access, not
      // silently tolerated.
      volatile uint8_t sink = bytes[0];
      sink = bytes[produced];
      (void)sink;
      free(bytes);
    }

    NullSink out;
    zip.readFileToStream(name.c_str(), out, 512);
  }
  zip.close();
}

void driveXtc(const std::string& logicalPath) {
  xtc::XtcParser parser;
  if (parser.open(logicalPath.c_str()) != xtc::XtcError::OK) return;
  parser.getTitle();
  parser.getAuthor();
  parser.getChapters();

  const uint32_t pages = parser.getPageCount();
  std::vector<uint8_t> page(64 * 1024);
  for (uint32_t i = 0; i < std::min<uint32_t>(pages, 8); ++i) {
    xtc::PageInfo info{};
    parser.getPageInfo(i, info);
    parser.loadPage(i, page.data(), page.size());
    size_t streamed = 0;
    parser.loadPageStreaming(i, [&streamed](const uint8_t*, const size_t size, size_t) { streamed += size; }, 512);
  }
  parser.close();
}

void writeCase(const std::string& logicalPath, const std::vector<uint8_t>& data) {
  const auto physical = Storage.resolve(logicalPath.c_str());
  std::filesystem::create_directories(physical.parent_path());
  std::ofstream out(physical, std::ios::binary | std::ios::trunc);
  out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) {
    std::fprintf(stderr, "usage: %s <corpus-dir> <work-dir> [iterations] [seed]\n", argv[0]);
    return 2;
  }
  const std::filesystem::path corpusDir = argv[1];
  const std::filesystem::path workDir = argv[2];
  const int iterations = argc > 3 ? std::atoi(argv[3]) : 2000;
  const uint32_t seed = argc > 4 ? static_cast<uint32_t>(std::strtoul(argv[4], nullptr, 10)) : 1234;

  std::filesystem::create_directories(workDir);
  Storage.setRoot(workDir);

  struct Seed {
    std::vector<uint8_t> bytes;
    std::string extension;
  };
  std::vector<Seed> seeds;
  for (const auto& entry : std::filesystem::recursive_directory_iterator(corpusDir)) {
    if (!entry.is_regular_file()) continue;
    auto bytes = readFile(entry.path());
    if (bytes.empty()) continue;
    seeds.push_back({std::move(bytes), entry.path().extension().string()});
  }
  if (seeds.empty()) {
    std::fprintf(stderr, "no corpus files under %s\n", corpusDir.c_str());
    return 2;
  }
  std::printf("corpus: %zu file(s), %d iterations, seed %u\n", seeds.size(), iterations, seed);

  // Parse every seed unmutated first. A fuzzer that has broken the happy path
  // reports "no findings" for the wrong reason, so make the corpus prove the
  // readers still work before any byte is flipped.
  int healthy = 0;
  for (size_t i = 0; i < seeds.size(); ++i) {
    const std::string logical = "/fuzz/seed" + std::to_string(i) + seeds[i].extension;
    writeCase(logical, seeds[i].bytes);
    if (seeds[i].extension == ".xtc" || seeds[i].extension == ".xth") {
      xtc::XtcParser parser;
      if (parser.open(logical.c_str()) == xtc::XtcError::OK && parser.getPageCount() > 0) ++healthy;
      parser.close();
    } else {
      ZipFile zip(logical);
      int entries = 0;
      if (zip.open() && zip.loadAllFileStatSlims()) {
        zip.enumerateFilePaths([&entries](std::string_view) { ++entries; });
      }
      zip.close();
      if (entries > 0) ++healthy;
    }
  }
  std::printf("sanity: %d/%zu corpus file(s) parsed clean\n", healthy, seeds.size());
  if (healthy != static_cast<int>(seeds.size())) {
    std::fprintf(stderr, "a valid corpus file no longer parses - fix that before trusting a clean fuzz run\n");
    return 1;
  }

  std::mt19937 rng(seed);
  std::uniform_int_distribution<size_t> pickSeed(0, seeds.size() - 1);

  for (int i = 0; i < iterations; ++i) {
    const Seed& chosen = seeds[pickSeed(rng)];
    std::vector<uint8_t> data = chosen.bytes;
    const int rounds = 1 + static_cast<int>(rng() % 4);
    for (int r = 0; r < rounds; ++r) mutate(data, rng);

    const std::string logical = "/fuzz/case" + chosen.extension;
    writeCase(logical, data);

    if (chosen.extension == ".xtc" || chosen.extension == ".xth") {
      driveXtc(logical);
    } else {
      driveZip(logical);
    }

    if ((i + 1) % 500 == 0) std::printf("  %d/%d\n", i + 1, iterations);
  }

  std::printf("done: %d iterations, no sanitizer report\n", iterations);
  return 0;
}
