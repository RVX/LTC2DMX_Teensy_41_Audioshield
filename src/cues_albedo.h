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
//      → "barely glowing" trough floor = DMX 22  (~9%)
//      → intentional hard black = DMX 0 (opening strikes, apex break)
//
// Format: { HH, MM, SS, FF, fadeMs, { {ch,val}, ... }, numChannels }
//
// ── COMPOSITION ─────────────────────────────────────────────────────────────
//
//   ALBEDO: the fraction of light reflected by a surface (snow, ice, glacier).
//   Concept — slow sea-wave breath of reflected light. The light does not
//   originate here; it arrives, bounces, recedes. Warm 3200K acts as trapped
//   sunlight inside a white body.
//
//   Wave shape: asymmetric — fast flood (surge), slow ebb. 3–4 control points.
//   Trough floor: DMX 22 (barely visible warm glow — the surface never goes
//   fully cold until the very end).
//   Peak arc: rises from DMX 58 (awakening) to DMX 235 / 92% (apex at 27:58).
//
//   Arc:
//     0:00 –  0:55  Opening strikes  — 4 hard descending pulses, full black gaps
//     0:55 –  5:15  Awakening        — floor 22, peaks 58→78
//     5:15 – 13:38  Growing          — floor 22, peaks 92→138
//    13:38 – 21:16  Building         — floor 22, peaks 122→178
//    21:16 – 26:11  Pre-apex         — floor 22, peaks 188→208
//    26:11 – 29:41  Apex             — floor 22→0 (break), peaks 215→235(92%)
//    29:41 – 37:15  Recession        — floor 22→32, peaks 215→158
//    37:15 – 42:36  Closing          — floor 35→45, peaks 128→60
//    42:36 –        Final 45 s fade to black
//
//   Two brief strobe-texture moments (ch2=5, barely perceptible shimmer):
//     pre-apex wave 14 peak  (~23:35)
//     apex wave 17 shoulder  (~28:10) — just before the break
// ─────────────────────────────────────────────────────────────────────────────

#include "dmx_controller.h"
#include "../include/config.h"

#define V(d, s) \
    { {DMX_CH_MASTER,(d)}, {DMX_CH_STROBE,(s)} }, 2

