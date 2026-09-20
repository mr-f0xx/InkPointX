<h1 align="center">InkPoint X</h1>

<p align="center">Open-source e-reader firmware for XTEINK X3 and X4.</p>

<p align="center">
  <a href="https://github.com/yokki-vans/InkPointX/releases/latest"><img alt="Version 2.3" src="https://img.shields.io/badge/version-2.3-111111"></a>
  <a href="LICENSE"><img alt="License: MIT" src="https://img.shields.io/badge/license-MIT-555555"></a>
  <a href="https://ko-fi.com/yokkivans"><img alt="Support on Ko-fi" src="https://img.shields.io/badge/support-Ko--fi-F16061"></a>
</p>

<p align="center">
  <img src="docs/qa/v2.3/settings-light.png" width="240" alt="Light theme on XTEINK X4">
  <img src="docs/qa/v2.3/settings-dark.png" width="240" alt="Dark theme on XTEINK X4">
</p>

> [!IMPORTANT]
> `dev` is the canonical development and release branch. Firmware tags must point to commits already merged into
> `dev`; the release workflow rejects tags cut from `main` or feature branches. For a prebuilt, user-facing binary,
> use the [Releases](https://github.com/yokki-vans/InkPointX/releases) page. Devices already running InkPoint X update from
> **Settings → System → Check for updates**.

[Download firmware](https://github.com/yokki-vans/InkPointX/releases/latest) ·
[Installation](docs/INSTALLATION.md) · [User guide](USER_GUIDE.md) ·
[Report an issue](https://github.com/yokki-vans/InkPointX/issues)

## New in 2.3

**Dark mode** covers the home screen, library, settings, reading, and sleep screens.
Enable it in **Settings → Screen & Power → Dark mode**. The choice survives restarts;
existing installations stay in the light theme until you enable it.

Dark mode uses monochrome output; four-level grayscale and text antialiasing resume
in the light theme. The separate reading-only inversion setting still works and
never cancels the system theme. Covers keep their original black/white colors.
Dark mode uses the same fast refresh cadence as the light theme. The completed dark
frame is sent directly to the display, without a light intermediate frame. Periodic
reader cleanup follows your refresh setting. See the [release notes](docs/releases/v2.3.2.md).

**X3 stability:** v2.3.3 addresses slow navigation and startup/sleep recovery
reported on X3, including delayed UC8279 display refreshes. See the
[v2.3.3 notes](docs/releases/v2.3.3.md).

## Features

- **Books:** EPUB 2/3, FB2, PDF, Markdown, TXT, XTC and XTCH; bookmarks, contents,
  footnotes, search, dictionaries, reading progress and statistics.
- **Library:** covers, folders, favorites, recent books, reading goals and achievements.
- **Typography:** adjustable fonts, spacing, margins, orientation and hyphenation;
  downloadable fonts and 28 interface languages, including Russian and Ukrainian.
- **Files and images:** file management, gallery, custom sleep images and book-cover sleep screens.
- **Connectivity:** Wi-Fi file transfer, WebDAV, Calibre Wireless, OPDS, KOReader sync
  and verified OTA updates.
- **Hardware:** one ESP32-C3 image detects X3 (528 × 792) and X4 (480 × 800) at boot.
  X4 Pro is not supported by this release.

FB2 and PDF are prepared on the device and cached on microSD. Complex PDFs can have
missing content or unsupported features; EPUB is the best choice for reflowable books.
Books and settings stay on the card in their existing locations; firmware updates
preserve them. See [supported formats and workflows](USER_GUIDE.md).

## Install or update

**Already using InkPoint X:** connect to Wi-Fi, then open
**Settings → System → Check for updates**.

**Stock X3/X4 firmware:** download `update.bin` from the
[latest release](https://github.com/yokki-vans/InkPointX/releases/latest), copy it to
the root of a FAT32 microSD card, power off, then hold **left side / Up** while powering on.
Keep the device powered until installation finishes.

**InkPoint X / CrossPoint SD recovery:** use `firmware.bin` instead; select it in the
recovery file picker. `firmware.bin`, `update.bin` and the X3/X4-labelled downloads
contain the same firmware. `SHA256SUMS` provides file checksums.

[Full installation and USB instructions](docs/INSTALLATION.md) ·
[X3 hardware notes](docs/X3_SUPPORT_RU.md)

## Build

Install Python 3, PlatformIO Core and the font pipeline requirements:

```bash
git clone --recurse-submodules https://github.com/yokki-vans/InkPointX.git
cd InkPointX
python3 -m pip install -r lib/EpdFont/scripts/requirements.txt
pio run -e gh_release
```

Output: `.pio/build/gh_release/firmware.bin`. Use `pio run -e default` for a build
with serial diagnostics. `main` contains stable source; `dev` is the development branch.

[Build, tests and release workflow](docs/DEVELOPMENT.md) ·
[Contributing](docs/contributing/README.md)

## Credits and license

Based on [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader)
and [FreeInk SDK](https://github.com/yokki-vans/community-sdk), with the InkPoint X
interface and device integrations. Firmware is [MIT-licensed](LICENSE); bundled
components and fonts retain their [respective licenses](LICENSES).

## Validation

Run localization and font coverage checks:

```bash
python3 scripts/validate_i18n.py
```

Run host tests:

```bash
cmake -S test -B test/build -DCMAKE_BUILD_TYPE=Release
cmake --build test/build
ctest --test-dir test/build --output-on-failure
```

Run static analysis:

```bash
pio check -e default --fail-on-defect=medium
```

Release validation includes the host suite, localization coverage across 28 languages, development and release
compilation, static analysis, PDF conversion checks, and a hard flash budget so the image cannot silently grow into
the OTA slot's limit.

## Releases and OTA

Pushing a version tag for a commit already on `dev` builds the universal `gh_release` image and publishes `firmware.bin`, the stock-recovery alias
`update.bin`, X3/X4-labelled aliases, and SHA-256 checksums. The on-device updater reads `releases/latest` and looks
for exactly `firmware.bin`. It downloads to a
temporary SD-card file, requires an exact size and GitHub release SHA-256 match, validates the complete ESP image,
and only then writes the inactive OTA slot. Network operations retry three times; a failed download or validation
never selects the candidate image. Version comparison is semantic — major, then minor, then patch — with release
candidates treated as older than the final tag.

The workflow also requires the tag to match `platformio.ini` and a non-empty
`docs/releases/<tag>.md`. See the [release process](docs/RELEASE_PROCESS.md).

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

The `freeink-sdk` submodule points to
[`yokki-vans/community-sdk`](https://github.com/yokki-vans/community-sdk/tree/inkpointx-v2.2), the InkPoint X hardware
branch based on FreeInk SDK. It provides the runtime X3/X4 board profiles, panel drivers, input, sensors, storage,
TLS transport, and power management used by this firmware.

## Data and storage

Books stay on the microSD card. InkPoint X stores its generated caches and application data under `/.crosspoint/`.
Settings and application state are written atomically, so an interrupted save leaves the previous file intact —
but a separate backup of irreplaceable files remains the safest protection before testing development builds.

The web interface and device file manager can modify or delete files. Destructive operations require confirmation.

> [!NOTE]
> Updating to 2.0 changes the reader viewport slightly, because the reading page's status bar is smaller. Each book
> re-indexes once the first time it is opened afterwards; reading positions are preserved.

## Project origin and attribution

InkPoint X is derived from CrossPoint Reader and includes work from its contributors and the wider open-source
e-reader community. Third-party components, fonts, and icons retain their original licenses.

- Project license: [MIT](LICENSE)
- Third-party notices: [docs/third-party-notices.md](docs/third-party-notices.md)
- Lucide icon notice: [docs/licenses/lucide-ISC.txt](docs/licenses/lucide-ISC.txt)

## Contributing

Bug reports, hardware observations, translations, documentation improvements, and focused pull requests are
welcome. When changing the interface, validate it against both 528 × 792 and 480 × 800 framebuffers and, whenever
possible, physical X3 and X4 panels.

Please run the relevant validation commands above before opening a pull request.

## Support

If InkPoint X is useful to you, you can support ongoing development on
[Ko-fi](https://ko-fi.com/yokkivans).
