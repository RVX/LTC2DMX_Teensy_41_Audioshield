# LTC2DMX — Teensy 4.1 + Audio Shield

Reads **SMPTE LTC timecode** (Blackmagic HyperDeck) and drives **DMX-512 fixtures** in sync with video.  
Built for **Julian Charrière — Museo Correr, Venice 2026** by Victor Mazon Gardoqui.

---

## 🟢 Known-good state

**Tag:** `good-state-2026-04-28` · commit `6a56486`  
Roll back any time with: `git checkout good-state-2026-04-28`

| Channel | Range | Mean | Notes |
|---|---|---|---|
| ch2 backlight | 1–72 | 25.2 | bias 8 + blackouts (10:00–10:13, 14:02–14:18) + back-thunder (12:50, 15:37, 16:40) + ch7 sync |
| ch5 corridor  | 40–149 | 61.7 | `SABER2_BIAS=44` |
| ch7 saber WW  | 2–255 | 102.7 | exports `ch7_strobe_times.json` |

---

## Compositions

| Composition | Duration | FPS | PlatformIO env | Main cue file |
|---|---|---|---|---|
| **Albedo** | 43 min | 25 | `albedo` | `src/cues_albedo.h` |
| **Controlled Burn** | 32 min | 30 | `controlled_burn` | `src/cues_control_burn.h` |

Switch by changing `default_envs` in `platformio.ini`. All frame-rate constants recalculate at compile time.

---

## Hardware

| Component | Notes |
|-----------|-------|
| **Teensy 4.1** | 600 MHz, IMXRT1062 |
| **Teensy Audio Shield** (SGTL5000) | LINE IN left = LTC (HyperDeck BNC→3.5 mm) |
| **MAX485 module** | TX1 (pin 1) → DI; DE+RE tied to VCC/GND; A→XLR-3, B→XLR-2 |
| **SSD1306 OLED 128×64** | I2C 0x3C |

> Fixture doesn't respond? Try **swapping A/B** on XLR — polarity is the most common issue.

---

## DMX Universe — Controlled Burn (channels 1–7)

| CH | Fixture | Address | Mode | Behaviour |
|----|---------|---------|------|-----------|
| 2 | Varytec LED Theater Spot 50 3200K × 4 | 002 | 3-ch (Master/Fine/Strobe) | Inverse-luma backlight; erratic breath/wobble/chaos; injected DIPs + flickers |
| 5 | ADJ Saber Spot WW (Unit 2) | 005 | 1-ch dimmer | Corridor — slightly inversed to ch2 (dim when ch2 lifts) |
| 7 | ADJ Saber Spot WW (Unit 1) | 007 | 1-ch dimmer | p99 envelope follower + chaotic-fireworks strobe zones |

`DMX_UNIVERSE_SIZE=7` covers all channels. `platformio.ini` build flags: `DMX_BASE_ADDR=2`, `DMX_NUM_CH=3`.

### Varytec fixture setup
Front panel: **Mode → 3 CH → Enter**, **Address → 002**.

### ADJ Saber Spot WW setup
Front panel: **1 CH mode**, addresses **005** and **007** respectively.

---

## ⚡ FAST-TWEAK CHEAT SHEET (pre-opening)

All three follow-the-video channels are auto-generated. Edit the constants at the top of each generator script, run it, then `pio run -e controlled_burn --target upload`.

