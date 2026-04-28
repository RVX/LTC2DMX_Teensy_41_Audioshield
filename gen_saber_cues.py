#!/usr/bin/env python3
"""
gen_saber_cues.py — Generate src/cues_saber_ww.h for the ADJ Saber Spot WW
                    DMX address 7, 1-channel mode (dimmer only)
                    ENVELOPE FOLLOWER on luma p99.

Usage:  python gen_saber_cues.py
Output: src/cues_saber_ww.h

Envelope follower model
  Attack  : instant snap to peak p99 value (0 ms)
  Release : linear decay at RELEASE_RATE DMX units/second
              → from DMX 255 to 0 in ~3 seconds (configurable)
  Result  : the lamp leaps to match every micro-spark/explosion peak,
            then glows down smoothly instead of cutting off instantly.
            This creates a warm "fire afterglow" tail after each burst.
"""

import csv
import os
import random
import sys
from datetime import datetime

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
CSV_PATH     = os.path.join(PROJECT_ROOT, "controlled_burn_luma.csv")
OUTPUT_PATH  = os.path.join(PROJECT_ROOT, "src", "cues_saber_ww.h")
# Side-file: every "strobe-like" attack moment (sec, frame) for ch2 sync.
STROBE_TIMES_PATH = os.path.join(PROJECT_ROOT, "ch7_strobe_times.json")

# ── ADJ Saber Spot WW ─────────────────────────────────────────────────────────
SABER_CH   = 7    # DMX channel 7 = dimmer (address 7, 1-ch mode)
SABER_APEX = 160  # cap at 160 — keeps peaks varied and avoids sustained saturation
SABER_FLOOR = 2   # minimum visible

# ── p99 → DMX thresholds ─────────────────────────────────────────────────────
P99_DARK  = 15    # p99 below this → DMX 0 (genuinely dark)
P99_FLOOR = 30    # p99 below this → DMX 2 (ember floor glow)

# ── Fire zone boundaries (inclusive, in seconds) ──────────────────────────────
#   Zone A: 5:08–21:55  (308–1315 s)
#   Zone B: 24:34–30:24 (1474–1824 s)
ZONE_A = (308, 1315)
ZONE_B = (1474, 1828)   # extended to 30:28 so release is generated naturally

# ── Pulse parameters ─────────────────────────────────────────────────────────
# Every qualifying second emits TWO cues:
#   1. Attack  — instant snap to peak at frame 0
#   2. Release — forced fade-to-0 starting at HOLD_FRAMES into the same second
#
# At 30 fps:  frame 8  ≈ 267 ms   (burst / strobe mode)
#             frame 15 ≈ 500 ms   (normal hold before decay)
#
# Release fade duration = dmx / RELEASE_RATE * 1000 ms
#   RELEASE_RATE = 150  → 255→0 in ~1.7 s
DECAY_BUCKETS_MS    = [(2500, "SHARP"), (1500, "FAST"), (900, "SOFT")]
DECAY_WEIGHTS       = [3, 4, 2]
HOLD_CHOICES_FRAMES = [1, 1, 2]
BRIGHTNESS_JITTER    = 0.30
BRIGHTNESS_DELTA_MIN = 18
SUB_FLASH_THRESH_2  = 60
SUB_FLASH_THRESH_3  = 110
SUB_FLASH_PROB_2    = 0.55
SUB_FLASH_PROB_3    = 0.35
MIN_SUBFLASH_GAP_F  = 5
MAX_SUBFLASH_GAP_F  = 14
RELEASE_RATE        = 1500
HOLD_FRAMES_NORMAL  = 1
HOLD_FRAMES_BURST   = 1
BURST_THRESH_DMX    = 120
JITTER_MAX_FRAMES   = 25
RANDOM_SEED         = 42

# ── Cue filter ─────────────────────────────────────────────────────────────────────
DELTA_THRESH       = 3
MIN_ATTACK_GAP_SEC = 0   # no per-second cooldown — sub-flashes drive density

