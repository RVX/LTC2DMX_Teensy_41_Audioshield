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
import sys
from datetime import datetime

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
CSV_PATH     = os.path.join(PROJECT_ROOT, "controlled_burn_luma.csv")
OUTPUT_PATH  = os.path.join(PROJECT_ROOT, "src", "cues_saber_ww.h")

# ── ADJ Saber Spot WW ─────────────────────────────────────────────────────────
SABER_CH   = 7    # DMX channel 7 = dimmer (address 7, 1-ch mode)
SABER_APEX = 255  # 0–255 full range
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
RELEASE_RATE        = 150   # DMX units / second — decay speed
HOLD_FRAMES_NORMAL  = 15    # frames before release starts (normal fire)
HOLD_FRAMES_BURST   = 8     # frames before release starts (explosion moments)
BURST_THRESH_DMX    = 180   # DMX value above which burst hold is used

# ── Cue filter ────────────────────────────────────────────────────────────────
DELTA_THRESH  = 3    # minimum DMX value to emit a cue (ignore noise near 0)


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
    Per-second pulse model — replaces envelope follower.

    For every second inside a fire zone where p99 > threshold:
      1. Attack  : { H, M, S,          0, 0,        W(dmx) }  snap to peak
      2. Release : { H, M, S, HOLD_FRAMES, fade_ms,  W(0)  }  fade to 0

    High-intensity seconds (dmx >= BURST_THRESH_DMX) use HOLD_FRAMES_BURST
    (~267 ms) for a strobe feel; others use HOLD_FRAMES_NORMAL (500 ms).

    Tuple format: (h, m, s, frames, fade_ms, dmx, comment)
    """
    cues = []
    cues.append((0, 0, 0, 0, 0, 0, "hard black at start"))

    for sec, p99 in data:
        if not in_fire_zone(sec):
            continue
        dmx = p99_to_dmx(p99)
        if dmx <= DELTA_THRESH:
            continue

        h, m, s  = sec_to_hms(sec)
        hold_f   = HOLD_FRAMES_BURST if dmx >= BURST_THRESH_DMX else HOLD_FRAMES_NORMAL
        fade_ms  = int(dmx / RELEASE_RATE * 1000)
        mode_tag = "BURST" if dmx >= BURST_THRESH_DMX else "pulse"

        # 1. Instant attack at start of second
        cues.append((h, m, s, 0, 0, dmx,
                     f"{sec//60}:{sec%60:02d} {mode_tag} attack\u2192{dmx}"))
        # 2. Forced release after hold
        cues.append((h, m, s, hold_f, fade_ms, 0,
                     f"{sec//60}:{sec%60:02d}+{hold_f}f release {dmx}\u21920 ({dmx/RELEASE_RATE:.1f}s)"))

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
        else:
            zone = "END"

        if zone != last_zone:
            labels = {
                "INIT": "INIT — hard black",
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

    # Quick stats — attack cues only (dmx > 0, not the hard-black bookends)
    dmx_vals = [c[5] for c in cues if c[5] > 0 and "black" not in c[6]]
    if dmx_vals:
        print(f"  DMX range: {min(dmx_vals)}–{max(dmx_vals)}, "
              f"mean {sum(dmx_vals)/len(dmx_vals):.1f}")

    print("Done.")


if __name__ == "__main__":
    main()
