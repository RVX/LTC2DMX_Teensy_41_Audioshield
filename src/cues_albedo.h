#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Cue list — ALBEDO  (43 min · 25fps)
// Varytec LED Theater Spot 50 3200K  (2-channel DMX mode)
// Julian Charrière · CORRER · Venice 2026
//
// Fixture: 1× COB LED 50W, 3200K warm white, Fresnel lens.
//          All 4 fixtures share DMX address 001, 2-channel mode.
//   CH 1 = Master dimmer  ( 0 = off, 255 = full )
//   CH 2 = Strobe         ( 0 = off, 4-255 = slow → fast )
//
// Format: { HH, MM, SS, FF, fadeMs, { {ch,val}, ... }, numChannels }
//
// ── COMPOSITION ─────────────────────────────────────────────────────────────
//
//   Superslow sea-wave breath: organic oscillation between 10% (26) and 80%
//   (204).  No strobes.  No sharp cuts.  Every cue fires exactly when the
//   previous fade completes — the fadeMs IS the wave motion itself.
//
//   Structural arc (bell curve):
//     00:00 – 04:46  Opening  — awakening, range  26– 85, period ~40–55 s
//     05:30 – 12:02  Growing  — expanding,  range  25–140, period ~40–55 s
//     12:59 – 20:22  Building — widening,   range  25–178, period ~43–58 s
//     21:19 – 28:32  Peak     — full range, range  25–204, period ~44–56 s
//     29:28 – 35:29  Receding — contracting,range  28–196, period ~46–60 s
//     36:29 – 41:45  Closing  — fading,     range  36–130, period ~50–56 s
//     41:45          Final 30 s fade to black
//
//   Stochastic variation: peak levels shift ±8–15 DMX / trough ±2–8 DMX /
//   fade durations vary ±4–16 s so no two waves feel identical.
// ─────────────────────────────────────────────────────────────────────────────

#include "dmx_controller.h"
#include "../include/config.h"

// Shorthand: V(dim, strobe) — 2 channels only
#define V(d, s) \
    { {DMX_CH_MASTER,(d)}, {DMX_CH_STROBE,(s)} }, 2

