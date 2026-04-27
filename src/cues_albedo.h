#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Cue list — ALBEDO  (43 min · 25fps)
// Varytec LED Theater Spot 50 3200K  (2-channel DMX mode)
// Julian Charrière · CORRER · Venice 2026
//
//   Fixture: 4× Varytec LED Theater Spot 50, 3200K warm white, 2-ch DMX mode.
//            All 4 at DMX address 001 — all fire identically.
//   CH 1 = Master dimmer  ( 0 = off, 255 = full )
//   CH 2 = Strobe         ( 0 = off, 4 = slowest, 255 = fastest )
//
//   !! IMPORTANT — fixture minimum visible threshold !!
//      The Varytec LED Theater Spot 50 COB LED requires ≥ DMX 22 on CH1
//      before any light is visible. Below 22 the lamp is physically dark.
//      → trough floor = DMX 40 (~16%) — always visibly glowing warm
//      → DMX 0 only at the very final fade-out
//
// Format: { HH, MM, SS, FF, fadeMs, { {ch,val}, ... }, numChannels }
//
// ── COMPOSITION — ALBEDO ────────────────────────────────────────────────────
//
//   ALBEDO: the fraction of incident light reflected by a surface.
//   Here: reflected sunlight inside a white body — glacier, ice, snow.
//   The surface always holds some light. It breathes. It never fully dies.
//
//   20 asymmetric sea-waves, continuous from 0:00:00.
//   No hard cuts. No strobes. No silence. No darkness.
//
//   Wave shape: asymmetric — fast flood (~25%), slow ebb (~65%).
//   Each wave has 4 control points: rise → surge → shoulder → ebb.
//   Floor: DMX 40 throughout (well above fixture threshold).
//   Apex break: snaps to floor (DMX 40), not to black.
//
//   Arc:
//     0:00 –  6:12  Awakening  — floor 40, peaks 78→102
//     6:12 – 15:17  Growing    — floor 40, peaks 102→142
//    15:17 – 24:55  Building   — floor 40, peaks 142→185
//    24:55 – 29:45  Apex zone  — floor 40, peaks 185→235 (92%)
//                              apex REACHED at 0:27:58 — break to DMX 40
//    29:45 – 38:00  Recession  — floor 40→45, peaks 215→140
//    38:00 – 42:32  Closing    — floor 48→52, peaks 140→82
//    42:32 –        Final 48 s fade to black
// ─────────────────────────────────────────────────────────────────────────────

#include "dmx_controller.h"
#include "../include/config.h"

#define V(d, s) \
    { {DMX_CH_MASTER,(d)}, {DMX_CH_STROBE,(s)} }, 2

