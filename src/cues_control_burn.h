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
//   STRATEGY v2 — FIRE DIALOGUE (analysis shows fire = isolated 1s flashes, not sustained)
//   Fire events are overwhelmingly single-second spikes with dark gaps between them.
//   Lamp THREADS through the gaps: holds low ember glow, snaps to 0 at each flash,
//   rises back immediately after. The lamp IS in conversation with the fire.
//
//   In sustained dense burns (Major 1+2): lamp finds the breathing holes (4–7s gaps)
//   and touches briefly — ghost presence.
//
//   Two major dark windows inside the scattered fire zone (19:28–21:56):
//     19:58–20:12  (14s) — lamp breathes to DMX 30
//     21:01–21:23  (23s) — lamp builds to DMX 39, approaching the apex
//
//   DMX 0 = lamp physically off. Snaps are fadeMs ≤ 1500. Rises are fadeMs 3000–8000.
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
    // OPENING — 00:00–00:40  SMOOTH LINEAR RISE
    // Pure uninterrupted climb from black. No steps, no oscillation.
    // =========================================================================

    { 0,  0,  0, 0,      0, V(  0, 0) },   // hard black at start
    { 0,  0,  1, 0,  39000, V( 35, 0) },   // 1s dark → 39s smooth linear rise → DMX 35 at 0:40

    // =========================================================================
    // CHAOS — 00:40–04:48  INCREASINGLY ERRATIC
    // First disruption jolts at 0:40. Irregular oscillations trend upward.
    // Dips grow sharper/deeper as the fire approaches. Hard stutters from 1:55.
    // Peaks at DMX 91 from 3:09, then stuttering climax before pre-empt.
    // =========================================================================

    { 0,  0, 40, 0,   5000, V( 18, 0) },   // 5s sharp dip — first disruption
    { 0,  0, 45, 0,   8000, V( 50, 0) },   // 8s jump up
    { 0,  0, 53, 0,   4000, V( 31, 0) },   // 4s dip
    { 0,  0, 57, 0,   9000, V( 62, 0) },   // 9s rise
    { 0,  1,  6, 0,  11000, V( 70, 0) },   // 11s slow build
    { 0,  1, 17, 0,   3000, V( 23, 0) },   // 3s stutter dip
    { 0,  1, 20, 0,   7000, V( 65, 0) },   // 7s recovery
    { 0,  1, 27, 0,   8000, V( 78, 0) },   // 8s build
    { 0,  1, 35, 0,   4000, V( 47, 0) },   // 4s dip
    { 0,  1, 39, 0,   6000, V( 70, 0) },   // 6s rise
    { 0,  1, 45, 0,  10000, V( 82, 0) },   // 10s climb
    { 0,  1, 55, 0,   2000, V( 12, 0) },   // 2s HARD stutter — darkness bites in
    { 0,  1, 57, 0,   5000, V( 67, 0) },   // 5s fast recovery
    { 0,  2,  2, 0,   9000, V( 82, 0) },   // 9s build
    { 0,  2, 11, 0,   4000, V( 43, 0) },   // 4s dip
    { 0,  2, 15, 0,   6000, V( 78, 0) },   // 6s rise
    { 0,  2, 21, 0,   3000, V( 31, 0) },   // 3s sharp dip
    { 0,  2, 24, 0,   5000, V( 75, 0) },   // 5s recovery
    { 0,  2, 29, 0,  10000, V( 86, 0) },   // 10s push toward peak
    { 0,  2, 39, 0,   4000, V( 51, 0) },   // 4s dip
    { 0,  2, 43, 0,   6000, V( 78, 0) },   // 6s rise
    { 0,  2, 49, 0,   2000, V( 27, 0) },   // 2s hard stutter
    { 0,  2, 51, 0,   8000, V( 86, 0) },   // 8s push
    { 0,  2, 59, 0,  10000, V( 91, 0) },   // 10s → first peak touch at 3:09
    { 0,  3,  9, 0,   3000, V( 51, 0) },   // 3s sudden fall
    { 0,  3, 12, 0,   4000, V( 82, 0) },   // 4s recovery
    { 0,  3, 16, 0,   2000, V( 27, 0) },   // 2s double stutter
    { 0,  3, 18, 0,   6000, V( 86, 0) },   // 6s rise
    { 0,  3, 24, 0,   9000, V( 91, 0) },   // 9s at peak → 3:33
    { 0,  3, 33, 0,   5000, V( 59, 0) },   // 5s fall
    { 0,  3, 38, 0,   6000, V( 86, 0) },   // 6s recover
    { 0,  3, 44, 0,   3000, V( 39, 0) },   // 3s dip
    { 0,  3, 47, 0,   5000, V( 82, 0) },   // 5s recovery
    { 0,  3, 52, 0,   2000, V( 15, 0) },   // 2s brutal stutter
    { 0,  3, 54, 0,   4000, V( 78, 0) },   // 4s fast up
    { 0,  3, 58, 0,   8000, V( 91, 0) },   // 8s at peak → 4:06
    { 0,  4,  6, 0,   4000, V( 47, 0) },   // 4s drop
    { 0,  4, 10, 0,   5000, V( 86, 0) },   // 5s recovery
    { 0,  4, 15, 0,   2000, V( 27, 0) },   // 2s stutter
    { 0,  4, 17, 0,   3000, V( 78, 0) },   // 3s fast up
    { 0,  4, 20, 0,   2000, V( 18, 0) },   // 2s brutal stutter — double punch
    { 0,  4, 22, 0,   4000, V( 86, 0) },   // 4s jump
    { 0,  4, 26, 0,  12000, V( 91, 0) },   // 12s hold at peak
    { 0,  4, 38, 0,  10000, V( 91, 0) },   // 10s sustain into pre-empt zone

    // Pre-empt first fire cluster at 05:13 — dip hard 17s ahead
    { 0,  4, 56, 0,   7000, V( 31, 0) },   // 7s fast fall
    { 0,  5,  3, 0,   5000, V(  0, 0) },   // 5s snap to black before fire

    // =========================================================================
    // FIRST FIRE CLUSTER — 05:13–07:26  (spikes 41–255)
    // Lamp stays completely dark. The fire owns the room.
    // =========================================================================

    // =========================================================================
    // FIRST FIRE ZONE — 05:08–06:48
    // Fire is NOT sustained — isolated 1s flashes: 05:13, 05:30, 06:05, 06:16, 06:38, 06:45
    // Lamp holds low glow (DMX 8–22), snaps to 0 at each flash, rises back immediately.
    // =========================================================================

    { 0,  5,  8, 0,   4000, V(  8, 0) },   // 4s rise — lamp whispers back into the room
    { 0,  5, 12, 0,   1000, V(  0, 0) },   // snap to 0 — 05:13 explosion (peak 214)
    { 0,  5, 14, 0,   5000, V( 10, 0) },   // 5s recover to low glow
    { 0,  5, 19, 0,   9000, V( 18, 0) },   // 9s breathe up — 15s dark window
    { 0,  5, 28, 0,   1000, V(  0, 0) },   // snap to 0 — 05:30 explosion (peak 255)
    { 0,  5, 31, 0,   6000, V( 12, 0) },   // 6s recover
    { 0,  5, 37, 0,  16000, V( 22, 0) },   // 16s climb — long 33s dark window
    { 0,  5, 53, 0,   8000, V( 12, 0) },   // 8s settle
    { 0,  6,  3, 0,   1500, V(  0, 0) },   // snap — 06:05 explosion (peak 112)
    { 0,  6,  5, 0,   5000, V(  8, 0) },   // 5s recover
    { 0,  6, 10, 0,   4000, V( 15, 0) },   // 4s breathe up — 10s dark window
    { 0,  6, 14, 0,   1500, V(  0, 0) },   // snap — 06:16 explosion (peak 212)
    { 0,  6, 17, 0,  10000, V( 10, 0) },   // 10s low glow — 20s dark window
    { 0,  6, 27, 0,   7000, V( 18, 0) },   // 7s build
    { 0,  6, 34, 0,   3000, V(  0, 0) },   // 3s snap — 06:38 flash (peak 155)
    { 0,  6, 39, 0,   4000, V(  8, 0) },   // 4s recover through 06:40-44 dark gap
    { 0,  6, 43, 0,   1500, V(  0, 0) },   // snap — 06:45 flash cluster (peak 182)
    { 0,  6, 48, 0,   3000, V(  5, 0) },   // barely alive — valley begins
    // =========================================================================
    // DARK VALLEY 1 — 06:48–08:45  (118s, fire stubs at 07:01/07:10/07:15/07:26)
    // Stubs are isolated 1s flashes — lamp holds low glow with snaps at each.
    // =========================================================================

    { 0,  6, 51, 0,   7000, V( 15, 0) },   // 7s rise from 5 to 15
    { 0,  6, 58, 0,   1500, V(  0, 0) },   // snap — 07:01 flash (peak 51)
    { 0,  7,  2, 0,   6000, V( 12, 0) },   // 6s recover
    { 0,  7,  8, 0,   1500, V(  0, 0) },   // snap — 07:10 flash (peak 41)
    { 0,  7, 10, 0,   4000, V( 12, 0) },   // 4s recover
    { 0,  7, 14, 0,   1500, V(  0, 0) },   // snap — 07:15 flash (peak 107)
    { 0,  7, 16, 0,   8000, V( 18, 0) },   // 8s recover — 10s dark window
    { 0,  7, 24, 0,   1500, V(  0, 0) },   // snap — 07:26 flash (peak 42)
    { 0,  7, 27, 0,  15000, V(  2, 0) },   // 15s breathe up (clear until 08:45)
    { 0,  7, 42, 0,  13000, V( 23, 0) },   // 13s breathe up
    { 0,  7, 55, 0,  10000, V( 10, 0) },   // 10s exhale
    { 0,  8,  5, 0,  12000, V( 27, 0) },   // 12s breathe up
    { 0,  8, 17, 0,   8000, V( 10, 0) },   // 8s exhale
    { 0,  8, 25, 0,   4000, V(  0, 0) },   // 4s snap dark — pre-empt 08:45 flash
    { 0,  8, 29, 0,  16000, V(  0, 0) },   // hold dark through 08:45–08:46 + 09:08

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
    // Dense — but dark gaps at 11:42 (1s), 12:12 (1s), 12:21 (1s).
    // Lamp is mostly dark with ghost flickers in those brief holes.
    // =========================================================================

    { 0, 11, 13, 0,  28000, V(  0, 0) },   // 28s black — fire building 11:19-11:41
    { 0, 11, 41, 0,    500, V(  5, 0) },   // ghost flash in 11:42 dark gap
    { 0, 11, 42, 0,  29000, V(  0, 0) },   // back to 0 — fire resumes
    { 0, 12, 11, 0,    500, V(  5, 0) },   // ghost flash in 12:12 dark gap
    { 0, 12, 12, 0,   8000, V(  0, 0) },   // back to 0
    { 0, 12, 20, 0,    500, V(  8, 0) },   // ghost flash in 12:21 dark gap
    { 0, 12, 21, 0,  42000, V(  0, 0) },   // 42s black through burn end

    // =========================================================================
    // BRIEF RECOVERY — 12:57–13:23  (26s between burns)
    // Very short gap, Major Burn 2 at 13:23. Show floor only.
    // =========================================================================

    { 0, 13,  3, 0,  10000, V(  2, 0) },   // 10s quick rise to floor only
    { 0, 13, 13, 0,  10000, V(  0, 0) },   // 10s snap to black before Major Burn 2

    // =========================================================================
    // MAJOR BURN 2 — 13:23–14:51  (peak 247 dense, but with real breathing gaps)
    // Dark gaps: 13:28-31 (4s), 13:39-45 (7s), 14:03-04 (2s), 14:30-31 (2s)
    // Lamp pulses through gaps — fire has dark lungs.
    // =========================================================================

    { 0, 13, 23, 0,   4000, V(  0, 0) },   // 4s black — first bursts 13:23-24
    // 13:28-31 dark gap (4s)
    { 0, 13, 27, 0,   2000, V( 18, 0) },   // 2s lamp touch in 4s gap
    { 0, 13, 29, 0,   2000, V(  0, 0) },   // snap back — 13:32 fire resumes
    { 0, 13, 31, 0,   3000, V(  0, 0) },   // hold — 13:34-38 fire (5s, peak 146)
    // 13:39-45 dark gap (7s) — best window in this burn
    { 0, 13, 39, 0,   4000, V( 30, 0) },   // 4s rise — lamp breathes in the gap
    { 0, 13, 43, 0,   2000, V( 10, 0) },   // 2s fall — 13:45 fire incoming
    { 0, 13, 45, 0,   1000, V(  0, 0) },   // snap — 13:46 dense fire (17s, peak 247)
    { 0, 13, 46, 0,  17000, V(  0, 0) },   // 17s black through 13:46-14:02
    // 14:03-04 dark gap (2s)
    { 0, 14,  3, 0,   1500, V(  8, 0) },   // ghost touch in 2s gap
    { 0, 14,  5, 0,  24000, V(  0, 0) },   // 24s black — 14:05-29 dense fire
    // 14:30-31 dark gap (2s)
    { 0, 14, 29, 0,   1500, V(  5, 0) },   // ghost touch in 2s gap
    { 0, 14, 31, 0,  22000, V(  0, 0) },   // 22s black through 14:37-51

    // =========================================================================
    // DARK VALLEY 4 — 14:57–15:39  (43s)
    // Short window before fire at 15:40. Quick breathe only.
    // =========================================================================

    { 0, 14, 51, 0,  12000, V(  2, 0) },   // 12s rise to floor
    { 0, 15,  3, 0,  12000, V( 18, 0) },   // 12s small breath
    { 0, 15, 15, 0,  10000, V(  2, 0) },   // 10s exhale back
    { 0, 15, 25, 0,   9000, V(  0, 0) },   // 9s snap dark before 15:40 fire

    // =========================================================================
    // FIRE CLUSTER — 15:40–16:31
    // Sub-cluster A: 15:56-16:03 (8s, peak 253). Dark gap: 16:04-06 (3s).
    // Sub-cluster B: 16:08-16:31 (24s, peak 220). Lamp flashes in the 3s gap.
    // =========================================================================

    { 0, 15, 34, 0,  28000, V(  0, 0) },   // 28s dark through 15:40-16:03
    // 16:04-16:06 dark gap (3s)
    { 0, 16,  2, 0,   2000, V(  0, 0) },   // hold — gap imminent
    { 0, 16,  4, 0,   2000, V( 22, 0) },   // 2s lamp flash in 3s gap
    { 0, 16,  6, 0,   2000, V(  0, 0) },   // snap back — 16:08 sub-cluster B
    { 0, 16,  8, 0,  24000, V(  0, 0) },   // 24s black through 16:08-16:31

    // =========================================================================
    // DARK VALLEY 5 — 16:32–17:07  (fire stubs: 16:33, 16:35, 16:49, 16:53-55)
    // Residual stubs — lamp snaps at each, rises between, pre-empts 17:08.
    // =========================================================================

    { 0, 16, 32, 0,   1500, V(  0, 0) },   // snap — 16:33 stub (peak 80)
    { 0, 16, 34, 0,    500, V(  5, 0) },   // tiny flash in gap
    { 0, 16, 35, 0,   1500, V(  0, 0) },   // snap — 16:35 stub (peak 58)
    // 16:36-48 dark gap (8s)
    { 0, 16, 37, 0,   9000, V( 15, 0) },   // 9s rise in 8s dark gap
    { 0, 16, 46, 0,   2000, V(  0, 0) },   // snap — 16:49 stub (peak 54)
    { 0, 16, 50, 0,   2500, V( 10, 0) },   // 2.5s recover
    { 0, 16, 53, 0,   4000, V(  0, 0) },   // snap through 16:53-55 stub cluster
    { 0, 16, 57, 0,   5000, V(  0, 0) },   // hold to 0 — 17:08 fire imminent

    // =========================================================================
    // FIRE CLUSTER — 17:08–17:40  (peak 231, dark gap at 17:35-36 (2s))
    // =========================================================================

    { 0, 17,  2, 0,  32000, V(  0, 0) },   // 32s dark through 17:08-17:34
    // 17:35-36 dark gap (2s) inside cluster
    { 0, 17, 35, 0,   1500, V( 12, 0) },   // ghost flash in gap
    { 0, 17, 37, 0,   3000, V(  0, 0) },   // snap back — 17:38-40 fire
    { 0, 17, 40, 0,   6000, V(  0, 0) },   // hold dark to 17:46

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
    // FLASH — 18:47  +  DARK VALLEY 7 — 18:48–19:27
    // Dark gap at 18:48-50 (3s) right after flash. Fire stub at 19:08 (1s).
    // =========================================================================

    { 0, 18, 41, 0,   6000, V(  0, 0) },   // 6s black — hold through 18:47 flash
    // 18:48-50 dark gap (3s) — lamp touches
    { 0, 18, 47, 0,   1500, V( 10, 0) },   // touch in 3s gap
    { 0, 18, 49, 0,   2500, V(  0, 0) },   // snap back — 18:51 fire stub
    // 18:52 onwards dark
    { 0, 18, 52, 0,  12000, V( 18, 0) },   // 12s rise
    { 0, 19,  4, 0,   3000, V(  0, 0) },   // snap — 19:08 stub (3s early)
    { 0, 19,  7, 0,  12000, V(  8, 0) },   // 12s low glow — scattered fire approaching
    { 0, 19, 19, 0,   5000, V(  0, 0) },   // 5s snap to 0 before 19:28 scattered fire

    // =========================================================================
    // SCATTERED FIRE — 19:28–21:56  (many rapid 1s bursts with dark gaps)
    // NEW STRATEGY: lamp breathes through the gaps. Ember baseline (DMX 5-10),
    // rises in dark windows. Two major luminous windows:
    //   19:58-20:12 (14s) → lamp to DMX 30
    //   21:01-21:23 (23s) → lamp builds to DMX 39, approaching apex
    // =========================================================================

    // --- 19:28 burst zone — isolated 1s flashes with 2-5s gaps ---
    { 0, 19, 24, 0,   3000, V(  0, 0) },   // hold — 19:28 flash (peak 107) imminent
    { 0, 19, 29, 0,   3000, V( 10, 0) },   // 3s rise in 2s dark gap after flash
    { 0, 19, 32, 0,   1000, V(  0, 0) },   // snap — 19:33 flash (peak 158)
    { 0, 19, 34, 0,   4000, V( 18, 0) },   // 4s rise — 5s dark gap
    { 0, 19, 38, 0,   1000, V(  0, 0) },   // snap — 19:39 flash (peak 43)
    { 0, 19, 40, 0,   2000, V(  8, 0) },   // 2s ember
    { 0, 19, 42, 0,   1000, V(  0, 0) },   // snap — 19:42 flash (peak 41)
    { 0, 19, 43, 0,   2000, V( 10, 0) },   // 2s ember
    { 0, 19, 45, 0,   3000, V(  0, 0) },   // snap through 19:45-47 fire (3s)
    { 0, 19, 48, 0,   2000, V( 15, 0) },   // 2s rise — 3s dark gap
    { 0, 19, 50, 0,   5000, V(  0, 0) },   // snap — 19:51-55 fire (5s) + 19:57 flash

    // --- 19:58-20:12 — 14s DARK WINDOW — lamp breathes to DMX 30 ---
    { 0, 19, 58, 0,   8000, V( 30, 0) },   // 8s rise into silence
    { 0, 20,  6, 0,   8000, V( 22, 0) },   // hold warm — 20:13 flash incoming
    { 0, 20, 12, 0,   1000, V(  0, 0) },   // snap — 20:13 flash (peak 57)

    // --- 20:14-20 dark gap (7s) ---
    { 0, 20, 14, 0,   5000, V( 25, 0) },   // 5s rise in 7s gap
    { 0, 20, 19, 0,   2000, V(  0, 0) },   // snap — 20:21 fire cluster (7s, peak 84)
    { 0, 20, 27, 0,   1000, V(  0, 0) },   // hold dark — fire ending
    // 20:28-30 dark gap (3s)
    { 0, 20, 28, 0,   2000, V( 15, 0) },   // touch in 3s gap
    { 0, 20, 30, 0,   1000, V(  0, 0) },   // snap — 20:31 fire (5s, peak 155)
    // 20:36-38 brief gap; 20:39-41 fire (3s)
    { 0, 20, 35, 0,   4000, V(  0, 0) },   // hold — more fire 20:39-41 and 20:43+

    // --- 20:43-20:59 fire (17s) — blackout ---
    { 0, 20, 43, 0,  17000, V(  0, 0) },   // 17s black — densest fire in this zone

    // --- 21:00-21:23 — 23s DARK WINDOW — lamp builds toward apex ---
    { 0, 21,  0, 0,   8000, V( 20, 0) },   // 8s rise — room goes quiet
    { 0, 21,  8, 0,  10000, V( 39, 0) },   // 10s build — apex approaching
    { 0, 21, 18, 0,   4000, V( 27, 0) },   // 4s settle — 21:24 explosion imminent
    { 0, 21, 22, 0,   1500, V(  0, 0) },   // snap — 21:24 flash (peak 180)

    // --- 21:25-28 dark gap (4s) ---
    { 0, 21, 25, 0,   3000, V( 20, 0) },   // 3s in gap
    { 0, 21, 28, 0,   1000, V(  0, 0) },   // snap — 21:29 fire (12s, peak 205)
    { 0, 21, 29, 0,  11000, V(  0, 0) },   // 11s black

    // --- 21:41 1s dark gap ---
    { 0, 21, 40, 0,    500, V(  5, 0) },   // ghost flash in gap
    { 0, 21, 41, 0,   3000, V(  0, 0) },   // snap — 21:42-44 fire (3s, peak 111)
    // 21:45-46 dark (2s)
    { 0, 21, 44, 0,   1500, V(  8, 0) },   // ghost in 2s gap
    { 0, 21, 46, 0,   1000, V(  0, 0) },   // snap — 21:47-50 fire (4s, peak 226)
    // 21:51-53 dark gap (3s) — final ghost before APEX
    { 0, 21, 51, 0,   2000, V( 12, 0) },   // brief ghost
    { 0, 21, 53, 0,   3000, V(  0, 0) },   // fade to 0 — 21:54-56 final flashes, then APEX

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

    // =========================================================================
    // FIRE CLUSTER — 26:18–27:15  (isolated stubs with dark gaps)
    // DARK VALLEY 9 — 27:16–29:00  (130s clean)
    // =========================================================================

    // 26:18-24 fire (7s), 26:26-27 gap, 26:28 flash, 26:32-34 gap, 26:35 flash
    // 26:40-42 gap, 26:44/46 flashes, 26:48-27:02 sustained (15s, peak 84)
    // 27:03-11 dark (9s), 27:12 stub, 27:13-15 gap (3s)
    { 0, 26, 20, 0,   6000, V(  0, 0) },   // 6s black — 26:18-24 fire
    // 26:26-27 dark gap (2s)
    { 0, 26, 26, 0,   1500, V( 12, 0) },   // touch in gap
    { 0, 26, 28, 0,   2000, V(  0, 0) },   // snap — 26:28 flash (peak 150)
    // 26:32-34 gap (3s)
    { 0, 26, 32, 0,   2000, V( 15, 0) },   // 2s rise in 3s gap
    { 0, 26, 34, 0,   1000, V(  0, 0) },   // snap — 26:35 flash (peak 149)
    // 26:40-42 gap (3s)
    { 0, 26, 40, 0,   2000, V( 12, 0) },   // touch
    { 0, 26, 42, 0,   2000, V(  0, 0) },   // snap — 26:44/46 flashes
    // 26:48-27:02 sustained fire (15s)
    { 0, 26, 47, 0,  16000, V(  0, 0) },   // 16s black through 26:48-27:02
    // 27:03-11 dark (9s)
    { 0, 27,  3, 0,   7000, V( 18, 0) },   // 7s rise in 9s window
    { 0, 27, 10, 0,   2000, V(  0, 0) },   // snap — 27:12 stub (peak 65)
    // 27:13-15 gap (3s)
    { 0, 27, 13, 0,   2000, V( 12, 0) },   // touch in gap
    { 0, 27, 15, 0,   2000, V(  0, 0) },   // back to 0 — valley begins cleanly

    // Dark Valley 9 clean section
    { 0, 27, 17, 0,  14000, V(  2, 0) },   // 14s slow rise to floor
    { 0, 27, 31, 0,  15000, V( 23, 0) },   // 15s breathe up
    { 0, 27, 46, 0,  14000, V(  8, 0) },   // 14s exhale
    { 0, 28,  0, 0,  16000, V( 34, 0) },   // 16s breathe up
    { 0, 28, 16, 0,  12000, V( 15, 0) },   // 12s exhale
    { 0, 28, 28, 0,  14000, V( 27, 0) },   // 14s breathe up
    { 0, 28, 42, 0,   5000, V(  0, 0) },   // 5s snap dark before final fire at 29:01

    // =========================================================================
    // FINAL FIRE — 29:01–30:25  (dense bursts, but real dark gaps throughout)
    // Dark gaps: 29:04-09 (6s), 29:28-30 (3s), 29:33-36 (4s), 29:38-41 (4s),
    //            29:48-53 (6s), 29:58-59 (2s), 30:04-12 (8s), 30:17-21 (5s)
    // Lamp pulses through each gap — final dialogue with fire.
    // =========================================================================

    { 0, 28, 47, 0,  16000, V(  0, 0) },   // 16s black through 29:01-02 bursts
    // 29:04-09 dark gap (6s)
    { 0, 29,  3, 0,   4000, V( 20, 0) },   // 4s rise in 6s gap
    { 0, 29,  7, 0,   3000, V(  0, 0) },   // snap — 29:10→29:13 fire approaching
    { 0, 29, 10, 0,   7000, V(  0, 0) },   // hold — 29:13-17 fire (5s), 29:19-27 (9s, peak 237)
    // 29:28-30 dark gap (3s)
    { 0, 29, 28, 0,   2000, V( 15, 0) },   // 2s touch
    { 0, 29, 30, 0,   2000, V(  0, 0) },   // snap — 29:31 flash (peak 90)
    // 29:33-36 dark gap (4s)
    { 0, 29, 33, 0,   3000, V( 18, 0) },   // 3s rise
    { 0, 29, 36, 0,   1500, V(  0, 0) },   // snap — 29:37 flash (peak 51)
    // 29:38-41 dark gap (4s)
    { 0, 29, 38, 0,   3000, V( 18, 0) },   // 3s rise
    { 0, 29, 41, 0,   1000, V(  0, 0) },   // snap — 29:42-47 fire (6s, peak 191)
    // 29:48-53 dark gap (6s)
    { 0, 29, 48, 0,   4000, V( 22, 0) },   // 4s rise in 6s gap
    { 0, 29, 52, 0,   2000, V(  0, 0) },   // snap — 29:55-56 fire
    // 29:58-59 dark gap (2s)
    { 0, 29, 58, 0,   1500, V(  8, 0) },   // brief touch
    { 0, 30,  0, 0,   2000, V(  0, 0) },   // snap — 30:01-03 fire (3s)
    // 30:04-12 dark (8s)
    { 0, 30,  4, 0,   7000, V( 20, 0) },   // 7s rise — 8s dark window
    { 0, 30, 11, 0,   2000, V(  0, 0) },   // snap — 30:13-15 fire (3s, peak 181)
    // 30:17-21 dark gap (5s)
    { 0, 30, 17, 0,   3000, V( 20, 0) },   // 3s rise in 5s gap
    { 0, 30, 20, 0,   4000, V(  0, 0) },   // fade to 0 — 30:22 fire ends, fade-out begins

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
