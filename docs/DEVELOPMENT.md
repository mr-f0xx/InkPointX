# Building and validating InkPoint X

### Requirements

- Git with submodule support;
- Python 3;
- [PlatformIO Core](https://platformio.org/install/cli);
- internet access on the first build for declared toolchains, libraries, and font sources.

### Clone the stable source

```bash
git clone --branch main --recurse-submodules https://github.com/yokki-vans/InkPointX.git
cd InkPointX
```

If the repository was cloned without submodules:

```bash
git submodule update --init --recursive
```

### Build

Development build:

```bash
pio run -e default
```

For USB acceptance tests on a device, use the compact diagnostic environment:

```bash
pio run -e device_qa
```

It includes serial screenshots and navigation commands with main-loop diagnostics.
Its version is `v2.3.0-rc.1`, so it can also exercise an OTA upgrade to the final
release. The full `default` debug build can exceed the 6,553,600-byte OTA slot;
always check the binary size before flashing. Test commands include
`CMD:PROFILE_SETTINGS:1` (Screen & Power), `CMD:PROFILE_CONFIRM`,
`CMD:PROFILE_BACK`, `CMD:PROFILE_HOME`, and `CMD:SCREENSHOT`.

Release-style binary:

```bash
pio run -e gh_release
```

The resulting binary is:

```text
.pio/build/gh_release/firmware.bin
```

Upload directly through PlatformIO:

```bash
pio run -e gh_release --target upload
```

## Validation

Run localization and font coverage checks:

```bash
python3 scripts/validate_i18n.py
```

Run host tests:

```bash
cmake -S test -B build/test -DCMAKE_BUILD_TYPE=Release
cmake --build build/test
ctest --test-dir build/test --output-on-failure
```

Run static analysis:

```bash
pio check -e default --fail-on-defect=medium
```

Release checks cover the host suite, localization coverage across 28 languages, firmware builds, static analysis,
and the OTA slot size limit. Hardware checks and their scope are recorded with each release.

## Releases and OTA

Pushing a tag builds the universal `gh_release` image and publishes `firmware.bin`, the stock-recovery alias
`update.bin`, X3/X4-labelled aliases, and SHA-256 checksums. The on-device updater reads `releases/latest` and looks
for exactly `firmware.bin`. It downloads to a
temporary SD-card file, requires an exact size and GitHub release SHA-256 match, validates the complete ESP image,
and only then writes the inactive OTA slot. Network operations retry three times; a failed download or validation
never selects the candidate image. Version comparison is semantic — major, then minor, then patch — with release
candidates treated as older than the final tag.

## Repository layout

```text
src/                         Firmware activities, settings, stores, and UI
lib/                         Readers, rendering, fonts, bidi, i18n, and HAL
freeink-sdk/                 X3/X4 display, input, storage, and hardware libraries
scripts/                     Code generation, font subsetting, and validation
test/                        Host-side unit and policy tests
docs/                        User, developer, attribution, and visual QA docs
design-qa.md                 Design audit findings and deliberate decisions
platformio.ini               ESP32-C3 build environments
partitions.csv               16 MB flash partition layout
```

The `freeink-sdk` submodule pins a commit of
[`yokki-vans/community-sdk`](https://github.com/yokki-vans/community-sdk), the InkPoint X hardware
branch based on FreeInk SDK. It provides the runtime X3/X4 board profiles, panel drivers, input, sensors, storage,
TLS transport, and power management used by this firmware.

