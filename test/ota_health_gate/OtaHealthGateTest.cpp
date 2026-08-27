#include <gtest/gtest.h>

#include "src/network/OtaHealthGate.h"

TEST(OtaHealthGate, AcceptsPromptlyAfterARealFrame) {
  OtaHealthGate gate;
  EXPECT_FALSE(gate.observe(1400, true, true, true, true));
  EXPECT_FALSE(gate.observe(1490, true, true, true, true));
  EXPECT_TRUE(gate.observe(1500, true, true, true, true));
}

TEST(OtaHealthGate, NeverAcceptsWithoutEveryFunctionalMilestone) {
  OtaHealthGate gate;
  for (uint32_t i = 0; i < 10; ++i) {
    EXPECT_FALSE(gate.observe(2000 + i, true, true, false, true));
  }
  EXPECT_FALSE(gate.observe(3000, true, true, true, true));
  EXPECT_FALSE(gate.observe(3001, true, true, true, true));
  EXPECT_TRUE(gate.observe(3002, true, true, true, true));
}

TEST(OtaHealthGate, ResetsConsecutiveSamplesAfterAHealthRegression) {
  OtaHealthGate gate;
  EXPECT_FALSE(gate.observe(2000, true, true, true, true));
  EXPECT_FALSE(gate.observe(2001, true, true, true, true));
  EXPECT_FALSE(gate.observe(2002, true, true, false, true));
  EXPECT_FALSE(gate.observe(2003, true, true, true, true));
  EXPECT_FALSE(gate.observe(2004, true, true, true, true));
  EXPECT_TRUE(gate.observe(2005, true, true, true, true));
}
