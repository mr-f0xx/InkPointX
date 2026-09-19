# X3 input/display investigation — 2026-09-19

Status: candidate fixes, **not hardware-validated on X3**. No stable release
has been published for this investigation. The current stable release remains
v2.3.2. The affected users' exact installed versions are not yet confirmed.

## Evidence and limits

- The supplied first video retains a “Going to sleep” popup, then later shows
  the home screen. A retained e-ink frame alone cannot establish whether the
  CPU is asleep, blocked, or rebooting.
- In the second video the settings selection moves. The report was clarified
  as slow/intermittent response, not a confirmed permanent failure of the
  front buttons. The sampled frames do not measure input-to-display latency.
- The supplied box photograph is labelled “赠品套装” (accessory/gift set),
  production year 2026. It does not identify the display controller.
- No X3 or X4 was connected during this investigation. Earlier X4 acceptance
  tests of v2.3.2 do not validate either X3 controller.

## Reproduced software defects

1. The UC8279 driver polled BUSY-start for only 50 ms, then ignored refresh
   completion failure and wrote the OLD plane. With a delayed BUSY edge, the
   facade can mark the bus unhealthy and reset the controller on the next
   navigation frame. A recording bus linked to the **production driver**
   reproduces premature completion with a 250 ms start delay. Missing-start,
   completion-timeout, and power-on-failure scenarios also failed before the
   patch. This is a plausible explanation for field delays, not proof that
   these particular devices use UC8279 or experienced that timing.
2. Every X3 cold power-on was classified as a Power-button wake, even though
   X3 has no VBUS detector. USB insertion without Power held could therefore
   enter deep sleep during startup. Cold X3 starts now remain interactive;
   genuine GPIO wake from deep sleep still verifies the press.
3. Holding Power at the end of startup discarded **all** input and returned
   before housekeeping. A HAL filter now suppresses only the wake Power press
   and its release; navigation and the main loop continue.

The UC8279 patch uses the existing bounded 1000 ms BUSY-start handshake (it
returns immediately when BUSY asserts), stops on failures, and invalidates the
baseline for a clean retry. It does not change LUTs, ADC thresholds, pin
assignments, cover rendering, or normal fast-refresh cadence. The recovery
  chord is checked for every reset source, including X3 cold starts and USB resets.

Candidate image validation also found a stale weak application descriptor in
the prebuilt Arduino/ESP-IDF archive. `AppDescriptor.cpp` supplies the firmware's
own descriptor, using `CROSSPOINT_VERSION` and preserving SDK chip-revision,
secure-version and MMU settings. The OTA descriptor and UI now agree.

## Validation

- Four of five new UC8279 driver tests fail against the old driver. All five
  pass with the patch: delayed start, missing start, completion failure,
  power-on failure, and immediate fast update.
- Seven boot/input policy tests cover X3 cold start, real GPIO wake, X4
  battery/USB routing, software restart, all six navigation buttons during a
  held wake press, suppressed wake release, and the next genuine Power press.
- All 196 host tests pass sequentially. A parallel run exposed an unrelated
  FB2 test-fixture collision (one test read another test's temporary book);
  this is not reported as a firmware regression or silently excluded.
- Production and diagnostic firmware must fit the 0x640000-byte app slot.
  Build logs and candidate hashes are kept in `artifacts/qa-x3-input/`.
- Both builds passed: production 6,533,840 bytes; diagnostic v2.3.3-rc.1
  6,539,152 bytes (14,448 bytes below the slot limit). The candidate's embedded
  version, ESP image checksum/hash and the packaged binary were verified.
  Candidate SHA-256:
  `065980c4e95ecb32d5ec09d89f92bf80d48c979b0d935337916df90fa2f1487f`.
- Canonical static analysis passed: 0 high, 0 medium, 41 low findings.
- SDK change: [community-sdk PR #1](https://github.com/yokki-vans/community-sdk/pull/1),
  commit `cc2db24922aedf79dd0da97bbeb0e40a8d3dca95`.

## Required X3 acceptance

1. Record the installed version and controller profile; distinguish UC8253
   from UC8279. Preserve `/.crosspoint/diag.log` before resetting preferences.
2. On the candidate, press each front and side button separately in Home and
   Settings, then repeat after at least 10 seconds idle. Check both quick taps
   and one-second holds; distinguish missing presses from slow display updates.
3. Capture a diagnostic USB log of the slow case. The candidate prints
   `Hardware detect: xteink_x3[_uc8279]` and EPD wait durations. The command
   `CMD:INPUT_DIAG` reports raw ADC1/ADC2 and the debounced physical-button mask.
   It temporarily holds normal CPU speed, so it diagnoses button electrical
   readings, not low-power latency. Do not infer optical quality from the log.
4. Check sleep/wake, USB insertion, recovery entry, light/dark navigation,
   unchanged cover colours, and ordinary page turns. Repeat on both X3 panel
   revisions before claiming broad X3 support for this fix.

## Recovery and evidence collection (RU)

Сначала сохраните копию карты памяти и файл `/.crosspoint/diag.log`.
Уточните точное имя установленного файла прошивки или версию на экране
«Информация об устройстве». Не форматируйте карту для диагностики.

Если меню отвечает с задержкой, нажимайте по одной кнопке и дождитесь результата.
Можно открыть обычное обновление с SD и выбрать заведомо работавший на этом
устройстве образ. Не считайте старую версию автоматически совместимой с новой
ревизией экрана X3.

При зависании попробуйте штатный перезапуск: нажать и отпустить Reset, затем
сразу удерживать Power несколько секунд. Такой порядок описан в
[руководстве CrossPoint](https://github.com/crosspoint-reader/crosspoint-reader/blob/develop/USER_GUIDE.md#2-power--startup).

Если после перезапуска компьютер видит устройство, USB-прошивка через
[CrossPoint flasher](https://crosspointreader.com/) не требует Select в меню
ридера. Выберите X3 и подходящий образ; следуйте инструкции этого прошивальщика
после записи. Используйте обновление приложения, сохраняющее таблицу разделов
и NVS. Источник поведения: [flasher.js](https://github.com/crosspoint-reader/crosspoint-tools/blob/master/src/lib/flasher.js).

Если USB-устройство не появляется, этот путь пока недоступен. Нужны сведения,
работал ли USB до установки, и какой способ установки использовался. Для
USB-locked X3 нельзя обещать, что обычный USB-прошивальщик подключится. Не
используйте `erase_flash`, чужой полный дамп или запись по угаданному адресу.
