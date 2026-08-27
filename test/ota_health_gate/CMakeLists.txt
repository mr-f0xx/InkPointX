add_executable(ota_health_gate_tests
  OtaHealthGateTest.cpp
)

target_link_libraries(ota_health_gate_tests PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(ota_health_gate_tests)