# ── EXTRA FLASH BURSTS ───────────────────────────────────────────────────────
# Manually injected ultra-fast micro-bursts at specific timestamps.
# Each entry triggers 4–6 super-quick flashes packed inside ~400ms,
# random intensities in EXTRA_DMX_RANGE, very short decay (flash/strobe feel).
# These run regardless of fire-zone membership.
EXTRA_FLASH_MOMENTS_MMSS = [
    # Original tightened batch (3:21..5:15)
    (3,21), (3,30), (3,38), (3,42), (3,52),
    (4, 0), (4, 7), (4,11), (4,53),
    (5, 7), (5,15),
    # New strobe batch (5:22..15:27)
    (5,22), (5,25), (5,36), (5,43), (5,51),
    (6, 1), (6, 3), (6, 4), (6, 9), (6,12), (6,18),
    (6,26), (6,33), (6,38), (6,55),
    (7,19),
    (10,28), (10,43), (10,50), (10,57),
    (11, 6), (11,22), (11,51),
    (12, 4), (12,24), (12,38), (12,54),
    (13,58),
    (14,14), (14,34), (14,44), (14,49), (14,56),
    (15, 7), (15,18), (15,27),
    # Late-show batch (17:09..24:36) — simple snap-strobes (no flicker-only entries)
    (17, 9), (17,15), (17,18),
    (18, 6), (18,17), (18,18),
    (19, 9),
    (20,59),
    (23, 8), (23,22), (23,52),
    (24,36),
]

# ── STROBE SUPPRESSION WINDOWS ─────────────────────────────────────────────────
# Inside these (start_sec, end_sec) windows the chaotic-fireworks generator is
# muted entirely — no flashes are emitted.
STROBE_SUPPRESS_SEC = [
    ( 8*60 + 30,  8*60 + 40),    # 8:30 – 8:40
    ( 9*60 + 30,  9*60 + 47),    # 9:30 – 9:47
]

def in_suppress_window(sec: int) -> bool:
    return any(a <= sec <= b for a, b in STROBE_SUPPRESS_SEC)
EXTRA_DMX_MIN        = 150
EXTRA_DMX_MAX        = 255
EXTRA_FLASH_COUNT    = (1, 4)     # min,max micro-flashes per burst (random, not always flicker)
EXTRA_GAP_FRAMES     = (1, 4)     # 33–133 ms between micro-flashes
EXTRA_HOLD_FRAMES    = 1          # 33 ms hold
EXTRA_FADE_MS        = (40, 110)  # ultra-short decay per flash

# ── STROBE OVERRIDE ZONES ───────────────────────────────────────────────────
# Inside these (start_sec, end_sec) windows the chaotic-fireworks generator is
# replaced by ultra-fast EXTRA-style strobe bursts (33 ms hold, 40–110 ms decay,
# brightness 100–230 jittered, 1–3 micro-flashes per active second). Density and
# brightness still scale with the luma p99 of that second.
STROBE_ZONES_SEC = [
    (11*60 + 20, 17*60 + 40),    # 11:20 – 17:40 — match the snappy 2:20–7:40 feel
]
STROBE_ACTIVATE_DMX  = 60         # base_dmx threshold below which the strobe stays silent
STROBE_TRIGGER_PROB  = 0.30       # only ~30% of qualifying seconds actually fire — sparse strobe
STROBE_DMX_MIN       = 100
STROBE_DMX_MAX       = 230
STROBE_FLASH_PROB_2  = 0.20       # 2nd flash rare — need clean black space to read as a strobe
STROBE_FLASH_PROB_3  = 0.05       # 3rd flash very rare
STROBE_GAP_FRAMES    = (3, 8)     # 100–267 ms between sub-flashes when they do happen
STROBE_HOLD_FRAMES   = 1          # 33 ms hold
STROBE_FADE_MS       = (30, 70)   # ultra-short decay so lamp hits 0 well before the next sec
STROBE_MIN_GAP_SEC   = 1          # at least 1 fully dark second between strobes

def in_strobe_zone(sec: int) -> bool:
    return any(a <= sec <= b for a, b in STROBE_ZONES_SEC)


# ─────────────────────────────────────────────────────────────────────────────

