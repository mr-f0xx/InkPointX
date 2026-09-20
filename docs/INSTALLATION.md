# Installing InkPoint X

### Over the air

On a device already running InkPoint X, open **Settings → System → Check for updates**. The updater verifies the
release over HTTPS, stages the image, and switches boot slots only after the whole binary has landed. If the new
firmware fails to start, the bootloader rolls back to the previous slot automatically.

### Prebuilt firmware

Download the appropriate filename from [Releases](https://github.com/yokki-vans/InkPointX/releases). The image is
universal for X3/X4; `firmware.bin`, `update.bin`, and the device-labelled files are byte-identical aliases. The
different names select the updater that is already installed on the reader.

#### Stock X3/X4 recovery from microSD

1. Format a microSD card as FAT32.
2. Copy the release asset named exactly `update.bin` to the root of the card.
3. Safely eject the card and insert it into the reader.
4. Power the device off.
5. Hold the **left side / Up** button while powering on (on X3 this is the top-left side button).
6. Keep the device powered while the stock recovery installs the image and restarts.

The stock recovery does not browse for arbitrary files: `firmware.bin` is ignored there. `update.bin` contains only
the application firmware; it does not replace the bootloader, partition table, NVS, or the SD recovery mechanism.
New-production X3 units with the revised blank-MTP UC8279D display require InkPoint X 2.2.26 or newer; earlier builds can finish
installation but leave the updater's last e-ink frame visible because they select the legacy display controller.

#### Recovery from InkPoint X or CrossPoint

1. Copy `firmware.bin` to the root of a FAT32 microSD card.
2. Insert it, power the reader off, then hold **left side / Up** while powering on.
3. Select `firmware.bin` in the recovery file picker and confirm.
4. Keep the reader powered until it restarts.

#### Flash over USB

Install [esptool](https://github.com/espressif/esptool), connect the reader over USB-C, and run:

```bash
esptool --chip esp32c3 \
  --port /dev/ttyACM0 \
  --baud 921600 \
  write-flash 0x10000 firmware.bin
```

Replace `/dev/ttyACM0` with the actual serial port. On macOS it is usually named `/dev/cu.usbmodem*`.

> [!CAUTION]
> Flash only InkPoint X binaries built for the XTEINK X3/X4 ESP32-C3 family, do not disconnect power while writing, and keep a recovery-capable
> microSD card available when testing development builds.

