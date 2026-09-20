#pragma once
// On device HalStorage.h reaches Arduino first; the readers rely on that for
// Print and friends, so pull the shim in before the shared storage mock.
#include "Arduino.h"

#include "../../fb2_encoding/mocks/HalStorage.h"
