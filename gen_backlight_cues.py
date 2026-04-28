#!/usr/bin/env python3
"""
gen_backlight_cues.py — Generate src/cues_control_burn.h
Varytec LED Theater Spot 50  3200K  (3-channel mode: Master / Fine / Strobe)
DMX channels 2 / 3 / 4  (CH2 = Master dimmer)

BEHAVIOUR
─────────
The backlight behind the screen is INVERTED relative to the video luma:
brighter when the video is darker, dimmer when the video is bright.
A constant slow breathing oscillation is layered on top so the lamp
never sits perfectly still — it keeps "breathing" around the inverted
narrative curve.

  yavg_raw  →   smooth (5-s rolling mean) → inverse → DMX
  +  slow breath sine (period ≈ 13 s, amplitude ±12)
  +  micro narrative wobble (period ≈ 4.7 s, amplitude ±5)

  yavg ≈   0  (pure dark video)  →  near DMX cap (~170)
  yavg = 128  (mid)              →  ~mid range
  yavg ≈ 255  (very bright)      →  near floor (DMX 4)

  DMX is CAPPED at BACKLIGHT_CAP — never reaches 255.

Usage:  python gen_backlight_cues.py
Output: src/cues_control_burn.h
"""

import csv
import math
import os
import sys
from datetime import datetime

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
CSV_PATH     = os.path.join(PROJECT_ROOT, "controlled_burn_luma.csv")
OUTPUT_PATH  = os.path.join(PROJECT_ROOT, "src", "cues_control_burn.h")

# ── Mapping parameters ───────────────────────────────────────────────────────
BACKLIGHT_FLOOR    = 4       # never go fully black during composition body
BACKLIGHT_CAP      = 170     # CAP — never go to max (255)
YAVG_BRIGHT_REF    = 110.0   # video luma above this → backlight at floor
SMOOTH_WINDOW_SEC  = 5       # rolling mean window on yavg

# ── Oscillation layers ───────────────────────────────────────────────────────
BREATH_PERIOD_SEC  = 13.0
BREATH_AMP         = 12      # ±12 DMX slow sine
WOBBLE_PERIOD_SEC  = 4.7
WOBBLE_AMP         = 5       # ±5 DMX faster narrative jitter

# ── Cue emission cadence ─────────────────────────────────────────────────────
EMIT_EVERY_SEC     = 2       # one cue every 2 s
FADE_MS            = 2000    # fade between cues = 2 s → seamless interpolation
DELTA_SKIP         = 2       # skip if change vs previous emitted cue < this
INTRO_FADE_MS      = 28000   # smooth rise from black at start
OUTRO_FADE_MS      = 8000    # smooth fade-to-black at end

# ── Composition bookends ─────────────────────────────────────────────────────
START_RISE_TIME    = (0, 0,  1)   # h,m,s — when intro fade begins
START_RISE_TARGET  = 30           # DMX value reached after INTRO_FADE_MS
END_FADE_TIME      = (0, 30, 24)  # start of outro fade
END_BLACK_TIME     = (0, 31, 38)  # final hard black

# ─────────────────────────────────────────────────────────────────────────────

def load_yavg(csv_path):
    data = []
    with open(csv_path, newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f):
            try:
                sec  = int(float(row["second"]))
                yavg = float(row["yavg_raw"])
            except (KeyError, ValueError):
                continue
            data.append((sec, yavg))
    return sorted(data, key=lambda r: r[0])


def smooth(values, window):
    half = window // 2
    out = []
    for i in range(len(values)):
        lo = max(0, i - half)
        hi = min(len(values), i + half + 1)
        out.append(sum(values[lo:hi]) / (hi - lo))
    return out


def yavg_to_inverted_dmx(yavg_smooth):
    # Map yavg ∈ [0, YAVG_BRIGHT_REF] → darkness ∈ [1, 0]
    darkness = 1.0 - min(1.0, max(0.0, yavg_smooth / YAVG_BRIGHT_REF))
    return BACKLIGHT_FLOOR + darkness * (BACKLIGHT_CAP - BACKLIGHT_FLOOR)


def sec_to_hms(sec):
    return sec // 3600, (sec % 3600) // 60, sec % 60


