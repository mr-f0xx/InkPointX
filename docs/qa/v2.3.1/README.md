# v2.3.1 acceptance checks

Connected hardware: XTEINK X4, ESP32-C3. Tests performed on 2026-09-19.

- 184 host tests passed, including repeated dark refreshes with automatic cleanup
  disabled and returning to the light theme.
- All 28 locales and 12 UI fonts passed localization validation.
- Local production and compact USB diagnostic builds fit the 0x640000-byte OTA slot.
- Saved and verified the installed v2.3.0 app partition before USB testing.
- Compared the same real book cover on Home in light and dark themes: every pixel
  in the 349 × 522-pixel interior matched. The surrounding interface inverted.
- Dark frames completed the full waveform in approximately 3729 ms, versus the
  previous 503 ms differential update. Dark output also requests panel power-off.

Private book screenshots and flash backups remain local. Framebuffer captures prove
image polarity and layout, not the panel's physical ghosting or its optical density.
The full waveform and power-off address both accumulated residue and idle fading;
remaining physical artifacts need direct observation on the panel.
