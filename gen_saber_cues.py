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
ZONE_B = (1474, 1824)

# ── Envelope follower parameters ─────────────────────────────────────────────
# Attack: instant — lamp snaps to every p99 peak immediately (0 ms fade).
# Release: linear decay at RELEASE_RATE DMX units per second.
#   RELEASE_RATE = 80  →  255→0 in ~3.2 s  (snappy, stays within burst gaps)
#   RELEASE_RATE = 50  →  255→0 in ~5.1 s  (longer warm glow between bursts)
ATTACK_MS    = 0    # ms — instant snap on rising edge
RELEASE_RATE = 80   # DMX units / second — linear decay on falling edge

# ── Cue filter ────────────────────────────────────────────────────────────────
DELTA_THRESH  = 3    # minimum DMX change to emit a new cue


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
        if target >= prev:
            # Attack: snap to peak
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
    Envelope-follower cue generator.

    Strategy:
      1. Pre-compute the full envelope (fast attack, slow release).
      2. Scan for transitions:
           ATTACK  — envelope rises  → emit snap cue (0 ms) at the peak.
           RELEASE — envelope starts falling → emit ONE fade-to-0 cue
                      with fadeMs = current_dmx / RELEASE_RATE * 1000.
                      A subsequent attack cue will interrupt the fade.
      3. Emit no cue during the release glide (the hardware fade handles it).
    """
    env = compute_envelope(data)
    env_secs = sorted(env.keys())

    cues = []
    cues.append((0, 0, 0, 0, 0, "hard black at start"))

    last_dmx   = 0   # last emitted DMX target
    in_release = False

    for sec in env_secs:
        env_val = env[sec]

        if env_val > last_dmx + DELTA_THRESH:
            # ── ATTACK: rising edge ──────────────────────────────────────────
            h, m, s = sec_to_hms(sec)
            cues.append((h, m, s, ATTACK_MS, env_val,
                         f"{sec//60}:{sec%60:02d} attack→dmx{env_val}"))
            last_dmx   = env_val
            in_release = False

        elif env_val < last_dmx - DELTA_THRESH and not in_release:
            # ── RELEASE: start of descent ────────────────────────────────────
            # Emit a single linear fade from current value to 0.
            # Duration = time for RELEASE_RATE to drain last_dmx to 0.
            fade_ms = int(last_dmx / RELEASE_RATE * 1000)
            h, m, s  = sec_to_hms(sec)
            cues.append((h, m, s, fade_ms, 0,
                         f"{sec//60}:{sec%60:02d} release dmx{last_dmx}→0 over {last_dmx/RELEASE_RATE:.1f}s"))
            last_dmx   = 0   # fading to 0; next attack overrides
            in_release = True

        # While in release (env still decaying), skip — hardware fade does it.
        # in_release is cleared only on next attack.

    # Hard black at end of composition
    cues.append((0, 31, 38, 0, 0, "hard black at end"))

    return cues


def write_header(cues, output_path: str):
    lines = []
    lines.append("#pragma once")
    lines.append("// AUTO-GENERATED — do not edit by hand.")
    lines.append(f"// Generated: {datetime.utcnow().strftime('%Y-%m-%d %H:%M')} UTC  by gen_saber_cues.py")
    lines.append("// Source: controlled_burn_luma.csv  metric: yp99_raw")
    lines.append("//")
    lines.append("// ADJ Saber Spot WW — ENVELOPE FOLLOWER on p99")
    lines.append(f"//   DMX address {SABER_CH} · 1-channel mode (CH{SABER_CH} = dimmer, 0–255)")
    lines.append("//   Attack  : instant snap (0 ms) to every p99 peak.")
    lines.append(f"//   Release : linear decay at {RELEASE_RATE} DMX/s  (≈{255/RELEASE_RATE:.1f}s from DMX 255 to 0).")
    lines.append("//   One fade-to-0 cue emitted at start of each release phase;")
    lines.append("//   subsequent attack cue overrides the ongoing fade on hardware.")    
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
            f"    {{ {h:2d}, {m:2d}, {s:2d},  0,  {fade:6d}, W({dmx:3d}) }},   // {comment}"
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
