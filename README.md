# LTC2DMX â€” Teensy 4.1 + Audio Shield

Reads **SMPTE LTC timecode** from a video player's LTC output (Blackmagic HyperDeck) and drives **DMX-512 lighting fixtures** in sync with the video. Includes a **live compose mode** for programming cues in real time while scrubbing the video.

Built for **Julian CharriÃ¨re â€” Museo Correr, Venice 2026** by Victor Mazon Gardoqui

---

## Two Compositions

| Composition | Duration | Frame rate | Active cue file |
|---|---|---|---|
| **Albedo** | 43 min | **25 fps** | `src/cues_albedo.h` |
| **Controlled Burn** | 32 min | **30 fps** | `src/cues_control_burn.h` |

**Switching compositions** â€” change **one line** in `src/main.cpp`:

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
| **MAX485 TTLâ†’RS485 Module** | Auto-direction, 3.3V compatible, converts serial TX â†’ DMX differential pair |
| **Blackmagic HyperDeck Studio HD Plus** | BNC LTC OUT â†’ 3.5 mm LINE IN |
| **Varytec LED Theater Spot 50 3200K** Ã— 4 | 50W COB, Fresnel lens, warm white. All 4 at **DMX address 001, 2-channel mode** |
| **SSD1306 OLED 128Ã—64** | I2C address 0x3C â€” displays timecode, mode, OLED messages |

### Wiring â€” MAX485

```
Teensy 4.1          MAX485 Module        XLR 3-pin (DMX)
â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
Pin 1  (TX1) â”€â”€â”€â”€â”€â–º DI
3.3 V  â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â–º VCC   DE (tie to VCC)
GND    â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â–º GND   RE (tie to GND)
                     A  â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â–º XLR pin 3  (Data+)
                     B  â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â–º XLR pin 2  (Dataâˆ’)
                     GND â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â–º XLR pin 1  (Shield/GND)
```

> If the fixture doesn't respond, try **swapping A and B** on the XLR side â€” polarity is the most common issue.

### Wiring â€” LTC Input

HyperDeck BNC LTC OUT â†’ BNC-to-3.5 mm cable â†’ **LINE IN** jack on Audio Shield (left channel). No termination resistor needed.

### Varytec Fixture Setup

Set each fixture via its front-panel menu:

1. **Mode â†’ 2 CH â†’ Enter** (2-channel DMX mode)
2. **Address â†’ 001** (same on all 4 fixtures â€” they all respond identically)

---

## Fixture â€” DMX Channel Map

**Varytec LED Theater Spot 50 3200K â€” 2-channel mode**

| CH | Function | Range |
|----|----------|-------|
| 1 | **Master dimmer** | 0 = off, 255 = full |
| 2 | **Strobe** | 0 = off, 4 = slowest, 255 = fastest |

| Strobe value | Effect |
|---|---|
| 0 | Off |
| 4â€“10 | Barely visible, organic film flicker |
| 20â€“60 | Slow, deliberate pulses |
| 80â€“150 | Medium strobe |
| 200â€“255 | Fast strobe |

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
â”œâ”€â”€ include/
â”‚   â”œâ”€â”€ config.h              # All tunable parameters â€” FPS auto-set by cue file
â”‚   â””â”€â”€ display.h             # SSD1306 OLED driver
â”œâ”€â”€ src/
â”‚   â”œâ”€â”€ main.cpp              # Entry point, audio graph, serial command handler
â”‚   â”œâ”€â”€ ltc_decoder.h/cpp     # SMPTE LTC biphase-mark decoder
â”‚   â”œâ”€â”€ dmx_controller.h/cpp  # Cue engine with fade interpolation
â”‚   â”œâ”€â”€ cues_albedo.h         # Albedo composition (43 min, 25 fps)
â”‚   â”œâ”€â”€ cues_control_burn.h   # Controlled Burn composition (32 min, 30 fps)
â”‚   â””â”€â”€ cues.h                # Legacy fallback (unused)
â”œâ”€â”€ test_cues_albedo.csv      # Short test sequence for Albedo (SD-card format)
â”œâ”€â”€ test_cues_control_burn.csv# Short test sequence for Controlled Burn
â”œâ”€â”€ scripts/
â”‚   â””â”€â”€ inject_version.py     # Git hash + build timestamp injected at compile time
â””â”€â”€ platformio.ini
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
  Mode   : LIVE (compose)
  TC print: OFF
