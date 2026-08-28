#include <gtest/gtest.h>

#include "freeink-sdk/libs/display/FreeInkDisplay/src/driver/X3RefreshPolicy.h"
#include "freeink-sdk/libs/display/FreeInkDisplay/src/lut/Uc8279X3Luts.h"
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

TEST(X3ControllerInitialization, BlankMtpPanelReceivesCompleteCrossPointRegisterScript) {
  const auto findCommand = [](uint8_t wanted, uint8_t* length) -> const uint8_t* {
    size_t offset = 0;
    while (offset + 2 <= sizeof(freeink::kUc8279X3_Init)) {
      const uint8_t command = freeink::kUc8279X3_Init[offset++];
      const uint8_t commandLength = freeink::kUc8279X3_Init[offset++];
      if (offset + commandLength > sizeof(freeink::kUc8279X3_Init)) return nullptr;
      if (command == wanted) {
        *length = commandLength;
        return freeink::kUc8279X3_Init + offset;
      }
      offset += commandLength;
    }
    return nullptr;
  };

  uint8_t length = 0;
  const uint8_t* psr = findCommand(0x00, &length);
  ASSERT_NE(psr, nullptr);
  ASSERT_EQ(length, 2);
  EXPECT_EQ(psr[0], 0x3F);  // REG=1: use external waveform LUTs
  EXPECT_EQ(psr[1], 0x4A);

  const uint8_t* window = findCommand(0x90, &length);
  ASSERT_NE(window, nullptr);
  ASSERT_EQ(length, 9);
  EXPECT_EQ(window[2], 0x03);
  EXPECT_EQ(window[3], 0x17);  // X end 791
  EXPECT_EQ(window[6], 0x02);
  EXPECT_EQ(window[7], 0x0F);  // Y end 527

  const uint8_t* power = findCommand(0x01, &length);
  ASSERT_NE(power, nullptr);
  ASSERT_EQ(length, 5);
  EXPECT_EQ(power[0], 0x43);  // source/gate rails enabled
  EXPECT_EQ(power[2], 0x78);
  EXPECT_EQ(power[3], 0x78);

  ASSERT_NE(findCommand(0x82, &length), nullptr);  // VCOM DC
  ASSERT_NE(findCommand(0x06, &length), nullptr);  // booster soft-start
  ASSERT_NE(findCommand(0x30, &length), nullptr);  // PLL
  EXPECT_EQ(sizeof(freeink::kUc8279X3_BwGc), 5U * 43U);
  EXPECT_EQ(sizeof(freeink::kUc8279X3_BwDu), 5U * 43U);
}
