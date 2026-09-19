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

[Support development on Ko-fi](https://ko-fi.com/yokkivans).
