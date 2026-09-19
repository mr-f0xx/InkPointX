# v2.3.2 dark-mode speed verification

Connected XTEINK X4, 2026-09-19. This fixes the v2.3.1 regression: forcing FULL on
all dark frames made every interaction take about 3.7 seconds and visibly cycle
through light/inverted phases. v2.3.2 uses the caller's normal refresh cadence and
power setting in both themes. Pixel inversion happens in RAM before submission.

## Measured on the same device

Eight alternating Down/Up actions in Screen & Power, with identical settings and
no screenshot transfers during the measurements:

| Theme | Panel refresh range | Median | Submissions / completed refreshes |
| --- | --- | --- | --- |
| Light | 503–504 ms | 504 ms | 8 / 8 |
| Dark | 503–504 ms | 503 ms | 8 / 8 |

Each action submitted one FAST frame (`mode=2`) and completed one refresh. No FULL
or HALF was requested during either navigation sequence. These are controller
BUSY-wait timings, not a camera recording or a measurement of total layout time.
The test profile enables SDK display tracing; production does not include it.

EPUB forward/back checks likewise used 503–504 ms FAST updates. Initial entry and
the existing periodic reader cleanup used HALF (1716 ms), following the user's
configured cadence. Theme switching still performed its single intentional clean.

The cover interior matched the prior light-theme image pixel for pixel. Cached
and fresh cover redraws also matched; their only full-screen difference was the
battery percentage in the header. Private book captures remain local.

[Measured samples](refresh-comparison.json).

## Build checks

- 184 host tests passed, including a one-time theme refresh followed by 64 FAST
  frames and preservation of the reader's cleanup cadence.
- 28 locales and 12 generated UI fonts validated.
- Static analysis: no high or medium findings (41 low-level findings).
- Production image: 6,533,744 bytes locally, below the 6,553,600-byte OTA slot.
- The installed v2.3.1 image was verified before loading the diagnostic build.
