#pragma once

#include <cstdint>

namespace BootInputPolicy {
enum class WakeupReason { PowerButton, AfterFlash, AfterUSBPower, Other };

constexpr WakeupReason classifyWakeup(bool gpioWake, bool coldPowerOn, bool unknownReset, bool isX3, bool usbPresent) {
  if (gpioWake) return WakeupReason::PowerButton;
  if (unknownReset) return WakeupReason::AfterFlash;
  // X3 has no VBUS detector. A cold start can be USB insertion, so requiring
  // a held Power button here makes an otherwise healthy X3 immediately sleep.
  if (coldPowerOn && !isX3) return usbPresent ? WakeupReason::AfterUSBPower : WakeupReason::PowerButton;
  return WakeupReason::Other;
}

class PowerReleaseGuard {
 public:
  static constexpr uint8_t POWER_MASK = 1U << 6;

  void begin(bool powerHeld) {
    waitingForRelease = powerHeld;
    suppressSample = powerHeld;
  }

  void update(bool powerHeld) {
    suppressSample = waitingForRelease;
    if (!powerHeld) waitingForRelease = false;
  }

  uint8_t filter(uint8_t buttons) const {
    return suppressSample ? static_cast<uint8_t>(buttons & ~POWER_MASK) : buttons;
  }

 private:
  bool waitingForRelease = false;
  bool suppressSample = false;
};
}  // namespace BootInputPolicy
