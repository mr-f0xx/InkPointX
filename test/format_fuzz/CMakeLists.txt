# Mutation fuzzer for the untrusted-file readers. Not a CTest case: it takes a
# corpus directory at runtime and is meant to be run under
# -fsanitize=address,undefined, so it is built here as an ABI/compile gate and
# invoked explicitly.
find_package(ZLIB REQUIRED)

add_executable(format_fuzz_harness
  FormatFuzzHarness.cpp
  ${REPO_ROOT}/lib/ZipFile/ZipFile.cpp
  ${REPO_ROOT}/lib/InflateReader/InflateReader.cpp
  ${REPO_ROOT}/lib/uzlib/src/tinflate.c
  ${REPO_ROOT}/lib/Xtc/Xtc/XtcParser.cpp
)

target_include_directories(format_fuzz_harness BEFORE PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/mocks
  ${REPO_ROOT}/test/fb2_encoding/mocks
  ${REPO_ROOT}/lib/ZipFile
  ${REPO_ROOT}/lib/InflateReader
  ${REPO_ROOT}/lib/uzlib/src
  ${REPO_ROOT}/lib/Xtc
  ${REPO_ROOT}/lib/Xtc/Xtc
  ${REPO_ROOT}/lib/Serialization
  ${REPO_ROOT}/lib/FsHelpers
)

# The firmware compiles only tinflate.c from uzlib and never calls
# uzlib_uncompress_chksum, so adler32/crc32 are never linked. Strip the unused
# function here the same way the device build's --gc-sections does.
target_compile_options(format_fuzz_harness PRIVATE -ffunction-sections -fdata-sections)
if(APPLE)
  target_link_options(format_fuzz_harness PRIVATE -Wl,-dead_strip)
else()
  target_link_options(format_fuzz_harness PRIVATE -Wl,--gc-sections)
endif()

target_link_libraries(format_fuzz_harness PRIVATE crosspoint_test_common ZLIB::ZLIB)
