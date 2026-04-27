#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Cue list — CONTROLLED BURN  (32 min · 30fps)  v1
// Varytec LED Theater Spot 50 3200K  (2-channel DMX mode)
// Julian Charrière · CORRER · Venice 2026
//
//   Fixture minimum visible threshold: DMX 2   (below = physically dark)
//   Floor glow = DMX 2.   Hard black = DMX 0.
//   Lamp apex = DMX 150 at 23:24 (longest true dark valley 21:57–24:33).
//
// ── COMPOSITIONAL STRATEGY — FIRE COUNTERPOINT ───────────────────────────────
//
//   Source: 32×18 spatial luma extraction (analyze_movie_luma.py)
//   Metric: p95 per second — captures brief fire flashes that mean average misses.
//
//   True dark windows (lamp CAN be bright):
//     00:00–05:12  (313s)  — pure dark opening — slow atmospheric build
//     06:48–08:45  (118s)  — dark between fire flashes
//     08:47–09:37   (51s)  — short dark gap
//     09:39–11:19  (101s)  — dark before major burn 1
//     14:57–15:39   (43s)  — brief pause
//     16:32–17:07   (36s)  — brief pause
//     17:41–18:46   (66s)  — dark between fire clusters
//     18:48–19:27   (40s)  — dark gap
//     19:58–20:31   (34s)  — dark gap
//     21:57–24:33  (157s)  — LONGEST DARK — LAMP APEX HERE
//     24:36–26:27  (112s)  — long dark tail
//     26:51–29:00  (130s)  — long dark final valley
//
//   Fire event zones (lamp MUST be dark — pre-empt 6–10s before each cluster):
//     05:13–06:04  brief fire sparks (peak 255) — first fire
//     06:16–06:47  second cluster (peak 212)
//     07:01–07:26  scattered sparks (peak 107)
//     08:45–08:46  isolated flash (peak 178)
//     09:37–09:41  brief flash (peak 91)
//     11:19–12:57  MAJOR BURN 1 — sustained fire (peak 255)
//     13:23–14:51  MAJOR BURN 2 — dense fire (peak 247)
//     15:40–16:03  fire cluster (peak 253)
//     16:08–16:31  sustained fire (peak 219)
//     17:08–17:40  dense cluster (peak 231)
//     18:47–18:52  isolated flash (peak 191)
//     19:28–21:56  scattered rapid fire bursts (peaks 41–226)
//     24:33–24:35  brief flash
//     26:28–26:50  fire cluster (peak 150)
//     29:01–30:25  final dense fire (peak 237)
//
//   RULE: Every fire zone → lamp FAST DIP to 0 (3–8s pre-empt).
//         Lamp recovers slowly in dark valleys (10–20s). Floor ~22.
//         No lamp brightness during any fire. The lamp IS the shadow of the fire.
//
// ─────────────────────────────────────────────────────────────────────────────

#include "dmx_controller.h"
#include "../include/config.h"

// Shorthand: V(dim, strobe) — 3-channel mode (Master / Fine / Strobe)
// Fine dimmer = 0: no fractional add above the master coarse value.
#define V(d, s) \
    { {DMX_CH_MASTER,(d)}, {DMX_CH_FINE,0}, {DMX_CH_STROBE,(s)} }, 3

