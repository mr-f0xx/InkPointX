#include "LanguageRegistry.h"

#include <algorithm>
#include <array>

#include "HyphenationCommon.h"

// Hyphenation pattern tries are the largest single data items in the image:
// German alone is ~206 KB, and all nine together are ~349 KB. The 0x640000 app
// slot leaves no room for a build with LOG_* enabled, so a development build
// can drop languages it does not need in order to link at all. Every language
// defaults to on, so release images are unaffected; set CROSSPOINT_HYPH_<XX>=0
// in an environment's build_flags to omit one. English is unconditional: it is
// the fallback face and keeps the registry non-empty.
#ifndef CROSSPOINT_HYPH_DE
#define CROSSPOINT_HYPH_DE 1
#endif
#ifndef CROSSPOINT_HYPH_ES
#define CROSSPOINT_HYPH_ES 1
#endif
#ifndef CROSSPOINT_HYPH_FR
#define CROSSPOINT_HYPH_FR 1
#endif
#ifndef CROSSPOINT_HYPH_IT
#define CROSSPOINT_HYPH_IT 1
#endif
#ifndef CROSSPOINT_HYPH_PL
#define CROSSPOINT_HYPH_PL 1
#endif
#ifndef CROSSPOINT_HYPH_RU
#define CROSSPOINT_HYPH_RU 1
#endif
#ifndef CROSSPOINT_HYPH_SV
#define CROSSPOINT_HYPH_SV 1
#endif
#ifndef CROSSPOINT_HYPH_UK
#define CROSSPOINT_HYPH_UK 1
#endif

#include "generated/hyph-en.trie.h"
#if CROSSPOINT_HYPH_DE
#include "generated/hyph-de.trie.h"
#endif
#if CROSSPOINT_HYPH_ES
#include "generated/hyph-es.trie.h"
#endif
#if CROSSPOINT_HYPH_FR
#include "generated/hyph-fr.trie.h"
#endif
#if CROSSPOINT_HYPH_IT
#include "generated/hyph-it.trie.h"
#endif
#if CROSSPOINT_HYPH_PL
#include "generated/hyph-pl.trie.h"
#endif
#if CROSSPOINT_HYPH_RU
#include "generated/hyph-ru.trie.h"
#endif
#if CROSSPOINT_HYPH_SV
#include "generated/hyph-sv.trie.h"
#endif
#if CROSSPOINT_HYPH_UK
#include "generated/hyph-uk.trie.h"
#endif

namespace {

// English hyphenation patterns (3/3 minimum prefix/suffix length)
LanguageHyphenator englishHyphenator(en_patterns, isLatinLetter, toLowerLatin, 3, 3);
#if CROSSPOINT_HYPH_FR
LanguageHyphenator frenchHyphenator(fr_patterns, isLatinLetter, toLowerLatin);
#endif
#if CROSSPOINT_HYPH_DE
LanguageHyphenator germanHyphenator(de_patterns, isLatinLetter, toLowerLatin);
#endif
#if CROSSPOINT_HYPH_RU
LanguageHyphenator russianHyphenator(ru_patterns, isCyrillicLetter, toLowerCyrillic);
#endif
#if CROSSPOINT_HYPH_ES
LanguageHyphenator spanishHyphenator(es_patterns, isLatinLetter, toLowerLatin);
#endif
#if CROSSPOINT_HYPH_IT
LanguageHyphenator italianHyphenator(it_patterns, isLatinLetter, toLowerLatin);
#endif
#if CROSSPOINT_HYPH_SV
LanguageHyphenator swedishHyphenator(sv_patterns, isLatinLetter, toLowerLatin);
#endif
#if CROSSPOINT_HYPH_UK
LanguageHyphenator ukrainianHyphenator(uk_patterns, isCyrillicLetter, toLowerCyrillic);
#endif
#if CROSSPOINT_HYPH_PL
LanguageHyphenator polishHyphenator(pl_patterns, isLatinLetter, toLowerLatin);
#endif

constexpr size_t kLanguageCount = 1  // English is always compiled in.
                                  + CROSSPOINT_HYPH_FR + CROSSPOINT_HYPH_DE + CROSSPOINT_HYPH_RU + CROSSPOINT_HYPH_ES +
                                  CROSSPOINT_HYPH_IT + CROSSPOINT_HYPH_PL + CROSSPOINT_HYPH_SV + CROSSPOINT_HYPH_UK;

using EntryArray = std::array<LanguageEntry, kLanguageCount>;

const EntryArray& entries() {
  static const EntryArray kEntries = {{
      {"english", "en", &englishHyphenator},
#if CROSSPOINT_HYPH_FR
      {"french", "fr", &frenchHyphenator},
#endif
#if CROSSPOINT_HYPH_DE
      {"german", "de", &germanHyphenator},
#endif
#if CROSSPOINT_HYPH_RU
      {"russian", "ru", &russianHyphenator},
#endif
#if CROSSPOINT_HYPH_ES
      {"spanish", "es", &spanishHyphenator},
#endif
#if CROSSPOINT_HYPH_IT
      {"italian", "it", &italianHyphenator},
#endif
#if CROSSPOINT_HYPH_PL
      {"polish", "pl", &polishHyphenator},
#endif
#if CROSSPOINT_HYPH_SV
      {"swedish", "sv", &swedishHyphenator},
#endif
#if CROSSPOINT_HYPH_UK
      {"ukrainian", "uk", &ukrainianHyphenator},
#endif
  }};
  return kEntries;
}

}  // namespace

const LanguageHyphenator* getLanguageHyphenatorForPrimaryTag(const std::string& primaryTag) {
  const auto& allEntries = entries();
  const auto it = std::find_if(allEntries.begin(), allEntries.end(),
                               [&primaryTag](const LanguageEntry& entry) { return primaryTag == entry.primaryTag; });
  return (it != allEntries.end()) ? it->hyphenator : nullptr;
}

LanguageEntryView getLanguageEntries() {
  const auto& allEntries = entries();
  return LanguageEntryView{allEntries.data(), allEntries.size()};
}
