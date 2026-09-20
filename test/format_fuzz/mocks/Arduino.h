#pragma once
// Minimal Arduino surface needed to compile the binary format readers on a
// host. Only what the parsers under test actually touch.
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

inline unsigned long millis() { return 0; }
inline void delay(unsigned long) {}

class Print {
 public:
  virtual ~Print() = default;
  virtual size_t write(const uint8_t* buffer, size_t size) = 0;
  virtual size_t write(uint8_t value) { return write(&value, 1); }
};