-----------------------------------------
```

### 2. Test DMX hardware (no video needed)

Press **`t`** (no Enter) â€” fixtures cycle: ramp up â†’ off â†’ slow strobe â†’ blackout. If nothing happens: check wiring, swap A/B on XLR, verify fixture address = 001 in 2-channel mode.

### 3. Play the video

Start HyperDeck. The Teensy decodes LTC and runs the composition automatically in PLAY mode.

---

## Cue File Guide

### `src/cues_albedo.h` / `src/cues_control_burn.h` â€” compiled-in

One row per cue:

```cpp
//  HH  MM  SS  FF   fadeMs    V(dimmer, strobe)
   { 0, 10, 30, 0,    400,     V(255, 0) },
```

| Field | Notes |
|---|---|
| `HH MM SS` | LTC start time (hours, minutes, seconds) |
| `FF` | Frame within the second (0â€“24 for Albedo 25fps, 0â€“29 for Controlled Burn 30fps). Use **0** unless you need sub-second precision â€” 1 frame = 40 ms at 25fps, 33 ms at 30fps |
| `fadeMs` | `0` = instant snap. Any positive value = fade duration in milliseconds |
| `V(d, s)` | `d` = dimmer 0â€“255, `s` = strobe 0â€“255 |

Rules:
- Cues **must be in chronological order**
- Fixture holds the last cue's state â€” explicitly fade to black at end
- After any edit â†’ **rebuild + reflash**

### `test_cues_albedo.csv` / `test_cues_control_burn.csv` â€” SD card (runtime)

Loaded from SD card at boot as `CUES.CSV` â€” **overrides compiled cues, no recompile needed.**

```csv
# HH:MM:SS:FF,  fadeMs,  ch:val,  ch:val
00:10:30:00,      400,   1:255,   2:0
00:12:00:00,     3000,   1:220,   2:8
```

| Field | Notes |
|---|---|
| `HH:MM:SS:FF` | LTC trigger time |
| `fadeMs` | 0 = snap, >0 = fade in ms |
| `1:val` | CH1 = Master dimmer (0â€“255) |
| `2:val` | CH2 = Strobe (0â€“255) |
| `#` lines | Comments â€” ignored |

SD card workflow:
1. Edit the `.csv` in any text editor
2. Rename to **`CUES.CSV`**
3. Copy to the **SD card root**
4. Reboot Teensy â€” it loads automatically and boots into PLAY mode

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
| `p` | Toggle LIVE â†” PLAY mode |
| `f` | Full bright (dimmer=255, strobe=0) |
| `b` | Blackout |
| `i NNN` | Set dimmer to NNN (0â€“255) |
| `s NNN` | Set strobe to NNN (0â€“255) |
| `+` / `-` | Dimmer Â±10 |
| `]` / `[` | Dimmer Â±1 |
| `.` / `,` | Strobe Â±10 |
| `m` | **Mark cue** at current LTC timecode (snap) |
| `m 2000` | Mark cue with 2000 ms fade |
| `u` | Undo last mark |
| `c` | Clear all marks |
| `l` | List all marks |
| `e` | **Export** marks as CUES.CSV text |
| `?` | Help |

### Live Compose Workflow

1. Play/scrub the video â†’ adjust dimmer/strobe â†’ press `m` to stamp a cue
2. Move to another timecode â†’ adjust â†’ `m 3000` (3 s fade)
3. `l` to review, `e` to export text â†’ paste into a `.csv` file â†’ SD card â†’ reboot

---

## Two Modes

| Mode | How | What |
|---|---|---|
| **LIVE** (default on boot) | You control manually via serial | Adjust lights in real time, compose cues |
| **PLAY** | Triggered by `p` or auto at boot if `CUES.CSV` found on SD | LTC drives cues automatically with fade interpolation |

---

## Configuration â€” `include/config.h`

