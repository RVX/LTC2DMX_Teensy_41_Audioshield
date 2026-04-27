# LTC2DMX — Teensy 4.1 + Audio Shield

Reads **SMPTE LTC timecode** from a video player's LTC output (Blackmagic HyperDeck) and drives **DMX-512 lighting fixtures** in sync with the video. Includes a **live compose mode** for programming cues in real time while scrubbing the video.

Built for **Julian Charrière — Museo Correr, Venice 2026** by Victor Mazon Gardoqui

---

## Two Compositions

| Composition | Duration | Frame rate | Active cue file |
|---|---|---|---|
| **Albedo** | 43 min | **25 fps** | `src/cues_albedo.h` |
| **Controlled Burn** | 32 min | **30 fps** | `src/cues_control_burn.h` |

**Switching compositions** — change **one line** in `src/main.cpp`:

```cpp
// Albedo (25fps)
#include "cues_albedo.h"

// Controlled Burn (30fps)
#include "cues_control_burn.h"
```

Then rebuild and upload. All frame-rate dependent values (`LTC_SAMPLES_PER_BIT`, `DISPLAY_REFRESH_MS`, `DMX_LOOP_JUMP_FRAMES`) recalculate automatically at compile time.

---

## Hardware

| Component | Notes |
|-----------|-------|
| **Teensy 4.1** | IMXRT1062, 600 MHz, built-in SD slot |
| **Teensy Audio Shield** (SGTL5000) | LINE IN = left 3.5 mm jack (LTC input) |
| **MAX485 TTL→RS485 Module** | Auto-direction, 3.3V compatible, converts serial TX → DMX differential pair |
| **Blackmagic HyperDeck Studio HD Plus** | BNC LTC OUT → 3.5 mm LINE IN |
| **Varytec LED Theater Spot 50 3200K** × 4 | 50W COB, Fresnel lens, warm white. All 4 at **DMX address 001, 2-channel mode** |
| **SSD1306 OLED 128×64** | I2C address 0x3C — displays timecode, mode, OLED messages |

### Wiring — MAX485

```
Teensy 4.1          MAX485 Module        XLR 3-pin (DMX)
─────────────────────────────────────────────────────────
Pin 1  (TX1) ─────► DI
3.3 V  ───────────► VCC   DE (tie to VCC)
GND    ───────────► GND   RE (tie to GND)
                     A  ──────────────► XLR pin 3  (Data+)
                     B  ──────────────► XLR pin 2  (Data−)
                     GND ─────────────► XLR pin 1  (Shield/GND)
```

> If the fixture doesn't respond, try **swapping A and B** on the XLR side — polarity is the most common issue.

### Wiring — LTC Input

HyperDeck BNC LTC OUT → BNC-to-3.5 mm cable → **LINE IN** jack on Audio Shield (left channel). No termination resistor needed.

### Varytec Fixture Setup

Set each fixture via its front-panel menu:

1. **Mode → 2 CH → Enter** (2-channel DMX mode)
2. **Address → 001** (same on all 4 fixtures — they all respond identically)

---

## Fixture — DMX Channel Map

**Varytec LED Theater Spot 50 3200K — 2-channel mode**

| CH | Function | Range |
|----|----------|-------|
| 1 | **Master dimmer** | 0 = off, 255 = full |
| 2 | **Strobe** | 0 = off, 4 = slowest, 255 = fastest |

| Strobe value | Effect |
|---|---|
| 0 | Off |
| 4–10 | Barely visible, organic film flicker |
| 20–60 | Slow, deliberate pulses |
| 80–150 | Medium strobe |
| 200–255 | Fast strobe |

| Dimmer value | Brightness |
|---|---|
| 0 | Off |
| 64 | 25% |
| 128 | 50% |
| 192 | 75% |
| 255 | 100% |

---

## Software

### Build & Upload

