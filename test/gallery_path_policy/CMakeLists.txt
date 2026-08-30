add_executable(gallery_path_policy_tests GalleryPathPolicyTest.cpp)

target_include_directories(gallery_path_policy_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/../src
)

target_link_libraries(gallery_path_policy_tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(gallery_path_policy_tests)
