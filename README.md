# LTC2DMX — Teensy 4.1 + Audio Shield

Reads **SMPTE LTC timecode** from a video player's LTC output (e.g. Blackmagic HyperDeck) and drives **DMX-512 lighting fixtures** in sync with the video. Includes a **live compose mode** for programming cues in real time while scrubbing the video.

Built for the art installation by **Julian Charrière** at the Museo Correr, Venice Biennale 2026.

---

## Hardware

| Component | Notes |
|-----------|-------|
| **Teensy 4.1** | IMXRT1062, 600 MHz, built-in SD slot |
| **Teensy Audio Shield** (SGTL5000) | LINE IN = left 3.5 mm jack (LTC input) |
| **MAX485** RS-485 transceiver | Converts Teensy serial → DMX differential pair |
| **Blackmagic HyperDeck Studio HD Plus** | BNC LTC OUT → 3.5 mm LINE IN (no termination) |
| **4× Varytec LED Theater Spot 50 3200K** | 2-channel DMX: CH1 = intensity, CH2 = strobe. All fixtures on same address. |

### Wiring — MAX485

```
Teensy 4.1          MAX485              XLR 5-pin (DMX)
──────────────────────────────────────────────────────
Pin 1  (TX1) ─────► DI   (pin 4)
3.3 V  ───────────── VCC  (pin 8)
3.3 V  ───────────── DE   (pin 3)   ← always drive
GND    ───────────── GND  (pin 5)
GND    ───────────── RE   (pin 2)   ← disable receiver
                     A    (pin 6)  ──► XLR pin 3  (DMX+)
                     B    (pin 7)  ──► XLR pin 2  (DMX−)
GND    ──────────────────────────────► XLR pin 1  (shield)
```

### Wiring — LTC Input

BNC LTC OUT from HyperDeck → BNC-to-3.5 mm cable → **LINE IN** jack on Audio Shield (left channel). No termination resistor needed (direct connection).

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
│   └── cues.h              # Built-in fallback cue list
├── platformio.ini
└── README.md
```

### Dependencies

| Library | Purpose |
|---------|---------|
| `ssilverman/TeensyDMX` | DMX-512 output via hardware UART + MAX485 |
| Teensy Audio Library | I2S codec control, sample capture |

---

## Two Modes

### LIVE mode (compose)

Default on first boot (no SD card). You control the lights in real time from the **serial monitor** while the video plays on the HyperDeck.

| Command | Action |
|---------|--------|
| `+` / `-` | Intensity ±10 |
| `]` / `[` | Intensity ±1 |
| `.` / `,` | Strobe ±10 |
| `i NNN` | Set intensity (0–255) |
| `s NNN` | Set strobe (0–255) |
| `f` | Full (I=255, S=0) |
| `b` | Blackout |
| `m` | **Mark cue** at current timecode (snap) |
| `m 2000` | Mark cue with 2000 ms fade |
| `u` | Undo last mark |
| `c` | Clear all marks |
| `l` | List all marks |
| `e` | **Export** marks as `CUES.CSV` text |
| `p` | Toggle LIVE ↔ PLAY mode |
| `?` | Help |

**Workflow:** play/scrub the video → adjust lights → type `m` to stamp the cue at the current timecode → repeat → type `e` to export → copy the output into a `CUES.CSV` file on a micro-SD card.

### PLAY mode (automatic)

When a `/CUES.CSV` file is found on the Teensy's built-in SD card, the system boots in PLAY mode. LTC timecode from the video triggers cues automatically with fade interpolation. Loop detection resets the cue engine when the video restarts.

Type `p` in the serial monitor to toggle back to LIVE mode for adjustments.

---

## CUES.CSV Format

Plain text, one cue per line. Comments start with `#`.

```csv
# HH:MM:SS:FF, fadeMs, ch:val, ch:val, ...
00:00:00:00, 0, 1:0, 2:0
00:00:10:00, 3000, 1:128, 2:0
00:15:00:00, 5000, 1:255, 2:0
00:31:50:00, 9000, 1:0, 2:0
```

- **fadeMs = 0** → instant snap
- **fadeMs > 0** → linear fade over that duration
- **ch:val** → DMX channel : value (0–255)
- Cues must be in ascending timecode order

---

## Configuration

All parameters are in [`include/config.h`](include/config.h):

- **LTC_FRAMERATE** — 24, 25, or 30
- **DMX_BASE_ADDR** — DMX start address for fixtures
- **DMX_CH_INTENSITY / DMX_CH_STROBE** — channel mapping
- **AUDIO_LINEIN_LEVEL** — input sensitivity (0 = max)
- **DMX_LOOP_JUMP_FRAMES** — frames threshold for loop detection

---

## License

MIT
