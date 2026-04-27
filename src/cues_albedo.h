#pragma once

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
// Cue list â€” ALBEDO  (43 min Â· 25fps)
// Varytec LED Theater Spot 50 3200K  (2-channel DMX mode)
// Julian CharriÃ¨re Â· CORRER Â· Venice 2026
//
//   Fixture minimum visible threshold: DMX 22  (below = physically dark)
//   Floor glow = DMX 38.  Hard black = DMX 0 (final fade only).
//
// â”€â”€ COMPOSITIONAL STRATEGY v3 â€” P95 CORRECTED COUNTERPOINT â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//
//   Source: 32x18 spatial luma extraction (analyze_movie_luma.py --reextract)
//   Metric: p95 per second â€” captures bright hot-spots (specular ice/sun)
//           that the old 1x1 mean average could not see.
//
//   The earlier analysis (1x1 area mean) was wrong: it showed a long "dark"
//   valley at 27:45â€“35:17 â€” but p95 reveals that zone is full of intense
//   glacial reflections peaking at 255/255. The lamp apex there was WRONG.
//
//   TRUE dark windows (both mean AND p95 dark â€” lamp CAN be bright):
//     00:00â€“03:33  (213s)  â€” pure dark opening
//     04:52â€“08:49  (237s)  â€” long dark valley
//     22:12â€“24:33  (141s)  â€” post-peak silence *** LAMP APEX HERE ***
//     35:38â€“36:36  (58s)   â€” brief silence before final cluster
//
//   Film bright events by p95 (lamp MUST dip â€” fast 3â€“8s fades):
//     03:33â€“04:52  p95 198   early bright
//     08:49â€“09:48  p95 167   glacial flash
//     10:29â€“10:58  p95 132
//     11:16â€“11:53  p95 132
//     13:00â€“13:36  p95 108
//     14:03â€“15:50  p95 201   sustained major
//     16:58â€“19:01  p95 199   2min sustained
//     20:45â€“22:13  p95 211   building to peak
//     22:56â€“23:33  p95 255   *** ABSOLUTE PEAK ZONE 1 ***
//     24:33â€“26:48  p95 236   sustained
//     26:58â€“27:45  p95 255   *** ABSOLUTE PEAK ZONE 2 ***
//     28:34â€“28:41  p95 106   isolated flash
//     31:07â€“31:17  p95 129   flash
//     32:11â€“32:55  p95 148
//     33:50â€“34:32  p95 161
//     35:09â€“35:38  p95 234   major flash
//     36:53â€“37:45  p95 255   *** ABSOLUTE PEAK ZONE 3 + afterglow ***
//     38:10â€“38:51  p95 231   sustained bright ending
//
//   RULE: Every film bright event â†’ lamp FAST DIP to DMX 22 (3â€“8s fade in)
//         then SLOW RECOVERY back to floor after event (10â€“30s).
//         Lamp apex (DMX 235) only during 22:12â€“24:33 â€” the one long true dark.
//
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

#include "dmx_controller.h"
#include "../include/config.h"

#define V(d, s) \
    { {DMX_CH_MASTER,(d)}, {DMX_CH_STROBE,(s)} }, 2

