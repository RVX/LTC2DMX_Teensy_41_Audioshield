# LTC2DMX — Teensy 4.1 + Audio Shield

Reads **SMPTE LTC timecode** from a video player's LTC output (e.g. Blackmagic HyperDeck) and drives **DMX-512 lighting fixtures** in sync with the video. Includes a **live compose mode** for programming cues in real time while scrubbing the video.

Built for Julian Charrière — Museo Correr, Venice 2026 by Victor Mazon Gardoqui

---

## Hardware

| Component | Notes |
|-----------|-------|
| **Teensy 4.1** *(recommended)* | IMXRT1062, 600 MHz, built-in SD slot |
| **Teensy 3.6** *(supported)* | MK66FX1M0, 180 MHz, built-in SD slot — see [Using a Teensy 3.6](#using-a-teensy-36) |
| **Teensy Audio Shield** (SGTL5000) | LINE IN = left 3.5 mm jack (LTC input) |
| **JZK TTL→RS485 Module** | Auto-direction, 3.3V compatible, converts serial → DMX differential pair |
| **Blackmagic HyperDeck Studio HD Plus** | BNC LTC OUT → 3.5 mm LINE IN (no termination) |
| **RGB PAR LED Light** | 7-channel DMX: CH1=Master, CH2=R, CH3=G, CH4=B, CH5=Strobe, CH6=Mode, CH7=Speed |

### Wiring — JZK TTL→RS485 Module

```
Teensy 4.1          JZK RS485 Module     XLR 3-pin (DMX)
───────────────────────────────────────────────────────────
Pin 1  (TX1) ─────► TXD
3.3 V  ───────────► VCC
GND    ───────────► GND
                     A  ────────────────► XLR pin 3  (Data+)
                     B  ────────────────► XLR pin 2  (Data−)
                     GND ───────────────► XLR pin 1  (Shield/GND)
```

> **Note:** The JZK module has automatic hardware flow control — no DE/RE pins needed. If the light doesn't respond, try **swapping A and B** on the XLR side (polarity is the most common issue).

### Wiring — LTC Input

BNC LTC OUT from HyperDeck → BNC-to-3.5 mm cable → **LINE IN** jack on Audio Shield (left channel). No termination resistor needed (direct connection).

### Light Settings

- Set DMX address to **d001** (7-channel mode)
- Channel map: Master | Red | Green | Blue | Strobe | Mode | Speed

---

## Software

### Build

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
│   └── config.h            # All tunable parameters (audio, LTC, DMX, debug)
├── src/
│   ├── main.cpp            # Entry point, audio graph, serial command handler
│   ├── ltc_decoder.h/cpp   # SMPTE LTC biphase-mark decoder
│   ├── dmx_controller.h/cpp# Cue engine with fade interpolation
│   └── cues.h              # Built-in 71-cue list for 32-min movie
├── platformio.ini
└── README.md
```

### Dependencies

| Library | Purpose |
|---------|---------|
| `ssilverman/TeensyDMX` | DMX-512 output via hardware UART |
| Teensy Audio Library | I2S codec control, sample capture |

---

## Quick Start

### 1. Power on

Plug in the Teensy via USB. Open serial monitor (115200 baud). You'll see:

```
-----------------------------------------
  Teensy 4.1  LTC + DMX  Live Compose
-----------------------------------------
  Codec  : OK
  FPS    : 30
  Cues   : 71  (built-in fallback)
  Mode   : LIVE (compose)
  TC print: OFF (press q to toggle)
  Press t (no Enter) to run DMX test
-----------------------------------------
```

### 2. Test DMX hardware (no video needed)

Press **`t`** (no Enter needed) — the light cycles through:
1. White ramp 0→255
2. RED only
3. GREEN only
4. BLUE only
5. WHITE + strobe
6. Blackout

If nothing happens: check wiring, swap A/B, verify light is on d001.

### 3. Switch to PLAY mode

Type **`p`** + Enter → switches to **PLAY mode** where LTC timecode drives the 71 built-in cues automatically.

### 4. Play the movie

Start the video with LTC audio. The Teensy decodes timecode and triggers cues in sync:

| Time | What happens |
|------|-------------|
| 0:00 – 0:30 | Black → slow warm white fade-in (3000K) |
| 0:30 – 3:00 | Hold steady, gentle settle |
| 3:00 – 6:00 | Slow breathing cycles |
| 6:00 – 10:00 | Build-up with snaps and pulses |
| 10:00 – 13:00 | Dramatic flash → near-black → flash back |
| 13:00 – 15:00 | Subtle strobe builds, then off |
| 15:00 – 19:00 | Calm warm glow, slow breathing |
| 19:00 – 23:00 | Deeper breathing (40↔255) |
| 23:00 – 27:00 | Climax: full bright, sharp drops, brief strobe |
| 27:00 – 29:30 | Rapid flicker sequence |
| 30:00 – 31:59 | Long cascading fade to full blackout |

Loop detection resets the cue engine when the video restarts.

---

## Two Modes

### LIVE mode (compose)

Default on boot. You control the lights manually from the serial monitor while the video plays.

### PLAY mode (automatic)

LTC timecode triggers cues automatically with fade interpolation. Toggle with `p` + Enter.

If a `/CUES.CSV` file is found on the Teensy's built-in SD card, it loads those cues and boots directly into PLAY mode.

---

## Serial Commands

### Instant keys (no Enter needed)

| Key | Action |
|-----|--------|
| `t` | Run DMX hardware test (cycles R/G/B/strobe) |
| `q` | Toggle TC timecode printing on/off |

### Commands (type + Enter)

| Command | Action |
|---------|--------|
| `+` / `-` | Intensity ±10 |
| `]` / `[` | Intensity ±1 |
| `.` / `,` | Strobe ±10 |
| `i NNN` | Set intensity (0–255) |
| `s NNN` | Set strobe (0–255) |
| `f` | Full white (I=255, S=0) |
| `b` | Blackout |
| `m` | **Mark cue** at current timecode (snap) |
| `m 2000` | Mark cue with 2000 ms fade |
| `u` | Undo last mark |
| `c` | Clear all marks |
| `l` | List all marks |
| `e` | **Export** marks as `CUES.CSV` text |
| `p` | Toggle LIVE ↔ PLAY mode |
| `?` | Help |

### Compose Workflow

1. Play/scrub the video → adjust lights → type `m` to stamp a cue
2. Scrub to another point → adjust → `m 3000` (with 3s fade)
3. Type `l` → list marks, `e` → export as CUES.CSV
4. Copy exported text to `/CUES.CSV` on micro-SD card
5. Reboot → system loads SD cues and enters PLAY mode automatically

---

## CUES.CSV Format

Plain text, one cue per line. Comments start with `#`.

```csv
# HH:MM:SS:FF, fadeMs, ch:val, ch:val, ...
00:00:00:00, 0, 1:255, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0
00:00:03:00, 8000, 1:60, 2:255, 3:180, 4:80, 5:0, 6:0, 7:0
00:31:50:00, 0, 1:0, 2:0, 3:0, 4:0, 5:0, 6:0, 7:0
```

- **fadeMs = 0** → instant snap
- **fadeMs > 0** → linear fade over that duration
- **ch:val** → DMX channel : value (0–255)
- Cues must be in ascending timecode order
- 7-channel fixture: 1=Master, 2=Red, 3=Green, 4=Blue, 5=Strobe, 6=Mode, 7=Speed

---

## DMX Channel Map (RGB PAR Light)

| Channel | Function | Range |
|---------|----------|-------|
| 1 | Master dimmer | 0–255 (0=off, 255=full) |
| 2 | Red | 0–255 |
| 3 | Green | 0–255 |
| 4 | Blue | 0–255 |
| 5 | Strobe | 0–3=off, 4–255=slow→fast |
| 6 | Mode | 0–49=manual RGB, 50+=effects |
| 7 | Effect speed | 0–255 (only with modes 50+) |

**Warm white 3000K** ≈ R:255, G:180, B:80

---

## Configuration

All parameters are in [`include/config.h`](include/config.h):

| Parameter | Default | Description |
|-----------|---------|-------------|
| `LTC_FRAMERATE` | 30 | 24, 25, or 30 fps |
| `DMX_BASE_ADDR` | 1 | DMX start address |
| `DMX_UNIVERSE_SIZE` | 7 | Number of DMX channels |
| `AUDIO_LINEIN_LEVEL` | 0 | Input sensitivity (0=max, 15=min) |
| `DMX_LOOP_JUMP_FRAMES` | 150 | Frames threshold for loop detection |
| `LTC_ZC_THRESHOLD` | 0.001 | Zero-crossing threshold |

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No timecode in serial | Uncomment `#define DEBUG_AUDIO_LEVEL` in config.h to verify audio signal |
| Garbled TC bits | Uncomment `#define DEBUG_LTC_BITS` to see raw bit stream |
| Signal too hot (clipping) | Change `AUDIO_LINEIN_LEVEL` from `0` to `5` in config.h |
| Light doesn't respond | Run `t` test; check wiring; try swapping A/B on XLR |
| Wrong colors | Verify light is in 7-channel mode at address d001 |
| TC spam floods serial | Press `q` to mute (muted by default) |
| Can't type commands | Press `q` first to stop TC printing, then type commands |

---

## Using a Teensy 3.6

The project runs on a Teensy 3.6 without any code changes. A dedicated build environment is already configured in `platformio.ini`.

### Build and upload for Teensy 3.6

```bash
# Build
pio run -e teensy36

# Upload
pio run -e teensy36 --target upload

# Serial monitor (same baud rate)
pio device monitor
```

In VS Code / PlatformIO IDE: select the **`teensy36`** environment in the bottom status bar before clicking Build / Upload.

### What works identically

- SMPTE LTC decoding (same audio shield, same I2S pinout)
- DMX output on Serial1 pin 1 TX (identical to 4.1)
- Built-in SD card slot (`BUILTIN_SDCARD`)
- OLED display on I2C (SDA/SCL same pins)
- All serial commands, cue engine, fade interpolation
- Hardware FPU — Cortex-M4F, so all float math in the LTC decoder runs in hardware

### Known differences vs Teensy 4.1

| | Teensy 4.1 | Teensy 3.6 |
|---|---|---|
| CPU speed | 600 MHz | 180 MHz |
| I2C max speed | 1 MHz (Fast-mode+) | 400 kHz (Fast-mode) |
| Production status | Current | Discontinued |

**I2C speed** is the only thing that requires a code adaptation. The `TEENSY36` build flag (set automatically by `platformio.ini`) makes `display.h` call `Wire.setClock(400000)` instead of `Wire.setClock(1000000)`. Display refresh rate is identical — the I2C bandwidth at 400 kHz is still ~40× more than the display needs at 30 fps.

**CPU speed** is not a bottleneck. The LTC decoder processes one audio block (~3 ms of audio) per iteration; at 180 MHz there are still thousands of spare cycles per sample.

### No changes needed in config.h

All parameters in `include/config.h` are board-agnostic. `LTC_SAMPLES_PER_BIT` uses `AUDIO_SAMPLE_RATE_EXACT` which is the same (`44117.647 Hz`) on both boards since it comes from the audio shield clock.

---

## License

MIT