static const DMXCue CUE_LIST[] = {

    // ── 0:00:00  GENTLE OPEN ────────────────────────────────────────────────
    { 0,  0,  0, 0,  10000, V( 40, 0) },   // 10s warm fade-in to floor (→ 0:00:10)

    // ── AWAKENING  0:00:10 – 6:12  (floor 40, peaks 78→102) ─────────────────

    // Wave 1  (0:00:10 → trough 0:02:05)  period 115s
    { 0,  0, 10, 0,  28000, V( 62, 0) },   // 28s rise (→ 0:00:38)
    { 0,  0, 38, 0,  12000, V( 78, 0) },   // 12s surge to peak (→ 0:00:50)
    { 0,  0, 50, 0,   8000, V( 68, 0) },   // 8s shoulder (→ 0:00:58)
    { 0,  0, 58, 0,  67000, V( 40, 0) },   // 67s ebb to floor (→ 0:02:05)

    // Wave 2  (0:02:05 → trough 0:04:05)  period 120s
    { 0,  2,  5, 0,  28000, V( 68, 0) },   // 28s rise (→ 0:02:33)
    { 0,  2, 33, 0,  12000, V( 85, 0) },   // 12s surge to peak (→ 0:02:45)
    { 0,  2, 45, 0,   8000, V( 75, 0) },   // 8s shoulder (→ 0:02:53)
    { 0,  2, 53, 0,  72000, V( 40, 0) },   // 72s ebb to floor (→ 0:04:05)

    // Wave 3  (0:04:05 → trough 0:06:12)  period 127s
    { 0,  4,  5, 0,  28000, V( 72, 0) },   // 28s rise (→ 0:04:33)
    { 0,  4, 33, 0,  13000, V( 92, 0) },   // 13s surge to peak (→ 0:04:46)
    { 0,  4, 46, 0,   8000, V( 82, 0) },   // 8s shoulder (→ 0:04:54)
    { 0,  4, 54, 0,  78000, V( 40, 0) },   // 78s ebb to floor (→ 0:06:12)

    // ── GROWING  6:12 – 15:17  (floor 40, peaks 102→142) ────────────────────

    // Wave 4  (0:06:12 → trough 0:08:22)  period 130s
    { 0,  6, 12, 0,  30000, V( 80, 0) },   // 30s rise (→ 0:06:42)
    { 0,  6, 42, 0,  14000, V(102, 0) },   // 14s surge to peak (→ 0:06:56)
    { 0,  6, 56, 0,   9000, V( 92, 0) },   // 9s shoulder (→ 0:07:05)
    { 0,  7,  5, 0,  77000, V( 40, 0) },   // 77s ebb to floor (→ 0:08:22)

    // Wave 5  (0:08:22 → trough 0:10:37)  period 135s
    { 0,  8, 22, 0,  30000, V( 88, 0) },   // 30s rise (→ 0:08:52)
    { 0,  8, 52, 0,  14000, V(112, 0) },   // 14s surge to peak (→ 0:09:06)
    { 0,  9,  6, 0,   9000, V(102, 0) },   // 9s shoulder (→ 0:09:15)
    { 0,  9, 15, 0,  82000, V( 40, 0) },   // 82s ebb to floor (→ 0:10:37)

    // Wave 6  (0:10:37 → trough 0:12:54)  period 137s
    { 0, 10, 37, 0,  30000, V( 95, 0) },   // 30s rise (→ 0:11:07)
    { 0, 11,  7, 0,  16000, V(122, 0) },   // 16s surge to peak (→ 0:11:23)
    { 0, 11, 23, 0,   9000, V(112, 0) },   // 9s shoulder (→ 0:11:32)
    { 0, 11, 32, 0,  82000, V( 40, 0) },   // 82s ebb to floor (→ 0:12:54)

    // Wave 7  (0:12:54 → trough 0:15:17)  period 143s
    { 0, 12, 54, 0,  32000, V(105, 0) },   // 32s rise (→ 0:13:26)
    { 0, 13, 26, 0,  16000, V(132, 0) },   // 16s surge to peak (→ 0:13:42)
    { 0, 13, 42, 0,   9000, V(122, 0) },   // 9s shoulder (→ 0:13:51)
    { 0, 13, 51, 0,  86000, V( 40, 0) },   // 86s ebb to floor (→ 0:15:17)

    // ── BUILDING  15:17 – 24:55  (floor 40, peaks 142→185) ──────────────────

    // Wave 8  (0:15:17 → trough 0:17:45)  period 148s
    { 0, 15, 17, 0,  32000, V(112, 0) },   // 32s rise (→ 0:15:49)
    { 0, 15, 49, 0,  18000, V(142, 0) },   // 18s surge to peak (→ 0:16:07)
    { 0, 16,  7, 0,  10000, V(132, 0) },   // 10s shoulder (→ 0:16:17)
    { 0, 16, 17, 0,  88000, V( 40, 0) },   // 88s ebb to floor (→ 0:17:45)

    // Wave 9  (0:17:45 → trough 0:20:10)  period 145s
    { 0, 17, 45, 0,  32000, V(118, 0) },   // 32s rise (→ 0:18:17)
    { 0, 18, 17, 0,  18000, V(152, 0) },   // 18s surge to peak (→ 0:18:35)
    { 0, 18, 35, 0,  10000, V(142, 0) },   // 10s shoulder (→ 0:18:45)
    { 0, 18, 45, 0,  85000, V( 40, 0) },   // 85s ebb to floor (→ 0:20:10)

    // Wave 10  (0:20:10 → trough 0:22:35)  period 145s
    { 0, 20, 10, 0,  32000, V(125, 0) },   // 32s rise (→ 0:20:42)
    { 0, 20, 42, 0,  18000, V(162, 0) },   // 18s surge to peak (→ 0:21:00)
    { 0, 21,  0, 0,  10000, V(152, 0) },   // 10s shoulder (→ 0:21:10)
    { 0, 21, 10, 0,  85000, V( 40, 0) },   // 85s ebb to floor (→ 0:22:35)

    // Wave 11  (0:22:35 → trough 0:24:55)  period 140s
    { 0, 22, 35, 0,  30000, V(132, 0) },   // 30s rise (→ 0:23:05)
    { 0, 23,  5, 0,  18000, V(172, 0) },   // 18s surge to peak (→ 0:23:23)
    { 0, 23, 23, 0,  10000, V(162, 0) },   // 10s shoulder (→ 0:23:33)
    { 0, 23, 33, 0,  82000, V( 40, 0) },   // 82s ebb to floor (→ 0:24:55)

    // ── APEX ZONE  24:55 – 29:45  (floor 40, peaks 185→235) ─────────────────

    // Wave 12  (0:24:55 → trough 0:27:00)  period 125s — shorter, rising urgency
    { 0, 24, 55, 0,  26000, V(145, 0) },   // 26s rise (→ 0:25:21)
    { 0, 25, 21, 0,  18000, V(185, 0) },   // 18s surge to peak (→ 0:25:39)
    { 0, 25, 39, 0,  10000, V(175, 0) },   // 10s shoulder (→ 0:25:49)
    { 0, 25, 49, 0,  71000, V( 40, 0) },   // 71s ebb to floor (→ 0:27:00)

    // Wave 13 — ███ APEX ███  (0:27:00 → trough 0:29:45)
    //   235 DMX reached at exactly 0:27:58
    { 0, 27,  0, 0,  22000, V(178, 0) },   // 22s fast rise from floor (→ 0:27:22)
    { 0, 27, 22, 0,  16000, V(212, 0) },   // 16s surge (→ 0:27:38)
    { 0, 27, 38, 0,  20000, V(235, 0) },   // 20s ██ APEX 92% — reaches 235 at 0:27:58 ██
    { 0, 27, 58, 0,  15000, V(228, 0) },   // 15s hold/shoulder at peak (→ 0:28:13)
    { 0, 28, 13, 0,  10000, V( 40, 0) },   // 10s SNAP to floor — THE BREAK (→ 0:28:23)
    { 0, 28, 23, 0,  15000, V(210, 0) },   // 15s aftershock surge (→ 0:28:38)
    { 0, 28, 38, 0,  67000, V( 40, 0) },   // 67s long withdrawal to floor (→ 0:29:45)

    // ── RECESSION  29:45 – 38:00  (floor 40→45, peaks 215→140) ─────────────

    // Wave 14  (0:29:45 → trough 0:31:45)  period 120s
    { 0, 29, 45, 0,  26000, V(182, 0) },   // 26s rise (→ 0:30:11)
    { 0, 30, 11, 0,  18000, V(215, 0) },   // 18s to post-apex peak (→ 0:30:29)
    { 0, 30, 29, 0,  10000, V(205, 0) },   // 10s shoulder (→ 0:30:39)
    { 0, 30, 39, 0,  66000, V( 40, 0) },   // 66s ebb (→ 0:31:45)

    // Wave 15  (0:31:45 → trough 0:33:50)  period 125s
    { 0, 31, 45, 0,  26000, V(168, 0) },   // 26s rise (→ 0:32:11)
    { 0, 32, 11, 0,  18000, V(198, 0) },   // 18s to peak (→ 0:32:29)
    { 0, 32, 29, 0,  10000, V(188, 0) },   // 10s shoulder (→ 0:32:39)
    { 0, 32, 39, 0,  71000, V( 40, 0) },   // 71s ebb (→ 0:33:50)

    // Wave 16  (0:33:50 → trough 0:35:55)  period 125s
    { 0, 33, 50, 0,  26000, V(152, 0) },   // 26s rise (→ 0:34:16)
    { 0, 34, 16, 0,  18000, V(180, 0) },   // 18s to peak (→ 0:34:34)
    { 0, 34, 34, 0,  10000, V(170, 0) },   // 10s shoulder (→ 0:34:44)
    { 0, 34, 44, 0,  71000, V( 42, 0) },   // 71s ebb — floor rising (→ 0:35:55)

    // Wave 17  (0:35:55 → trough 0:38:00)  period 125s
    { 0, 35, 55, 0,  26000, V(138, 0) },   // 26s rise (→ 0:36:21)
    { 0, 36, 21, 0,  18000, V(162, 0) },   // 18s to peak (→ 0:36:39)
    { 0, 36, 39, 0,  10000, V(152, 0) },   // 10s shoulder (→ 0:36:49)
    { 0, 36, 49, 0,  71000, V( 45, 0) },   // 71s ebb — floor rising (→ 0:38:00)

    // ── CLOSING  38:00 – 42:32  (floor 48→52, peaks 140→82) ─────────────────

    // Wave 18  (0:38:00 → trough 0:40:00)  period 120s
    { 0, 38,  0, 0,  24000, V(118, 0) },   // 24s rise (→ 0:38:24)
    { 0, 38, 24, 0,  18000, V(140, 0) },   // 18s to peak (→ 0:38:42)
    { 0, 38, 42, 0,  10000, V(130, 0) },   // 10s shoulder (→ 0:38:52)
    { 0, 38, 52, 0,  68000, V( 48, 0) },   // 68s ebb — floor rising (→ 0:40:00)

    // Wave 19  (0:40:00 → trough 0:41:50)  period 110s
    { 0, 40,  0, 0,  22000, V( 90, 0) },   // 22s rise (→ 0:40:22)
    { 0, 40, 22, 0,  16000, V(110, 0) },   // 16s to peak (→ 0:40:38)
    { 0, 40, 38, 0,  10000, V(100, 0) },   // 10s shoulder (→ 0:40:48)
    { 0, 40, 48, 0,  62000, V( 52, 0) },   // 62s ebb — floor rising (→ 0:41:50)

    // Wave 20 — last whisper  (0:41:50 → fade 0:43:20)
    { 0, 41, 50, 0,  20000, V( 68, 0) },   // 20s rise (→ 0:42:10)
    { 0, 42, 10, 0,  14000, V( 82, 0) },   // 14s last peak (→ 0:42:24)
    { 0, 42, 24, 0,   8000, V( 72, 0) },   // 8s shoulder (→ 0:42:32)

    // ── FINAL FADE TO BLACK  (42:32 – 43:20) ────────────────────────────────
    { 0, 42, 32, 0,  48000, V(  0, 0) },   // 48s fade to black (completes 0:43:20)
    { 0, 43,  0, 0,      0, V(  0, 0) },   // safety snap at loop end

};  // END CUE_LIST

#undef V

static const uint16_t CUE_COUNT = sizeof(CUE_LIST) / sizeof(CUE_LIST[0]);