static const DMXCue CUE_LIST[] = {


    // ----------------------------------------------------------------------
    // ZONE 1 — 00:00–03:33  TRUE DARK (213s)
    // Lamp warm, counterpoint. Film: solid black. Two waves.
    // ----------------------------------------------------------------------

    // Open: 8s fade to floor
    { 0,  0,  0, 0,   8000, V( 38, 0) },   // fade in (? 0:00:08)

    // Wave A  peak 95  (0:00:08 ? 0:01:50)
    { 0,  0,  8, 0,  30000, V( 72, 0) },   // 30s rise (? 0:00:38)
    { 0,  0, 38, 0,  20000, V( 95, 0) },   // 20s surge (? 0:00:58)
    { 0,  0, 58, 0,  52000, V( 38, 0) },   // 52s ebb (? 0:01:50)

    // Wave B  peak 118  (0:01:50 ? 0:03:28)
    { 0,  1, 50, 0,  32000, V( 88, 0) },   // 32s rise (? 0:02:22)
    { 0,  2, 22, 0,  20000, V(118, 0) },   // 20s surge (? 0:02:42)
    { 0,  2, 42, 0,  10000, V(108, 0) },   // 10s shoulder (? 0:02:52)
    { 0,  2, 52, 0,  36000, V( 38, 0) },   // 36s ebb to floor (? 0:03:28)

    // -- FILM BRIGHT  03:33–04:52  p95 198 — FAST DIP ---------------------
    { 0,  3, 28, 0,   5000, V( 22, 0) },   // 5s fast dip to threshold (? 0:03:33)
    { 0,  3, 33, 0,  79000, V( 22, 0) },   // 79s hold at threshold during film bright (? 0:04:52)
    { 0,  4, 52, 0,  15000, V( 38, 0) },   // 15s recover to floor (? 0:05:07)

    // ----------------------------------------------------------------------
    // ZONE 2 — 04:52–08:49  TRUE DARK (237s)
    // Lamp counterpoint, building. Film: solid black.
    // ----------------------------------------------------------------------

    // Wave C  peak 108  (0:05:07 ? 0:06:50)
    { 0,  5,  7, 0,  28000, V( 82, 0) },   // 28s rise (? 0:05:35)
    { 0,  5, 35, 0,  18000, V(108, 0) },   // 18s surge (? 0:05:53)
    { 0,  5, 53, 0,  57000, V( 38, 0) },   // 57s ebb (? 0:06:50)

    // Wave D  peak 128  (0:06:50 ? 0:08:44)
    { 0,  6, 50, 0,  30000, V( 98, 0) },   // 30s rise (? 0:07:20)
    { 0,  7, 20, 0,  20000, V(128, 0) },   // 20s surge (? 0:07:40)
    { 0,  7, 40, 0,  10000, V(118, 0) },   // 10s shoulder (? 0:07:50)
    { 0,  7, 50, 0,  54000, V( 38, 0) },   // 54s ebb to floor (? 0:08:44)

    // -- FILM BRIGHT  08:49–09:48  p95 167 — FAST DIP ---------------------
    { 0,  8, 44, 0,   5000, V( 22, 0) },   // 5s fast dip (? 0:08:49)
    { 0,  8, 49, 0,  59000, V( 22, 0) },   // 59s hold at threshold (? 0:09:48)
    { 0,  9, 48, 0,  12000, V( 38, 0) },   // 12s recover (? 0:10:00)

    // -- FILM BRIGHT  10:29–10:58  p95 132 — FAST DIP ---------------------
    { 0, 10,  0, 0,  25000, V( 62, 0) },   // 25s brief rise in dark gap (? 0:10:25)
    { 0, 10, 25, 0,   4000, V( 22, 0) },   // 4s fast dip (? 0:10:29)
    { 0, 10, 29, 0,  29000, V( 22, 0) },   // 29s hold (? 0:10:58)
    { 0, 10, 58, 0,  10000, V( 38, 0) },   // 10s recover (? 0:11:08)

    // -- FILM BRIGHT  11:16–11:53  p95 132 — FAST DIP ---------------------
    { 0, 11,  8, 0,   8000, V( 22, 0) },   // 8s dip (? 0:11:16)
    { 0, 11, 16, 0,  37000, V( 22, 0) },   // 37s hold (? 0:11:53)
    { 0, 11, 53, 0,  12000, V( 38, 0) },   // 12s recover (? 0:12:05)

    // -- FILM BRIGHT  13:00–13:36  p95 108 — FAST DIP ---------------------
    { 0, 12,  5, 0,  49000, V( 62, 0) },   // 49s gentle rise in dark gap (? 0:12:54)
    { 0, 12, 54, 0,   6000, V( 22, 0) },   // 6s dip (? 0:13:00)
    { 0, 13,  0, 0,  36000, V( 22, 0) },   // 36s hold (? 0:13:36)
    { 0, 13, 36, 0,  12000, V( 38, 0) },   // 12s recover (? 0:13:48)

    // -- FILM BRIGHT  14:03–15:50  p95 201 — MAJOR SUSTAINED DIP ----------
    { 0, 13, 48, 0,  15000, V( 22, 0) },   // 15s dip anticipating bright (? 0:14:03)
    { 0, 14,  3, 0, 107000, V( 22, 0) },   // 107s hold — lamp at threshold through entire flare (? 0:15:50)
    { 0, 15, 50, 0,  15000, V( 38, 0) },   // 15s recover (? 0:16:05)

    // -- FILM BRIGHT  16:58–19:01  p95 199 — 2MIN SUSTAINED DIP -----------
    //   Gap 15:50–16:58: 68s dark. Lamp gets one small pulse then dips.
    { 0, 16,  5, 0,  28000, V( 72, 0) },   // 28s brief counterpoint pulse in gap (? 0:16:33)
    { 0, 16, 33, 0,  25000, V( 38, 0) },   // 25s settle (? 0:16:58)
    { 0, 16, 58, 0,   5000, V( 22, 0) },   // 5s fast dip (? 0:17:03)
    { 0, 17,  3, 0, 118000, V( 22, 0) },   // 118s hold — 2min film bright (? 0:19:01)
    { 0, 19,  1, 0,  20000, V( 38, 0) },   // 20s recover (? 0:19:21)

    // -- FILM BRIGHT  19:26–19:31  p95 108 — brief flash ------------------
    { 0, 19, 21, 0,   5000, V( 22, 0) },   // 5s dip for brief flash (? 0:19:26)
    { 0, 19, 26, 0,   5000, V( 22, 0) },   // 5s hold (? 0:19:31)
    { 0, 19, 31, 0,  12000, V( 38, 0) },   // 12s recover (? 0:19:43)

    // -- FILM BRIGHT  20:45–22:13  p95 211 — building to absolute peak -----
    //   Gap 19:31–20:43: 72s dark. Lamp breathes briefly.
    { 0, 19, 43, 0,  30000, V( 65, 0) },   // 30s brief rise in dark gap (? 0:20:13)
    { 0, 20, 13, 0,  32000, V( 38, 0) },   // 32s settle (? 0:20:45)
    { 0, 20, 45, 0,   5000, V( 22, 0) },   // 5s fast dip (? 0:20:50)
    { 0, 20, 50, 0,  83000, V( 22, 0) },   // 83s hold through entire bright zone (? 0:22:13)
    { 0, 22, 13, 0,   5000, V( 38, 0) },   // 5s recover to floor (? 0:22:18)

    // ----------------------------------------------------------------------
    // ¦¦¦¦ LAMP APEX — 22:18–24:33  TRUE DARK (141s) ¦¦¦¦
    // The ONLY long genuinely dark window in the second half.
    // Lamp rises alone to full power here. Post absolute peak silence.
    // ----------------------------------------------------------------------

    { 0, 22, 18, 0,  20000, V( 72, 0) },   // 20s floor to first level (? 0:22:38)
    { 0, 22, 38, 0,  22000, V(118, 0) },   // 22s climb (? 0:23:00)
    { 0, 23,  0, 0,  20000, V(162, 0) },   // 20s climb (? 0:23:20)
    { 0, 23, 20, 0,  13000, V(198, 0) },   // 13s climb (? 0:23:33)
    // ¦¦ APEX 0:23:33 — DMX 235 ¦¦
    { 0, 23, 33, 0,  12000, V(235, 0) },   // 12s to full power (? 0:23:45)
    { 0, 23, 45, 0,  15000, V(228, 0) },   // 15s peak shoulder (? 0:24:00)
    { 0, 24,  0, 0,  33000, V( 38, 0) },   // 33s descent to floor before film returns (? 0:24:33)

    // -- FILM BRIGHT  24:33–26:48  p95 236 — MAJOR SUSTAINED DIP ----------
    { 0, 24, 33, 0,   5000, V( 22, 0) },   // 5s fast dip (? 0:24:38)
    { 0, 24, 38, 0, 130000, V( 22, 0) },   // 130s hold — very long sustained bright (? 0:26:48)
    { 0, 26, 48, 0,   8000, V( 38, 0) },   // 8s recover (? 0:26:56)

    // -- FILM BRIGHT  26:58–27:45  p95 255 — FAST DIP ---------------------
    { 0, 26, 56, 0,   2000, V( 22, 0) },   // 2s dip (? 0:26:58)
    { 0, 26, 58, 0,  47000, V( 22, 0) },   // 47s hold (? 0:27:45)
    { 0, 27, 45, 0,  10000, V( 38, 0) },   // 10s recover (? 0:27:55)

    // -- FILM BRIGHT  28:34–28:41  p95 106 — brief isolated flash ----------
    { 0, 27, 55, 0,  32000, V( 65, 0) },   // 32s brief counterpoint pulse in gap (? 0:28:27)
    { 0, 28, 27, 0,   7000, V( 22, 0) },   // 7s dip for flash (? 0:28:34)
    { 0, 28, 34, 0,   7000, V( 22, 0) },   // 7s hold (? 0:28:41)
    { 0, 28, 41, 0,  12000, V( 38, 0) },   // 12s recover (? 0:28:53)

    // -- Dark gap  28:53–29:41 (48s) — small counterpoint pulse -----------
    { 0, 28, 53, 0,  25000, V( 62, 0) },   // 25s rise (? 0:29:18)
    { 0, 29, 18, 0,  23000, V( 38, 0) },   // 23s settle (? 0:29:41)

    // -- FILM BRIGHT  29:41–29:53  p95 54 ---------------------------------
    { 0, 29, 41, 0,   5000, V( 22, 0) },   // 5s dip (? 0:29:46)
    { 0, 29, 46, 0,   7000, V( 22, 0) },   // 7s hold (? 0:29:53)
    { 0, 29, 53, 0,  10000, V( 38, 0) },   // 10s recover (? 0:30:03)

    // -- FILM BRIGHT  31:07–31:17  p95 129 --------------------------------
    { 0, 30,  3, 0,  55000, V( 62, 0) },   // 55s small pulse in dark gap (? 0:30:58)
    { 0, 30, 58, 0,   9000, V( 22, 0) },   // 9s dip anticipating (? 0:31:07)
    { 0, 31,  7, 0,  10000, V( 22, 0) },   // 10s hold (? 0:31:17)
    { 0, 31, 17, 0,  12000, V( 38, 0) },   // 12s recover (? 0:31:29)

    // -- FILM BRIGHT  32:11–32:55  p95 148 --------------------------------
    { 0, 31, 29, 0,  32000, V( 62, 0) },   // 32s pulse in dark gap 31:17?32:11 (? 0:32:01)
    { 0, 32,  1, 0,  10000, V( 22, 0) },   // 10s dip anticipating (? 0:32:11)
    { 0, 32, 11, 0,  44000, V( 22, 0) },   // 44s hold (? 0:32:55)
    { 0, 32, 55, 0,  12000, V( 38, 0) },   // 12s recover (? 0:33:07)

    // -- FILM BRIGHT  33:50–34:32  p95 161 --------------------------------
    { 0, 33,  7, 0,  35000, V( 62, 0) },   // 35s pulse in dark gap (? 0:33:42)
    { 0, 33, 42, 0,   8000, V( 22, 0) },   // 8s dip (? 0:33:50)
    { 0, 33, 50, 0,  42000, V( 22, 0) },   // 42s hold (? 0:34:32)
    { 0, 34, 32, 0,  12000, V( 38, 0) },   // 12s recover (? 0:34:44)

    // -- FILM BRIGHT  35:09–35:38  p95 234 — MAJOR FLASH ------------------
    { 0, 34, 44, 0,  20000, V( 55, 0) },   // 20s brief pulse in dark gap (? 0:35:04)
    { 0, 35,  4, 0,   5000, V( 22, 0) },   // 5s dip (? 0:35:09)
    { 0, 35,  9, 0,  29000, V( 22, 0) },   // 29s hold (? 0:35:38)
    { 0, 35, 38, 0,  10000, V( 38, 0) },   // 10s recover (? 0:35:48)

    // ----------------------------------------------------------------------
    // BRIEF TRUE DARK  35:38–36:36  (58s)
    // Small counterpoint pulse — last warmth before film's final peak cluster.
    // ----------------------------------------------------------------------
    { 0, 35, 48, 0,  22000, V( 72, 0) },   // 22s rise (? 0:36:10)
    { 0, 36, 10, 0,  26000, V( 38, 0) },   // 26s settle (? 0:36:36)

    // -- FILM BRIGHT  36:36–36:42  p95 82 — small flash -------------------
    { 0, 36, 36, 0,   4000, V( 22, 0) },   // 4s dip (? 0:36:40)
    { 0, 36, 40, 0,   2000, V( 22, 0) },   // 2s hold (? 0:36:42)
    { 0, 36, 42, 0,   8000, V( 38, 0) },   // 8s recover (? 0:36:50)

    // -- FILM ABSOLUTE PEAK CLUSTER  36:53–37:45  p95 255 -----------------
    //   Three consecutive 255/255 windows. Lamp at threshold throughout.
    { 0, 36, 50, 0,   3000, V( 22, 0) },   // 3s dip (? 0:36:53)
    { 0, 36, 53, 0,  52000, V( 22, 0) },   // 52s hold at threshold — absolute peak cluster (? 0:37:45)
    { 0, 37, 45, 0,  15000, V( 38, 0) },   // 15s recover to floor (? 0:38:00)

    // -- FILM BRIGHT  38:10–38:51  p95 231 — sustained final bright --------
    { 0, 38,  0, 0,   8000, V( 22, 0) },   // 8s dip anticipating (? 0:38:08)
    { 0, 38,  8, 0,   2000, V( 22, 0) },   // 2s hold before bright (? 0:38:10)
    { 0, 38, 10, 0,  41000, V( 22, 0) },   // 41s hold through sustained bright (? 0:38:51)
    { 0, 38, 51, 0,  12000, V( 38, 0) },   // 12s recover to floor (? 0:39:03)

    // ----------------------------------------------------------------------
    // CLOSING — 38:51–42:32  film dark, lamp final exhale
    // ----------------------------------------------------------------------
    { 0, 39,  3, 0,  40000, V( 78, 0) },   // 40s rise (? 0:39:43)
    { 0, 39, 43, 0,  20000, V( 95, 0) },   // 20s peak (? 0:40:03)
    { 0, 40,  3, 0,  10000, V( 88, 0) },   // 10s shoulder (? 0:40:13)
    { 0, 40, 13, 0, 139000, V( 38, 0) },   // 139s very long ebb to floor (? 0:42:32)

    // -- FINAL FADE TO BLACK  42:32 – 43:20 -------------------------------
    { 0, 42, 32, 0,  48000, V(  0, 0) },   // 48s fade to black (? 0:43:20)
    { 0, 43,  0, 0,      0, V(  0, 0) },   // safety snap at loop end

};  // END CUE_LIST

#undef V

static const uint16_t CUE_COUNT = sizeof(CUE_LIST) / sizeof(CUE_LIST[0]);