static const DMXCue CUE_LIST[] = {

    // ── OPENING: black → first whisper ──────────────────────────────────────
    // ── OPENING STRIKE  0:00:00 – 0:00:55 ──────────────────────────────────
    //   Four descending hard pulses into near-black — fast contrast shock.
    //   Each burst: 1–2s snap up, 4–7s drop back to dark.
    //   Resolves into silence before the sea-breath begins.

    { 0,  0,  0, 0,      0, V(  0, 0) },   // hard black
    { 0,  0,  1, 0,   1500, V(210, 0) },   // 1.5s snap → 82% (first strike)
    { 0,  0,  2, 0,   4000, V(  0, 0) },   // 4s hard black (→ 0:00:06)
    { 0,  0,  6, 0,   1500, V(175, 0) },   // 1.5s snap → 69% (second strike)
    { 0,  0,  8, 0,   5000, V(  0, 0) },   // 5s hard black (→ 0:00:13)
    { 0,  0, 13, 0,   2000, V(138, 0) },   // 2s snap → 54% (third, decaying)
    { 0,  0, 15, 0,   6000, V(  0, 0) },   // 6s hard black (→ 0:00:21)
    { 0,  0, 21, 0,   2500, V( 95, 0) },   // 2.5s snap → 37% (fourth, faint echo)
    { 0,  0, 24, 0,  31000, V(  0, 0) },   // 31s hard black — silence before sea-breath (→ 0:00:55)

    // ── AWAKENING  0:00:55 – 5:15  (floor 22, peaks 58→78) ─────────────────

    // Wave 1  (0:00:55 → trough 0:02:18)
    { 0,  0, 55, 0,  18000, V( 40, 0) },   // 18s slow rise (→ 0:01:13)
    { 0,  1, 13, 0,  14000, V( 58, 0) },   // 14s surge to peak (→ 0:01:27)
    { 0,  1, 27, 0,   6000, V( 50, 0) },   // 6s shoulder (→ 0:01:33)
    { 0,  1, 33, 0,  45000, V( 22, 0) },   // 45s long ebb to faint glow (→ 0:02:18)

    // Wave 2  (0:02:18 → trough 0:03:48)
    { 0,  2, 18, 0,  20000, V( 48, 0) },   // 20s rise (→ 0:02:38)
    { 0,  2, 38, 0,  15000, V( 68, 0) },   // 15s surge to peak (→ 0:02:53)
    { 0,  2, 53, 0,   7000, V( 60, 0) },   // 7s shoulder (→ 0:03:00)
    { 0,  3,  0, 0,  48000, V( 22, 0) },   // 48s ebb to faint glow (→ 0:03:48)

    // Wave 3  (0:03:48 → trough 0:05:15)
    { 0,  3, 48, 0,  20000, V( 55, 0) },   // 20s rise (→ 0:04:08)
    { 0,  4,  8, 0,  14000, V( 78, 0) },   // 14s surge to peak (→ 0:04:22)
    { 0,  4, 22, 0,   7000, V( 68, 0) },   // 7s shoulder (→ 0:04:29)
    { 0,  4, 29, 0,  46000, V( 22, 0) },   // 46s ebb to faint glow (→ 0:05:15)

    // ── GROWING  5:15 – 13:38  (floor 22, peaks 92→138) ───────────────────

    // Wave 4  (5:15 → trough 6:51)
    { 0,  5, 15, 0,  22000, V( 70, 0) },   // 22s rise (→ 0:05:37)
    { 0,  5, 37, 0,  16000, V( 92, 0) },   // 16s surge to peak (→ 0:05:53)
    { 0,  5, 53, 0,   8000, V( 82, 0) },   // 8s shoulder (→ 0:06:01)
    { 0,  6,  1, 0,  50000, V( 22, 0) },   // 50s ebb to faint glow (→ 0:06:51)

    // Wave 5  (6:51 → trough 8:29)
    { 0,  6, 51, 0,  22000, V( 80, 0) },   // 22s rise (→ 0:07:13)
    { 0,  7, 13, 0,  16000, V(105, 0) },   // 16s surge to peak (→ 0:07:29)
    { 0,  7, 29, 0,   8000, V( 95, 0) },   // 8s shoulder (→ 0:07:37)
    { 0,  7, 37, 0,  52000, V( 22, 0) },   // 52s ebb to faint glow (→ 0:08:29)

    // Wave 6  (8:29 → trough 10:09)
    { 0,  8, 29, 0,  22000, V( 88, 0) },   // 22s rise (→ 0:08:51)
    { 0,  8, 51, 0,  18000, V(115, 0) },   // 18s surge to peak (→ 0:09:09)
    { 0,  9,  9, 0,   8000, V(105, 0) },   // 8s shoulder (→ 0:09:17)
    { 0,  9, 17, 0,  52000, V( 22, 0) },   // 52s ebb to faint glow (→ 0:10:09)

    // Wave 7  (10:09 → trough 11:53)
    { 0, 10,  9, 0,  24000, V( 98, 0) },   // 24s rise (→ 0:10:33)
    { 0, 10, 33, 0,  18000, V(125, 0) },   // 18s surge to peak (→ 0:10:51)
    { 0, 10, 51, 0,   8000, V(115, 0) },   // 8s shoulder (→ 0:10:59)
    { 0, 10, 59, 0,  54000, V( 22, 0) },   // 54s ebb to faint glow (→ 0:11:53)

    // Wave 8  (11:53 → trough 13:38)
    { 0, 11, 53, 0,  24000, V(110, 0) },   // 24s rise (→ 0:12:17)
    { 0, 12, 17, 0,  18000, V(138, 0) },   // 18s surge to peak (→ 0:12:35)
    { 0, 12, 35, 0,   8000, V(128, 0) },   // 8s shoulder (→ 0:12:43)
    { 0, 12, 43, 0,  55000, V( 22, 0) },   // 55s ebb to faint glow (→ 0:13:38)

    // ── BUILDING  13:38 – 21:16  (floor 22, peaks 122→178) ────────────────

    // Wave 9  (13:38 → trough 15:32)
    { 0, 13, 38, 0,  26000, V(122, 0) },   // 26s rise (→ 0:14:04)
    { 0, 14,  4, 0,  20000, V(152, 0) },   // 20s surge to peak (→ 0:14:24)
    { 0, 14, 24, 0,  10000, V(142, 0) },   // 10s shoulder (→ 0:14:34)
    { 0, 14, 34, 0,  58000, V( 22, 0) },   // 58s ebb to faint glow (→ 0:15:32)

    // Wave 10  (15:32 → trough 17:26)
    { 0, 15, 32, 0,  26000, V(132, 0) },   // 26s rise (→ 0:15:58)
    { 0, 15, 58, 0,  20000, V(162, 0) },   // 20s surge to peak (→ 0:16:18)
    { 0, 16, 18, 0,  10000, V(152, 0) },   // 10s shoulder (→ 0:16:28)
    { 0, 16, 28, 0,  58000, V( 22, 0) },   // 58s ebb to faint glow (→ 0:17:26)

    // Wave 11  (17:26 → trough 19:20)
    { 0, 17, 26, 0,  26000, V(140, 0) },   // 26s rise (→ 0:17:52)
    { 0, 17, 52, 0,  20000, V(170, 0) },   // 20s surge to peak (→ 0:18:12)
    { 0, 18, 12, 0,  10000, V(160, 0) },   // 10s shoulder (→ 0:18:22)
    { 0, 18, 22, 0,  58000, V( 22, 0) },   // 58s ebb to faint glow (→ 0:19:20)

    // Wave 12  (19:20 → trough 21:16)
    { 0, 19, 20, 0,  28000, V(148, 0) },   // 28s rise (→ 0:19:48)
    { 0, 19, 48, 0,  20000, V(178, 0) },   // 20s surge to peak (→ 0:20:08)
    { 0, 20,  8, 0,  10000, V(168, 0) },   // 10s shoulder (→ 0:20:18)
    { 0, 20, 18, 0,  58000, V( 22, 0) },   // 58s ebb to faint glow (→ 0:21:16)

    // ── PRE-APEX TENSION  21:16 – 26:11  (floor 22, peaks 188→208) ─────────

    // Wave 13  (21:16 → trough 22:59)  — slightly shorter, more urgent
    { 0, 21, 16, 0,  22000, V(155, 0) },   // 22s rise (→ 0:21:38)
    { 0, 21, 38, 0,  18000, V(188, 0) },   // 18s surge to peak (→ 0:21:56)
    { 0, 21, 56, 0,   8000, V(178, 0) },   // 8s shoulder (→ 0:22:04)
    { 0, 22,  4, 0,  55000, V( 22, 0) },   // 55s ebb to faint glow (→ 0:22:59)

    // Wave 14  (22:59 → trough 24:38)  — faint strobe shimmer at peak
    { 0, 22, 59, 0,  20000, V(162, 0) },   // 20s rise (→ 0:23:19)
    { 0, 23, 19, 0,  16000, V(195, 5) },   // 16s to peak — ch2=5 barely-visible shimmer (→ 0:23:35)
    { 0, 23, 35, 0,   8000, V(185, 0) },   // 8s kill strobe + shoulder (→ 0:23:43)
    { 0, 23, 43, 0,  55000, V( 22, 0) },   // 55s ebb to faint glow (→ 0:24:38)

    // Wave 15  (24:38 → trough 26:11)  — apex approach
    { 0, 24, 38, 0,  18000, V(172, 0) },   // 18s rise (→ 0:24:56)
    { 0, 24, 56, 0,  15000, V(208, 0) },   // 15s surge to near-apex (→ 0:25:11)
    { 0, 25, 11, 0,   8000, V(198, 0) },   // 8s shoulder (→ 0:25:19)
    { 0, 25, 19, 0,  52000, V( 22, 0) },   // 52s ebb to faint glow (→ 0:26:11)

    // ── APEX  26:11 – 29:41  (floor 22 / break=0, peaks 215→235) ───────────

    // Wave 16  (26:11 → trough 27:44)
    { 0, 26, 11, 0,  16000, V(182, 0) },   // 16s fast surge from near-black (→ 0:26:27)
    { 0, 26, 27, 0,  14000, V(215, 0) },   // 14s to high peak (→ 0:26:41)
    { 0, 26, 41, 0,   8000, V(205, 0) },   // 8s shoulder (→ 0:26:49)
    { 0, 26, 49, 0,  55000, V( 22, 0) },   // 55s ebb to faint glow (→ 0:27:44)

    // Wave 17  — ███ THE APEX ███  (27:44 → trough 29:41)
    { 0, 27, 44, 0,  14000, V(195, 0) },   // 14s fast surge (→ 0:27:58)
    { 0, 27, 58, 0,  12000, V(235, 0) },   // 12s ███ 92% APEX ███ (→ 0:28:10)
    { 0, 28, 10, 0,  10000, V(225, 5) },   // 10s shoulder — ch2=5 shimmer (→ 0:28:20)
    { 0, 28, 20, 0,   3000, V(  0, 0) },   // 3s SNAP to hard black — THE BREAK (→ 0:28:23)
    { 0, 28, 23, 0,  18000, V(200, 0) },   // 18s surge back — aftershock (→ 0:28:41)
    { 0, 28, 41, 0,  60000, V( 22, 0) },   // 60s long withdrawal to faint glow (→ 0:29:41)

    // ── RECESSION  29:41 – 37:15  (floor 22→32, peaks 215→158) ─────────────

    // Wave 18  (29:41 → trough 31:31)
    { 0, 29, 41, 0,  20000, V(192, 0) },   // 20s rise (→ 0:30:01)
    { 0, 30,  1, 0,  18000, V(215, 0) },   // 18s to post-apex peak (→ 0:30:19)
    { 0, 30, 19, 0,  10000, V(205, 0) },   // 10s shoulder (→ 0:30:29)
    { 0, 30, 29, 0,  62000, V( 22, 0) },   // 62s ebb (→ 0:31:31)

    // Wave 19  (31:31 → trough 33:25)
    { 0, 31, 31, 0,  22000, V(172, 0) },   // 22s rise (→ 0:31:53)
    { 0, 31, 53, 0,  18000, V(198, 0) },   // 18s to peak (→ 0:32:11)
    { 0, 32, 11, 0,  10000, V(188, 0) },   // 10s shoulder (→ 0:32:21)
    { 0, 32, 21, 0,  64000, V( 25, 0) },   // 64s ebb — floor rising (→ 0:33:25)

    // Wave 20  (33:25 → trough 35:21)
    { 0, 33, 25, 0,  24000, V(152, 0) },   // 24s rise (→ 0:33:49)
    { 0, 33, 49, 0,  18000, V(178, 0) },   // 18s to peak (→ 0:34:07)
    { 0, 34,  7, 0,  10000, V(168, 0) },   // 10s shoulder (→ 0:34:17)
    { 0, 34, 17, 0,  64000, V( 28, 0) },   // 64s ebb — floor rising (→ 0:35:21)

    // Wave 21  (35:21 → trough 37:15)
    { 0, 35, 21, 0,  24000, V(132, 0) },   // 24s rise (→ 0:35:45)
    { 0, 35, 45, 0,  18000, V(158, 0) },   // 18s to peak (→ 0:36:03)
    { 0, 36,  3, 0,  10000, V(148, 0) },   // 10s shoulder (→ 0:36:13)
    { 0, 36, 13, 0,  62000, V( 32, 0) },   // 62s ebb — floor rising (→ 0:37:15)

    // ── CLOSING  37:15 – 42:36  (floor 35→45, peaks 128→60) ────────────────

    // Wave 22  (37:15 → trough 39:07)
    { 0, 37, 15, 0,  24000, V(105, 0) },   // 24s rise (→ 0:37:39)
    { 0, 37, 39, 0,  18000, V(128, 0) },   // 18s to peak (→ 0:37:57)
    { 0, 37, 57, 0,   8000, V(118, 0) },   // 8s shoulder (→ 0:38:05)
    { 0, 38,  5, 0,  62000, V( 35, 0) },   // 62s ebb — floor rising (→ 0:39:07)

    // Wave 23  (39:07 → trough 40:59)
    { 0, 39,  7, 0,  26000, V( 68, 0) },   // 26s rise (→ 0:39:33)
    { 0, 39, 33, 0,  18000, V( 88, 0) },   // 18s to peak (→ 0:39:51)
    { 0, 39, 51, 0,   8000, V( 80, 0) },   // 8s shoulder (→ 0:39:59)
    { 0, 39, 59, 0,  60000, V( 40, 0) },   // 60s ebb — floor rising (→ 0:40:59)

    // Wave 24  — last whisper  (40:59 → trough 42:36)
    { 0, 40, 59, 0,  22000, V( 42, 0) },   // 22s rise (→ 0:41:21)
    { 0, 41, 21, 0,  15000, V( 60, 0) },   // 15s last peak (→ 0:41:36)
    { 0, 41, 36, 0,   8000, V( 52, 0) },   // 8s shoulder (→ 0:41:44)
    { 0, 41, 44, 0,  52000, V( 45, 0) },   // 52s ebb — last warm glow (→ 0:42:36)

    // ── FINAL FADE TO BLACK  (42:36 – 43:21) ────────────────────────────────
    { 0, 42, 36, 0,  45000, V(  0, 0) },   // 45s fade to black (completes 0:43:21)
    { 0, 43,  0, 0,      0, V(  0, 0) },   // safety snap at loop end

};  // END CUE_LIST

#undef V

static const uint16_t CUE_COUNT = sizeof(CUE_LIST) / sizeof(CUE_LIST[0]);