### `gen_backlight_cues.py` — ch2 Varytec backlight
| Constant | Current | Effect |
|---|---|---|
| `BACKLIGHT_FLOOR` | 1 | absolute minimum during body (DIPs go to 0) |
| `BACKLIGHT_BASE_CAP` | **15** | ceiling of inverse-luma base before breath/wobble |
| `BACKLIGHT_HARD_CAP` | **100** | absolute ceiling — peaks with chaos can reach this |
| `BACKLIGHT_BIAS` | **8** | constant uplift → sets body mean (currently ≈ 25) |
| `BLACKOUT_RANGES` | `(10:00–10:13)`, `(14:02–14:18)` | hard pitch-black windows (suppresses oscillation + all events) |
| `BLACKOUT_DOWN_MS / RECOVER_MS` | 250 / 800 | snap to 0 at start, smooth recovery at end |
| `BACK_THUNDER_MOMENTS_MMSS` | `12:50, 15:37, 16:40` | independent ch2 lightning bursts (4–7 inverse-strobes/burst) |
| `BACK_THUNDER_HOLD/DOWN/RECOVER` | 2 f / 25 ms / 120 ms | shape of each strobe inside a back-thunder burst |
| `THUNDER_ENABLE` | True | mirror every ch7 strobe with an inverse dip on ch2 |
| `THUNDER_MIN_DMX` | 80 | only mirror saber flashes brighter than this |
| `THUNDER_DOWN/HOLD/RECOVER` | 30 ms / 3 f / 180 ms | shape of the lightning-shadow dip |
| `THUNDER_MIN_GAP_F` | 6 | min frames between consecutive thunder dips (skip dense bursts) |
| `BREATH_AMP_MAX / MIN` | 30 / 4 | breath swing in pitch-dark / bright video |
| `BREATH_PERIOD_DARK_SEC` | 9.0 | slowest breath period |
| `BREATH_PERIOD_BRIGHT_SEC` | 3.5 | fastest breath period |
| `WOBBLE_AMP_MAX / MIN` | 12 / 3 | narrative micro-jitter amplitude |
| `CHAOS_AMP_MAX / MIN` | 16 / 3 | erratic multi-sine layer (3 incommensurate periods + per-sec noise) |
| `EMIT_EVERY_SEC / FADE_MS` | 2 / 1000 | cue cadence — keep ≥2 s to stay under Teensy RAM cap |
| `DIP_EVENTS_MMSS` | list | timestamps where light snaps to 0 then recovers (`(mm,ss)` or `(mm,ss,frame)` for frame-precise) |
| `FLICKER_EVENTS` | list | single-burst flickers `(mm,ss,lo,hi,n,gap_lo,gap_hi,fade_ms)` |
| `FLICKER_RANGES` | list | continuous random flicker windows `(start_mm,start_ss,end_mm,end_ss,lo,hi,gap_lo,gap_hi,fade_ms)` |
| `SAFETY_DMX` | **25** | hold value during ch2 safety zone (post-show, room not pitch-black) |
| `SAFETY_OSC_AMP / PERIOD` | ±4 / 8 s | tiny oscillation during safety zone |
| Safety zone | **30:20 → 31:50** | safety hold active after body ends |

### `gen_saber_ww2_cues.py` — ch5 corridor (NEW: slightly inversed to ch2)
| Constant | Current | Effect |
|---|---|---|
| `SABER2_FLOOR` | 12 | minimum — corridor never fully off |
| `SABER2_BASE_CAP` | **90** | ceiling of brightness-following base |
| `SABER2_HARD_CAP` | **220** | absolute peak with breath/chaos |
| `SABER2_BIAS` | **44** | constant uplift — adds body brightness without raising caps |
| `BREATH_AMP_MAX / MIN` | 48 / 6 | breath swing |
| `BREATH_PERIOD_SEC` | 11.0 | slow, longer than ch2 → decoupled feel |
| `WOBBLE_AMP_MAX / MIN` | 24 / 6 | jitter |
| `CHAOS_AMP_MAX / MIN` | 36 / 6 | erratic layer |
| `SAFETY_DMX` | **60** | hold value during ch5 safety zones (room never pitch-black) |
| `SAFETY_OSC_AMP / PERIOD` | ±8 / 7 s | tiny oscillation during safety zones |
| Safety zones | **0:00 → 0:29 and 30:00 → 31:38** | safety hold active outside body |

### `gen_saber_cues.py` — ch7 envelope follower + strobe zones
| Constant | Current | Effect |
|---|---|---|
| `RELEASE_RATE` | (chaotic fireworks model) | decay shape |
| `STROBE_TRIGGER_PROB` | 0.30 | probability per second of firing a strobe in zone |
| `STROBE_MIN_GAP_SEC` | 1 | minimum gap between strobes |
| `STROBE_FADE_MS` | (30, 70) | strobe fade range |
| `STROBE_OVERRIDE_RANGE` | 11:20–17:40 | window where strobe behaviour overrides envelope |
| `STROBE_SUPPRESS_SEC` | [(8:30–8:40), (9:30–9:47)] | windows where ALL strobes are suppressed |
| `EXTRA` | list | additional manual strobe timestamps |
| `BRIGHTNESS_JITTER` | 0.30 | per-strobe brightness variation |
| `RANDOM_SEED` | 42 | reproducibility |

### Build sequence after any tweak
```powershell
$env:PYTHONIOENCODING="utf-8"
python gen_backlight_cues.py    # ch2
python gen_saber_ww2_cues.py    # ch5
python gen_saber_cues.py        # ch7
pio run -e controlled_burn --target upload
```

### Visual verification
```powershell
python analyze_movie_luma.py --comp controlled_burn --output lumaplots/controlled_burn_lumaXX.png
```
Plot colours: blue=ch2 Varytec, lime=ch7 Saber, violet=ch5 corridor.

