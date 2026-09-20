#pragma once
// The format fuzzer reuses the filesystem-backed storage mock that the FB2 and
// PDF host tests already share, so a corpus file on disk is what the reader
// sees. Arduino.h supplies Print, which ZipFile.h declares against and the
// device build gets transitively through the real HalStorage.h.
#include "../../fb2_encoding/mocks/HalStorage.h"
#include "Arduino.h"
