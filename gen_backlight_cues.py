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
import random
import sys
from datetime import datetime

PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
CSV_PATH     = os.path.join(PROJECT_ROOT, "controlled_burn_luma.csv")
OUTPUT_PATH  = os.path.join(PROJECT_ROOT, "src", "cues_control_burn.h")

# ── Mapping parameters ───────────────────────────────────────────────────────
# Very dim body lamp: base cap 15, hard cap 100. A constant BIAS lifts the
# mean so darkness still inverts the brightness, but the lamp never goes above
# 100. Tuned so body mean ≈ DMX 60.
BACKLIGHT_FLOOR    = 1       # absolute minimum during composition body
BACKLIGHT_BASE_CAP = 15      # ceiling of the inverse base before breath/wobble
BACKLIGHT_HARD_CAP = 100     # total ceiling — lamp never exceeds this
BACKLIGHT_BIAS     = 47      # constant uplift so mean (body) ≈ 60
YAVG_BRIGHT_REF    = 110.0   # video luma above this → backlight at floor
SMOOTH_WINDOW_SEC  = 5       # rolling mean window on yavg

# ── Oscillation layers (amplitudes scale with darkness — reactive breathing) ──
# Amps scaled down to fit inside the new HARD_CAP=100 envelope while still
# feeling erratic. Periods unchanged from previous round.
BREATH_PERIOD_DARK_SEC   = 9.0
BREATH_PERIOD_BRIGHT_SEC = 3.5
BREATH_AMP_MAX           = 30     # was 70
BREATH_AMP_MIN           = 4      # was 8
WOBBLE_PERIOD_SEC        = 2.9
WOBBLE_AMP_MAX           = 12     # was 22
WOBBLE_AMP_MIN           = 3      # was 4

# ── Erratic chaos layer (multi-sine + per-second jitter) ──
CHAOS_AMP_MAX            = 16     # was 30
CHAOS_AMP_MIN            = 3      # was 4
CHAOS_PERIODS_SEC        = (3.1, 7.3, 11.7)
CHAOS_PHASES             = (0.7, 2.4, 4.1)
CHAOS_NOISE_SEED         = 1337

# ── Cue emission cadence ─────────────────────────────────────────────────────
# Tightened from 2s/2000ms to 1s/900ms so the new chaos+shorter-breath layers
# are actually visible — slow fades would smear the erratic motion.
EMIT_EVERY_SEC     = 2       # one cue every 2 s — keeps RAM under Teensy cap
FADE_MS            = 1000    # half-emit fade so chaos+breath motion stays crisp (was 2000)
DELTA_SKIP         = 5       # skip if change vs previous emitted cue is small
INTRO_FADE_MS      = 29000   # smoother rise from black at start
OUTRO_FADE_MS      = 8000    # smooth fade-to-black at end

# ── Composition bookends ────────────────────────────────────────────────
START_RISE_TIME    = (0, 0,  1)   # h,m,s — when intro fade begins
START_RISE_TARGET  = 40           # DMX value reached after INTRO_FADE_MS
END_FADE_TIME      = (0, 30, 20)  # body ends — safety-hold begins here
END_BLACK_TIME     = (0, 31, 50)  # final hard black after safety-hold tail

# ── SAFETY-LIGHT HOLD (ch2) ──
# From END_FADE_TIME → SAFETY_TAIL_END_SEC the lamp holds at SAFETY_DMX with a
# tiny oscillation — same idea as ch5 — so the room is never pitch-black on
# ch2 during the post-show window.
SAFETY_DMX             = 25      # baseline value during ch2 hold zone
SAFETY_OSC_AMP         = 4       # ±4 DMX gentle breath
SAFETY_OSC_PERIOD_SEC  = 8.0     # slow pulse, slightly different from ch5 (7s)
SAFETY_EMIT_EVERY_SEC  = 3       # cue every 3s during hold
SAFETY_FADE_MS         = 2500    # smooth interpolation
SAFETY_BRIDGE_MS       = 4000    # 4s bridge from body to SAFETY_DMX
SAFETY_TAIL_END_SEC    = 31 * 60 + 50   # 31:50 — hold ends, hard black after