static const DMXCue CUE_LIST[] = {

    // =========================================================================
    // OPENING — 00:00–05:12  PURE DARK (313s)
    // The room is still. Slow ember-like build from nothing over 5 minutes.
    // =========================================================================

    { 0,  0,  0, 0,      0, V(  0, 0) },   // hard black at start
    { 0,  0,  8, 0,  20000, V(  2, 0) },   // 20s slow ember rise to floor
    { 0,  0, 28, 0,  18000, V( 20, 0) },   // 18s slow rise
    { 0,  0, 46, 0,  12000, V( 12, 0) },   // 12s settle
    { 0,  0, 58, 0,  15000, V( 27, 0) },   // 15s breathe up
    { 0,  1, 13, 0,  10000, V( 15, 0) },   // 10s breathe down
    { 0,  1, 23, 0,  20000, V( 39, 0) },   // 20s slow climb
    { 0,  1, 43, 0,  12000, V( 27, 0) },   // 12s exhale
    { 0,  1, 55, 0,  17000, V( 50, 0) },   // 17s breathe up
    { 0,  2, 12, 0,  11000, V( 35, 0) },   // 11s settle
    { 0,  2, 23, 0,  19000, V( 59, 0) },   // 19s slow climb
    { 0,  2, 42, 0,  13000, V( 42, 0) },   // 13s exhale
    { 0,  2, 55, 0,  16000, V( 67, 0) },   // 16s breathe up
    { 0,  3, 11, 0,  14000, V( 51, 0) },   // 14s exhale
    { 0,  3, 25, 0,  18000, V( 78, 0) },   // 18s slow rise — tension building
    { 0,  3, 43, 0,  12000, V( 59, 0) },   // 12s exhale
    { 0,  3, 55, 0,  15000, V( 86, 0) },   // 15s breathe up
    { 0,  4, 10, 0,  13000, V( 67, 0) },   // 13s exhale
    { 0,  4, 23, 0,  15000, V( 91, 0) },   // 15s near-peak of opening
    { 0,  4, 38, 0,  10000, V( 75, 0) },   // 10s settle
    { 0,  4, 48, 0,   8000, V( 91, 0) },   // 8s hold

    // Pre-empt first fire cluster at 05:13 — dip hard 17s ahead
    { 0,  4, 56, 0,   7000, V( 31, 0) },   // 7s fast fall
    { 0,  5,  3, 0,   5000, V(  0, 0) },   // 5s snap to black before fire

    // =========================================================================
    // FIRST FIRE CLUSTER — 05:13–07:26  (spikes 41–255)
    // Lamp stays completely dark. The fire owns the room.
    // =========================================================================

    { 0,  5,  8, 0,  58000, V(  0, 0) },   // 58s hold dark through all fire spikes to 06:06

    // =========================================================================
    // DARK VALLEY 1 — 06:48–08:45  (118s)
    // Fire still at 06:16–06:47, 07:01–07:26, 08:45. Recover cautiously.
    // =========================================================================

    { 0,  6, 48, 0,  12000, V(  2, 0) },   // 12s slow rise to floor
    { 0,  7,  0, 0,   8000, V(  0, 0) },   // 8s dip for 07:01 fire flash
    { 0,  7,  8, 0,  22000, V(  0, 0) },   // hold dark through 07:10–07:26 sparks
    { 0,  7, 30, 0,  15000, V(  2, 0) },   // 15s cautious rise to floor
    { 0,  7, 45, 0,  16000, V( 23, 0) },   // 16s breathe up
    { 0,  8,  1, 0,  12000, V( 10, 0) },   // 12s exhale
    { 0,  8, 13, 0,  15000, V( 31, 0) },   // 15s breathe up
    { 0,  8, 28, 0,   7000, V(  0, 0) },   // 7s snap dark — pre-empt 08:45 flash
    { 0,  8, 35, 0,  15000, V(  0, 0) },   // hold dark through 08:45–08:46 + 09:08

    // =========================================================================
    // DARK VALLEY 2 — 08:47–09:37  (51s)
    // Small recovery window, then flash at 09:37–09:41.
    // =========================================================================

    { 0,  8, 50, 0,  15000, V(  2, 0) },   // 15s rise to floor
    { 0,  9,  5, 0,  12000, V( 15, 0) },   // 12s breathe up
    { 0,  9, 17, 0,   8000, V(  0, 0) },   // 8s snap dark before 09:37 flash
    { 0,  9, 25, 0,  20000, V(  0, 0) },   // hold dark through 09:37–09:41

    // =========================================================================
    // DARK VALLEY 3 — 09:39–11:19  (101s)
    // Long clear window. Lamp breathes and builds. Black by 11:11.
    // =========================================================================

    { 0,  9, 45, 0,  14000, V(  2, 0) },   // 14s rise to floor
    { 0,  9, 59, 0,  16000, V( 34, 0) },   // 16s breathe up
    { 0, 10, 15, 0,  12000, V( 20, 0) },   // 12s exhale
    { 0, 10, 27, 0,  16000, V( 47, 0) },   // 16s breathe up
    { 0, 10, 43, 0,  13000, V( 31, 0) },   // 13s exhale
    { 0, 10, 56, 0,   9000, V( 43, 0) },   // 9s small peak
    { 0, 11,  5, 0,   8000, V(  0, 0) },   // 8s fast snap to black — Major Burn 1 incoming

    // =========================================================================
    // MAJOR BURN 1 — 11:19–12:57  (peak 255 sustained)
    // Total blackout. Lamp is nothing.
    // =========================================================================

    { 0, 11, 13, 0, 110000, V(  0, 0) },   // 110s hard black through entire burn

    // =========================================================================
    // BRIEF RECOVERY — 12:57–13:23  (26s between burns)
    // Very short gap, Major Burn 2 at 13:23. Show floor only.
    // =========================================================================

    { 0, 13,  3, 0,  10000, V(  2, 0) },   // 10s quick rise to floor only
    { 0, 13, 13, 0,  10000, V(  0, 0) },   // 10s snap to black before Major Burn 2

    // =========================================================================
    // MAJOR BURN 2 — 13:23–14:51  (peak 247 dense)
    // Total blackout again.
    // =========================================================================

    { 0, 13, 23, 0,  88000, V(  0, 0) },   // 88s hard black through entire burn

    // =========================================================================
    // DARK VALLEY 4 — 14:57–15:39  (43s)
    // Short window before fire at 15:40. Quick breathe only.
    // =========================================================================

    { 0, 14, 51, 0,  12000, V(  2, 0) },   // 12s rise to floor
    { 0, 15,  3, 0,  12000, V( 18, 0) },   // 12s small breath
    { 0, 15, 15, 0,  10000, V(  2, 0) },   // 10s exhale back
    { 0, 15, 25, 0,   9000, V(  0, 0) },   // 9s snap dark before 15:40 fire

    // =========================================================================
    // FIRE CLUSTER — 15:40–16:31  (fire 15:40–16:03, gap 16:03–16:07, fire 16:08–16:31)
    // Gap too short to react. Hold dark through entire stretch.
    // =========================================================================

    { 0, 15, 34, 0,  62000, V(  0, 0) },   // 62s black through 15:40→16:34 fire cluster

    // =========================================================================
    // DARK VALLEY 5 — 16:32–17:07  (36s)
    // Very short. Pre-empt 17:08 cluster (snap at 17:02).
    // =========================================================================

    { 0, 16, 34, 0,  12000, V(  2, 0) },   // 12s rise to floor
    { 0, 16, 46, 0,  10000, V( 15, 0) },   // 10s tiny breath
    { 0, 16, 56, 0,   6000, V(  0, 0) },   // 6s snap dark before 17:08 cluster

    // =========================================================================
    // FIRE CLUSTER — 17:08–17:40  (peak 231)
    // =========================================================================

    { 0, 17,  2, 0,  44000, V(  0, 0) },   // 44s dark through cluster to 17:46

    // =========================================================================
    // DARK VALLEY 6 — 17:41–18:46  (66s)
    // Longer recovery. Pre-empt 18:47 flash (snap at 18:41).
    // =========================================================================

    { 0, 17, 46, 0,  13000, V(  2, 0) },   // 13s rise to floor
    { 0, 17, 59, 0,  14000, V( 27, 0) },   // 14s breathe up
    { 0, 18, 13, 0,  12000, V( 12, 0) },   // 12s exhale
    { 0, 18, 25, 0,  10000, V( 23, 0) },   // 10s breathe up
    { 0, 18, 35, 0,   6000, V(  0, 0) },   // 6s snap dark before 18:47 flash

    // =========================================================================
    // FLASH — 18:47–18:52  brief
    // DARK VALLEY 7 — 18:48–19:27  (40s)
    // Pre-empt 19:28 fire.
    // =========================================================================

    { 0, 18, 41, 0,  10000, V(  0, 0) },   // hold dark through 18:47 flash
    { 0, 18, 51, 0,  14000, V(  2, 0) },   // 14s rise to floor
    { 0, 19,  5, 0,  11000, V( 15, 0) },   // 11s breath up
    { 0, 19, 16, 0,   8000, V(  0, 0) },   // 8s snap dark before 19:28 cluster

    // =========================================================================
    // SCATTERED FIRE — 19:28–21:56  (many rapid bursts, peaks 41–226)
    // Hold completely dark. This is the final approach to the great silence.
    // =========================================================================

    { 0, 19, 24, 0, 152000, V(  0, 0) },   // 152s total blackout through all scattered fire

    // =========================================================================
    // **** LAMP APEX — 21:57–24:33  LONGEST DARK (157s) ****
    //
    // The film has burned itself out. Deep silence.
    // Lamp climbs slowly from nothing to its maximum, holds warm,
    // then yields to the flash that resumes at 24:33.
    // =========================================================================

    { 0, 21, 56, 0,  18000, V(  2, 0) },   // 18s slow ember rise to floor
    { 0, 22, 14, 0,  20000, V( 39, 0) },   // 20s climb — room breathes again
    { 0, 22, 34, 0,  16000, V( 70, 0) },   // 16s climb
    { 0, 22, 50, 0,  14000, V(102, 0) },   // 14s climb
    { 0, 23,  4, 0,  12000, V(126, 0) },   // 12s near-peak
    { 0, 23, 16, 0,   8000, V(150, 0) },   // 8s APEX — DMX 115
    { 0, 23, 24, 0,  14000, V(118, 0) },   // 14s hold warm
    { 0, 23, 38, 0,  12000, V(150, 0) },   // 12s second hold at apex
    { 0, 23, 50, 0,  10000, V(102, 0) },   // 10s slow descent
    { 0, 24,  0, 0,  12000, V( 62, 0) },   // 12s descent — fire returning soon
    { 0, 24, 12, 0,  11000, V( 23, 0) },   // 11s descent
    { 0, 24, 23, 0,   8000, V(  0, 0) },   // 8s snap to black before 24:33 flash

    // =========================================================================
    // BRIEF FLASH — 24:33–24:35  (peak 173, 2s)
    // DARK VALLEY 8 — 24:36–26:27  (112s)
    // Long clear valley. Lamp breathes and recovers post-apex.
    // Pre-empt fire at 26:28.
    // =========================================================================

    { 0, 24, 31, 0,   9000, V(  0, 0) },   // hold dark through flash
    { 0, 24, 40, 0,  18000, V(  2, 0) },   // 18s slow rise to floor
    { 0, 24, 58, 0,  16000, V( 31, 0) },   // 16s breathe up
    { 0, 25, 14, 0,  14000, V( 15, 0) },   // 14s exhale
    { 0, 25, 28, 0,  16000, V( 43, 0) },   // 16s breathe up
    { 0, 25, 44, 0,  13000, V( 23, 0) },   // 13s exhale
    { 0, 25, 57, 0,  15000, V( 39, 0) },   // 15s breathe up
    { 0, 26, 12, 0,   8000, V(  0, 0) },   // 8s snap dark before 26:28 fire

    // =========================================================================
    // FIRE CLUSTER — 26:28–26:50  (peak 150)
    // DARK VALLEY 9 — 26:51–29:00  (130s)
    // Long final dark valley. Lamp slow recovery, building toward final fire.
    // Pre-empt final fire cluster at 29:01.
    // =========================================================================

    { 0, 26, 20, 0,  35000, V(  0, 0) },   // 35s dark through 26:28–26:55 cluster
    { 0, 26, 55, 0,  16000, V(  2, 0) },   // 16s slow rise
    { 0, 27, 11, 0,  15000, V( 23, 0) },   // 15s breathe up
    { 0, 27, 26, 0,  14000, V(  8, 0) },   // 14s exhale
    { 0, 27, 40, 0,  16000, V( 34, 0) },   // 16s breathe up
    { 0, 27, 56, 0,  13000, V( 15, 0) },   // 13s exhale
    { 0, 28,  9, 0,  14000, V( 27, 0) },   // 14s breathe up
    { 0, 28, 23, 0,  13000, V(  8, 0) },   // 13s exhale
    { 0, 28, 36, 0,  11000, V( 20, 0) },   // 11s breathe — tension
    { 0, 28, 47, 0,   7000, V(  0, 0) },   // 7s snap dark before final fire at 29:01

    // =========================================================================
    // FINAL FIRE — 29:01–30:25  (dense cluster, peak 237)
    // Lamp stays dark as the composition burns to its end.
    // =========================================================================

    { 0, 28, 54, 0,  90000, V(  0, 0) },   // 90s dark through all final fire

    // =========================================================================
    // FADE OUT — 30:25–32:00
    // Film cooling down. Lamp rises one last time, then fades to black.
    // =========================================================================

    { 0, 30, 24, 0,  20000, V(  2, 0) },   // 20s slow rise from ash
    { 0, 30, 44, 0,  18000, V( 20, 0) },   // 18s breathe up
    { 0, 31,  2, 0,  16000, V(  8, 0) },   // 16s exhale
    { 0, 31, 18, 0,  20000, V( 23, 0) },   // 20s last warm breath
    { 0, 31, 38, 0,  22000, V(  0, 0) },   // 22s final fade to black

};  // END CUE_LIST

#undef V

static const uint16_t CUE_COUNT = sizeof(CUE_LIST) / sizeof(CUE_LIST[0]);
