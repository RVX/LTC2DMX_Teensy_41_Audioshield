#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
analyze_movie_luma.py  --  Video luminance analysis for LTC2DMX cue composition
─────────────────────────────────────────────────────────────────────────────────
Extracts per-second Y-luma from a film via FFmpeg (scale=1:1 area average),
then plots it alongside the current DMX cue arc parsed from the matching .h file.

Usage:
    python analyze_movie_luma.py --comp albedo
    python analyze_movie_luma.py --comp controlled_burn

Outputs (project root):
    {comp}_luma.csv   — second, timecode, yavg_raw, yavg_0_255
    {comp}_luma.png   — dark plot: gold = video luma, blue = DMX arc

Compositional strategy documented in README.md:
    FOLLOW      — DMX rises with film's bright clusters  (reinforcement)
    COUNTERPOINT— DMX glows during film's dark valleys   (opposition)
"""

import subprocess, re, csv, sys, argparse
from pathlib import Path

# ── Config per composition ──────────────────────────────────────────────────
PROJECT = Path(r"C:\Users\ubema\OneDrive\Documents\0_ART_COMMISIONS\Julian_Charriere\2026_CORRER_VENICE\LTC_READER_TENNSY")

COMPOSITIONS = {
    "albedo": {
        "movie":      PROJECT / "Movies" / "v02_260415_Albedo_Correr_Preview.mov",
        "cues_h":     PROJECT / "src"    / "cues_albedo.h",
        "framerate":  25,
        "total_sec":  2580,          # 43:00
        "apex_sec":   27*60 + 58,    # 0:27:58
        "label":      "ALBEDO  43 min · 25 fps",
    },
    "controlled_burn": {
        "movie":      PROJECT / "Movies" / "2604_ControlledBurn_Correr_UHD_LTC_5-1_30fps_v3_Preview.mov",
        "cues_h":     PROJECT / "src"    / "cues_control_burn.h",
        "framerate":  30,
        "total_sec":  1920,          # 32:00
        "apex_sec":   None,          # update once cue arc is written
        "label":      "CONTROLLED BURN  32 min · 30 fps",
    },
}

# Rec.709 limited range
Y_BLACK = 16
Y_WHITE = 235

# Frame size for spatial luma sampling.
# 32×18 = 576 pixels per frame — enough to compute percentiles while staying fast.
# This captures small bright regions (e.g. glacial specular flares) that the
# old 1×1 area-average would drown in surrounding darkness.
FRAME_W = 32
FRAME_H = 18
FRAME_BYTES = FRAME_W * FRAME_H   # 576 bytes per frame

# ── 1. FFmpeg luma extraction ───────────────────────────────────────────────
def extract_luma(movie_path):
    """
    Scale each frame to 32×18 gray at 1 fps, pipe as raw bytes.
    Per frame (576 px) compute: mean, 95th-percentile, max.
    Returns list of (second_index, yavg, yp95, ymax) tuples.
    """
    import numpy as np
    cmd = [
        "ffmpeg", "-hide_banner", "-i", str(movie_path),
        "-vf", f"fps=1,scale={FRAME_W}:{FRAME_H}:flags=area,format=gray",
        "-f", "rawvideo", "pipe:1"
    ]
    print(f"→ Decoding {movie_path.name}")
    print(f"  Extracting {FRAME_W}x{FRAME_H} gray frames at 1fps — mean + p95 + max per second…")
    proc = subprocess.run(cmd, capture_output=True)

    raw = proc.stdout
    n_frames = len(raw) // FRAME_BYTES
    rows = []
    for i in range(n_frames):
        frame = np.frombuffer(raw[i*FRAME_BYTES:(i+1)*FRAME_BYTES], dtype=np.uint8)
        rows.append((
            i,
            float(np.mean(frame)),
            float(np.percentile(frame, 95)),
            float(np.max(frame)),
        ))
    print(f"  → {len(rows)} seconds extracted")
    return rows


# ── 2. Parse src/cues_*.h → DMX arc ────────────────────────────────────────
def parse_cues(cues_h_path, framerate):
    """
    Parse lines of the form:
        { H, M, S, F, fadeMs, V(dmx, strobe) }
    Returns list of (trigger_sec_float, dmx_target, fade_ms) sorted by time.
    """
    pat = re.compile(
        r"\{\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*V\(\s*(\d+),\s*(\d+)\s*\)"
    )
    cues = []
    for line in cues_h_path.read_text(encoding="utf-8").splitlines():
        m = pat.search(line)
        if m:
            h, mn, s, f, fade_ms, dmx, _strobe = [int(x) for x in m.groups()]
            t = h * 3600 + mn * 60 + s + f / framerate
            cues.append((t, dmx, fade_ms))
    return sorted(cues, key=lambda c: c[0])


def simulate_dmx_arc(cues, total_sec):
    """
    Simulate the DMX master channel value at each integer second.
    Each cue linearly fades FROM the value at trigger time TO the target
    over fade_ms milliseconds.
    """
    arc = [0.0] * (total_sec + 1)

    # running state: value & fade in progress at start of each cue
    run_val    = 0.0
    run_start  = 0.0
    run_target = 0.0
    run_fade   = 0.0     # seconds

    def val_at(t):
        elapsed = t - run_start
        if run_fade <= 0:
            return run_target
        return run_val + (run_target - run_val) * min(1.0, elapsed / run_fade)

    for i, (trigger, target, fade_ms) in enumerate(cues):
        start_val  = val_at(trigger)
        fade_sec   = fade_ms / 1000.0
        next_trig  = cues[i + 1][0] if i + 1 < len(cues) else float(total_sec)

        lo = max(0, int(trigger))
        hi = min(total_sec, int(next_trig))
        for sec in range(lo, hi + 1):
            elapsed = sec - trigger
            if fade_sec > 0:
                arc[sec] = start_val + (target - start_val) * min(1.0, elapsed / fade_sec)
            else:
                arc[sec] = target

        # Advance running state for the next iteration
        run_val    = start_val
        run_start  = trigger
        run_target = target
        run_fade   = fade_sec

    return arc


# ── 3. Main ─────────────────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(description="Video luma vs DMX arc analyser")
    parser.add_argument("--comp", choices=list(COMPOSITIONS.keys()),
                        default="albedo",
                        help="Which composition to analyse (default: albedo)")
    parser.add_argument("--output", default=None,
                        help="Override output PNG filename (relative to project root)")
    parser.add_argument("--apex", type=int, default=None,
                        help="Override DMX apex time in seconds (e.g. 1890 for 31:30)")
    parser.add_argument("--reextract", action="store_true",
                        help="Force re-extraction from movie even if CSV exists")
    args = parser.parse_args()

    cfg       = COMPOSITIONS[args.comp]
    MOVIE     = cfg["movie"]
    CUES_H    = cfg["cues_h"]
    FRAMERATE = cfg["framerate"]
    TOTAL_SEC = cfg["total_sec"]
    APEX_SEC  = args.apex if args.apex is not None else cfg["apex_sec"]
    LABEL     = cfg["label"]
    OUT_CSV   = PROJECT / f"{args.comp}_luma.csv"
    OUT_PNG   = PROJECT / (args.output if args.output else f"{args.comp}_luma.png")

    # ── Luma extraction (skip if CSV exists unless --reextract) ──────────
    if OUT_CSV.exists() and not args.reextract:
        print(f"→ Using existing CSV: {OUT_CSV}")
        luma_rows = []
        import csv as _csv
        with open(OUT_CSV, newline="") as f:
            reader = _csv.DictReader(f)
            for row in reader:
                yavg = float(row["yavg_raw"])
                yp95 = float(row["yp95_raw"]) if "yp95_raw" in row else yavg
                ymax = float(row["ymax_raw"]) if "ymax_raw" in row else yavg
                luma_rows.append((int(row["second"]), yavg, yp95, ymax))
        print(f"  → {len(luma_rows)} seconds loaded from CSV")
        has_spatial = "yp95_raw" in open(OUT_CSV).readline()
        if not has_spatial:
            print("  NOTE: CSV has only mean luma — re-run with --reextract to get p95/max hot-spot data")
    else:
        if not MOVIE.exists():
            sys.exit(f"ERROR: movie not found and no CSV to fall back on:\n  {MOVIE}")
        luma_rows = extract_luma(MOVIE)
        if not luma_rows:
            sys.exit("ERROR: No luma data extracted. Check FFmpeg is on PATH.")

    # Rec.709 limited: 16=black, 235=white. Normalise to 0–255 for DMX comparison.
    def norm(y):
        return max(0.0, min(255.0, (y - Y_BLACK) / (Y_WHITE - Y_BLACK) * 255.0))

    # luma_rows: (sec, yavg_raw, yp95_raw, ymax_raw)
    luma_norm = [
        (sec, float(ya), norm(ya), float(yp), norm(yp), float(ym), norm(ym))
        for sec, ya, yp, ym in luma_rows
    ]
    # columns: sec, ya_raw, ya_norm, yp_raw, yp_norm, ym_raw, ym_norm

    # ── Write CSV (only when freshly extracted) ──────────────────────────
    if args.reextract or not OUT_CSV.exists():
        with open(OUT_CSV, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(["second", "timecode",
                        "yavg_raw", "yavg_0_255",
                        "yp95_raw", "yp95_0_255",
                        "ymax_raw", "ymax_0_255"])
            for sec, ya, ya_n, yp, yp_n, ym, ym_n in luma_norm:
                h, rem = divmod(sec, 3600)
                mn, s  = divmod(rem, 60)
                w.writerow([sec, f"{h:02d}:{mn:02d}:{s:02d}",
                            f"{ya:.2f}", f"{ya_n:.1f}",
                            f"{yp:.2f}", f"{yp_n:.1f}",
                            f"{ym:.2f}", f"{ym_n:.1f}"])
    print(f"CSV → {OUT_CSV}")

    # ── Parse cues & simulate DMX arc ────────────────────────────────────
    if CUES_H.exists():
        cues    = parse_cues(CUES_H, FRAMERATE)
        dmx_arc = simulate_dmx_arc(cues, TOTAL_SEC)
        print(f"Parsed {len(cues)} cues from {CUES_H.name}")
    else:
        print(f"NOTE: {CUES_H.name} not found — DMX arc will be empty")
        cues    = []
        dmx_arc = [0] * (TOTAL_SEC + 1)

    # ── Plot ──────────────────────────────────────────────────────────────
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    import matplotlib.ticker as ticker

    BG       = "#111111"
    AX       = "#1a1a1a"
    COL_LUMA = "#f0d080"   # warm gold  — video luma
    COL_DMX  = "#4fc3f7"   # cool blue  — DMX arc

    fig, ax = plt.subplots(figsize=(42, 9))
    fig.patch.set_facecolor(BG)
    ax.set_facecolor(AX)

    COL_P95  = "#ff8c42"   # orange — p95 hot spots
    COL_PMAX = "#ff4444"   # red    — per-frame max

    seconds   = [r[0] for r in luma_norm]
    luma_vals = [r[2] for r in luma_norm]    # yavg normalised 0–255
    p95_vals  = [r[4] for r in luma_norm]    # yp95 normalised 0–255
    pmax_vals = [r[6] for r in luma_norm]    # ymax normalised 0–255

    # Mean — background fill
    ax.fill_between(seconds, luma_vals, alpha=0.12, color=COL_LUMA)
    ax.plot(seconds, luma_vals,
            color=COL_LUMA, linewidth=0.8, alpha=0.7,
            label="Mean luma (frame avg)", zorder=3)

    # p95 — bright hot-spots within frame
    ax.fill_between(seconds, p95_vals, alpha=0.10, color=COL_P95)
    ax.plot(seconds, p95_vals,
            color=COL_P95, linewidth=1.2,
            label="p95 luma (bright area, 5% of frame)", zorder=4)

    # pmax — absolute brightest pixel per second
    ax.plot(seconds, pmax_vals,
            color=COL_PMAX, linewidth=0.6, alpha=0.55, linestyle="-",
            label="Max luma (brightest pixel)", zorder=4)

    if cues:
        dmx_secs = list(range(TOTAL_SEC + 1))
        ax.fill_between(dmx_secs, dmx_arc, alpha=0.10, color=COL_DMX)
        ax.plot(dmx_secs, dmx_arc,
                color=COL_DMX, linewidth=1.6, label="DMX CH1  (current cue arc)", zorder=4)

    # Film brightest moment marker (based on p95, not mean)
    if p95_vals:
        film_peak_sec = luma_norm[p95_vals.index(max(p95_vals))][0]
        h, r2 = divmod(film_peak_sec, 3600); mn, s = divmod(r2, 60)
        ax.axvline(x=film_peak_sec, color=COL_P95, linewidth=0.8, linestyle=":", alpha=0.7)
        ax.text(film_peak_sec + 6, 248, f"Film peak (p95)  {h:02d}:{mn:02d}:{s:02d}",
                color=COL_P95, fontsize=8, alpha=0.9)

    # DMX apex marker
    if APEX_SEC is not None:
        ax.axvline(x=APEX_SEC, color="#ff6e6e", linewidth=0.8, linestyle="--", alpha=0.6)
        h, r2 = divmod(APEX_SEC, 3600); mn, s = divmod(r2, 60)
        ax.text(APEX_SEC + 6, 232, f"DMX apex  {h:02d}:{mn:02d}:{s:02d}",
                color="#ff6e6e", fontsize=8, alpha=0.8)

    ax.set_xlim(0, TOTAL_SEC)
    ax.set_ylim(0, 270)
    ax.set_xlabel("Time  (MM:SS)", color="#aaaaaa", fontsize=11)
    ax.set_ylabel("Value  (0 – 255)", color="#aaaaaa", fontsize=11)
    ax.set_title(
        f"{LABEL}  —  Video Luminance  vs.  DMX Cue Arc\n{MOVIE.name}",
        color="#ffffff", fontsize=13, pad=14
    )
    ax.tick_params(colors="#666666")
    for spine in ax.spines.values():
        spine.set_color("#333333")

    def fmt_mmss(x, _):
        mn, s = divmod(int(x), 60)
        return f"{mn:02d}:{s:02d}"

    ax.xaxis.set_major_formatter(ticker.FuncFormatter(fmt_mmss))
    ax.xaxis.set_major_locator(ticker.MultipleLocator(20))    # every 20 s   (labeled)
    ax.xaxis.set_minor_locator(ticker.MultipleLocator(10))    # every 10 s   (tick only)
    ax.yaxis.set_major_locator(ticker.MultipleLocator(10))    # every 10 DMX (labeled)
    ax.yaxis.set_minor_locator(ticker.MultipleLocator(5))     # every 5 DMX  (tick only)
    # major grid
    ax.grid(True, which="major", color="#2d2d2d", linewidth=0.6, zorder=0)
    # minor grid — very faint
    ax.grid(True, which="minor", color="#1f1f1f", linewidth=0.25, zorder=0)
    ax.tick_params(axis="x", which="major", labelsize=6.5, rotation=60)
    ax.tick_params(axis="x", which="minor", length=3,   labelsize=0)
    ax.tick_params(axis="y", which="major", labelsize=7.5)
    ax.tick_params(axis="y", which="minor", length=3,   labelsize=0)

    ax.legend(facecolor="#222222", edgecolor="#444444",
              labelcolor="#cccccc", fontsize=10, loc="upper left")

    plt.tight_layout()
    plt.savefig(str(OUT_PNG), dpi=150, facecolor=fig.get_facecolor())
    print(f"Plot → {OUT_PNG}")
    plt.close()

    # ── Quick stats ──────────────────────────────────────────────────────
    if luma_vals:
        def tc(sec):
            h, r = divmod(sec, 3600); mn, s = divmod(r, 60); return f"{h:02d}:{mn:02d}:{s:02d}"
        brightest_p95 = max(luma_norm, key=lambda r: r[4])
        brightest_avg = max(luma_norm, key=lambda r: r[2])
        print(f"\nVideo luma statistics — {LABEL}:")
        print(f"  mean avg   min={min(luma_vals):.1f}  max={max(luma_vals):.1f}  mean={sum(luma_vals)/len(luma_vals):.1f}")
        print(f"  p95  avg   min={min(p95_vals):.1f}  max={max(p95_vals):.1f}  mean={sum(p95_vals)/len(p95_vals):.1f}")
        print(f"  brightest frame (p95)  {tc(brightest_p95[0])}  ({brightest_p95[4]:.1f}/255)")
        print(f"  brightest frame (avg)  {tc(brightest_avg[0])}  ({brightest_avg[2]:.1f}/255)")

        # Bright clusters — use p95 with lower threshold to catch hot spots
        print(f"\nBright clusters (p95 > 40):")
        in_b, b_start, b_peak = False, 0, 0.0
        for row in luma_norm:
            sec = row[0]; y = row[4]  # yp95_norm
            if y > 40 and not in_b:
                in_b, b_start, b_peak = True, sec, y
            elif in_b:
                if y > b_peak: b_peak = y
                if y <= 40:
                    print(f"  {tc(b_start)} -> {tc(sec-1)}  peak {b_peak:.0f}")
                    in_b = False
        if in_b:
            print(f"  {tc(b_start)} -> {tc(luma_norm[-1][0])}  peak {b_peak:.0f}")

        # Dark valleys (mean luma < 3, >= 30 s)
        print(f"\nSustained dark valleys (mean luma < 3, >= 30 s):")
        in_d, d_start = False, 0
        for row in luma_norm:
            sec = row[0]; y = row[2]  # yavg_norm
            if y < 3 and not in_d:
                in_d, d_start = True, sec
            elif y >= 3 and in_d:
                dur = sec - d_start
                if dur >= 30:
                    print(f"  {tc(d_start)} -> {tc(sec-1)}  ({dur} s)")
                in_d = False


if __name__ == "__main__":
    main()