def p99_to_dmx(p99: float) -> int:
    """Map raw p99 luma (0–255 float) to Saber DMX value (0–255)."""
    if p99 < P99_DARK:
        return 0
    if p99 < P99_FLOOR:
        return SABER_FLOOR
    # Linear: FLOOR → APEX over p99 range [P99_FLOOR, 255]
    t = (p99 - P99_FLOOR) / (255.0 - P99_FLOOR)
    return int(round(SABER_FLOOR + t * (SABER_APEX - SABER_FLOOR)))


def sec_to_hms(sec: int):
    h = sec // 3600
    m = (sec % 3600) // 60
    s = sec % 60
    return h, m, s


def in_fire_zone(sec: int) -> bool:
    return (ZONE_A[0] <= sec <= ZONE_A[1]) or (ZONE_B[0] <= sec <= ZONE_B[1])


# ─────────────────────────────────────────────────────────────────────────────

def load_p99(csv_path: str):
    """Return sorted list of (second, p99_raw) from the luma CSV."""
    data = []
    with open(csv_path, newline='', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            try:
                sec  = int(float(row['second']))
                p99r = float(row.get('yp99_raw', row.get('yp99_0_255', 0)))
            except (ValueError, KeyError):
                continue
            data.append((sec, p99r))
    data.sort(key=lambda r: r[0])
    return data


def compute_envelope(data):
    """
    Compute an envelope-follower curve over the raw p99→DMX values.

    For each second inside a fire zone:
      - If p99_dmx >= envelope: ATTACK  — envelope snaps up instantly.
      - If p99_dmx <  envelope: RELEASE — envelope decays linearly at
                                           RELEASE_RATE DMX units / second.

    Seconds outside fire zones reset the envelope to 0.
    Returns {second: dmx_value} dict.
    """
    p99_map = {sec: p99 for sec, p99 in data}
    all_secs = sorted(p99_map.keys())
    env = {}
    prev = 0
    prev_sec = None
    for sec in all_secs:
        if not in_fire_zone(sec):
            prev = 0
            prev_sec = None
            continue
        target = p99_to_dmx(p99_map[sec])
        if target >= prev + DELTA_THRESH:
            # Attack: new peak is meaningfully higher than current level → snap up
            env[sec] = target
        else:
            # Release: linear decay, accounting for any time gap
            elapsed = (sec - prev_sec) if prev_sec is not None else 1
            env[sec] = max(0, prev - RELEASE_RATE * elapsed)
        prev = env[sec]
        prev_sec = sec
    return env


def generate_cues(data):
    """
    Chaotic fireworks model.

    For every second inside a fire zone with p99-derived dmx > threshold,
    spawn 1–3 sub-flashes. Each sub-flash has:
      - randomised brightness   = base_dmx ± BRIGHTNESS_JITTER, anti-repeated
      - randomised sharpness    = pick from DECAY_BUCKETS_MS
      - randomised hold time    = pick from HOLD_CHOICES_FRAMES
      - randomised sub-second offset, with min spacing between sub-flashes

    Cue tuple: (h, m, s, frames, fade_ms, dmx, comment)
    """
    cues = []
    cues.append((0, 0, 0, 0, 0, 0, "hard black at start"))

    # Track every strobe-like attack (sec, frame, dmx) for ch2 thunderstorm sync.
    # Populated by EXTRA pass and STROBE-zone pass below; written by main().
    global STROBE_TIMES
    STROBE_TIMES = []

    rng = random.Random(RANDOM_SEED)
    last_brightness = 0  # for anti-repetition across consecutive flashes

    def pick_brightness(base):
        # Jitter ±BRIGHTNESS_JITTER, then enforce min delta from last flash.
        for _ in range(6):
            j = 1.0 + rng.uniform(-BRIGHTNESS_JITTER, BRIGHTNESS_JITTER)
            v = int(round(base * j))
            v = max(SABER_FLOOR, min(SABER_APEX, v))
            if abs(v - last_brightness) >= BRIGHTNESS_DELTA_MIN:
                return v
        # Fallback: force a delta in opposite direction
        if base >= last_brightness:
            return min(SABER_APEX, last_brightness + BRIGHTNESS_DELTA_MIN + rng.randint(0, 10))
        return max(SABER_FLOOR, last_brightness - BRIGHTNESS_DELTA_MIN - rng.randint(0, 10))

    def normalise(h, m, s, f):
        if f >= 30:
            s += f // 30
            f = f % 30
        if s >= 60:
            m += s // 60
            s = s % 60
        if m >= 60:
            h += m // 60
            m = m % 60
        return h, m, s, f

    for sec, p99 in data:
        if not in_fire_zone(sec):
            continue
        if in_strobe_zone(sec):
            continue   # handled by the strobe pass below
        if in_suppress_window(sec):
            continue   # explicitly muted moments
        base_dmx = p99_to_dmx(p99)
        if base_dmx <= DELTA_THRESH:
            continue

        h, m, s = sec_to_hms(sec)

        # How many sub-flashes this second?
        n = 1
        if base_dmx >= SUB_FLASH_THRESH_2 and rng.random() < SUB_FLASH_PROB_2:
            n = 2
        if base_dmx >= SUB_FLASH_THRESH_3 and rng.random() < SUB_FLASH_PROB_3:
            n = 3

        # Pick non-overlapping sub-frame offsets within the second.
        # Reserve enough room so the last attack still fits and its release
        # does not push too far past the second.
        max_start = 30 - 4  # leave 4 frames at end for the final hold
        offsets = []
        cursor = rng.randint(0, 6)
        for _ in range(n):
            if cursor > max_start:
                break
            offsets.append(cursor)
            cursor += rng.randint(MIN_SUBFLASH_GAP_F, MAX_SUBFLASH_GAP_F)
        if not offsets:
            offsets = [rng.randint(0, max_start)]

        for of in offsets:
            dmx_f       = pick_brightness(base_dmx)
            hold_f      = rng.choice(HOLD_CHOICES_FRAMES)
            decay_rate, tag = rng.choices(DECAY_BUCKETS_MS, weights=DECAY_WEIGHTS, k=1)[0]
            fade_ms     = max(20, int(dmx_f / decay_rate * 1000))

            # 1. Attack at jittered frame
            ah, am, as_, af = normalise(h, m, s, of)
            cues.append((ah, am, as_, af, 0, dmx_f,
                         f"{sec//60}:{sec%60:02d}+{of}f {tag} att {dmx_f}"))

            # 2. Release after hold
            rh, rm, rs, rf = normalise(h, m, s, of + hold_f)
            cues.append((rh, rm, rs, rf, fade_ms, 0,
                         f"{sec//60}:{sec%60:02d}+{of+hold_f}f rel {dmx_f} 0 ({fade_ms}ms,{tag})"))

            last_brightness = dmx_f

    # ── EXTRA injected flash bursts (manual fireworks moments) ──
    extra_last = 0
    for mm, ss in EXTRA_FLASH_MOMENTS_MMSS:
        h0, m0, s0 = sec_to_hms(mm * 60 + ss)
        n_flash = rng.randint(EXTRA_FLASH_COUNT[0], EXTRA_FLASH_COUNT[1])
        cur_f = rng.randint(0, 2)
        for i in range(n_flash):
            # anti-repetition brightness in [EXTRA_DMX_MIN, EXTRA_DMX_MAX]
            for _ in range(6):
                v = rng.randint(EXTRA_DMX_MIN, EXTRA_DMX_MAX)
                if abs(v - extra_last) >= 25:
                    break
            extra_last = v
            ah, am, as_, af = normalise(h0, m0, s0, cur_f)
            cues.append((ah, am, as_, af, 0, v,
                         f"{mm}:{ss:02d} EXTRA flash#{i+1} att {v}"))
            STROBE_TIMES.append((ah*3600 + am*60 + as_, af, v, "EXTRA"))
            rh, rm, rs, rf = normalise(h0, m0, s0, cur_f + EXTRA_HOLD_FRAMES)
            fade = rng.randint(EXTRA_FADE_MS[0], EXTRA_FADE_MS[1])
            cues.append((rh, rm, rs, rf, fade, 0,
                         f"{mm}:{ss:02d} EXTRA flash#{i+1} rel ({fade}ms)"))
            cur_f += EXTRA_HOLD_FRAMES + rng.randint(EXTRA_GAP_FRAMES[0], EXTRA_GAP_FRAMES[1])

    # ── STROBE-ZONE pass: ultra-fast EXTRA-style bursts driven by luma p99 ──
    strobe_last = 0
    last_strobe_sec = -10
    for sec, p99 in data:
        if not in_strobe_zone(sec):
            continue
        if in_suppress_window(sec):
            continue
        base_dmx = p99_to_dmx(p99)
        if base_dmx < STROBE_ACTIVATE_DMX:
            continue
        if sec - last_strobe_sec <= STROBE_MIN_GAP_SEC:
            continue   # enforce a fully dark second between strobes
        if rng.random() > STROBE_TRIGGER_PROB:
            continue   # most qualifying seconds skipped — sparse strobe feel
        last_strobe_sec = sec
        h, m, s = sec_to_hms(sec)

        # Density: usually 1 flash, very rarely 2 or 3
        n = 1
        if base_dmx >= 90 and rng.random() < STROBE_FLASH_PROB_2:
            n = 2
        if base_dmx >= 130 and rng.random() < STROBE_FLASH_PROB_3:
            n = 3

        # Brightness target proportional to base_dmx, but inside the strobe range
        bright_t = min(1.0, base_dmx / SABER_APEX)
        b_centre = STROBE_DMX_MIN + bright_t * (STROBE_DMX_MAX - STROBE_DMX_MIN)

        cur_f = rng.randint(0, 4)
        for i in range(n):
            # ±25 % jitter around centre, anti-repetition vs previous strobe flash
            for _ in range(6):
                v = int(round(b_centre * (1.0 + rng.uniform(-0.25, 0.25))))
                v = max(STROBE_DMX_MIN, min(STROBE_DMX_MAX, v))
                if abs(v - strobe_last) >= 20:
                    break
            strobe_last = v

            ah, am, as_, af = normalise(h, m, s, cur_f)
            cues.append((ah, am, as_, af, 0, v,
                         f"{sec//60}:{sec%60:02d}+{cur_f}f STROBE att {v}"))
            STROBE_TIMES.append((ah*3600 + am*60 + as_, af, v, "STROBE"))
            rh, rm, rs, rf = normalise(h, m, s, cur_f + STROBE_HOLD_FRAMES)
            fade = rng.randint(STROBE_FADE_MS[0], STROBE_FADE_MS[1])
            cues.append((rh, rm, rs, rf, fade, 0,
                         f"{sec//60}:{sec%60:02d}+{cur_f+STROBE_HOLD_FRAMES}f STROBE rel ({fade}ms)"))
            cur_f += STROBE_HOLD_FRAMES + rng.randint(STROBE_GAP_FRAMES[0], STROBE_GAP_FRAMES[1])
            if cur_f > 26:
                break

    # Hard black at end of composition
    cues.append((0, 31, 38, 0, 0, 0, "hard black at end"))

    # Sort by absolute frame position
    cues.sort(key=lambda c: (c[0] * 3600 + c[1] * 60 + c[2]) * 30 + c[3])
    return cues


def write_header(cues, output_path: str):
    lines = []
    lines.append("#pragma once")
    lines.append("// AUTO-GENERATED — do not edit by hand.")
    lines.append(f"// Generated: {datetime.utcnow().strftime('%Y-%m-%d %H:%M')} UTC  by gen_saber_cues.py")
    lines.append("// Source: controlled_burn_luma.csv  metric: yp99_raw")
    lines.append("//")
    lines.append("// ADJ Saber Spot WW — PER-SECOND PULSE MODEL on p99")
    lines.append(f"//   DMX address {SABER_CH} · 1-channel mode (CH{SABER_CH} = dimmer, 0–255)")
    lines.append("//   Each qualifying second = attack (frame 0) + forced release (HOLD_FRAMES).")
    lines.append(f"//   Normal pulse  : hold {HOLD_FRAMES_NORMAL} frames (~{HOLD_FRAMES_NORMAL/30*1000:.0f} ms) then fade at {RELEASE_RATE} DMX/s.")
    lines.append(f"//   Burst pulse   : hold {HOLD_FRAMES_BURST} frames (~{HOLD_FRAMES_BURST/30*1000:.0f} ms) for dmx≥{BURST_THRESH_DMX} — strobe feel.")
    lines.append("//   Light ALWAYS returns to 0; next attack overrides any ongoing fade.")    
    lines.append("//")
    lines.append("// p99 → DMX mapping:")
    lines.append(f"//   p99 <  {P99_DARK:3d}  → DMX   0  (dark)")
    lines.append(f"//   p99 <  {P99_FLOOR:3d}  → DMX   2  (floor ember)")
    lines.append(f"//   p99 = 255  → DMX {SABER_APEX}")
    lines.append("")
    lines.append('#include "dmx_controller.h"')
    lines.append("")
    lines.append(f"// W(dim) — 1-channel cue macro for the Saber Spot WW")
    lines.append(f"#define SABER_CH  {SABER_CH}  // ADJ Saber Spot WW — dimmer channel")
    lines.append(f"#define W(d)  {{ {{SABER_CH, (d)}} }}, 1")
    lines.append("")
    lines.append("static const DMXCue SABER_CUE_LIST[] = {")
    lines.append("")

    # Group sections by zone for readability
    last_zone = None
    for h, m, s, frames, fade, dmx, comment in cues:
        total_s = h * 3600 + m * 60 + s
        if total_s == 0 and frames == 0 and dmx == 0:
            zone = "INIT"
        elif in_fire_zone(total_s):
            zone = "A" if total_s <= ZONE_A[1] else "B"
        elif total_s < ZONE_A[0] and (dmx > 0 or fade > 0):
            zone = "X"
        else:
            zone = "END"

        if zone != last_zone:
            labels = {
                "INIT": "INIT — hard black",
                "X":    "EXTRA INJECTED FLASH BURSTS (manual fireworks moments)",
                "A":    f"FIRE ZONE A — {ZONE_A[0]//60}:{ZONE_A[0]%60:02d}–{ZONE_A[1]//60}:{ZONE_A[1]%60:02d}",
                "B":    f"FIRE ZONE B — {ZONE_B[0]//60}:{ZONE_B[0]%60:02d}–{ZONE_B[1]//60}:{ZONE_B[1]%60:02d}",
                "END":  "FADE OUT",
            }
            lines.append(f"    // {'='*68}")
            lines.append(f"    // {labels[zone]}")
            lines.append(f"    // {'='*68}")
            lines.append("")
            last_zone = zone

        lines.append(
            f"    {{ {h:2d}, {m:2d}, {s:2d}, {frames:2d},  {fade:6d}, W({dmx:3d}) }},   // {comment}"
        )

    lines.append("")
    lines.append("};  // END SABER_CUE_LIST")
    lines.append("")
    lines.append("#undef W")
    lines.append("")
    lines.append("static const uint16_t SABER_CUE_COUNT =")
    lines.append("    sizeof(SABER_CUE_LIST) / sizeof(SABER_CUE_LIST[0]);")
    lines.append("")

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines))
        f.write('\n')


