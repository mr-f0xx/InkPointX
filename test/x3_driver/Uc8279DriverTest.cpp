#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <vector>

#include "freeink-sdk/libs/display/FreeInkDisplay/src/driver/Uc8279Driver.h"
#include "freeink-sdk/libs/display/FreeInkDisplay/src/lut/Uc8279X3Luts.h"

namespace {
unsigned long clockMs = 0;
unsigned long refreshAt = 0;
uint32_t startDelay = 0;
bool powerOk = true;
bool finishOk = true;
bool finishBeforeStart = false;
int oldWrites = 0;
uint8_t lastCommand = 0;
const uint8_t* vcomBank = nullptr;
std::vector<uint8_t> commands;
std::array<uint8_t, 52272> frame{};
}  // namespace

unsigned long millis() { return clockMs; }
void delay(unsigned long ms) { clockMs += ms; }
int digitalRead(int) { return clockMs - refreshAt >= startDelay ? LOW : HIGH; }

// Link the production driver against a recording bus. No physical SPI/ISR
// timing is claimed: these tests exercise the real driver's command ordering
// and state transitions when the panel is late, absent or times out.
namespace freeink {
void EpdBus::reset(uint16_t) {}
void EpdBus::cmd(uint8_t c) {
  lastCommand = c;
  commands.push_back(c);
  if (c == 0x12) refreshAt = clockMs;
}
void EpdBus::data(uint8_t) {}
void EpdBus::data(const uint8_t* data, uint16_t) {
  if (lastCommand == 0x20) vcomBank = data;
}
void EpdBus::cmdData(uint8_t c, const uint8_t*, uint16_t) { cmd(c); }
void EpdBus::beginTxn() {}
void EpdBus::endTxn() {}
void EpdBus::rawWriteBytes(const uint8_t*, uint16_t) {}
void EpdBus::fillPlane(uint8_t c, uint8_t, uint16_t, uint16_t) { cmd(c); }
void EpdBus::sendPlaneFlipped(uint8_t c, const uint8_t*, uint16_t, uint16_t) {
  cmd(c);
  if (c == 0x10) ++oldWrites;
}
void EpdBus::sendPlaneFlippedInverted(uint8_t c, const uint8_t* p, uint16_t h, uint16_t w) {
  sendPlaneFlipped(c, p, h, w);
}
bool EpdBus::waitBusy(const char*) { return powerOk; }
bool EpdBus::waitForBusyStart(uint32_t timeout, const char*) {
  clockMs += std::min(startDelay, timeout);
  return startDelay <= timeout;
}
bool EpdBus::waitRefreshComplete(const char*) {
  finishBeforeStart = clockMs - refreshAt < startDelay;
  return finishOk && !finishBeforeStart;
}
}  // namespace freeink

class Uc8279DriverTest : public ::testing::Test {
 protected:
  freeink::EpdBus bus;
  freeink::Uc8279Driver driver;
  void SetUp() override {
    clockMs = refreshAt = startDelay = 0;
    powerOk = finishOk = true;
    finishBeforeStart = false;
    oldWrites = 0;
    commands.clear();
    vcomBank = nullptr;
    driver.begin(bus);
    driver.skipInitialResync();
    commands.clear();
  }
  bool start() { return driver.displayStart(bus, frame.data(), nullptr, freeink::RefreshMode::Fast, false); }
};

TEST_F(Uc8279DriverTest, WaitsForDelayedBusyBeforeCompletingAndWritingOldPlane) {
  startDelay = 250;
  ASSERT_TRUE(start());
  EXPECT_GE(clockMs - refreshAt, 250u);
  EXPECT_EQ(oldWrites, 0);
  driver.displayFinish(bus, frame.data());
  EXPECT_FALSE(finishBeforeStart);
  EXPECT_EQ(oldWrites, 1);
  EXPECT_EQ(vcomBank, &freeink::kUc8279X3_BwDu[0][1]);
}

TEST_F(Uc8279DriverTest, MissingBusyDoesNotCommitFrameAndForcesCleanRetry) {
  startDelay = 2000;
  EXPECT_FALSE(start());
  driver.displayFinish(bus, frame.data());
  EXPECT_EQ(oldWrites, 0);
  startDelay = 0;
  ASSERT_TRUE(start());
  EXPECT_EQ(vcomBank, &freeink::kUc8279X3_BwGc[0][1]);
}

TEST_F(Uc8279DriverTest, CompletionTimeoutDoesNotCommitFrameAndForcesCleanRetry) {
  ASSERT_TRUE(start());
  finishOk = false;
  driver.displayFinish(bus, frame.data());
  EXPECT_EQ(oldWrites, 0);
  finishOk = true;
  ASSERT_TRUE(start());
  EXPECT_EQ(vcomBank, &freeink::kUc8279X3_BwGc[0][1]);
}

TEST_F(Uc8279DriverTest, PowerFailureDoesNotIssueRefresh) {
  powerOk = false;
  EXPECT_FALSE(start());
  EXPECT_EQ(std::count(commands.begin(), commands.end(), 0x12), 0);
  driver.displayFinish(bus, frame.data());
  EXPECT_EQ(oldWrites, 0);
}

TEST_F(Uc8279DriverTest, ReadyPanelDoesNotPayFullTimeoutOrPromoteFastFrame) {
  startDelay = 1;
  ASSERT_TRUE(start());
  EXPECT_EQ(clockMs - refreshAt, 1u);
  driver.displayFinish(bus, frame.data());
  EXPECT_EQ(vcomBank, &freeink::kUc8279X3_BwDu[0][1]);
  EXPECT_EQ(std::count(commands.begin(), commands.end(), 0x12), 1);
}
