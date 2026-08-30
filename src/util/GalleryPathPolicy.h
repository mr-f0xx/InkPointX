#pragma once

#include <string_view>

namespace GalleryPathPolicy {

inline bool shouldSkipEntry(std::string_view name) {
  return name.empty() || name.front() == '.' || name == "System Volume Information";
}

}  // namespace GalleryPathPolicy