# ── INJECTED DIP EVENTS ─────────────────────────────────────────────────────────
# At each (mm, ss) timestamp the backlight does a fast pitch-down to 0 then
# returns to whatever value it would otherwise be at that moment.
# Each entry is (mm, ss) or (mm, ss, frame).
DIP_EVENTS_MMSS = [
    # Frame-precise dips
    (3,14, 15),
    (3,20, 10),
    # Whole-second dips
    (5,22), (5,25), (5,36), (5,43), (5,51),
    (6, 1), (6, 3), (6, 4), (6, 9), (6,12), (6,18),
    (6,38), (6,55), (7,19),
    (10,43), (10,50), (10,57), (11, 6),
    (14,34), (14,44), (14,49), (14,56),
    (15, 7), (15,18),
    # Late-show batch
    (17, 9), (17,15), (17,18),
    (18, 6), (18,17), (18,18),
    (19, 9),
    (20,59),
    (23, 8), (23,22), (23,52),
    (24,36),
    # Closing batch
    (29, 9), (29,20), (29,27), (29,36),
]
DIP_DOWN_MS         = 80          # snap-down fade duration
DIP_HOLD_FRAMES     = 4           # ~133 ms of darkness
DIP_RECOVER_MS      = 250         # quick fade back up to base value

# ── INJECTED FLICKER EVENTS ─────────────────────────────────────────────────────
# (mm, ss, dmx_lo, dmx_hi, n_flicks, gap_frames_min, gap_frames_max, fade_ms)
# Then return to base after the burst.
FLICKER_EVENTS = [
    # Subtle flicker 5–30
    ( 6,26,  5, 30, 6, 3, 6, 80),
    ( 6,33,  5, 30, 6, 3, 6, 80),
    # Subtle flicker 5–40
    (10,28,  5, 40, 6, 3, 6, 90),
    (12, 4,  5, 40, 6, 3, 6, 90),
    (12,38,  5, 40, 6, 3, 6, 90),
    (12,54,  5, 40, 6, 3, 6, 90),
    # Mid flicker 50–80
    (11,22, 50, 80, 5, 2, 5, 70),
    (11,51, 50, 80, 5, 2, 5, 70),
    (12,24, 50, 80, 5, 2, 5, 70),
    # Superfast flicker 50–80
    (13,58, 50, 80, 10, 1, 2, 40),
    (14,14, 50, 80, 10, 1, 2, 40),
    (15,27, 50, 80, 10, 1, 2, 40),
    (19,28, 50, 80, 10, 1, 2, 40),
    (23,29, 50, 80, 10, 1, 2, 40),
    # Subtle flicker 5–40
    (19,39,  5, 40, 6, 3, 6, 90),
    # Subtle flicker 2–10
    (24, 8,  2, 10, 6, 3, 6, 110),
]

# (start_mm, start_ss, end_mm, end_ss, dmx_lo, dmx_hi, gap_frames_min, gap_frames_max, fade_ms)
# Continuous random flicker across a window — emits a flick every gap_random frames.
FLICKER_RANGES = [
    (19,59, 20,15,  2, 20, 4,  9, 110),   # 19:59 – 20:15  very subtle 2–20
    (25,15, 25,28,  2,  8, 5, 12, 130),   # 25:15 – 25:28  very subtle 2–8
    (27, 1, 27, 8,  2,  8, 5, 12, 130),   # 27:01 – 27:08  very subtle 2–8
    (29,46, 29,49,  5, 40, 3,  6,  90),   # 29:46 – 29:49  subtle 5–40
]
FLICKER_RANDOM_SEED = 7

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
    return BACKLIGHT_FLOOR + darkness * (BACKLIGHT_BASE_CAP - BACKLIGHT_FLOOR), darkness


def sec_to_hms(sec):
    return sec // 3600, (sec % 3600) // 60, sec % 60


def normalise(h, m, s, f):
    """Carry frame/sec/min overflow up the units."""
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


