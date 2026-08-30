#include <gtest/gtest.h>

#include "util/GalleryPathPolicy.h"

TEST(GalleryPathPolicy, SkipsHiddenMetadataAndCacheEntries) {
  EXPECT_TRUE(GalleryPathPolicy::shouldSkipEntry(".metadata"));
  EXPECT_TRUE(GalleryPathPolicy::shouldSkipEntry(".crosspoint"));
  EXPECT_TRUE(GalleryPathPolicy::shouldSkipEntry(".thumbnails"));
  EXPECT_TRUE(GalleryPathPolicy::shouldSkipEntry(".thumb.jpg"));
}

TEST(GalleryPathPolicy, SkipsEmptyAndSystemMetadataEntries) {
  EXPECT_TRUE(GalleryPathPolicy::shouldSkipEntry(""));
  EXPECT_TRUE(GalleryPathPolicy::shouldSkipEntry("System Volume Information"));
}

TEST(GalleryPathPolicy, KeepsVisibleFoldersAndImages) {
  EXPECT_FALSE(GalleryPathPolicy::shouldSkipEntry("Pictures"));
  EXPECT_FALSE(GalleryPathPolicy::shouldSkipEntry("thumb.jpg"));
  EXPECT_FALSE(GalleryPathPolicy::shouldSkipEntry("holiday.png"));
}
