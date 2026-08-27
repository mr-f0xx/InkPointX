#pragma once

#include <cstdint>

// A pending OTA image should be accepted once it has proved the parts that can
// strand a reader: storage, the selected panel driver, and one completed UI
// frame.  Waiting an arbitrary 30 seconds after those milestones only widens
// the rollback window: a normal power/reset during that interval sends a fully
// working image back to the previous firmware.  A few consecutive healthy main
// loop samples filter a transient observation without delaying confirmation.
class OtaHealthGate {
 public:
  static constexpr uint32_t MIN_UPTIME_MS = 1500;
  static constexpr uint8_t REQUIRED_HEALTHY_SAMPLES = 3;

  bool observe(uint32_t uptimeMs, bool coreReady, bool storageReady, bool displayReady, bool frameCompleted) {
    if (!(coreReady && storageReady && displayReady && frameCompleted)) {
      healthySamples = 0;
      return false;
    }
    if (healthySamples < REQUIRED_HEALTHY_SAMPLES) ++healthySamples;
    return uptimeMs >= MIN_UPTIME_MS && healthySamples >= REQUIRED_HEALTHY_SAMPLES;
  }

 private:
  uint8_t healthySamples = 0;
};