[PlatformIO](https://platformio.org/) with Arduino framework.

```bash
# Build
pio run

# Upload
pio run --target upload

# Serial Monitor (115200 baud)
pio device monitor
```

### Project Structure

```
├── include/
│   ├── config.h              # All tunable parameters — FPS auto-set by cue file
│   └── display.h             # SSD1306 OLED driver
├── src/
│   ├── main.cpp              # Entry point, audio graph, serial command handler
│   ├── ltc_decoder.h/cpp     # SMPTE LTC biphase-mark decoder
│   ├── dmx_controller.h/cpp  # Cue engine with fade interpolation
│   ├── cues_albedo.h         # Albedo composition (43 min, 25 fps)
│   ├── cues_control_burn.h   # Controlled Burn composition (32 min, 30 fps)
│   └── cues.h                # Legacy fallback (unused)
├── test_cues_albedo.csv      # Short test sequence for Albedo (SD-card format)
├── test_cues_control_burn.csv# Short test sequence for Controlled Burn
├── scripts/
│   └── inject_version.py     # Git hash + build timestamp injected at compile time
└── platformio.ini
```

### Dependencies

| Library | Purpose |
|---------|---------|
| `ssilverman/TeensyDMX` 4.2.0 | DMX-512 output via hardware UART |
| Teensy Audio Library | I2S codec control, sample capture |
| Adafruit SSD1306 | OLED display driver |

### Firmware Fingerprint

Every build prints its git hash + build timestamp on boot:

```
Build: a3f9c12*  2026-04-27 14:30:00
```

The `*` means the build was made with uncommitted changes (dirty working tree).

---

## Quick Start

### 1. Power on

Open serial monitor at **115200 baud**. Boot banner:

```
-----------------------------------------
  Teensy 4.1  LTC + DMX  Live Compose
-----------------------------------------
  Build  : a3f9c12*  2026-04-27 14:30
  Codec  : OK
  FPS    : 25
  Cues   : 71  (built-in)
  Mode   : PLAY (LTC)
  TC print: OFF
-----------------------------------------
```

### 2. Test DMX hardware (no video needed)

Press **`t`** (no Enter) — fixtures cycle: ramp up → off → slow strobe → blackout. If nothing happens: check wiring, swap A/B on XLR, verify fixture address = 001 in 2-channel mode.

### 3. Play the video

Start HyperDeck. The Teensy decodes LTC and runs the composition automatically in PLAY mode.

---

## Cue File Guide

### `src/cues_albedo.h` / `src/cues_control_burn.h` — compiled-in

One row per cue:

```cpp
//  HH  MM  SS  FF   fadeMs    V(dimmer, strobe)
   { 0, 10, 30, 0,    400,     V(255, 0) },
```

| Field | Notes |
|---|---|
| `HH MM SS` | LTC start time (hours, minutes, seconds) |
| `FF` | Frame within the second (0–24 for Albedo 25fps, 0–29 for Controlled Burn 30fps). Use **0** unless you need sub-second precision — 1 frame = 40 ms at 25fps, 33 ms at 30fps |
| `fadeMs` | `0` = instant snap. Any positive value = fade duration in milliseconds |
| `V(d, s)` | `d` = dimmer 0–255, `s` = strobe 0–255 |

Rules:
- Cues **must be in chronological order**
- Fixture holds the last cue's state — explicitly fade to black at end
- After any edit → **rebuild + reflash**

### `test_cues_albedo.csv` / `test_cues_control_burn.csv` — SD card (runtime)

Loaded from SD card at boot as `CUES.CSV` — **overrides compiled cues, no recompile needed.**

```csv
# HH:MM:SS:FF,  fadeMs,  ch:val,  ch:val
00:10:30:00,      400,   1:255,   2:0
00:12:00:00,     3000,   1:220,   2:8
```

| Field | Notes |
|---|---|
| `HH:MM:SS:FF` | LTC trigger time |
| `fadeMs` | 0 = snap, >0 = fade in ms |
| `1:val` | CH1 = Master dimmer (0–255) |
| `2:val` | CH2 = Strobe (0–255) |
| `#` lines | Comments — ignored |

SD card workflow:
1. Edit the `.csv` in any text editor
2. Rename to **`CUES.CSV`**
3. Copy to the **SD card root**
4. Reboot Teensy — it loads automatically and boots into PLAY mode

---

## Serial Commands

### Instant keys (no Enter)

| Key | Action |
|-----|--------|
| `t` | DMX hardware test |
| `q` | Toggle TC timecode print on/off |

### Commands (type + Enter)

| Command | Action |
|---------|--------|
| `p` | Toggle LIVE ↔ PLAY mode |
| `f` | Full bright (dimmer=255, strobe=0) |
| `b` | Blackout |
| `i NNN` | Set dimmer to NNN (0–255) |
| `s NNN` | Set strobe to NNN (0–255) |
| `+` / `-` | Dimmer ±10 |
| `]` / `[` | Dimmer ±1 |
| `.` / `,` | Strobe ±10 |
| `m` | **Mark cue** at current LTC timecode (snap) |
| `m 2000` | Mark cue with 2000 ms fade |
| `u` | Undo last mark |
| `c` | Clear all marks |
| `l` | List all marks |
| `e` | **Export** marks as CUES.CSV text |
| `?` | Help |

### Live Compose Workflow

1. Play/scrub the video → adjust dimmer/strobe → press `m` to stamp a cue
2. Move to another timecode → adjust → `m 3000` (3 s fade)
3. `l` to review, `e` to export text → paste into a `.csv` file → SD card → reboot

---

## Two Modes

| Mode | How | What |
|---|---|---|
| **PLAY** (default on boot) | LTC timecode drives cues automatically | Cue engine fires at matching timecodes with fade interpolation |
| **LIVE** | Toggle with `p` | You control lights manually via serial — use for composing |

---

## OLED Display

The SSD1306 128×64 display at I2C 0x3C is divided into five zones (y positions are exact pixel offsets from the code):

```
┌────────────────────────────────┐
│  00:01:23:15                   │  ← y=0   timecode, size-2 (16 px tall)
│  ▶ PLAY  LTC:OK  ████████░░░  │  ← y=17  icon · mode · LTC status · level bar
├────────────────────────────────┤  ←       divider line at y=25
│ >CUE7  00:10:30:00             │  ← y=29  latest event message
│  C:7/69 N:00:12:00:00         │  ← y=38  cue position (PLAY mode only)
├────────────────────────────────┤  ←       divider line at y=46
│  DIM255 R0   G0   B0          │  ← y=48  DMX CH1–4 values
│  STR0   MOD0   SPD0           │  ← y=57  DMX CH5–7 values
└────────────────────────────────┘
```

### Timecode row (y=0)

**No signal** — full-width inverted white bar with `NO LTC IN` in large black text, centred.

**Signal present** — live timecode `HH:MM:SS:FF`. Drop-frame sources use a semicolon separator: `HH:MM:SS;FF`. Each digit is placed individually so all characters share the same baseline at size-2.

### Status row (y=17)

`▶ PLAY  LTC:OK  ████████░░░`

| Element | Position | Meaning |
|---|---|---|
| **▶** (triangle) | x=0 | Blinks on every decoded frame when LTC is locked |
| **■** (square) | x=0 | Solid when LTC signal is absent |
| **PLAY** / **LIVE** | x=10 | Current operating mode |
| **LTC:OK** / **LTC:--** | x=40 | Real-time lock status |
| Level bar | x=80, 46 px wide | LINE IN peak amplitude — fills left-to-right proportionally |

### Event log (y=29)

One line prefixed with `>`. Shows the most recent event pushed to the log:

| Message | Trigger |
|---|---|
| `LTC acquired` | First valid frame decoded after silence |
| `LTC LOST` | No LTC for `LTC_LOSS_TIMEOUT_MS` (default 1 s) |
| `CUE7  00:10:30:00` | Cue index 7 fired at that timecode |
| `skip +N @ HH:MM:SS:FF` | Frame-skip warning (N frames missed) |
| `TC jump HH:MM:SS:FF` | Timecode non-sequential jump detected |
| `>> PLAY MODE` / `>> LIVE MODE` | Mode toggled via `p` key |

### Cue row (y=38) — PLAY mode only

Hidden in LIVE mode. Three states:

| Display | Meaning |
|---|---|
| `C:--/69 N:00:10:30:00` | Before first cue; shows total count and next cue TC |
| `C:7/69 N:00:12:00:00` | Cue 7 of 69 active; next cue triggers at `00:12:00:00` |
| `C:69/69  END OF SHOW` | Last cue has fired |

### DMX rows (y=48 and y=57)

Raw values sent to the DMX universe, updated every frame:

| Row | Format | Channels |
|---|---|---|
| Top | `DIM255 R0   G0   B0  ` | CH1 master dimmer, CH2 red, CH3 green, CH4 blue |
| Bottom | `STR0   MOD0   SPD0   ` | CH5 strobe, CH6 mode, CH7 speed |

For the **Varytec 2-channel fixture** only `DIM` (CH1) and `STR` (CH2) are ever non-zero — `R/G/B/MOD/SPD` will always read `0`.

> Display is disabled automatically if the audio codec fails to initialise.

---

## Configuration — `include/config.h`

| Parameter | Notes |
|---|---|
| `LTC_FRAMERATE` | Set automatically by the active cue header — **do not edit here** |
| `LTC_SAMPLES_PER_BIT` | Derived from `LTC_FRAMERATE` — auto |
| `DISPLAY_REFRESH_MS` | `1000 / LTC_FRAMERATE` — auto |
| `DMX_LOOP_JUMP_FRAMES` | `LTC_FRAMERATE × 5` — auto (125 @ 25fps, 150 @ 30fps) |
| `DMX_BASE_ADDR` | `1` — fixture DMX start address |
| `DMX_UNIVERSE_SIZE` | `2` — 2 channels (dimmer + strobe) |
| `AUDIO_LINEIN_LEVEL` | `0` = max sensitivity. Raise to 5 if signal clips |
| `LTC_ZC_THRESHOLD` | `0.001` — zero-crossing sensitivity |
| `LTC_LOSS_TIMEOUT_MS` | `1000` — ms silence before LTC LOST warning |
| `HEARTBEAT_INTERVAL_MS` | `5000` — interval for `[STAT]` serial line |

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No timecode in serial | Uncomment `DEBUG_AUDIO_LEVEL` in config.h to verify signal |
| Garbled TC / `rej:` counter climbing | Uncomment `DEBUG_LTC_BITS` to see raw bit stream |
| Signal too hot (clipping) | Raise `AUDIO_LINEIN_LEVEL` from `0` to `5` in config.h |
| Fixture doesn't respond | Run `t` test; check wiring; try swapping A/B on XLR |
| Fixture in wrong mode | Verify: Menu → 2 CH, Address = 001 on every fixture |
| Regular frame-skip warnings | Check `LTC_FRAMERATE` matches the actual video source |
| TC print floods serial | Press `q` to mute before typing commands |
| Build mismatch at venue | Check boot banner `Build:` line — git hash + dirty flag |

---

## Known Hardware Issues & Workarounds

### BCD tens-digit flip at 30 fps

**Symptom** — With a 30 fps LTC source, `[!!] Frame skip: +9` and `[!!] TC jump` warnings appear once per second even on a clean, sequential tape. The serial monitor shows only frames `:09`, `:19`, `:29` passing the filter; all intermediate frames are eaten.

**Root cause** — At 30 fps the biphase decoder's sample window is marginally tighter than at 25 fps. For frames where the units digit is `9` (frames `:09`, `:19`, `:29`), timing noise occasionally causes the decoder to increment the BCD *tens* digit by 1. Three flavours:

| Case | Correct frame | Decoded as | Raw diff |
|---|---|---|---|
| A | `HH:MM:SS:09` (tens=0) | `HH:MM:SS:19` (tens=1) | +10 |
| B | `HH:MM:SS:19` (tens=1) | `HH:MM:SS:29` (tens=2) | +10 |
| C | `HH:MM:SS:29` (tens=2) | `HH:MM:SS:39` → BCD rollover → **`HH:MM:(SS+1):09`** | +11 (`tc.frames = 9`) |

Case C is the most damaging: the corrupted frame carries into the next second. The corrected frame appears as `(SS+1):09`. All frames `(SS+1):00–:08` that follow have a *negative* diff relative to the jumped position and are discarded by the backward glitch filter — so only one frame per second survives until the cycle repeats.

**Fix (in `src/main.cpp`)** — When `diff` lands in `[9, 12]`, subtract 10 from the absolute frame counter and recompute *all* TC fields (HH, MM, SS, FF) from the corrected value using integer division. Accept the corrected frame only if `correctedDiff == 1` (single-frame advance); discard otherwise. Removing the earlier `tc.frames >= 10` guard is essential — Case C produces `tc.frames = 9`, which would bypass a tens-only check.

```cpp
if (diff >= 9 && diff <= 12) {
    uint32_t correctedAbs  = curAbs - 10;
    int32_t  correctedDiff = (int32_t)correctedAbs - (int32_t)prevTCFrames;
    if (correctedDiff == 1) {
        uint32_t totalSec  = correctedAbs / LTC_FRAMERATE;
        tc.frames  = (uint8_t)(correctedAbs % LTC_FRAMERATE);
        tc.seconds = (uint8_t)(totalSec % 60);
        tc.minutes = (uint8_t)((totalSec / 60) % 60);
        tc.hours   = (uint8_t)((totalSec / 3600) % 24);
        curAbs     = correctedAbs;
        diff       = 1;
    } else {
        ltcDecoder.reset();
        return;
    }
}
```

**If this bug recurs** — the symptom is always `[!!] Frame skip: +9` precisely once per second, and `[!!] TC jump` that lands on `...:09`. Check that the correction block is not guarded by `tc.frames >= 10`.

---

## License

MIT

