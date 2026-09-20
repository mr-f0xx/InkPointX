#include <gtest/gtest.h>

#include "lib/hal/BootInputPolicy.h"

using namespace BootInputPolicy;

TEST(BootInputPolicy, X3ColdStartWithoutPowerButtonStaysAwake) {
  EXPECT_EQ(classifyWakeup(false, true, false, true, false), WakeupReason::Other);
  // Charging status is not proof of a boot source either.
  EXPECT_EQ(classifyWakeup(false, true, false, true, true), WakeupReason::Other);
}

TEST(BootInputPolicy, GenuineSleepWakeStillVerifiesPowerPress) {
  EXPECT_EQ(classifyWakeup(true, false, false, true, false), WakeupReason::PowerButton);
  EXPECT_EQ(classifyWakeup(true, false, false, false, false), WakeupReason::PowerButton);
}

TEST(BootInputPolicy, X4ColdStartPreservesUsbAndBatteryBehavior) {
  EXPECT_EQ(classifyWakeup(false, true, false, false, false), WakeupReason::PowerButton);
  EXPECT_EQ(classifyWakeup(false, true, false, false, true), WakeupReason::AfterUSBPower);
}

TEST(BootInputPolicy, FlashAndSoftwareRestartRemainInteractive) {
  for (bool x3 : {false, true}) {
    EXPECT_EQ(classifyWakeup(false, false, true, x3, false), WakeupReason::AfterFlash);
    EXPECT_EQ(classifyWakeup(false, false, false, x3, false), WakeupReason::Other);
  }
}

TEST(BootInputPolicy, HeldWakePowerDoesNotBlockAnyNavigationButton) {
  PowerReleaseGuard guard;
  guard.begin(true);
  for (int sample = 0; sample < 100; ++sample) {
    guard.update(true);
    for (uint8_t button = 0; button < 6; ++button) {
      const uint8_t mask = 1U << button;
      EXPECT_EQ(guard.filter(mask | PowerReleaseGuard::POWER_MASK), mask);
    }
  }
}

TEST(BootInputPolicy, WakeReleaseIsDiscardedButNextPowerClickWorks) {
  PowerReleaseGuard guard;
  guard.begin(true);
  guard.update(false);
  EXPECT_EQ(guard.filter(PowerReleaseGuard::POWER_MASK), 0);
  guard.update(true);
  EXPECT_EQ(guard.filter(PowerReleaseGuard::POWER_MASK), PowerReleaseGuard::POWER_MASK);
  guard.update(false);
  EXPECT_EQ(guard.filter(PowerReleaseGuard::POWER_MASK), PowerReleaseGuard::POWER_MASK);
}

TEST(BootInputPolicy, BootWithoutHeldPowerDoesNotSuppressFirstPress) {
  PowerReleaseGuard guard;
  guard.begin(false);
  guard.update(true);
  EXPECT_EQ(guard.filter(0x7f), 0x7f);
}