### Memory budget warning
Teensy 4.1 RAM1 fills at ~2000 ch2 cues. If a build fails with `program exceeds memory space`, raise `EMIT_EVERY_SEC` or `DELTA_SKIP` in the offending generator to reduce cue count.

---

## Project Structure

```
src/
  main.cpp                # LTC decode, audio graph, DMX dispatch
  ltc_decoder.h/cpp       # Biphase-mark SMPTE decoder
  dmx_controller.h/cpp    # Cue engine with linear fade interpolation
  cues_albedo.h           # Albedo cue list (43 min, 25 fps) — DO NOT TOUCH
  cues_control_burn.h     # Varytec controlled burn (32 min, 30 fps)
  cues_saber_ww.h         # ADJ Saber ch7 — auto-generated by gen_saber_cues.py
  cues_saber_ww2.h        # ADJ Saber ch5 — fixed DMX 150
include/
  config.h                # Tunable parameters (FPS set via build flag)
  display.h               # OLED driver
lumaplots/
  controlled_burn_luma09.png   # Current plot — blue=Varytec, lime=ch7, violet=ch5
analyze_movie_luma.py     # Luma CSV + DMX arc plot generator
gen_saber_cues.py         # Regenerate cues_saber_ww.h (envelope follower)
scripts/inject_version.py # Git hash + timestamp injected at compile time
platformio.ini
```

---

## Build & Upload

```bash
pio run -e controlled_burn               # build
pio run -e controlled_burn --target upload  # build + flash
pio device monitor                       # serial at 115200
```

---

## Cue File Format

### Varytec — `V(dimmer, strobe)` macro

```cpp
//  HH  MM  SS  FF  fadeMs    channels
  { 0,  5,  8,  0,   200,    V(80, 0) },
```

`FF` = frame within second. `fadeMs=0` = instant snap. Cues must be chronological.

### ADJ Saber — `W(dmx)` / `W2(dmx)` macros

```cpp
  { 0, 5, 8, 0,  0,  W(200) },   // ch7 — instant to 200
  { 0, 5, 8, 0,  0, W2(150) },   // ch5 — fixed wash
```

After any edit → rebuild + reflash.

---

## Luma Analysis

`analyze_movie_luma.py` extracts per-second **p95/p99 spatial luma** from the film (32×18 grid, 1 fps via FFmpeg) and simulates the DMX arcs on a dark-background plot.

```bash
python analyze_movie_luma.py --comp controlled_burn \
  --output lumaplots/controlled_burn_luma09.png
```

Plot colours: gold=mean, orange=p95, yellow=p99, red=pmax, pink=hot-pixel coverage, blue=Varytec arc, lime=Saber ch7, violet=Saber ch5.

To retune the Saber ch7 envelope follower, edit `RELEASE_RATE` in `gen_saber_cues.py` and re-run it, then rebuild.

---

## Serial Commands

| Key / Command | Action |
|---|---|
| `t` | DMX hardware test |
| `q` | Toggle TC print |
| `p` + Enter | Toggle PLAY ↔ LIVE mode |
| `b` / `f` | Blackout / full bright |
| `i NNN` | Set dimmer to NNN |
| `m` / `m 2000` | Mark cue (snap / 2000 ms fade) |
| `e` | Export marks as CSV text |
| `?` | Help |

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| No TC in serial | Check `DEBUG_AUDIO_LEVEL` in config.h; verify LINE IN cable |
| Garbled TC / `rej:` rising | Enable `DEBUG_LTC_BITS`; check signal level |
| Signal clipping | Raise `AUDIO_LINEIN_LEVEL` from 0 → 5 |
| Fixture unresponsive | `t` test; swap A/B on XLR; verify address + mode |
| `Frame skip: +9` every second (30 fps) | BCD tens-digit flip bug — see correction block in `main.cpp` (`diff >= 9 && diff <= 12`) |
| Build mismatch at venue | Check boot banner `Build:` git hash + dirty flag |

---

## Known Issue: BCD Tens-Digit Flip (30 fps)

At 30 fps, timing noise can increment the BCD tens digit by 1 for frames `:09/:19/:29`, making them appear as `:19/:29` or rolling into the next second. The fix in `main.cpp` detects `diff ∈ [9,12]`, subtracts 10 from the absolute frame counter, and accepts only if `correctedDiff == 1`.

Symptom if broken: `[!!] Frame skip: +9` exactly once per second, TC jumps landing on `...:09`.

---

## License

MIT