# ─────────────────────────────────────────────────────────────────────────────

def main():
    if not os.path.exists(CSV_PATH):
        sys.exit(f"ERROR: CSV not found at {CSV_PATH}")

    print(f"Reading p99 data from {CSV_PATH} ...")
    data = load_p99(CSV_PATH)
    print(f"  {len(data)} seconds loaded")

    cues = generate_cues(data)
    print(f"  {len(cues)} Saber cues generated")

    write_header(cues, OUTPUT_PATH)
    print(f"  Written → {OUTPUT_PATH}")

    # Export strobe times for ch2 thunderstorm sync
    import json
    with open(STROBE_TIMES_PATH, "w", encoding="utf-8") as f:
        json.dump([{"sec": t[0], "frame": t[1], "dmx": t[2], "kind": t[3]}
                   for t in STROBE_TIMES], f, indent=1)
    print(f"  Wrote {len(STROBE_TIMES)} strobe times -> {STROBE_TIMES_PATH}")

    # Quick stats — attack cues only (dmx > 0, not the hard-black bookends)
    dmx_vals = [c[5] for c in cues if c[5] > 0 and "black" not in c[6]]
    if dmx_vals:
        print(f"  DMX range: {min(dmx_vals)}–{max(dmx_vals)}, "
              f"mean {sum(dmx_vals)/len(dmx_vals):.1f}")

    print("Done.")


if __name__ == "__main__":
    main()
