#pragma once
// Arduino's String, reduced to what FsHelpers.h needs to declare its API.
// XtcParser pulls FsHelpers in without calling it, so nothing here is executed;
// it only has to compile.
#include <string>

class String : public std::string {
 public:
  using std::string::string;
  String() = default;
  String(const std::string& value) : std::string(value) {}
  const char* c_str() const { return std::string::c_str(); }
};