| Parameter | Notes |
|---|---|
| `LTC_FRAMERATE` | Set automatically by the active cue header â€” **do not edit here** |
| `LTC_SAMPLES_PER_BIT` | Derived from `LTC_FRAMERATE` â€” auto |
| `DISPLAY_REFRESH_MS` | `1000 / LTC_FRAMERATE` â€” auto |
| `DMX_LOOP_JUMP_FRAMES` | `LTC_FRAMERATE Ã— 5` â€” auto (125 @ 25fps, 150 @ 30fps) |
| `DMX_BASE_ADDR` | `1` â€” fixture DMX start address |
| `DMX_UNIVERSE_SIZE` | `2` â€” 2 channels (dimmer + strobe) |
| `AUDIO_LINEIN_LEVEL` | `0` = max sensitivity. Raise to 5 if signal clips |
| `LTC_ZC_THRESHOLD` | `0.001` â€” zero-crossing sensitivity |
| `LTC_LOSS_TIMEOUT_MS` | `1000` â€” ms silence before LTC LOST warning |
| `HEARTBEAT_INTERVAL_MS` | `5000` â€” interval for `[STAT]` serial line |

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No timecode in serial | Uncomment `DEBUG_AUDIO_LEVEL` in config.h to verify signal |
| Garbled TC / `rej:` counter climbing | Uncomment `DEBUG_LTC_BITS` to see raw bit stream |
| Signal too hot (clipping) | Raise `AUDIO_LINEIN_LEVEL` from `0` to `5` in config.h |
| Fixture doesn't respond | Run `t` test; check wiring; try swapping A/B on XLR |
| Fixture in wrong mode | Verify: Menu â†’ 2 CH, Address = 001 on every fixture |
| Regular frame-skip warnings | Check `LTC_FRAMERATE` matches the actual video source |
| TC print floods serial | Press `q` to mute before typing commands |
| Build mismatch at venue | Check boot banner `Build:` line â€” git hash + dirty flag |

---

## Known Hardware Issues & Workarounds

### BCD tens-digit flip at 30 fps

**Symptom** â€” With a 30 fps LTC source, `[!!] Frame skip: +9` and `[!!] TC jump` warnings appear once per second even on a clean, sequential tape. The serial monitor shows only frames `:09`, `:19`, `:29` passing the filter; all intermediate frames are eaten.

**Root cause** â€” At 30 fps the biphase decoder's sample window is marginally tighter than at 25 fps. For frames where the units digit is `9` (frames `:09`, `:19`, `:29`), timing noise occasionally causes the decoder to increment the BCD *tens* digit by 1. Three flavours:

| Case | Correct frame | Decoded as | Raw diff |
|---|---|---|---|
| A | `HH:MM:SS:09` (tens=0) | `HH:MM:SS:19` (tens=1) | +10 |
| B | `HH:MM:SS:19` (tens=1) | `HH:MM:SS:29` (tens=2) | +10 |
| C | `HH:MM:SS:29` (tens=2) | `HH:MM:SS:39` â†’ BCD rollover â†’ **`HH:MM:(SS+1):09`** | +11 (`tc.frames = 9`) |

Case C is the most damaging: the corrupted frame carries into the next second. The corrected frame appears as `(SS+1):09`. All frames `(SS+1):00â€“:08` that follow have a *negative* diff relative to the jumped position and are discarded by the backward glitch filter â€” so only one frame per second survives until the cycle repeats.

**Fix (in `src/main.cpp`)** â€” When `diff` lands in `[9, 12]`, subtract 10 from the absolute frame counter and recompute *all* TC fields (HH, MM, SS, FF) from the corrected value using integer division. Accept the corrected frame only if `correctedDiff == 1` (single-frame advance); discard otherwise. Removing the earlier `tc.frames >= 10` guard is essential â€” Case C produces `tc.frames = 9`, which would bypass a tens-only check.

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

**If this bug recurs** â€” the symptom is always `[!!] Frame skip: +9` precisely once per second, and `[!!] TC jump` that lands on `...:09`. Check that the correction block is not guarded by `tc.frames >= 10`.

---

## License

MIT

