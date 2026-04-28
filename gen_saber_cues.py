#!/usr/bin/env python3
"""
gen_saber_cues.py — Generate src/cues_saber_ww.h for the ADJ Saber Spot WW
                    DMX address 7, 1-channel mode (dimmer only)
                    Follows fire intensity via luma p99 per second.

Usage:  python gen_saber_cues.py
Output: src/cues_saber_ww.h

p99 tracks the top 1% of pixels — it catches micro-sparks and the leading
edge of explosions before they bloom, making it ideal for a fast spotlight
that complements the Varytec (which follows p95).
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
ZONE_B = (1474, 1824)

# ── Cue filter ────────────────────────────────────────────────────────────────
DELTA_THRESH  = 3    # minimum DMX change to emit a new cue
EMBER_DARK_S  = 25   # consecutive dark seconds before valley-ember cue


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


def rise_fade(p99: float) -> int:
    """Snap rise: ultra-fast for bright sparks, slightly padded for dim ones."""
    if p99 >= 200: return 0    # instant snap
    if p99 >= 100: return 30   # 30 ms
    if p99 >= 50:  return 60   # 60 ms
    return 100                 # 100 ms


def fall_fade(last_dmx: int) -> int:
    """Fall from current brightness: faster from bright, slower from dim."""
    if last_dmx >= 120: return 50    # 50 ms
    if last_dmx >= 60:  return 100   # 100 ms
    return 200                       # 200 ms


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


def generate_cues(data):
    """
    Produce list of (h, m, s, fade_ms, dmx_val, comment) cue tuples.
    Covers only fire zones; outside zones the channel stays at its last value
    (naturally dark after zone exit since p99 drops to ~0 at zone boundaries).
    """
    cues = []

    # Hard black at composition start
    cues.append((0, 0, 0, 0, 0, "hard black at start"))

    last_dmx   = 0
    dark_count = 0   # consecutive DARK seconds (dmx == 0) inside fire zones

    for sec, p99 in data:
        if not in_fire_zone(sec):
            # Reset dark streak when outside fire zones
            dark_count = 0
            continue

        dmx   = p99_to_dmx(p99)
        delta = abs(dmx - last_dmx)

        if dmx == 0:
            dark_count += 1
            # Valley ember: one DMX-floor touch after 25 consecutive dark seconds
            if dark_count == EMBER_DARK_S:
                h, m, s = sec_to_hms(sec)
                cues.append((h, m, s, 5000, SABER_FLOOR,
                             f"{sec//60}:{sec%60:02d} valley-ember ({EMBER_DARK_S}s dark)"))
                last_dmx = SABER_FLOOR
            continue
        else:
            dark_count = 0

        if delta < DELTA_THRESH:
            continue  # not worth a cue

        h, m, s = sec_to_hms(sec)
        fade    = rise_fade(p99) if dmx > last_dmx else fall_fade(last_dmx)
        cues.append((h, m, s, fade, dmx, f"{sec//60}:{sec%60:02d} p99={int(p99)}"))
        last_dmx = dmx

    # Explicit fade-to-black at end of fade-out section (mirrors Varytec)
    cues.append((0, 31, 38, 22000, 0, "final fade to black"))

    return cues


def write_header(cues, output_path: str):
    lines = []
    lines.append("#pragma once")
    lines.append("// AUTO-GENERATED — do not edit by hand.")
    lines.append(f"// Generated: {datetime.utcnow().strftime('%Y-%m-%d %H:%M')} UTC  by gen_saber_cues.py")
    lines.append("// Source: controlled_burn_luma.csv  metric: yp99_raw")
    lines.append("//")
    lines.append("// ADJ Saber Spot WW")
    lines.append(f"//   DMX address {SABER_CH} · 1-channel mode (CH{SABER_CH} = dimmer, 0–255)")
    lines.append("//   Follows fire p99: brighter on micro-sparks and leading explosion edges.")
    lines.append("//   Complementary to the Varytec (ch2-4) which follows p95.")
    lines.append("//")
    lines.append("// p99 → DMX mapping:")
    lines.append(f"//   p99 <  {P99_DARK:3d}  → DMX   0  (dark)")
    lines.append(f"//   p99 <  {P99_FLOOR:3d}  → DMX   2  (floor ember)")
    lines.append(f"//   p99 = {P99_FLOOR:3d}  → DMX   2")
    lines.append(f"//   p99 = 255  → DMX {SABER_APEX}")
    lines.append("// Rise fades: ≥200→0ms snap  ≥100→30ms  ≥50→60ms  else 100ms")
    lines.append("// Fall fades: ≥120→50ms  ≥60→100ms  else 200ms")
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
    for h, m, s, fade, dmx, comment in cues:
        total_s = h * 3600 + m * 60 + s
        if total_s == 0:
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
            f"    {{ 0, {h:2d}, {m:2d}, {s:2d},  {fade:6d}, W({dmx:3d}) }},   // {comment}"
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

    # Quick stats
    dmx_vals = [c[4] for c in cues if c[4] > 0 and "ember" not in c[5] and "black" not in c[5]]
    if dmx_vals:
        print(f"  DMX range: {min(dmx_vals)}–{max(dmx_vals)}, "
              f"mean {sum(dmx_vals)/len(dmx_vals):.1f}")

    print("Done.")


if __name__ == "__main__":
    main()
