#include <gtest/gtest.h>

#include "freeink-sdk/libs/display/FreeInkDisplay/src/driver/X3RefreshPolicy.h"
#include "freeink-sdk/libs/hardware/XteinkDetect/include/XteinkDetect.h"

TEST(X3RefreshPolicy, AllowsSlowBusyAssertionWithoutPrematureRecovery) {
  EXPECT_GE(freeink::x3_refresh::BUSY_START_TIMEOUT_MS, 1000U);
}

TEST(X3RefreshPolicy, UsesOtpPartialOnlyWithAValidOldPlane) {
  using freeink::x3_refresh::useOtpPartial;

  EXPECT_TRUE(useOtpPartial(/*fastRequested=*/true, /*oldPlaneSynced=*/true, /*fullSyncForced=*/false));
  EXPECT_FALSE(useOtpPartial(/*fastRequested=*/true, /*oldPlaneSynced=*/false, /*fullSyncForced=*/false));
  EXPECT_FALSE(useOtpPartial(/*fastRequested=*/false, /*oldPlaneSynced=*/true, /*fullSyncForced=*/false));
  EXPECT_FALSE(useOtpPartial(/*fastRequested=*/true, /*oldPlaneSynced=*/true, /*fullSyncForced=*/true));
}

TEST(X3ControllerDetection, AcceptsOnlyFamilyMatchingFactoryCalibration) {
  using freeink::oemScreenTypeMatchesUltraChip;

  EXPECT_TRUE(oemScreenTypeMatchesUltraChip(2, /*x3Family=*/true));
  EXPECT_TRUE(oemScreenTypeMatchesUltraChip(0x0C, /*x3Family=*/true));
  EXPECT_FALSE(oemScreenTypeMatchesUltraChip(1, /*x3Family=*/true));
  EXPECT_FALSE(oemScreenTypeMatchesUltraChip(0x0B, /*x3Family=*/true));

  EXPECT_TRUE(oemScreenTypeMatchesUltraChip(1, /*x3Family=*/false));
  EXPECT_TRUE(oemScreenTypeMatchesUltraChip(0x0B, /*x3Family=*/false));
  EXPECT_FALSE(oemScreenTypeMatchesUltraChip(2, /*x3Family=*/false));
  EXPECT_FALSE(oemScreenTypeMatchesUltraChip(0x0C, /*x3Family=*/false));
}

TEST(X3ControllerDetection, AcceptsProgrammedAndStableBlankMtpUc8279Panels) {
  using freeink::uc81xxMtpReadbackIsValid;

  const uint8_t programmed[] = {0xA5, 0x00, 0x00, 0x00};
  EXPECT_TRUE(uc81xxMtpReadbackIsValid(programmed, nullptr, sizeof(programmed)));

  // New production X3 panels can have blank MTP except for a version stamp.
  const uint8_t blankMtp[] = {0x00, 0x00, 0x02, 0x00, 0x00};
  const uint8_t matchingRead[] = {0x00, 0x00, 0x02, 0x00, 0x00};
  const uint8_t unstableRead[] = {0x00, 0x00, 0x03, 0x00, 0x00};
  EXPECT_TRUE(uc81xxMtpReadbackIsValid(blankMtp, matchingRead, sizeof(blankMtp)));
  EXPECT_FALSE(uc81xxMtpReadbackIsValid(blankMtp, unstableRead, sizeof(blankMtp)));

  const uint8_t floatingHigh[] = {0xFF, 0xFF, 0xFF, 0xFF};
  const uint8_t floatingLow[] = {0x00, 0x00, 0x00, 0x00};
  EXPECT_FALSE(uc81xxMtpReadbackIsValid(floatingHigh, floatingHigh, sizeof(floatingHigh)));
  EXPECT_FALSE(uc81xxMtpReadbackIsValid(floatingLow, floatingLow, sizeof(floatingLow)));
}