def generate_cues(data):
    # 1. Smooth video luma so backlight follows narrative, not micro-flicker.
    secs   = [s   for s, _ in data]
    yavgs  = [y   for _, y in data]
    yavg_s = smooth(yavgs, SMOOTH_WINDOW_SEC)

    cues = []
    cues.append((0, 0, 0, 0, 0, "hard black at start"))
    # Smooth rise from black (composition opening)
    rh, rm, rs = START_RISE_TIME
    cues.append((rh, rm, rs, START_RISE_TARGET, INTRO_FADE_MS,
                 f"smooth rise → DMX {START_RISE_TARGET} over {INTRO_FADE_MS//1000}s"))

    # Begin oscillating inverted-luma generation AFTER the intro rise has reached target
    rise_end_sec = rh * 3600 + rm * 60 + rs + INTRO_FADE_MS // 1000
    end_sec      = END_FADE_TIME[0] * 3600 + END_FADE_TIME[1] * 60 + END_FADE_TIME[2]

    last_emit = START_RISE_TARGET
    for sec, yavg, yavg_sm in zip(secs, yavgs, yavg_s):
        if sec < rise_end_sec or sec >= end_sec:
            continue
        if (sec - rise_end_sec) % EMIT_EVERY_SEC != 0:
            continue

        base   = yavg_to_inverted_dmx(yavg_sm)
        breath = BREATH_AMP * math.sin(2 * math.pi * sec / BREATH_PERIOD_SEC)
        wobble = WOBBLE_AMP * math.sin(2 * math.pi * sec / WOBBLE_PERIOD_SEC + 1.3)
        dmx    = int(round(max(BACKLIGHT_FLOOR,
                               min(BACKLIGHT_CAP, base + breath + wobble))))

        if abs(dmx - last_emit) < DELTA_SKIP:
            continue

        h, m, s = sec_to_hms(sec)
        cues.append((h, m, s, dmx, FADE_MS,
                     f"yavg={yavg:5.1f} sm={yavg_sm:5.1f} inv→{int(round(base))} +breath={breath:+.0f} +wob={wobble:+.0f} → DMX {dmx}"))
        last_emit = dmx

    # Outro: fade to black
    eh, em, es = END_FADE_TIME
    cues.append((eh, em, es, 0, OUTRO_FADE_MS,
                 f"fade to black over {OUTRO_FADE_MS//1000}s"))
    bh, bm, bs = END_BLACK_TIME
    cues.append((bh, bm, bs, 0, 0, "hard black at end"))

    return cues


def write_header(cues, output_path):
    lines = []
    lines.append("#pragma once")
    lines.append("// AUTO-GENERATED — do not edit by hand.")
    lines.append(f"// Generated: {datetime.utcnow().strftime('%Y-%m-%d %H:%M')} UTC  by gen_backlight_cues.py")
    lines.append("// Source: controlled_burn_luma.csv  metric: yavg_raw (smoothed)")
    lines.append("//")
    lines.append("// Varytec LED Theater Spot 50 3200K — INVERTED LUMA BACKLIGHT")
    lines.append("//   DMX channels 2 (Master) / 3 (Fine) / 4 (Strobe), 3-ch mode")
    lines.append("//   Brighter when the video is darker, dimmer when the video is bright.")
    lines.append(f"//   DMX range: [{BACKLIGHT_FLOOR}, {BACKLIGHT_CAP}] — never reaches max (255).")
    lines.append("//")
    lines.append("//   Layered oscillation:")
    lines.append(f"//     base       = inverse of {SMOOTH_WINDOW_SEC}-s smoothed yavg_raw")
    lines.append(f"//     breath     = sine, period {BREATH_PERIOD_SEC:.1f}s, amplitude ±{BREATH_AMP}")
    lines.append(f"//     wobble     = sine, period {WOBBLE_PERIOD_SEC:.1f}s, amplitude ±{WOBBLE_AMP}")
    lines.append(f"//   Cue cadence: every {EMIT_EVERY_SEC}s with {FADE_MS}ms fade for smooth interpolation.")
    lines.append("//")
    lines.append('#include "dmx_controller.h"')
    lines.append('#include "../include/config.h"')
    lines.append("")
    lines.append("// V(dim, strobe) — 3-channel cue macro (Master / Fine=0 / Strobe)")
    lines.append("#define V(d, s) \\")
    lines.append("    { {DMX_CH_MASTER,(d)}, {DMX_CH_FINE,0}, {DMX_CH_STROBE,(s)} }, 3")
    lines.append("")
    lines.append("static const DMXCue CUE_LIST[] = {")
    lines.append("")

    last_min = None
    for h, m, s, dmx, fade, comment in cues:
        if m != last_min:
            if last_min is not None:
                lines.append("")
            lines.append(f"    // ── {h:02d}:{m:02d}:xx ──")
            last_min = m
        lines.append(f"    {{ {h:2d}, {m:2d}, {s:2d}, 0, {fade:6d}, V({dmx:3d}, 0) }},   // {comment}")

    lines.append("")
    lines.append("};  // END CUE_LIST")
    lines.append("")
    lines.append("#undef V")
    lines.append("")
    lines.append("static const uint16_t CUE_COUNT = sizeof(CUE_LIST) / sizeof(CUE_LIST[0]);")
    lines.append("")

    with open(output_path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))


def main():
    if not os.path.exists(CSV_PATH):
        sys.exit(f"ERROR: CSV not found at {CSV_PATH}")

    print(f"Reading yavg data from {CSV_PATH} ...")
    data = load_yavg(CSV_PATH)
    print(f"  {len(data)} seconds loaded")

    cues = generate_cues(data)
    body_dmx = [c[3] for c in cues if c[4] == FADE_MS]
    print(f"  {len(cues)} backlight cues generated  ({len(body_dmx)} oscillation cues)")
    if body_dmx:
        print(f"  Body DMX range: {min(body_dmx)}-{max(body_dmx)}, mean {sum(body_dmx)/len(body_dmx):.1f}")

    write_header(cues, OUTPUT_PATH)
    print(f"  Written -> {OUTPUT_PATH}")
    print("Done.")


if __name__ == "__main__":
    main()
