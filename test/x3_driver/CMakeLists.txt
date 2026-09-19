add_executable(x3_driver_tests
  Uc8279DriverTest.cpp
  ${REPO_ROOT}/freeink-sdk/libs/display/FreeInkDisplay/src/driver/Uc8279Driver.cpp
)
target_include_directories(x3_driver_tests BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mocks)
target_link_libraries(x3_driver_tests PRIVATE crosspoint_test_common GTest::gtest_main)
gtest_discover_tests(x3_driver_tests)