# Pre-built per-second deterministic noise table for the chaos layer.
_CHAOS_NOISE = None
def _chaos_noise(sec):
    global _CHAOS_NOISE
    if _CHAOS_NOISE is None:
        rng = random.Random(CHAOS_NOISE_SEED)
        _CHAOS_NOISE = [rng.uniform(-1.0, 1.0) for _ in range(4000)]
    if sec < 0 or sec >= len(_CHAOS_NOISE):
        return 0.0
    # Interpolate between integer-second samples for smoothness
    i = int(sec)
    frac = sec - i
    a = _CHAOS_NOISE[i]
    b = _CHAOS_NOISE[min(i + 1, len(_CHAOS_NOISE) - 1)]
    return a + (b - a) * frac


def compute_oscillating_dmx(sec, yavg_sm):
    """Replicate the regular oscillation formula for any second."""
    base, darkness = yavg_to_inverted_dmx(yavg_sm)
    breath_amp    = BREATH_AMP_MIN + darkness * (BREATH_AMP_MAX - BREATH_AMP_MIN)
    breath_period = BREATH_PERIOD_BRIGHT_SEC + darkness * (BREATH_PERIOD_DARK_SEC - BREATH_PERIOD_BRIGHT_SEC)
    wobble_amp    = WOBBLE_AMP_MIN + darkness * (WOBBLE_AMP_MAX - WOBBLE_AMP_MIN)
    chaos_amp     = CHAOS_AMP_MIN  + darkness * (CHAOS_AMP_MAX  - CHAOS_AMP_MIN)
    breath = breath_amp * math.sin(2 * math.pi * sec / breath_period)
    wobble = wobble_amp * math.sin(2 * math.pi * sec / WOBBLE_PERIOD_SEC + 1.3)
    # Erratic multi-sine chaos — three incommensurate periods sum to a non-repeating wave
    chaos_sum = 0.0
    for p, ph in zip(CHAOS_PERIODS_SEC, CHAOS_PHASES):
        chaos_sum += math.sin(2 * math.pi * sec / p + ph)
    chaos_sum /= len(CHAOS_PERIODS_SEC)            # normalise to ~[-1,1]
    chaos_sum  = 0.6 * chaos_sum + 0.4 * _chaos_noise(sec)   # blend in per-sec noise
    chaos = chaos_amp * chaos_sum
    return int(round(max(BACKLIGHT_FLOOR,
                         min(BACKLIGHT_HARD_CAP, base + BACKLIGHT_BIAS + breath + wobble + chaos))))


