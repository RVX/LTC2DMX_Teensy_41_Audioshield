#!/usr/bin/env python3
"""
gen_saber_ww2_cues.py — Generate src/cues_saber_ww2.h
ADJ Saber Spot WW  Unit 2  (ch5, corridor / outside light)

BEHAVIOUR
─────────
This corridor light moves *slightly inversed* to the ch2 backlight:
when ch2 (inverse-luma) is bright (i.e. video is dark), ch5 dims;
when ch2 dims (video bright), ch5 lifts. Same yavg source as ch2 but
NON-inverted, with very small caps so the corridor stays soft.

  brightness = clamp(yavg_sm / YAVG_BRIGHT_REF, 0, 1)
  base = floor + brightness * (BASE_CAP - floor)
  + tiny breath sine + tiny chaos so it isn't perfectly static
  hard-clamped to HARD_CAP

  yavg ≈   0  → near floor (very dim corridor)        ↔ ch2 bright
  yavg ≈ 110+ → near BASE_CAP (15) plus chaos peaks    ↔ ch2 dim

Julian Charrière · CORRER · Venice 2026
"""

import csv
import math
import os
import random
from datetime import datetime

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
CSV_PATH     = os.path.join(PROJECT_ROOT, "controlled_burn_luma.csv")
OUTPUT_PATH  = os.path.join(PROJECT_ROOT, "src", "cues_saber_ww2.h")

# ── DMX mapping ───────────────────────────────────────────────────────────────
SABER2_CH         = 5

# *** TWEAKABLE ENVELOPE *** (kept low — corridor must stay soft)
SABER2_FLOOR      = 6        # minimum during body — light never fully off
SABER2_BASE_CAP   = 45       # ceiling of the brightness-following base
SABER2_HARD_CAP   = 150      # absolute max with chaos/breath added
SABER2_BIAS       = 12       # constant uplift — adds body brightness
YAVG_BRIGHT_REF   = 110.0    # video luma above this → at BASE_CAP

# Subtle oscillation so the corridor isn't a flat dimmer.
# Amplitudes scale with brightness (the inverse of how ch2 scales).
BREATH_PERIOD_SEC = 11.0     # slow, longer than ch2 so they feel decoupled
BREATH_AMP_MAX    = 24       # ±24 DMX at full brightness (was 8)
BREATH_AMP_MIN    = 3
WOBBLE_PERIOD_SEC = 5.7
WOBBLE_AMP_MAX    = 12       # was 4
WOBBLE_AMP_MIN    = 3

# Chaos layer (multi-sine) — tiny, just to shake off perfect periodicity.
CHAOS_AMP_MAX     = 18       # was 6
CHAOS_AMP_MIN     = 3
CHAOS_PERIODS_SEC = (4.3, 9.1, 13.7)
CHAOS_PHASES      = (1.1, 3.0, 5.2)
CHAOS_NOISE_SEED  = 4242

SMOOTH_WINDOW_SEC = 5
EMIT_EVERY_SEC    = 2
FADE_MS           = 1000
DELTA_SKIP        = 2
INTRO_FADE_MS     = 29000
OUTRO_FADE_MS     = 8000

START_RISE_TIME   = (0, 0,  1)
START_RISE_TARGET = 6
END_FADE_TIME     = (0, 30, 24)
END_BLACK_TIME    = (0, 31, 38)

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


_CHAOS_NOISE = None
def _chaos_noise(sec):
    global _CHAOS_NOISE
    if _CHAOS_NOISE is None:
        rng = random.Random(CHAOS_NOISE_SEED)
        _CHAOS_NOISE = [rng.uniform(-1.0, 1.0) for _ in range(4000)]
    if sec < 0 or sec >= len(_CHAOS_NOISE):
        return 0.0
    i = int(sec)
    frac = sec - i
    a = _CHAOS_NOISE[i]
    b = _CHAOS_NOISE[min(i + 1, len(_CHAOS_NOISE) - 1)]
    return a + (b - a) * frac


def compute_dmx(sec, yavg_sm):
    brightness = max(0.0, min(1.0, yavg_sm / YAVG_BRIGHT_REF))
    base       = SABER2_FLOOR + brightness * (SABER2_BASE_CAP - SABER2_FLOOR)
    breath_amp = BREATH_AMP_MIN + brightness * (BREATH_AMP_MAX - BREATH_AMP_MIN)
    wobble_amp = WOBBLE_AMP_MIN + brightness * (WOBBLE_AMP_MAX - WOBBLE_AMP_MIN)
    chaos_amp  = CHAOS_AMP_MIN  + brightness * (CHAOS_AMP_MAX  - CHAOS_AMP_MIN)
    breath = breath_amp * math.sin(2 * math.pi * sec / BREATH_PERIOD_SEC + 0.4)
    wobble = wobble_amp * math.sin(2 * math.pi * sec / WOBBLE_PERIOD_SEC + 1.7)
    chaos_sum = 0.0
    for p, ph in zip(CHAOS_PERIODS_SEC, CHAOS_PHASES):
        chaos_sum += math.sin(2 * math.pi * sec / p + ph)
    chaos_sum /= len(CHAOS_PERIODS_SEC)
    chaos_sum  = 0.6 * chaos_sum + 0.4 * _chaos_noise(sec)
    chaos = chaos_amp * chaos_sum
    return int(round(max(SABER2_FLOOR,
                         min(SABER2_HARD_CAP, base + SABER2_BIAS + breath + wobble + chaos))))