static const DMXCue CUE_LIST[] = {

    // ── OPENING: black → base level  (0:00 – 0:10) ──────────────────────────

    // 00:00:00:00  Hard black at show start
    { 0,  0,  0, 0,      0, V(  0, 0) },
    // 00:00:05:00  5 s fade-in to base level
    { 0,  0,  5, 0,   5000, V( 30, 0) },

    // ── SECTION 1: Awakening  (0:10 – 4:46)  range 26–85 ────────────────────
    //   Each cue fires the instant the previous fade completes.
    //   fadeMs = length of THIS cue's fade (= gap to next cue).

    // 00:00:10:00  First swell — 35 s rise → peak at 0:45
    { 0,  0, 10, 0,  35000, V( 65, 0) },
    // 00:00:45:00  Ebb — 48 s fall → trough at 1:33
    { 0,  0, 45, 0,  48000, V( 28, 0) },
    // 00:01:33:00  Swell — 45 s rise → peak at 2:18
    { 0,  1, 33, 0,  45000, V( 70, 0) },
    // 00:02:18:00  Ebb — 52 s fall → trough at 3:10
    { 0,  2, 18, 0,  52000, V( 25, 0) },
    // 00:03:10:00  Swell — 46 s rise → peak at 3:56
    { 0,  3, 10, 0,  46000, V( 72, 0) },
    // 00:03:56:00  Ebb — 50 s fall → trough at 4:46
    { 0,  3, 56, 0,  50000, V( 30, 0) },

    // ── SECTION 2: Growing  (4:46 – 12:02)  range 25–140 ────────────────────

    // 00:04:46:00  Swell — 44 s → peak at 5:30
    { 0,  4, 46, 0,  44000, V( 85, 0) },
    // 00:05:30:00  Ebb — 58 s → trough at 6:28
    { 0,  5, 30, 0,  58000, V( 26, 0) },
    // 00:06:28:00  Swell — 42 s → peak at 7:10
    { 0,  6, 28, 0,  42000, V(100, 0) },
    // 00:07:10:00  Ebb — 53 s → trough at 8:03
    { 0,  7, 10, 0,  53000, V( 25, 0) },
    // 00:08:03:00  Swell — 42 s → peak at 8:45
    { 0,  8,  3, 0,  42000, V(115, 0) },
    // 00:08:45:00  Ebb — 54 s → trough at 9:39
    { 0,  8, 45, 0,  54000, V( 26, 0) },
    // 00:09:39:00  Swell — 43 s → peak at 10:22
    { 0,  9, 39, 0,  43000, V(128, 0) },
    // 00:10:22:00  Ebb — 56 s → trough at 11:18
    { 0, 10, 22, 0,  56000, V( 25, 0) },
    // 00:11:18:00  Swell — 44 s → peak at 12:02
    { 0, 11, 18, 0,  44000, V(140, 0) },

    // ── SECTION 3: Building  (12:02 – 20:22)  range 25–178 ──────────────────

    // 00:12:02:00  Ebb — 57 s → trough at 12:59
    { 0, 12,  2, 0,  57000, V( 26, 0) },
    // 00:12:59:00  Swell — 43 s → peak at 13:42
    { 0, 12, 59, 0,  43000, V(152, 0) },
    // 00:13:42:00  Ebb — 58 s → trough at 14:40
    { 0, 13, 42, 0,  58000, V( 25, 0) },
    // 00:14:40:00  Swell — 44 s → peak at 15:24
    { 0, 14, 40, 0,  44000, V(160, 0) },
    // 00:15:24:00  Ebb — 54 s → trough at 16:18
    { 0, 15, 24, 0,  54000, V( 26, 0) },
    // 00:16:18:00  Swell — 45 s → peak at 17:03
    { 0, 16, 18, 0,  45000, V(168, 0) },
    // 00:17:03:00  Ebb — 56 s → trough at 17:59
    { 0, 17,  3, 0,  56000, V( 25, 0) },
    // 00:17:59:00  Swell — 44 s → peak at 18:43
    { 0, 17, 59, 0,  44000, V(172, 0) },
    // 00:18:43:00  Ebb — 55 s → trough at 19:38
    { 0, 18, 43, 0,  55000, V( 27, 0) },
    // 00:19:38:00  Swell — 44 s → peak at 20:22
    { 0, 19, 38, 0,  44000, V(178, 0) },

    // ── SECTION 4: Peak  (20:22 – 28:32)  range 25–204 ──────────────────────

    // 00:20:22:00  Ebb — 57 s → trough at 21:19
    { 0, 20, 22, 0,  57000, V( 25, 0) },
    // 00:21:19:00  Swell — 44 s → peak at 22:03
    { 0, 21, 19, 0,  44000, V(185, 0) },
    // 00:22:03:00  Ebb — 54 s → trough at 22:57
    { 0, 22,  3, 0,  54000, V( 25, 0) },
    // 00:22:57:00  Swell — 44 s → peak at 23:41
    { 0, 22, 57, 0,  44000, V(192, 0) },
    // 00:23:41:00  Ebb — 53 s → trough at 24:34
    { 0, 23, 41, 0,  53000, V( 26, 0) },
    // 00:24:34:00  Swell — 44 s → peak at 25:18
    { 0, 24, 34, 0,  44000, V(196, 0) },
    // 00:25:18:00  Ebb — 52 s → trough at 26:10
    { 0, 25, 18, 0,  52000, V( 25, 0) },
    // 00:26:10:00  Swell — 44 s → peak at 26:54
    { 0, 26, 10, 0,  44000, V(200, 0) },
    // 00:26:54:00  Ebb — 54 s → trough at 27:48
    { 0, 26, 54, 0,  54000, V( 26, 0) },
    // 00:27:48:00  Swell — 44 s → peak (maximum: 80%) at 28:32
    { 0, 27, 48, 0,  44000, V(204, 0) },

    // ── SECTION 5: Receding  (28:32 – 35:29)  range 28–196 ──────────────────

    // 00:28:32:00  Ebb — 56 s → trough at 29:28
    { 0, 28, 32, 0,  56000, V( 25, 0) },
    // 00:29:28:00  Swell — 46 s → peak at 30:14
    { 0, 29, 28, 0,  46000, V(196, 0) },
    // 00:30:14:00  Ebb — 58 s → trough at 31:12
    { 0, 30, 14, 0,  58000, V( 28, 0) },
    // 00:31:12:00  Swell — 46 s → peak at 31:58
    { 0, 31, 12, 0,  46000, V(182, 0) },
    // 00:31:58:00  Ebb — 58 s → trough at 32:56
    { 0, 31, 58, 0,  58000, V( 30, 0) },
    // 00:32:56:00  Swell — 46 s → peak at 33:42
    { 0, 32, 56, 0,  46000, V(168, 0) },
    // 00:33:42:00  Ebb — 59 s → trough at 34:41
    { 0, 33, 42, 0,  59000, V( 32, 0) },
    // 00:34:41:00  Swell — 48 s → peak at 35:29
    { 0, 34, 41, 0,  48000, V(152, 0) },

    // ── SECTION 6: Closing  (35:29 – 41:45)  range 36–130 ───────────────────

    // 00:35:29:00  Ebb — 60 s → trough at 36:29
    { 0, 35, 29, 0,  60000, V( 34, 0) },
    // 00:36:29:00  Swell — 50 s → peak at 37:19
    { 0, 36, 29, 0,  50000, V(130, 0) },
    // 00:37:19:00  Ebb — 56 s → trough at 38:15
    { 0, 37, 19, 0,  56000, V( 36, 0) },
    // 00:38:15:00  Swell — 50 s → peak at 39:05
    { 0, 38, 15, 0,  50000, V(105, 0) },
    // 00:39:05:00  Ebb — 55 s → trough at 40:00
    { 0, 39,  5, 0,  55000, V( 38, 0) },
    // 00:40:00:00  Last swell — 50 s → peak at 40:50
    { 0, 40,  0, 0,  50000, V( 75, 0) },
    // 00:40:50:00  Final ebb — 55 s → trough at 41:45
    { 0, 40, 50, 0,  55000, V( 40, 0) },

    // ── FINAL FADE TO BLACK  (41:45 – 42:15) ────────────────────────────────

    // 00:41:45:00  30 s fade to black — completes at 42:15, leaves 45 s dark
    { 0, 41, 45, 0,  30000, V(  0, 0) },

};  // END CUE_LIST

#undef V

static const uint16_t CUE_COUNT = sizeof(CUE_LIST) / sizeof(CUE_LIST[0]);
