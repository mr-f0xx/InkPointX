#pragma once
#include <cstdint>
namespace BoardConfig {
struct Profile {
  uint16_t displayWidth = 792;
  uint16_t displayHeight = 528;
  uint32_t displaySpiHz = 20000000;
};
inline Profile ACTIVE;
}  // namespace BoardConfig