def sec_to_hms(sec):
    return sec // 3600, (sec % 3600) // 60, sec % 60


def generate_cues(data):
    secs   = [s for s, _ in data]
    yavgs  = [y for _, y in data]
    yavg_s = smooth(yavgs, SMOOTH_WINDOW_SEC)

    cues = []
    cues.append((0, 0, 0, 0, 0, "hard black at start"))
    rh, rm, rs = START_RISE_TIME
    cues.append((rh, rm, rs, START_RISE_TARGET, INTRO_FADE_MS,
                 f"smooth rise -> DMX {START_RISE_TARGET} over {INTRO_FADE_MS//1000}s"))

    rise_end_sec = rh * 3600 + rm * 60 + rs + INTRO_FADE_MS // 1000
    end_sec      = END_FADE_TIME[0] * 3600 + END_FADE_TIME[1] * 60 + END_FADE_TIME[2]

    last_emit = START_RISE_TARGET
    for sec, yavg, yavg_sm in zip(secs, yavgs, yavg_s):
        if sec < rise_end_sec or sec >= end_sec:
            continue
        if (sec - rise_end_sec) % EMIT_EVERY_SEC != 0:
            continue
        dmx = compute_dmx(sec, yavg_sm)
        if abs(dmx - last_emit) < DELTA_SKIP:
            continue
        h, m, s = sec_to_hms(sec)
        cues.append((h, m, s, dmx, FADE_MS,
                     f"yavg={yavg:5.1f} sm={yavg_sm:5.1f} -> DMX {dmx}"))
        last_emit = dmx

    eh, em, es = END_FADE_TIME
    cues.append((eh, em, es, 0, OUTRO_FADE_MS, f"outro fade -> 0 over {OUTRO_FADE_MS//1000}s"))
    bh, bm, bs = END_BLACK_TIME
    cues.append((bh, bm, bs, 0, 0, "hard black"))

    cues.sort(key=lambda c: c[0]*3600 + c[1]*60 + c[2])
    return cues


def write_header(cues, output_path):
    lines = []
    lines.append("#pragma once")
    lines.append("// AUTO-GENERATED — do not edit by hand. Regenerate with gen_saber_ww2_cues.py")
    lines.append(f"// Generated: {datetime.utcnow().strftime('%Y-%m-%d %H:%M')} UTC")
    lines.append("//")
    lines.append("// ADJ Saber Spot WW Unit 2 (ch5) — corridor light, slightly inversed to ch2.")
    lines.append(f"//   Floor {SABER2_FLOOR}  base cap {SABER2_BASE_CAP}  hard cap {SABER2_HARD_CAP}")
    lines.append("//   Source: yavg_raw (same as ch2) — but follows brightness directly,")
    lines.append("//   so when ch2 lifts (dark video) ch5 dims, and vice-versa.")
    lines.append("")
    lines.append('#include "dmx_controller.h"')
    lines.append("")
    lines.append(f"#define SABER2_CH  {SABER2_CH}   // ADJ Saber Spot WW Unit 2 — dimmer channel")
    lines.append(f"#define W2(d)  {{ {{SABER2_CH, (d)}} }}, 1")
    lines.append("")
    lines.append("static const DMXCue SABER2_CUE_LIST[] = {")

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
    lines.append("#undef W2")
    lines.append("")
    lines.append("static const uint16_t SABER2_CUE_COUNT =")
    lines.append("    sizeof(SABER2_CUE_LIST) / sizeof(SABER2_CUE_LIST[0]);")
    lines.append("")

    with open(output_path, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))


def main():
    print(f"Reading yavg data from {CSV_PATH} ...")
    data = load_yavg(CSV_PATH)
    print(f"  {len(data)} seconds loaded")
    cues = generate_cues(data)
    body_dmx = [c[3] for c in cues if c[4] == FADE_MS]
    print(f"  {len(cues)} Saber WW2 cues generated")
    if body_dmx:
        print(f"  Body DMX range: {min(body_dmx)}-{max(body_dmx)}, mean {sum(body_dmx)/len(body_dmx):.1f}")
    write_header(cues, OUTPUT_PATH)
    print(f"  Written -> {OUTPUT_PATH}")
    print("Done.")


if __name__ == "__main__":
    main()
