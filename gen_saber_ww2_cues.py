#!/usr/bin/env python3
"""
gen_saber_ww2_cues.py — Generate src/cues_saber_ww2.h
ADJ Saber Spot WW  Unit 2  (ch5, entrance / outside light)

Behaviour:
  Constant base of DMX 80 with a gentle ±30% oscillation tracking
  the per-second max-luma pixel value (ymax_raw column).

  ymax_raw 0   → DMX  56  (80 × 0.70)
  ymax_raw 128 → DMX  80  (base)
  ymax_raw 255 → DMX 104  (80 × 1.30)

  One cue per second; fadeMs = 2000 so the engine smoothly interpolates
  between consecutive target values — a slow breathing/oscillation effect.
  The light NEVER goes to 0.

Julian Charrière · CORRER · Venice 2026
"""

import csv
import os
from datetime import datetime

# ── Paths ─────────────────────────────────────────────────────────────────────
CSV_PATH    = os.path.join(os.path.dirname(__file__), "controlled_burn_luma.csv")
OUTPUT_PATH = os.path.join(os.path.dirname(__file__), "src", "cues_saber_ww2.h")

# ── DMX mapping ───────────────────────────────────────────────────────────────
SABER2_CH   = 5          # ADJ Saber Spot WW Unit 2 — DMX channel
BASE_DMX    = 80         # centre value
SWING       = 0.70       # ±70 % of base
DMX_MIN     = int(round(BASE_DMX * (1.0 - SWING)))   # 56
DMX_MAX     = int(round(BASE_DMX * (1.0 + SWING)))   # 104
FADE_MS     = 2000       # ms for each 1-second step interpolation

# ── Skip duplicate cues if value unchanged ────────────────────────────────────
SKIP_THRESH = 0          # emit every second (smooth tracking)

# ─────────────────────────────────────────────────────────────────────────────

def ymax_to_dmx(ymax: float) -> int:
    """Map ymax_raw (0–255) linearly to [DMX_MIN, DMX_MAX]."""
    t = max(0.0, min(1.0, ymax / 255.0))
    return int(round(DMX_MIN + t * (DMX_MAX - DMX_MIN)))


def sec_to_hms(sec: int):
    h = sec // 3600
    m = (sec % 3600) // 60
    s = sec % 60
    return h, m, s


def load_ymax(csv_path: str):
    data = []
    with open(csv_path, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            data.append((int(row["second"]), float(row["ymax_raw"])))
    return sorted(data, key=lambda x: x[0])


def generate_cues(data):
    """
    Emit one DMX cue per second for the entire composition.
    Light always stays in [DMX_MIN, DMX_MAX] — never 0.
    """
    cues = []
    prev_dmx = -1

    for sec, ymax in data:
        dmx = ymax_to_dmx(ymax)
        h, m, s = sec_to_hms(sec)
        fade = FADE_MS if sec > 0 else 0  # snap at second 0, fade from then on

        if abs(dmx - prev_dmx) > SKIP_THRESH or sec == 0:
            cues.append((h, m, s, dmx, fade,
                         f"{m:02d}:{s:02d}  ymax={ymax:.0f} → DMX {dmx}"))
            prev_dmx = dmx

    # Ensure a bookend at the very end — hold at BASE (not 0)
    # Find max second in data
    last_sec = max(s for s, _ in data)
    last_h, last_m, last_s = sec_to_hms(last_sec)

    # Add explicit end cue at 31:38 to hold base value
    end_dmx = BASE_DMX
    cues.append((0, 31, 38, end_dmx, 0,
                 f"31:38  composition end — hold at DMX {end_dmx}"))

    return cues


def write_header(cues, output_path: str):
    lines = []
    lines.append("#pragma once")
    lines.append("// AUTO-GENERATED — do not edit by hand.")
    lines.append(f"// Generated: {datetime.utcnow().strftime('%Y-%m-%d %H:%M')} UTC  by gen_saber_ww2_cues.py")
    lines.append("// Source: controlled_burn_luma.csv  metric: ymax_raw")
    lines.append("//")
    lines.append("// ADJ Saber Spot WW Unit 2 — OSCILLATING WASH on ymax_raw")
    lines.append(f"//   DMX address {SABER2_CH} · 1-channel mode (CH{SABER2_CH} = dimmer, 0–255)")
    lines.append(f"//   Base DMX {BASE_DMX} ± {int(SWING*100)}%  →  range [{DMX_MIN}, {DMX_MAX}]")
    lines.append(f"//   Tracks per-second max-luma pixel (ymax_raw). NEVER goes to 0.")
    lines.append(f"//   fadeMs = {FADE_MS} ms — smooth inter-second interpolation (breathing).")
    lines.append("//")
    lines.append(f"//   ymax_raw   0 → DMX {DMX_MIN}   (dim)")
    lines.append(f"//   ymax_raw 128 → DMX {BASE_DMX}  (base)")
    lines.append(f"//   ymax_raw 255 → DMX {DMX_MAX}  (peak)")
    lines.append("")
    lines.append('#include "dmx_controller.h"')
    lines.append("")
    lines.append(f"#define SABER2_CH  {SABER2_CH}   // ADJ Saber Spot WW Unit 2 — dimmer channel")
    lines.append(f"#define W2(d)  {{ {{SABER2_CH, (d)}} }}, 1")
    lines.append("")
    lines.append("static const DMXCue SABER2_CUE_LIST[] = {")
    lines.append("")

    last_min = None
    for h, m, s, dmx, fade, comment in cues:
        if m != last_min:
            if last_min is not None:
                lines.append("")
            lines.append(f"    // ── {m:02d}:xx ──")
            last_min = m
        lines.append(f"    {{  {h:2d}, {m:2d}, {s:2d},  0, {fade:6d}, W2({dmx:3d}) }},   // {comment}")

    lines.append("")
    lines.append("};  // END SABER2_CUE_LIST")
    lines.append("")
    lines.append("#undef W2")
    lines.append("")
    lines.append("static const uint16_t SABER2_CUE_COUNT =")
    lines.append("    sizeof(SABER2_CUE_LIST) / sizeof(SABER2_CUE_LIST[0]);")
    lines.append("")

    with open(output_path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))


def main():
    print(f"Reading ymax data from {CSV_PATH} ...")
    data = load_ymax(CSV_PATH)
    print(f"  {len(data)} seconds loaded")

    cues = generate_cues(data)
    print(f"  {len(cues)} Saber WW2 cues generated")
    print(f"  DMX range: {min(c[3] for c in cues)} – {max(c[3] for c in cues)}")

    write_header(cues, OUTPUT_PATH)
    print(f"  Written → {OUTPUT_PATH}")
    print("Done.")


if __name__ == "__main__":
    main()
