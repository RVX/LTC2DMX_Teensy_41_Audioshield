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
SABER2_FLOOR      = 12       # minimum during body — light never fully off
SABER2_BASE_CAP   = 90       # ceiling of the brightness-following base
SABER2_HARD_CAP   = 220      # absolute max with chaos/breath added
SABER2_BIAS       = 24       # constant uplift — adds body brightness
YAVG_BRIGHT_REF   = 110.0    # video luma above this → at BASE_CAP

# Subtle oscillation so the corridor isn't a flat dimmer.
# Amplitudes scale with brightness (the inverse of how ch2 scales).
BREATH_PERIOD_SEC = 11.0     # slow, longer than ch2 so they feel decoupled
BREATH_AMP_MAX    = 48       # ±48 DMX at full brightness (was 24)
BREATH_AMP_MIN    = 6
WOBBLE_PERIOD_SEC = 5.7
WOBBLE_AMP_MAX    = 24       # was 12
WOBBLE_AMP_MIN    = 6

# Chaos layer (multi-sine) — tiny, just to shake off perfect periodicity.
CHAOS_AMP_MAX     = 36       # was 18
CHAOS_AMP_MIN     = 6
CHAOS_PERIODS_SEC = (4.3, 9.1, 13.7)
CHAOS_PHASES      = (1.1, 3.0, 5.2)
CHAOS_NOISE_SEED  = 4242

SMOOTH_WINDOW_SEC = 5
EMIT_EVERY_SEC    = 2
FADE_MS           = 1000
DELTA_SKIP        = 2
INTRO_FADE_MS     = 29000
OUTRO_FADE_MS     = 8000

START_RISE_TIME   = (0, 0, 29)   # start of body — leaves the 0-29s hold zone
START_RISE_TARGET = 12           # gentle rise into a low corridor wash (doubled)
END_FADE_TIME     = (0, 30,  0)  # body ends at 30:00, then safety-light hold begins
END_BLACK_TIME    = (0, 31, 38)  # (legacy bookend — no longer used; light stays at SAFETY_DMX)

# ── SAFETY-LIGHT HOLD ZONES ──
# The room must never be pitch-black on ch5. From 0:00 → START_RISE_TIME and
# from END_FADE_TIME onwards, hold at SAFETY_DMX with a tiny oscillation.
SAFETY_DMX             = 60      # baseline value during hold zones (doubled)
SAFETY_OSC_AMP         = 8       # ±8 DMX tiny breath during hold zones (doubled)
SAFETY_OSC_PERIOD_SEC  = 7.0     # slow gentle pulse
SAFETY_EMIT_EVERY_SEC  = 3       # cue every 3s (small motion, save RAM)
SAFETY_FADE_MS         = 2500    # smooth interpolation between samples
SAFETY_TAIL_END_SEC    = 31 * 60 + 38   # write hold cues out to 31:38

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

    # ── OPENING SAFETY-LIGHT HOLD: 0:00 → START_RISE_TIME at SAFETY_DMX ± osc ──
    rh, rm, rs = START_RISE_TIME
    rise_start_sec = rh * 3600 + rm * 60 + rs
    cues.append((0, 0, 0, SAFETY_DMX, 0,
                 f"opening safety hold -> DMX {SAFETY_DMX}"))
    for sec in range(SAFETY_EMIT_EVERY_SEC, rise_start_sec, SAFETY_EMIT_EVERY_SEC):
        osc = SAFETY_OSC_AMP * math.sin(2 * math.pi * sec / SAFETY_OSC_PERIOD_SEC)
        v   = int(round(SAFETY_DMX + osc))
        h, m, s = sec_to_hms(sec)
        cues.append((h, m, s, v, SAFETY_FADE_MS,
                     f"safety hold osc -> DMX {v}"))

    # ── BRIDGE: smooth fade from SAFETY_DMX down to body start value ──
    cues.append((rh, rm, rs, START_RISE_TARGET, 4000,
                 f"bridge -> DMX {START_RISE_TARGET} (body start)"))

    rise_end_sec = rise_start_sec + 4   # 4s bridge, then body emits begin
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

    # ── CLOSING SAFETY-LIGHT HOLD: from END_FADE_TIME onwards at SAFETY_DMX ± osc ──
    eh, em, es = END_FADE_TIME
    end_start_sec = eh * 3600 + em * 60 + es
    # Bridge back up to safety value over 4s
    cues.append((eh, em, es, SAFETY_DMX, 4000,
                 f"closing safety hold -> DMX {SAFETY_DMX}"))
    for sec in range(end_start_sec + SAFETY_EMIT_EVERY_SEC,
                     SAFETY_TAIL_END_SEC + 1,
                     SAFETY_EMIT_EVERY_SEC):
        osc = SAFETY_OSC_AMP * math.sin(2 * math.pi * sec / SAFETY_OSC_PERIOD_SEC)
        v   = int(round(SAFETY_DMX + osc))
        h, m, s = sec_to_hms(sec)
        cues.append((h, m, s, v, SAFETY_FADE_MS,
                     f"safety hold osc -> DMX {v}"))

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