def generate_cues(data):
    # 1. Smooth video luma so backlight follows narrative, not micro-flicker.
    secs   = [s   for s, _ in data]
    yavgs  = [y   for _, y in data]
    yavg_s = smooth(yavgs, SMOOTH_WINDOW_SEC)

    cues = []
    cues.append((0, 0, 0, 0, 0, 0, "hard black at start"))
    # Smooth rise from black (composition opening)
    rh, rm, rs = START_RISE_TIME
    cues.append((rh, rm, rs, 0, START_RISE_TARGET, INTRO_FADE_MS,
                 f"smooth rise → DMX {START_RISE_TARGET} over {INTRO_FADE_MS//1000}s"))

    # Begin oscillating inverted-luma generation AFTER the intro rise has reached target
    rise_end_sec = rh * 3600 + rm * 60 + rs + INTRO_FADE_MS // 1000
    end_sec      = END_FADE_TIME[0] * 3600 + END_FADE_TIME[1] * 60 + END_FADE_TIME[2]
    yavg_sm_map  = dict(zip(secs, yavg_s))

    last_emit = START_RISE_TARGET
    for sec, yavg, yavg_sm in zip(secs, yavgs, yavg_s):
        if sec < rise_end_sec or sec >= end_sec:
            continue
        if (sec - rise_end_sec) % EMIT_EVERY_SEC != 0:
            continue

        dmx = compute_oscillating_dmx(sec, yavg_sm)

        if abs(dmx - last_emit) < DELTA_SKIP:
            continue

        h, m, s = sec_to_hms(sec)
        cues.append((h, m, s, 0, dmx, FADE_MS,
                     f"yavg={yavg:5.1f} sm={yavg_sm:5.1f} → DMX {dmx}"))
        last_emit = dmx

    # ── INJECTED DIP EVENTS ───────────────────────────────────────────────────
    for ev in DIP_EVENTS_MMSS:
        if len(ev) == 3:
            mm, ss, ff = ev
        else:
            mm, ss = ev
            ff = 0
        ev_sec = mm * 60 + ss
        if ev_sec not in yavg_sm_map:
            continue
        base_dmx = compute_oscillating_dmx(ev_sec, yavg_sm_map[ev_sec])
        h0, m0, s0 = sec_to_hms(ev_sec)
        ah0, am0, as0, af0 = normalise(h0, m0, s0, ff)
        # Snap down to 0
        cues.append((ah0, am0, as0, af0, 0, DIP_DOWN_MS,
                     f"{mm}:{ss:02d}+{ff}f DIP ↓ 0 ({DIP_DOWN_MS}ms)"))
        # Hold dark briefly, then quick recover to base
        rh2, rm2, rs2, rf2 = normalise(h0, m0, s0, ff + DIP_HOLD_FRAMES)
        cues.append((rh2, rm2, rs2, rf2, base_dmx, DIP_RECOVER_MS,
                     f"{mm}:{ss:02d}+{ff+DIP_HOLD_FRAMES}f DIP ↑ back to {base_dmx} ({DIP_RECOVER_MS}ms)"))

    # ── INJECTED FLICKER EVENTS ─────────────────────────────────────────────────
    rng = random.Random(FLICKER_RANDOM_SEED)
    for mm, ss, lo, hi, n_flicks, gap_lo, gap_hi, fade_ms in FLICKER_EVENTS:
        ev_sec = mm * 60 + ss
        if ev_sec not in yavg_sm_map:
            continue
        base_dmx = compute_oscillating_dmx(ev_sec, yavg_sm_map[ev_sec])
        h0, m0, s0 = sec_to_hms(ev_sec)
        cur_f = 0
        last_v = -999
        for i in range(n_flicks):
            for _ in range(6):
                v = rng.randint(lo, hi)
                if abs(v - last_v) >= max(3, (hi - lo) // 4):
                    break
            last_v = v
            ah, am, as_, af = normalise(h0, m0, s0, cur_f)
            cues.append((ah, am, as_, af, v, fade_ms,
                         f"{mm}:{ss:02d} FLICK#{i+1} → {v} ({fade_ms}ms)"))
            cur_f += rng.randint(gap_lo, gap_hi)
        # Recover to base
        rh2, rm2, rs2, rf2 = normalise(h0, m0, s0, cur_f + 2)
        cues.append((rh2, rm2, rs2, rf2, base_dmx, 300,
                     f"{mm}:{ss:02d} FLICK recover → {base_dmx}"))

    # ── INJECTED FLICKER RANGES (continuous random flicker across a window) ──
    for start_mm, start_ss, end_mm, end_ss, lo, hi, gap_lo, gap_hi, fade_ms in FLICKER_RANGES:
        start_sec = start_mm * 60 + start_ss
        end_sec_  = end_mm   * 60 + end_ss
        if start_sec not in yavg_sm_map:
            continue
        cur_total_f = start_sec * 30
        end_total_f = end_sec_  * 30
        last_v = -999
        idx = 0
        while cur_total_f < end_total_f:
            sec_now = cur_total_f // 30
            for _ in range(6):
                v = rng.randint(lo, hi)
                if abs(v - last_v) >= max(2, (hi - lo) // 4):
                    break
            last_v = v
            f_in_sec = cur_total_f % 30
            h0, m0, s0 = sec_to_hms(sec_now)
            ah, am, as_, af = normalise(h0, m0, s0, f_in_sec)
            cues.append((ah, am, as_, af, v, fade_ms,
                         f"{start_mm}:{start_ss:02d}-{end_mm}:{end_ss:02d} FLICK_RNG#{idx} → {v}"))
            cur_total_f += rng.randint(gap_lo, gap_hi)
            idx += 1
        # Recover to whatever base would have been at the end of the window
        if end_sec_ in yavg_sm_map:
            base_end = compute_oscillating_dmx(end_sec_, yavg_sm_map[end_sec_])
            eh, em, es = sec_to_hms(end_sec_)
            cues.append((eh, em, es, 0, base_end, 400,
                         f"{end_mm}:{end_ss:02d} FLICK_RNG recover → {base_end}"))

    # Outro: bridge to SAFETY_DMX, then hold with tiny oscillation, then black
    eh, em, es = END_FADE_TIME
    end_start_sec = eh * 3600 + em * 60 + es
    cues.append((eh, em, es, 0, SAFETY_DMX, SAFETY_BRIDGE_MS,
                 f"safety hold bridge -> DMX {SAFETY_DMX} over {SAFETY_BRIDGE_MS//1000}s"))
    for sec in range(end_start_sec + SAFETY_EMIT_EVERY_SEC,
                     SAFETY_TAIL_END_SEC + 1,
                     SAFETY_EMIT_EVERY_SEC):
        osc = SAFETY_OSC_AMP * math.sin(2 * math.pi * sec / SAFETY_OSC_PERIOD_SEC)
        v   = int(round(SAFETY_DMX + osc))
        h, m, s = sec_to_hms(sec)
        cues.append((h, m, s, 0, v, SAFETY_FADE_MS,
                     f"safety hold osc -> DMX {v}"))
    bh, bm, bs = END_BLACK_TIME
    cues.append((bh, bm, bs, 0, 0, 0, "hard black at end"))

    # Sort by absolute frame time so injected sub-second cues land in order
    cues.sort(key=lambda c: (c[0]*3600 + c[1]*60 + c[2]) * 30 + c[3])
    return cues


def write_header(cues, output_path):
    lines = []
    lines.append("#pragma once")
    lines.append("// AUTO-GENERATED — do not edit by hand.")
    lines.append(f"// Generated: {datetime.utcnow().strftime('%Y-%m-%d %H:%M')} UTC  by gen_backlight_cues.py")
    lines.append("// Source: controlled_burn_luma.csv  metric: yavg_raw (smoothed)")
    lines.append("//")
    lines.append("// Varytec LED Theater Spot 50 3200K — INVERTED LUMA BACKLIGHT (REACTIVE)")
    lines.append("//   DMX channels 2 (Master) / 3 (Fine) / 4 (Strobe), 3-ch mode")
    lines.append("//   Brighter when the video is darker, dimmer when the video is bright.")
    lines.append(f"//   Inverse base : DMX [{BACKLIGHT_FLOOR}, {BACKLIGHT_BASE_CAP}]   (hard cap {BACKLIGHT_HARD_CAP})")
    lines.append("//   Breath/wobble amplitude AND period scale with darkness — pitch-dark video")
    lines.append(f"//   produces deep slow swings of ±{BREATH_AMP_MAX} which can push the lamp briefly above {BACKLIGHT_BASE_CAP}.")
    lines.append("//")
    lines.append("//   Layered oscillation:")
    lines.append(f"//     base    = inverse of {SMOOTH_WINDOW_SEC}-s smoothed yavg_raw")
    lines.append(f"//     breath  = sine, period {BREATH_PERIOD_BRIGHT_SEC:.1f}-{BREATH_PERIOD_DARK_SEC:.1f}s, amplitude ±{BREATH_AMP_MIN}-{BREATH_AMP_MAX} (darkness-driven)")
    lines.append(f"//     wobble  = sine, period {WOBBLE_PERIOD_SEC:.1f}s, amplitude ±{WOBBLE_AMP_MIN}-{WOBBLE_AMP_MAX} (darkness-driven)")
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
    for h, m, s, frames, dmx, fade, comment in cues:
        if m != last_min:
            if last_min is not None:
                lines.append("")
            lines.append(f"    // ── {h:02d}:{m:02d}:xx ──")
            last_min = m
        lines.append(f"    {{ {h:2d}, {m:2d}, {s:2d}, {frames:2d}, {fade:6d}, V({dmx:3d}, 0) }},   // {comment}")

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
    body_dmx = [c[4] for c in cues if c[5] == FADE_MS]
    print(f"  {len(cues)} backlight cues generated  ({len(body_dmx)} oscillation cues)")
    if body_dmx:
        print(f"  Body DMX range: {min(body_dmx)}-{max(body_dmx)}, mean {sum(body_dmx)/len(body_dmx):.1f}")

    write_header(cues, OUTPUT_PATH)
    print(f"  Written -> {OUTPUT_PATH}")
    print("Done.")


if __name__ == "__main__":
    main()
