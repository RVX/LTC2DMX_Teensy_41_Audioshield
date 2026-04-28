#pragma once
// AUTO-GENERATED — do not edit by hand.
// Generated: 2026-04-28 19:05 UTC  by gen_saber_cues.py
// Source: controlled_burn_luma.csv  metric: yp99_raw
//
// ADJ Saber Spot WW — PER-SECOND PULSE MODEL on p99
//   DMX address 7 · 1-channel mode (CH7 = dimmer, 0–255)
//   Each qualifying second = attack (frame 0) + forced release (HOLD_FRAMES).
//   Normal pulse  : hold 1 frames (~33 ms) then fade at 1500 DMX/s.
//   Burst pulse   : hold 1 frames (~33 ms) for dmx≥120 — strobe feel.
//   Light ALWAYS returns to 0; next attack overrides any ongoing fade.
//
// p99 → DMX mapping:
//   p99 <   15  → DMX   0  (dark)
//   p99 <   30  → DMX   2  (floor ember)
//   p99 = 255  → DMX 160

#include "dmx_controller.h"

// W(dim) — 1-channel cue macro for the Saber Spot WW
#define SABER_CH  7  // ADJ Saber Spot WW — dimmer channel
#define W(d)  { {SABER_CH, (d)} }, 1

static const DMXCue SABER_CUE_LIST[] = {

    // ====================================================================
    // INIT — hard black
    // ====================================================================

    {  0,  0,  0,  0,       0, W(  0) },   // hard black at start
    // ====================================================================
    // EXTRA INJECTED FLASH BURSTS (manual fireworks moments)
    // ====================================================================

    {  0,  3, 21,  0,       0, W(222) },   // 3:21 EXTRA flash#1 att 222
    {  0,  3, 21,  1,      84, W(  0) },   // 3:21 EXTRA flash#1 rel (84ms)
    {  0,  3, 21,  4,       0, W(171) },   // 3:21 EXTRA flash#2 att 171
    {  0,  3, 21,  5,      81, W(  0) },   // 3:21 EXTRA flash#2 rel (81ms)
    {  0,  3, 30,  2,       0, W(215) },   // 3:30 EXTRA flash#1 att 215
    {  0,  3, 30,  3,      52, W(  0) },   // 3:30 EXTRA flash#1 rel (52ms)
    {  0,  3, 30,  5,       0, W(246) },   // 3:30 EXTRA flash#2 att 246
    {  0,  3, 30,  6,      92, W(  0) },   // 3:30 EXTRA flash#2 rel (92ms)
    {  0,  3, 30,  7,       0, W(185) },   // 3:30 EXTRA flash#3 att 185
    {  0,  3, 30,  8,      56, W(  0) },   // 3:30 EXTRA flash#3 rel (56ms)
    {  0,  3, 38,  0,       0, W(241) },   // 3:38 EXTRA flash#1 att 241
    {  0,  3, 38,  1,      81, W(  0) },   // 3:38 EXTRA flash#1 rel (81ms)
    {  0,  3, 38,  3,       0, W(200) },   // 3:38 EXTRA flash#2 att 200
    {  0,  3, 38,  4,     102, W(  0) },   // 3:38 EXTRA flash#2 rel (102ms)
    {  0,  3, 42,  2,       0, W(159) },   // 3:42 EXTRA flash#1 att 159
    {  0,  3, 42,  3,      51, W(  0) },   // 3:42 EXTRA flash#1 rel (51ms)
    {  0,  3, 42,  7,       0, W(215) },   // 3:42 EXTRA flash#2 att 215
    {  0,  3, 42,  8,      75, W(  0) },   // 3:42 EXTRA flash#2 rel (75ms)
    {  0,  3, 42, 11,       0, W(150) },   // 3:42 EXTRA flash#3 att 150
    {  0,  3, 42, 12,      51, W(  0) },   // 3:42 EXTRA flash#3 rel (51ms)
    {  0,  3, 52,  0,       0, W(251) },   // 3:52 EXTRA flash#1 att 251
    {  0,  3, 52,  1,     108, W(  0) },   // 3:52 EXTRA flash#1 rel (108ms)
    {  0,  3, 52,  5,       0, W(177) },   // 3:52 EXTRA flash#2 att 177
    {  0,  3, 52,  6,      94, W(  0) },   // 3:52 EXTRA flash#2 rel (94ms)
    {  0,  3, 52,  8,       0, W(213) },   // 3:52 EXTRA flash#3 att 213
    {  0,  3, 52,  9,      74, W(  0) },   // 3:52 EXTRA flash#3 rel (74ms)
    {  0,  4,  0,  1,       0, W(166) },   // 4:00 EXTRA flash#1 att 166
    {  0,  4,  0,  2,     102, W(  0) },   // 4:00 EXTRA flash#1 rel (102ms)
    {  0,  4,  0,  5,       0, W(237) },   // 4:00 EXTRA flash#2 att 237
    {  0,  4,  0,  6,      46, W(  0) },   // 4:00 EXTRA flash#2 rel (46ms)
    {  0,  4,  0,  7,       0, W(162) },   // 4:00 EXTRA flash#3 att 162
    {  0,  4,  0,  8,      98, W(  0) },   // 4:00 EXTRA flash#3 rel (98ms)
    {  0,  4,  7,  0,       0, W(206) },   // 4:07 EXTRA flash#1 att 206
    {  0,  4,  7,  1,      98, W(  0) },   // 4:07 EXTRA flash#1 rel (98ms)
    {  0,  4, 11,  0,       0, W(238) },   // 4:11 EXTRA flash#1 att 238
    {  0,  4, 11,  1,      56, W(  0) },   // 4:11 EXTRA flash#1 rel (56ms)
    {  0,  4, 11,  4,       0, W(170) },   // 4:11 EXTRA flash#2 att 170
    {  0,  4, 11,  5,      75, W(  0) },   // 4:11 EXTRA flash#2 rel (75ms)
    {  0,  4, 11,  6,       0, W(233) },   // 4:11 EXTRA flash#3 att 233
    {  0,  4, 11,  7,      86, W(  0) },   // 4:11 EXTRA flash#3 rel (86ms)
    {  0,  4, 11, 10,       0, W(160) },   // 4:11 EXTRA flash#4 att 160
    {  0,  4, 11, 11,      87, W(  0) },   // 4:11 EXTRA flash#4 rel (87ms)
    {  0,  4, 53,  1,       0, W(230) },   // 4:53 EXTRA flash#1 att 230
    {  0,  4, 53,  2,      79, W(  0) },   // 4:53 EXTRA flash#1 rel (79ms)
    {  0,  5,  7,  2,       0, W(161) },   // 5:07 EXTRA flash#1 att 161
    {  0,  5,  7,  3,      52, W(  0) },   // 5:07 EXTRA flash#1 rel (52ms)
    {  0,  5,  7,  4,       0, W(211) },   // 5:07 EXTRA flash#2 att 211
    {  0,  5,  7,  5,      49, W(  0) },   // 5:07 EXTRA flash#2 rel (49ms)
    {  0,  5,  7,  7,       0, W(178) },   // 5:07 EXTRA flash#3 att 178
    {  0,  5,  7,  8,     106, W(  0) },   // 5:07 EXTRA flash#3 rel (106ms)
    {  0,  5,  7, 12,       0, W(151) },   // 5:07 EXTRA flash#4 att 151
    {  0,  5,  7, 13,      41, W(  0) },   // 5:07 EXTRA flash#4 rel (41ms)
    // ====================================================================
    // FIRE ZONE A — 5:08–21:55
    // ====================================================================

    {  0,  5, 13,  2,       0, W(160) },   // 5:13+2f FAST att 160
    {  0,  5, 13,  4,     106, W(  0) },   // 5:13+4f rel 160 0 (106ms,FAST)
    {  0,  5, 13, 10,       0, W(106) },   // 5:13+10f SHARP att 106
    {  0,  5, 13, 11,      42, W(  0) },   // 5:13+11f rel 106 0 (42ms,SHARP)
    {  0,  5, 13, 18,       0, W(148) },   // 5:13+18f FAST att 148
    {  0,  5, 13, 19,      98, W(  0) },   // 5:13+19f rel 148 0 (98ms,FAST)
    {  0,  5, 15,  1,       0, W(238) },   // 5:15 EXTRA flash#1 att 238
    {  0,  5, 15,  2,      56, W(  0) },   // 5:15 EXTRA flash#1 rel (56ms)
    {  0,  5, 22,  0,       0, W(199) },   // 5:22 EXTRA flash#1 att 199
    {  0,  5, 22,  1,      51, W(  0) },   // 5:22 EXTRA flash#1 rel (51ms)
    {  0,  5, 22,  5,       0, W(  4) },   // 5:22+5f FAST att 4
    {  0,  5, 22,  6,      20, W(  0) },   // 5:22+6f rel 4 0 (20ms,FAST)
    {  0,  5, 25,  1,       0, W(167) },   // 5:25 EXTRA flash#1 att 167
    {  0,  5, 25,  2,      88, W(  0) },   // 5:25 EXTRA flash#1 rel (88ms)
    {  0,  5, 30,  6,       0, W(135) },   // 5:30+6f SOFT att 135
    {  0,  5, 30,  7,     150, W(  0) },   // 5:30+7f rel 135 0 (150ms,SOFT)
    {  0,  5, 30, 13,       0, W(160) },   // 5:30+13f SHARP att 160
    {  0,  5, 30, 15,      64, W(  0) },   // 5:30+15f rel 160 0 (64ms,SHARP)
    {  0,  5, 30, 24,       0, W(113) },   // 5:30+24f FAST att 113
    {  0,  5, 30, 25,      75, W(  0) },   // 5:30+25f rel 113 0 (75ms,FAST)
    {  0,  5, 34,  3,       0, W(  4) },   // 5:34+3f FAST att 4
    {  0,  5, 34,  5,      20, W(  0) },   // 5:34+5f rel 4 0 (20ms,FAST)
    {  0,  5, 35,  6,       0, W( 37) },   // 5:35+6f SHARP att 37
    {  0,  5, 35,  8,      20, W(  0) },   // 5:35+8f rel 37 0 (20ms,SHARP)
    {  0,  5, 36,  0,       0, W(217) },   // 5:36 EXTRA flash#1 att 217
    {  0,  5, 36,  1,      41, W(  0) },   // 5:36 EXTRA flash#1 rel (41ms)
    {  0,  5, 36,  3,       0, W(242) },   // 5:36 EXTRA flash#2 att 242
    {  0,  5, 36,  4,      67, W(  0) },   // 5:36 EXTRA flash#2 rel (67ms)
    {  0,  5, 42,  5,       0, W(  7) },   // 5:42+5f SOFT att 7
    {  0,  5, 42,  6,      20, W(  0) },   // 5:42+6f rel 7 0 (20ms,SOFT)
    {  0,  5, 43,  2,       0, W(206) },   // 5:43 EXTRA flash#1 att 206
    {  0,  5, 43,  3,      67, W(  0) },   // 5:43 EXTRA flash#1 rel (67ms)
    {  0,  5, 43,  4,       0, W(162) },   // 5:43 EXTRA flash#2 att 162
    {  0,  5, 43,  5,      57, W(  0) },   // 5:43 EXTRA flash#2 rel (57ms)
    {  0,  5, 43,  6,       0, W(225) },   // 5:43 EXTRA flash#3 att 225
    {  0,  5, 43,  7,      89, W(  0) },   // 5:43 EXTRA flash#3 rel (89ms)
    {  0,  5, 43, 10,       0, W(190) },   // 5:43 EXTRA flash#4 att 190
    {  0,  5, 43, 11,      57, W(  0) },   // 5:43 EXTRA flash#4 rel (57ms)
    {  0,  5, 49,  6,       0, W( 29) },   // 5:49+6f FAST att 29
    {  0,  5, 49,  7,      20, W(  0) },   // 5:49+7f rel 29 0 (20ms,FAST)
    {  0,  5, 50,  1,       0, W( 10) },   // 5:50+1f SHARP att 10
    {  0,  5, 50,  3,      20, W(  0) },   // 5:50+3f rel 10 0 (20ms,SHARP)
    {  0,  5, 51,  2,       0, W(160) },   // 5:51 EXTRA flash#1 att 160
    {  0,  5, 51,  3,      71, W(  0) },   // 5:51 EXTRA flash#1 rel (71ms)
    {  0,  5, 51,  6,       0, W(249) },   // 5:51 EXTRA flash#2 att 249
    {  0,  5, 51,  7,      43, W(  0) },   // 5:51 EXTRA flash#2 rel (43ms)
    {  0,  5, 51, 10,       0, W(176) },   // 5:51 EXTRA flash#3 att 176
    {  0,  5, 51, 11,     106, W(  0) },   // 5:51 EXTRA flash#3 rel (106ms)
    {  0,  6,  1,  1,       0, W(232) },   // 6:01 EXTRA flash#1 att 232
    {  0,  6,  1,  2,      46, W(  0) },   // 6:01 EXTRA flash#1 rel (46ms)
    {  0,  6,  1,  4,       0, W(199) },   // 6:01 EXTRA flash#2 att 199
    {  0,  6,  1,  5,      54, W(  0) },   // 6:01 EXTRA flash#2 rel (54ms)
    {  0,  6,  1,  7,       0, W(231) },   // 6:01 EXTRA flash#3 att 231
    {  0,  6,  1,  8,      49, W(  0) },   // 6:01 EXTRA flash#3 rel (49ms)
    {  0,  6,  1,  9,       0, W(196) },   // 6:01 EXTRA flash#4 att 196
    {  0,  6,  1, 10,      80, W(  0) },   // 6:01 EXTRA flash#4 rel (80ms)
    {  0,  6,  3,  2,       0, W(237) },   // 6:03 EXTRA flash#1 att 237
    {  0,  6,  3,  3,      62, W(  0) },   // 6:03 EXTRA flash#1 rel (62ms)
    {  0,  6,  3,  7,       0, W(159) },   // 6:03 EXTRA flash#2 att 159
    {  0,  6,  3,  8,      42, W(  0) },   // 6:03 EXTRA flash#2 rel (42ms)
    {  0,  6,  3,  9,       0, W(251) },   // 6:03 EXTRA flash#3 att 251
    {  0,  6,  3, 10,      90, W(  0) },   // 6:03 EXTRA flash#3 rel (90ms)
    {  0,  6,  3, 13,       0, W(214) },   // 6:03 EXTRA flash#4 att 214
    {  0,  6,  3, 14,      93, W(  0) },   // 6:03 EXTRA flash#4 rel (93ms)
    {  0,  6,  4,  0,       0, W(169) },   // 6:04 EXTRA flash#1 att 169
    {  0,  6,  4,  1,      75, W(  0) },   // 6:04 EXTRA flash#1 rel (75ms)
    {  0,  6,  4,  2,       0, W(215) },   // 6:04 EXTRA flash#2 att 215
    {  0,  6,  4,  3,      66, W(  0) },   // 6:04 EXTRA flash#2 rel (66ms)
    {  0,  6,  4,  5,       0, W(247) },   // 6:04 EXTRA flash#3 att 247
    {  0,  6,  4,  6,      48, W(  0) },   // 6:04 EXTRA flash#3 rel (48ms)
    {  0,  6,  4,  9,       0, W(205) },   // 6:04 EXTRA flash#4 att 205
    {  0,  6,  4, 10,      70, W(  0) },   // 6:04 EXTRA flash#4 rel (70ms)
    {  0,  6,  5,  1,       0, W( 74) },   // 6:05+1f FAST att 74
    {  0,  6,  5,  3,      49, W(  0) },   // 6:05+3f rel 74 0 (49ms,FAST)
    {  0,  6,  9,  0,       0, W(164) },   // 6:09 EXTRA flash#1 att 164
    {  0,  6,  9,  1,      98, W(  0) },   // 6:09 EXTRA flash#1 rel (98ms)
    {  0,  6,  9,  2,       0, W(189) },   // 6:09 EXTRA flash#2 att 189
    {  0,  6,  9,  3,      62, W(  0) },   // 6:09 EXTRA flash#2 rel (62ms)
    {  0,  6, 12,  2,       0, W(239) },   // 6:12 EXTRA flash#1 att 239
    {  0,  6, 12,  3,      41, W(  0) },   // 6:12 EXTRA flash#1 rel (41ms)
    {  0,  6, 16,  6,       0, W(142) },   // 6:16+6f SHARP att 142
    {  0,  6, 16,  7,      56, W(  0) },   // 6:16+7f rel 142 0 (56ms,SHARP)
    {  0,  6, 16, 11,       0, W( 99) },   // 6:16+11f SOFT att 99
    {  0,  6, 16, 13,     110, W(  0) },   // 6:16+13f rel 99 0 (110ms,SOFT)
    {  0,  6, 16, 19,       0, W(131) },   // 6:16+19f SOFT att 131
    {  0,  6, 16, 20,     145, W(  0) },   // 6:16+20f rel 131 0 (145ms,SOFT)
    {  0,  6, 17,  5,       0, W(  6) },   // 6:17+5f SHARP att 6
    {  0,  6, 17,  6,      20, W(  0) },   // 6:17+6f rel 6 0 (20ms,SHARP)
    {  0,  6, 18,  1,       0, W(183) },   // 6:18 EXTRA flash#1 att 183
    {  0,  6, 18,  2,      61, W(  0) },   // 6:18 EXTRA flash#1 rel (61ms)
    {  0,  6, 18,  5,       0, W(245) },   // 6:18 EXTRA flash#2 att 245
    {  0,  6, 18,  6,      74, W(  0) },   // 6:18 EXTRA flash#2 rel (74ms)
    {  0,  6, 18,  7,       0, W(153) },   // 6:18 EXTRA flash#3 att 153
    {  0,  6, 18,  8,      82, W(  0) },   // 6:18 EXTRA flash#3 rel (82ms)
    {  0,  6, 18, 12,       0, W(184) },   // 6:18 EXTRA flash#4 att 184
    {  0,  6, 18, 13,     107, W(  0) },   // 6:18 EXTRA flash#4 rel (107ms)
    {  0,  6, 26,  2,       0, W(223) },   // 6:26 EXTRA flash#1 att 223
    {  0,  6, 26,  3,      49, W(  0) },   // 6:26 EXTRA flash#1 rel (49ms)
    {  0,  6, 26,  7,       0, W(196) },   // 6:26 EXTRA flash#2 att 196
    {  0,  6, 26,  8,      47, W(  0) },   // 6:26 EXTRA flash#2 rel (47ms)
    {  0,  6, 26, 12,       0, W(222) },   // 6:26 EXTRA flash#3 att 222
    {  0,  6, 26, 13,      61, W(  0) },   // 6:26 EXTRA flash#3 rel (61ms)
    {  0,  6, 28,  4,       0, W( 32) },   // 6:28+4f SHARP att 32
    {  0,  6, 28,  5,      20, W(  0) },   // 6:28+5f rel 32 0 (20ms,SHARP)
    {  0,  6, 29,  0,       0, W(  8) },   // 6:29+0f SOFT att 8
    {  0,  6, 29,  1,      20, W(  0) },   // 6:29+1f rel 8 0 (20ms,SOFT)
    {  0,  6, 33,  1,       0, W(248) },   // 6:33 EXTRA flash#1 att 248
    {  0,  6, 33,  2,      53, W(  0) },   // 6:33 EXTRA flash#1 rel (53ms)
    {  0,  6, 33,  3,       0, W(179) },   // 6:33 EXTRA flash#2 att 179
    {  0,  6, 33,  4,     105, W(  0) },   // 6:33 EXTRA flash#2 rel (105ms)
    {  0,  6, 34,  3,       0, W( 30) },   // 6:34+3f SHARP att 30
    {  0,  6, 34,  5,      20, W(  0) },   // 6:34+5f rel 30 0 (20ms,SHARP)
    {  0,  6, 37,  4,       0, W( 12) },   // 6:37+4f FAST att 12
    {  0,  6, 37,  6,      20, W(  0) },   // 6:37+6f rel 12 0 (20ms,FAST)
    {  0,  6, 38,  0,       0, W(210) },   // 6:38 EXTRA flash#1 att 210
    {  0,  6, 38,  1,      86, W(  0) },   // 6:38 EXTRA flash#1 rel (86ms)
    {  0,  6, 38,  2,       0, W( 84) },   // 6:38+2f SOFT att 84
    {  0,  6, 38,  4,      93, W(  0) },   // 6:38+4f rel 84 0 (93ms,SOFT)
    {  0,  6, 38, 13,       0, W(103) },   // 6:38+13f FAST att 103
    {  0,  6, 38, 15,      68, W(  0) },   // 6:38+15f rel 103 0 (68ms,FAST)
    {  0,  6, 38, 20,       0, W(121) },   // 6:38+20f SOFT att 121
    {  0,  6, 38, 21,     134, W(  0) },   // 6:38+21f rel 121 0 (134ms,SOFT)
    {  0,  6, 39,  2,       0, W( 11) },   // 6:39+2f FAST att 11
    {  0,  6, 39,  3,      20, W(  0) },   // 6:39+3f rel 11 0 (20ms,FAST)
    {  0,  6, 41,  1,       0, W( 65) },   // 6:41+1f SOFT att 65
    {  0,  6, 41,  3,      72, W(  0) },   // 6:41+3f rel 65 0 (72ms,SOFT)
    {  0,  6, 42,  4,       0, W( 27) },   // 6:42+4f SOFT att 27
    {  0,  6, 42,  5,      30, W(  0) },   // 6:42+5f rel 27 0 (30ms,SOFT)
    {  0,  6, 44,  6,       0, W(  7) },   // 6:44+6f SOFT att 7
    {  0,  6, 44,  7,      20, W(  0) },   // 6:44+7f rel 7 0 (20ms,SOFT)
    {  0,  6, 45,  3,       0, W( 92) },   // 6:45+3f SOFT att 92
    {  0,  6, 45,  4,     102, W(  0) },   // 6:45+4f rel 92 0 (102ms,SOFT)
    {  0,  6, 45,  9,       0, W(120) },   // 6:45+9f FAST att 120
    {  0,  6, 45, 11,      80, W(  0) },   // 6:45+11f rel 120 0 (80ms,FAST)
    {  0,  6, 45, 22,       0, W( 98) },   // 6:45+22f FAST att 98
    {  0,  6, 45, 24,      65, W(  0) },   // 6:45+24f rel 98 0 (65ms,FAST)
    {  0,  6, 46,  5,       0, W(125) },   // 6:46+5f SHARP att 125
    {  0,  6, 46,  6,      50, W(  0) },   // 6:46+6f rel 125 0 (50ms,SHARP)
    {  0,  6, 47,  4,       0, W(107) },   // 6:47+4f FAST att 107
    {  0,  6, 47,  5,      71, W(  0) },   // 6:47+5f rel 107 0 (71ms,FAST)
    {  0,  6, 47, 17,       0, W(130) },   // 6:47+17f FAST att 130
    {  0,  6, 47, 18,      86, W(  0) },   // 6:47+18f rel 130 0 (86ms,FAST)
    {  0,  6, 47, 25,       0, W(107) },   // 6:47+25f FAST att 107
    {  0,  6, 47, 26,      71, W(  0) },   // 6:47+26f rel 107 0 (71ms,FAST)
    {  0,  6, 55,  0,       0, W(154) },   // 6:55 EXTRA flash#1 att 154
    {  0,  6, 55,  1,     110, W(  0) },   // 6:55 EXTRA flash#1 rel (110ms)
    {  0,  6, 55,  5,       0, W(191) },   // 6:55 EXTRA flash#2 att 191
    {  0,  6, 55,  6,      71, W(  0) },   // 6:55 EXTRA flash#2 rel (71ms)
    {  0,  7,  0,  4,       0, W( 20) },   // 7:00+4f SOFT att 20
    {  0,  7,  0,  5,      22, W(  0) },   // 7:00+5f rel 20 0 (22ms,SOFT)
    {  0,  7,  1,  0,       0, W( 87) },   // 7:01+0f SOFT att 87
    {  0,  7,  1,  1,      96, W(  0) },   // 7:01+1f rel 87 0 (96ms,SOFT)
    {  0,  7,  1, 11,       0, W( 67) },   // 7:01+11f SOFT att 67
    {  0,  7,  1, 13,      74, W(  0) },   // 7:01+13f rel 67 0 (74ms,SOFT)
    {  0,  7,  2,  0,       0, W( 48) },   // 7:02+0f FAST att 48
    {  0,  7,  2,  1,      32, W(  0) },   // 7:02+1f rel 48 0 (32ms,FAST)
    {  0,  7,  5,  1,       0, W(  6) },   // 7:05+1f SHARP att 6
    {  0,  7,  5,  2,      20, W(  0) },   // 7:05+2f rel 6 0 (20ms,SHARP)
    {  0,  7,  8,  0,       0, W( 25) },   // 7:08+0f SHARP att 25
    {  0,  7,  8,  1,      20, W(  0) },   // 7:08+1f rel 25 0 (20ms,SHARP)
    {  0,  7, 10,  3,       0, W( 50) },   // 7:10+3f FAST att 50
    {  0,  7, 10,  4,      33, W(  0) },   // 7:10+4f rel 50 0 (33ms,FAST)
    {  0,  7, 15,  6,       0, W(101) },   // 7:15+6f SHARP att 101
    {  0,  7, 15,  7,      40, W(  0) },   // 7:15+7f rel 101 0 (40ms,SHARP)
    {  0,  7, 16,  2,       0, W( 31) },   // 7:16+2f FAST att 31
    {  0,  7, 16,  4,      20, W(  0) },   // 7:16+4f rel 31 0 (20ms,FAST)
    {  0,  7, 19,  1,       0, W(255) },   // 7:19 EXTRA flash#1 att 255
    {  0,  7, 19,  2,      49, W(  0) },   // 7:19 EXTRA flash#1 rel (49ms)
    {  0,  7, 19,  5,       0, W(165) },   // 7:19 EXTRA flash#2 att 165
    {  0,  7, 19,  6,     104, W(  0) },   // 7:19 EXTRA flash#2 rel (104ms)
    {  0,  7, 19,  7,       0, W(252) },   // 7:19 EXTRA flash#3 att 252
    {  0,  7, 19,  8,     106, W(  0) },   // 7:19 EXTRA flash#3 rel (106ms)
    {  0,  7, 26,  0,       0, W( 49) },   // 7:26+0f SOFT att 49
    {  0,  7, 26,  2,      54, W(  0) },   // 7:26+2f rel 49 0 (54ms,SOFT)
    {  0,  7, 27,  4,       0, W( 15) },   // 7:27+4f SHARP att 15
    {  0,  7, 27,  6,      20, W(  0) },   // 7:27+6f rel 15 0 (20ms,SHARP)
    {  0,  7, 28,  1,       0, W( 33) },   // 7:28+1f FAST att 33
    {  0,  7, 28,  2,      22, W(  0) },   // 7:28+2f rel 33 0 (22ms,FAST)
    {  0,  7, 35,  4,       0, W( 11) },   // 7:35+4f FAST att 11
    {  0,  7, 35,  5,      20, W(  0) },   // 7:35+5f rel 11 0 (20ms,FAST)
    {  0,  7, 36,  4,       0, W( 35) },   // 7:36+4f SHARP att 35
    {  0,  7, 36,  5,      20, W(  0) },   // 7:36+5f rel 35 0 (20ms,SHARP)
    {  0,  7, 38,  5,       0, W(  8) },   // 7:38+5f SHARP att 8
    {  0,  7, 38,  6,      20, W(  0) },   // 7:38+6f rel 8 0 (20ms,SHARP)
    {  0,  8, 41,  5,       0, W( 31) },   // 8:41+5f SHARP att 31
    {  0,  8, 41,  6,      20, W(  0) },   // 8:41+6f rel 31 0 (20ms,SHARP)
    {  0,  8, 42,  4,       0, W( 52) },   // 8:42+4f SHARP att 52
    {  0,  8, 42,  5,      20, W(  0) },   // 8:42+5f rel 52 0 (20ms,SHARP)
    {  0,  8, 43,  3,       0, W( 30) },   // 8:43+3f SOFT att 30
    {  0,  8, 43,  5,      33, W(  0) },   // 8:43+5f rel 30 0 (33ms,SOFT)
    {  0,  8, 44,  6,       0, W( 49) },   // 8:44+6f SHARP att 49
    {  0,  8, 44,  7,      20, W(  0) },   // 8:44+7f rel 49 0 (20ms,SHARP)
    {  0,  8, 45,  5,       0, W( 79) },   // 8:45+5f FAST att 79
    {  0,  8, 45,  6,      52, W(  0) },   // 8:45+6f rel 79 0 (52ms,FAST)
    {  0,  8, 46,  0,       0, W(146) },   // 8:46+0f SHARP att 146
    {  0,  8, 46,  1,      58, W(  0) },   // 8:46+1f rel 146 0 (58ms,SHARP)
    {  0,  8, 46,  6,       0, W(105) },   // 8:46+6f FAST att 105
    {  0,  8, 46,  8,      70, W(  0) },   // 8:46+8f rel 105 0 (70ms,FAST)
    {  0,  8, 47,  4,       0, W( 53) },   // 8:47+4f SOFT att 53
    {  0,  8, 47,  6,      58, W(  0) },   // 8:47+6f rel 53 0 (58ms,SOFT)
    {  0,  8, 48,  4,       0, W( 19) },   // 8:48+4f FAST att 19
    {  0,  8, 48,  6,      20, W(  0) },   // 8:48+6f rel 19 0 (20ms,FAST)
    {  0,  9,  4,  3,       0, W(  2) },   // 9:04+3f FAST att 2
    {  0,  9,  4,  4,      20, W(  0) },   // 9:04+4f rel 2 0 (20ms,FAST)
    {  0,  9,  5,  2,       0, W( 26) },   // 9:05+2f SOFT att 26
    {  0,  9,  5,  3,      28, W(  0) },   // 9:05+3f rel 26 0 (28ms,SOFT)
    {  0,  9,  6,  5,       0, W(  6) },   // 9:06+5f SOFT att 6
    {  0,  9,  6,  7,      20, W(  0) },   // 9:06+7f rel 6 0 (20ms,SOFT)
    {  0,  9,  7,  6,       0, W( 43) },   // 9:07+6f SOFT att 43
    {  0,  9,  7,  8,      47, W(  0) },   // 9:07+8f rel 43 0 (47ms,SOFT)
    {  0,  9,  8,  2,       0, W( 68) },   // 9:08+2f SHARP att 68
    {  0,  9,  8,  3,      27, W(  0) },   // 9:08+3f rel 68 0 (27ms,SHARP)
    {  0,  9,  9,  6,       0, W( 32) },   // 9:09+6f SHARP att 32
    {  0,  9,  9,  8,      20, W(  0) },   // 9:09+8f rel 32 0 (20ms,SHARP)
    {  0, 10, 28,  0,       0, W(209) },   // 10:28 EXTRA flash#1 att 209
    {  0, 10, 28,  1,      76, W(  0) },   // 10:28 EXTRA flash#1 rel (76ms)
    {  0, 10, 28,  4,       0, W(161) },   // 10:28 EXTRA flash#2 att 161
    {  0, 10, 28,  5,     110, W(  0) },   // 10:28 EXTRA flash#2 rel (110ms)
    {  0, 10, 28,  9,       0, W(197) },   // 10:28 EXTRA flash#3 att 197
    {  0, 10, 28, 10,      65, W(  0) },   // 10:28 EXTRA flash#3 rel (65ms)
    {  0, 10, 28, 13,       0, W(222) },   // 10:28 EXTRA flash#4 att 222
    {  0, 10, 28, 14,      78, W(  0) },   // 10:28 EXTRA flash#4 rel (78ms)
    {  0, 10, 43,  1,       0, W(248) },   // 10:43 EXTRA flash#1 att 248
    {  0, 10, 43,  2,      65, W(  0) },   // 10:43 EXTRA flash#1 rel (65ms)
    {  0, 10, 43,  4,       0, W(169) },   // 10:43 EXTRA flash#2 att 169
    {  0, 10, 43,  5,      40, W(  0) },   // 10:43 EXTRA flash#2 rel (40ms)
    {  0, 10, 43,  9,       0, W(219) },   // 10:43 EXTRA flash#3 att 219
    {  0, 10, 43, 10,      84, W(  0) },   // 10:43 EXTRA flash#3 rel (84ms)
    {  0, 10, 43, 11,       0, W(192) },   // 10:43 EXTRA flash#4 att 192
    {  0, 10, 43, 12,      40, W(  0) },   // 10:43 EXTRA flash#4 rel (40ms)
    {  0, 10, 50,  0,       0, W(217) },   // 10:50 EXTRA flash#1 att 217
    {  0, 10, 50,  1,      70, W(  0) },   // 10:50 EXTRA flash#1 rel (70ms)
    {  0, 10, 50,  5,       0, W(157) },   // 10:50 EXTRA flash#2 att 157
    {  0, 10, 50,  6,      58, W(  0) },   // 10:50 EXTRA flash#2 rel (58ms)
    {  0, 10, 50,  9,       0, W(216) },   // 10:50 EXTRA flash#3 att 216
    {  0, 10, 50, 10,      92, W(  0) },   // 10:50 EXTRA flash#3 rel (92ms)
    {  0, 10, 52,  2,       0, W( 10) },   // 10:52+2f FAST att 10
    {  0, 10, 52,  3,      20, W(  0) },   // 10:52+3f rel 10 0 (20ms,FAST)
    {  0, 10, 53,  4,       0, W(  2) },   // 10:53+4f SHARP att 2
    {  0, 10, 53,  5,      20, W(  0) },   // 10:53+5f rel 2 0 (20ms,SHARP)
    {  0, 10, 57,  2,       0, W(160) },   // 10:57 EXTRA flash#1 att 160
    {  0, 10, 57,  3,      52, W(  0) },   // 10:57 EXTRA flash#1 rel (52ms)
    {  0, 10, 57,  5,       0, W(231) },   // 10:57 EXTRA flash#2 att 231
    {  0, 10, 57,  6,      90, W(  0) },   // 10:57 EXTRA flash#2 rel (90ms)
    {  0, 10, 57,  7,       0, W(157) },   // 10:57 EXTRA flash#3 att 157
    {  0, 10, 57,  8,      95, W(  0) },   // 10:57 EXTRA flash#3 rel (95ms)
    {  0, 10, 57, 10,       0, W(187) },   // 10:57 EXTRA flash#4 att 187
    {  0, 10, 57, 11,      73, W(  0) },   // 10:57 EXTRA flash#4 rel (73ms)
    {  0, 11,  5,  4,       0, W( 23) },   // 11:05+4f SHARP att 23
    {  0, 11,  5,  5,      20, W(  0) },   // 11:05+5f rel 23 0 (20ms,SHARP)
    {  0, 11,  6,  2,       0, W(244) },   // 11:06 EXTRA flash#1 att 244
    {  0, 11,  6,  3,       0, W(  2) },   // 11:06+3f SOFT att 2
    {  0, 11,  6,  3,      81, W(  0) },   // 11:06 EXTRA flash#1 rel (81ms)
    {  0, 11,  6,  4,      20, W(  0) },   // 11:06+4f rel 2 0 (20ms,SOFT)
    {  0, 11,  6,  6,       0, W(207) },   // 11:06 EXTRA flash#2 att 207
    {  0, 11,  6,  7,      74, W(  0) },   // 11:06 EXTRA flash#2 rel (74ms)
    {  0, 11,  6,  9,       0, W(159) },   // 11:06 EXTRA flash#3 att 159
    {  0, 11,  6, 10,      65, W(  0) },   // 11:06 EXTRA flash#3 rel (65ms)
    {  0, 11,  6, 12,       0, W(249) },   // 11:06 EXTRA flash#4 att 249
    {  0, 11,  6, 13,      53, W(  0) },   // 11:06 EXTRA flash#4 rel (53ms)
    {  0, 11, 18,  2,       0, W( 27) },   // 11:18+2f FAST att 27
    {  0, 11, 18,  3,      20, W(  0) },   // 11:18+3f rel 27 0 (20ms,FAST)
    {  0, 11, 19,  2,       0, W( 45) },   // 11:19+2f SHARP att 45
    {  0, 11, 19,  3,      20, W(  0) },   // 11:19+3f rel 45 0 (20ms,SHARP)
    {  0, 11, 22,  0,       0, W(207) },   // 11:22 EXTRA flash#1 att 207
    {  0, 11, 22,  1,      99, W(  0) },   // 11:22 EXTRA flash#1 rel (99ms)
    {  0, 11, 24,  4,       0, W(152) },   // 11:24+4f STROBE att 152
    {  0, 11, 24,  5,      36, W(  0) },   // 11:24+5f STROBE rel (36ms)
    {  0, 11, 26,  2,       0, W(184) },   // 11:26+2f STROBE att 184
    {  0, 11, 26,  3,      60, W(  0) },   // 11:26+3f STROBE rel (60ms)
    {  0, 11, 36,  4,       0, W(158) },   // 11:36+4f STROBE att 158
    {  0, 11, 36,  5,      53, W(  0) },   // 11:36+5f STROBE rel (53ms)
    {  0, 11, 40,  1,       0, W(217) },   // 11:40+1f STROBE att 217
    {  0, 11, 40,  2,      66, W(  0) },   // 11:40+2f STROBE rel (66ms)
    {  0, 11, 43,  0,       0, W(126) },   // 11:43+0f STROBE att 126
    {  0, 11, 43,  1,      52, W(  0) },   // 11:43+1f STROBE rel (52ms)
    {  0, 11, 47,  3,       0, W(229) },   // 11:47+3f STROBE att 229
    {  0, 11, 47,  4,      56, W(  0) },   // 11:47+4f STROBE rel (56ms)
    {  0, 11, 47, 11,       0, W(205) },   // 11:47+11f STROBE att 205
    {  0, 11, 47, 12,      45, W(  0) },   // 11:47+12f STROBE rel (45ms)
    {  0, 11, 49,  0,       0, W(230) },   // 11:49+0f STROBE att 230
    {  0, 11, 49,  1,      66, W(  0) },   // 11:49+1f STROBE rel (66ms)
    {  0, 11, 51,  0,       0, W(248) },   // 11:51 EXTRA flash#1 att 248
    {  0, 11, 51,  1,      66, W(  0) },   // 11:51 EXTRA flash#1 rel (66ms)
    {  0, 11, 51,  5,       0, W(189) },   // 11:51 EXTRA flash#2 att 189
    {  0, 11, 51,  6,      99, W(  0) },   // 11:51 EXTRA flash#2 rel (99ms)
    {  0, 11, 51,  9,       0, W(161) },   // 11:51 EXTRA flash#3 att 161
    {  0, 11, 51, 10,      60, W(  0) },   // 11:51 EXTRA flash#3 rel (60ms)
    {  0, 11, 51, 13,       0, W(255) },   // 11:51 EXTRA flash#4 att 255
    {  0, 11, 51, 14,      45, W(  0) },   // 11:51 EXTRA flash#4 rel (45ms)
    {  0, 11, 52,  3,       0, W(171) },   // 11:52+3f STROBE att 171
    {  0, 11, 52,  4,      40, W(  0) },   // 11:52+4f STROBE rel (40ms)
    {  0, 11, 55,  3,       0, W(230) },   // 11:55+3f STROBE att 230
    {  0, 11, 55,  4,      39, W(  0) },   // 11:55+4f STROBE rel (39ms)
    {  0, 11, 57,  0,       0, W(174) },   // 11:57+0f STROBE att 174
    {  0, 11, 57,  1,      65, W(  0) },   // 11:57+1f STROBE rel (65ms)
    {  0, 12,  3,  3,       0, W(216) },   // 12:03+3f STROBE att 216
    {  0, 12,  3,  4,      67, W(  0) },   // 12:03+4f STROBE rel (67ms)
    {  0, 12,  4,  0,       0, W(161) },   // 12:04 EXTRA flash#1 att 161
    {  0, 12,  4,  1,      71, W(  0) },   // 12:04 EXTRA flash#1 rel (71ms)
    {  0, 12,  4,  4,       0, W(200) },   // 12:04 EXTRA flash#2 att 200
    {  0, 12,  4,  5,     105, W(  0) },   // 12:04 EXTRA flash#2 rel (105ms)
    {  0, 12,  4,  6,       0, W(236) },   // 12:04 EXTRA flash#3 att 236
    {  0, 12,  4,  7,      78, W(  0) },   // 12:04 EXTRA flash#3 rel (78ms)
    {  0, 12,  7,  2,       0, W(189) },   // 12:07+2f STROBE att 189
    {  0, 12,  7,  3,      61, W(  0) },   // 12:07+3f STROBE rel (61ms)
    {  0, 12, 10,  4,       0, W(230) },   // 12:10+4f STROBE att 230
    {  0, 12, 10,  5,      55, W(  0) },   // 12:10+5f STROBE rel (55ms)
    {  0, 12, 16,  2,       0, W(166) },   // 12:16+2f STROBE att 166
    {  0, 12, 16,  3,      49, W(  0) },   // 12:16+3f STROBE rel (49ms)
    {  0, 12, 18,  0,       0, W(230) },   // 12:18+0f STROBE att 230
    {  0, 12, 18,  1,      44, W(  0) },   // 12:18+1f STROBE rel (44ms)
    {  0, 12, 24,  0,       0, W(201) },   // 12:24 EXTRA flash#1 att 201
    {  0, 12, 24,  1,      97, W(  0) },   // 12:24 EXTRA flash#1 rel (97ms)
    {  0, 12, 24,  3,       0, W(162) },   // 12:24 EXTRA flash#2 att 162
    {  0, 12, 24,  4,      99, W(  0) },   // 12:24 EXTRA flash#2 rel (99ms)
    {  0, 12, 27,  3,       0, W(178) },   // 12:27+3f STROBE att 178
    {  0, 12, 27,  4,      49, W(  0) },   // 12:27+4f STROBE rel (49ms)
    {  0, 12, 30,  0,       0, W(230) },   // 12:30+0f STROBE att 230
    {  0, 12, 30,  1,      42, W(  0) },   // 12:30+1f STROBE rel (42ms)
    {  0, 12, 32,  1,       0, W(182) },   // 12:32+1f STROBE att 182
    {  0, 12, 32,  2,      59, W(  0) },   // 12:32+2f STROBE rel (59ms)
    {  0, 12, 37,  3,       0, W(226) },   // 12:37+3f STROBE att 226
    {  0, 12, 37,  4,      52, W(  0) },   // 12:37+4f STROBE rel (52ms)
    {  0, 12, 38,  0,       0, W(249) },   // 12:38 EXTRA flash#1 att 249
    {  0, 12, 38,  1,      68, W(  0) },   // 12:38 EXTRA flash#1 rel (68ms)
    {  0, 12, 38,  3,       0, W(175) },   // 12:38 EXTRA flash#2 att 175
    {  0, 12, 38,  4,      91, W(  0) },   // 12:38 EXTRA flash#2 rel (91ms)
    {  0, 12, 39,  0,       0, W(192) },   // 12:39+0f STROBE att 192
    {  0, 12, 39,  1,      54, W(  0) },   // 12:39+1f STROBE rel (54ms)
    {  0, 12, 41,  1,       0, W(227) },   // 12:41+1f STROBE att 227
    {  0, 12, 41,  2,      43, W(  0) },   // 12:41+2f STROBE rel (43ms)
    {  0, 12, 49,  1,       0, W(192) },   // 12:49+1f STROBE att 192
    {  0, 12, 49,  2,      49, W(  0) },   // 12:49+2f STROBE rel (49ms)
    {  0, 12, 49, 10,       0, W(230) },   // 12:49+10f STROBE att 230
    {  0, 12, 49, 11,      47, W(  0) },   // 12:49+11f STROBE rel (47ms)
    {  0, 12, 54,  2,       0, W(225) },   // 12:54 EXTRA flash#1 att 225
    {  0, 12, 54,  3,      73, W(  0) },   // 12:54 EXTRA flash#1 rel (73ms)
    {  0, 12, 54,  4,       0, W(210) },   // 12:54+4f STROBE att 210
    {  0, 12, 54,  5,      36, W(  0) },   // 12:54+5f STROBE rel (36ms)
    {  0, 13, 25,  0,       0, W(150) },   // 13:25+0f STROBE att 150
    {  0, 13, 25,  1,      51, W(  0) },   // 13:25+1f STROBE rel (51ms)
    {  0, 13, 35,  2,       0, W(202) },   // 13:35+2f STROBE att 202
    {  0, 13, 35,  3,      45, W(  0) },   // 13:35+3f STROBE rel (45ms)
    {  0, 13, 53,  3,       0, W(163) },   // 13:53+3f STROBE att 163
    {  0, 13, 53,  4,      44, W(  0) },   // 13:53+4f STROBE rel (44ms)
    {  0, 13, 58,  0,       0, W(175) },   // 13:58 EXTRA flash#1 att 175
    {  0, 13, 58,  1,      96, W(  0) },   // 13:58 EXTRA flash#1 rel (96ms)
    {  0, 14,  6,  2,       0, W(230) },   // 14:06+2f STROBE att 230
    {  0, 14,  6,  3,      65, W(  0) },   // 14:06+3f STROBE rel (65ms)
    {  0, 14,  6,  7,       0, W(182) },   // 14:06+7f STROBE att 182
    {  0, 14,  6,  8,      44, W(  0) },   // 14:06+8f STROBE rel (44ms)
    {  0, 14, 14,  1,       0, W(209) },   // 14:14 EXTRA flash#1 att 209
    {  0, 14, 14,  2,      46, W(  0) },   // 14:14 EXTRA flash#1 rel (46ms)
    {  0, 14, 16,  4,       0, W(230) },   // 14:16+4f STROBE att 230
    {  0, 14, 16,  5,      34, W(  0) },   // 14:16+5f STROBE rel (34ms)
    {  0, 14, 16, 11,       0, W(209) },   // 14:16+11f STROBE att 209
    {  0, 14, 16, 12,      54, W(  0) },   // 14:16+12f STROBE rel (54ms)
    {  0, 14, 18,  1,       0, W(230) },   // 14:18+1f STROBE att 230
    {  0, 14, 18,  2,      51, W(  0) },   // 14:18+2f STROBE rel (51ms)
    {  0, 14, 18,  8,       0, W(169) },   // 14:18+8f STROBE att 169
    {  0, 14, 18,  9,      41, W(  0) },   // 14:18+9f STROBE rel (41ms)
    {  0, 14, 22,  2,       0, W(230) },   // 14:22+2f STROBE att 230
    {  0, 14, 22,  3,      66, W(  0) },   // 14:22+3f STROBE rel (66ms)
    {  0, 14, 22,  7,       0, W(209) },   // 14:22+7f STROBE att 209
    {  0, 14, 22,  8,      35, W(  0) },   // 14:22+8f STROBE rel (35ms)
    {  0, 14, 22, 12,       0, W(230) },   // 14:22+12f STROBE att 230
    {  0, 14, 22, 13,      54, W(  0) },   // 14:22+13f STROBE rel (54ms)
    {  0, 14, 24,  0,       0, W(159) },   // 14:24+0f STROBE att 159
    {  0, 14, 24,  1,      30, W(  0) },   // 14:24+1f STROBE rel (30ms)
    {  0, 14, 24,  9,       0, W(230) },   // 14:24+9f STROBE att 230
    {  0, 14, 24, 10,      44, W(  0) },   // 14:24+10f STROBE rel (44ms)
    {  0, 14, 28,  2,       0, W(159) },   // 14:28+2f STROBE att 159
    {  0, 14, 28,  3,      32, W(  0) },   // 14:28+3f STROBE rel (32ms)
    {  0, 14, 34,  2,       0, W(153) },   // 14:34 EXTRA flash#1 att 153
    {  0, 14, 34,  3,      88, W(  0) },   // 14:34 EXTRA flash#1 rel (88ms)
    {  0, 14, 34,  7,       0, W(195) },   // 14:34 EXTRA flash#2 att 195
    {  0, 14, 34,  8,      67, W(  0) },   // 14:34 EXTRA flash#2 rel (67ms)
    {  0, 14, 34, 10,       0, W(169) },   // 14:34 EXTRA flash#3 att 169
    {  0, 14, 34, 11,      44, W(  0) },   // 14:34 EXTRA flash#3 rel (44ms)
    {  0, 14, 34, 13,       0, W(232) },   // 14:34 EXTRA flash#4 att 232
    {  0, 14, 34, 14,      77, W(  0) },   // 14:34 EXTRA flash#4 rel (77ms)
    {  0, 14, 42,  3,       0, W(230) },   // 14:42+3f STROBE att 230
    {  0, 14, 42,  4,      55, W(  0) },   // 14:42+4f STROBE rel (55ms)
    {  0, 14, 44,  0,       0, W(178) },   // 14:44+0f STROBE att 178
    {  0, 14, 44,  1,      53, W(  0) },   // 14:44+1f STROBE rel (53ms)
    {  0, 14, 44,  2,       0, W(157) },   // 14:44 EXTRA flash#1 att 157
    {  0, 14, 44,  3,      51, W(  0) },   // 14:44 EXTRA flash#1 rel (51ms)
    {  0, 14, 44,  6,       0, W(199) },   // 14:44 EXTRA flash#2 att 199
    {  0, 14, 44,  7,      57, W(  0) },   // 14:44 EXTRA flash#2 rel (57ms)
    {  0, 14, 44, 11,       0, W(232) },   // 14:44 EXTRA flash#3 att 232
    {  0, 14, 44, 12,     107, W(  0) },   // 14:44 EXTRA flash#3 rel (107ms)
    {  0, 14, 44, 14,       0, W(152) },   // 14:44 EXTRA flash#4 att 152
    {  0, 14, 44, 15,      88, W(  0) },   // 14:44 EXTRA flash#4 rel (88ms)
    {  0, 14, 47,  2,       0, W(230) },   // 14:47+2f STROBE att 230
    {  0, 14, 47,  3,      69, W(  0) },   // 14:47+3f STROBE rel (69ms)
    {  0, 14, 49,  2,       0, W(250) },   // 14:49 EXTRA flash#1 att 250
    {  0, 14, 49,  3,     102, W(  0) },   // 14:49 EXTRA flash#1 rel (102ms)
    {  0, 14, 49,  6,       0, W(222) },   // 14:49 EXTRA flash#2 att 222
    {  0, 14, 49,  7,     104, W(  0) },   // 14:49 EXTRA flash#2 rel (104ms)
    {  0, 14, 49, 10,       0, W(184) },   // 14:49 EXTRA flash#3 att 184
    {  0, 14, 49, 11,      63, W(  0) },   // 14:49 EXTRA flash#3 rel (63ms)
    {  0, 14, 49, 12,       0, W(226) },   // 14:49 EXTRA flash#4 att 226
    {  0, 14, 49, 13,      68, W(  0) },   // 14:49 EXTRA flash#4 rel (68ms)
    {  0, 14, 56,  0,       0, W(253) },   // 14:56 EXTRA flash#1 att 253
    {  0, 14, 56,  1,     100, W(  0) },   // 14:56 EXTRA flash#1 rel (100ms)
    {  0, 14, 56,  4,       0, W(224) },   // 14:56 EXTRA flash#2 att 224
    {  0, 14, 56,  5,      69, W(  0) },   // 14:56 EXTRA flash#2 rel (69ms)
    {  0, 14, 56,  7,       0, W(162) },   // 14:56 EXTRA flash#3 att 162
    {  0, 14, 56,  8,      71, W(  0) },   // 14:56 EXTRA flash#3 rel (71ms)
    {  0, 15,  7,  2,       0, W(254) },   // 15:07 EXTRA flash#1 att 254
    {  0, 15,  7,  3,      47, W(  0) },   // 15:07 EXTRA flash#1 rel (47ms)
    {  0, 15,  7,  5,       0, W(223) },   // 15:07 EXTRA flash#2 att 223
    {  0, 15,  7,  6,      89, W(  0) },   // 15:07 EXTRA flash#2 rel (89ms)
    {  0, 15,  7,  9,       0, W(172) },   // 15:07 EXTRA flash#3 att 172
    {  0, 15,  7, 10,      62, W(  0) },   // 15:07 EXTRA flash#3 rel (62ms)
    {  0, 15, 18,  2,       0, W(239) },   // 15:18 EXTRA flash#1 att 239
    {  0, 15, 18,  3,      85, W(  0) },   // 15:18 EXTRA flash#1 rel (85ms)
    {  0, 15, 18,  4,       0, W(195) },   // 15:18 EXTRA flash#2 att 195
    {  0, 15, 18,  5,      58, W(  0) },   // 15:18 EXTRA flash#2 rel (58ms)
    {  0, 15, 18,  7,       0, W(253) },   // 15:18 EXTRA flash#3 att 253
    {  0, 15, 18,  8,     103, W(  0) },   // 15:18 EXTRA flash#3 rel (103ms)
    {  0, 15, 27,  1,       0, W(154) },   // 15:27 EXTRA flash#1 att 154
    {  0, 15, 27,  2,      51, W(  0) },   // 15:27 EXTRA flash#1 rel (51ms)
    {  0, 15, 27,  3,       0, W(179) },   // 15:27 EXTRA flash#2 att 179
    {  0, 15, 27,  4,      68, W(  0) },   // 15:27 EXTRA flash#2 rel (68ms)
    {  0, 15, 42,  0,       0, W(150) },   // 15:42+0f STROBE att 150
    {  0, 15, 42,  1,      37, W(  0) },   // 15:42+1f STROBE rel (37ms)
    {  0, 15, 42,  7,       0, W(188) },   // 15:42+7f STROBE att 188
    {  0, 15, 42,  8,      37, W(  0) },   // 15:42+8f STROBE rel (37ms)
    {  0, 15, 56,  1,       0, W(143) },   // 15:56+1f STROBE att 143
    {  0, 15, 56,  2,      70, W(  0) },   // 15:56+2f STROBE rel (70ms)
    {  0, 16,  1,  4,       0, W(185) },   // 16:01+4f STROBE att 185
    {  0, 16,  1,  5,      56, W(  0) },   // 16:01+5f STROBE rel (56ms)
    {  0, 16, 13,  3,       0, W(156) },   // 16:13+3f STROBE att 156
    {  0, 16, 13,  4,      42, W(  0) },   // 16:13+4f STROBE rel (42ms)
    {  0, 16, 19,  4,       0, W(128) },   // 16:19+4f STROBE att 128
    {  0, 16, 19,  5,      60, W(  0) },   // 16:19+5f STROBE rel (60ms)
    {  0, 16, 21,  0,       0, W(169) },   // 16:21+0f STROBE att 169
    {  0, 16, 21,  1,      37, W(  0) },   // 16:21+1f STROBE rel (37ms)
    {  0, 16, 24,  0,       0, W(129) },   // 16:24+0f STROBE att 129
    {  0, 16, 24,  1,      36, W(  0) },   // 16:24+1f STROBE rel (36ms)
    {  0, 16, 29,  0,       0, W(230) },   // 16:29+0f STROBE att 230
    {  0, 16, 29,  1,      53, W(  0) },   // 16:29+1f STROBE rel (53ms)
    {  0, 16, 29,  9,       0, W(187) },   // 16:29+9f STROBE att 187
    {  0, 16, 29, 10,      37, W(  0) },   // 16:29+10f STROBE rel (37ms)
    {  0, 16, 35,  1,       0, W(161) },   // 16:35+1f STROBE att 161
    {  0, 16, 35,  2,      58, W(  0) },   // 16:35+2f STROBE rel (58ms)
    {  0, 16, 35,  9,       0, W(196) },   // 16:35+9f STROBE att 196
    {  0, 16, 35, 10,      53, W(  0) },   // 16:35+10f STROBE rel (53ms)
    {  0, 16, 53,  3,       0, W(144) },   // 16:53+3f STROBE att 144
    {  0, 16, 53,  4,      59, W(  0) },   // 16:53+4f STROBE rel (59ms)
    {  0, 17,  9,  0,       0, W(228) },   // 17:09 EXTRA flash#1 att 228
    {  0, 17,  9,  1,      65, W(  0) },   // 17:09 EXTRA flash#1 rel (65ms)
    {  0, 17,  9,  3,       0, W(193) },   // 17:09 EXTRA flash#2 att 193
    {  0, 17,  9,  4,      62, W(  0) },   // 17:09 EXTRA flash#2 rel (62ms)
    {  0, 17,  9,  7,       0, W(157) },   // 17:09 EXTRA flash#3 att 157
    {  0, 17,  9,  8,      42, W(  0) },   // 17:09 EXTRA flash#3 rel (42ms)
    {  0, 17,  9, 10,       0, W(225) },   // 17:09 EXTRA flash#4 att 225
    {  0, 17,  9, 11,      58, W(  0) },   // 17:09 EXTRA flash#4 rel (58ms)
    {  0, 17, 15,  0,       0, W(197) },   // 17:15 EXTRA flash#1 att 197
    {  0, 17, 15,  0,       0, W(230) },   // 17:15+0f STROBE att 230
    {  0, 17, 15,  1,      90, W(  0) },   // 17:15 EXTRA flash#1 rel (90ms)
    {  0, 17, 15,  1,      40, W(  0) },   // 17:15+1f STROBE rel (40ms)
    {  0, 17, 15,  2,       0, W(167) },   // 17:15 EXTRA flash#2 att 167
    {  0, 17, 15,  3,      60, W(  0) },   // 17:15 EXTRA flash#2 rel (60ms)
    {  0, 17, 15,  4,       0, W(196) },   // 17:15+4f STROBE att 196
    {  0, 17, 15,  5,      67, W(  0) },   // 17:15+5f STROBE rel (67ms)
    {  0, 17, 15,  7,       0, W(251) },   // 17:15 EXTRA flash#3 att 251
    {  0, 17, 15,  8,     102, W(  0) },   // 17:15 EXTRA flash#3 rel (102ms)
    {  0, 17, 18,  2,       0, W(221) },   // 17:18 EXTRA flash#1 att 221
    {  0, 17, 18,  3,      85, W(  0) },   // 17:18 EXTRA flash#1 rel (85ms)
    {  0, 17, 18,  5,       0, W(254) },   // 17:18 EXTRA flash#2 att 254
    {  0, 17, 18,  6,      62, W(  0) },   // 17:18 EXTRA flash#2 rel (62ms)
    {  0, 17, 19,  3,       0, W(201) },   // 17:19+3f STROBE att 201
    {  0, 17, 19,  4,      60, W(  0) },   // 17:19+4f STROBE rel (60ms)
    {  0, 17, 21,  0,       0, W(137) },   // 17:21+0f STROBE att 137
    {  0, 17, 21,  1,      50, W(  0) },   // 17:21+1f STROBE rel (50ms)
    {  0, 17, 33,  1,       0, W(176) },   // 17:33+1f STROBE att 176
    {  0, 17, 33,  2,      54, W(  0) },   // 17:33+2f STROBE rel (54ms)
    {  0, 18,  5,  5,       0, W( 72) },   // 18:05+5f FAST att 72
    {  0, 18,  5,  6,      48, W(  0) },   // 18:05+6f rel 72 0 (48ms,FAST)
    {  0, 18,  6,  2,       0, W(187) },   // 18:06 EXTRA flash#1 att 187
    {  0, 18,  6,  3,      56, W(  0) },   // 18:06 EXTRA flash#1 rel (56ms)
    {  0, 18,  6,  5,       0, W(243) },   // 18:06 EXTRA flash#2 att 243
    {  0, 18,  6,  6,      40, W(  0) },   // 18:06 EXTRA flash#2 rel (40ms)
    {  0, 18,  6, 10,       0, W(154) },   // 18:06 EXTRA flash#3 att 154
    {  0, 18,  6, 11,      63, W(  0) },   // 18:06 EXTRA flash#3 rel (63ms)
    {  0, 18, 10,  6,       0, W(  7) },   // 18:10+6f FAST att 7
    {  0, 18, 10,  7,      20, W(  0) },   // 18:10+7f rel 7 0 (20ms,FAST)
    {  0, 18, 11,  4,       0, W( 27) },   // 18:11+4f SHARP att 27
    {  0, 18, 11,  6,      20, W(  0) },   // 18:11+6f rel 27 0 (20ms,SHARP)
    {  0, 18, 12,  4,       0, W( 45) },   // 18:12+4f SHARP att 45
    {  0, 18, 12,  5,      20, W(  0) },   // 18:12+5f rel 45 0 (20ms,SHARP)
    {  0, 18, 13,  3,       0, W( 20) },   // 18:13+3f SHARP att 20
    {  0, 18, 13,  4,      20, W(  0) },   // 18:13+4f rel 20 0 (20ms,SHARP)
    {  0, 18, 14,  5,       0, W( 46) },   // 18:14+5f SHARP att 46
    {  0, 18, 14,  6,      20, W(  0) },   // 18:14+6f rel 46 0 (20ms,SHARP)
    {  0, 18, 17,  2,       0, W(222) },   // 18:17 EXTRA flash#1 att 222
    {  0, 18, 17,  3,      53, W(  0) },   // 18:17 EXTRA flash#1 rel (53ms)
    {  0, 18, 17,  7,       0, W(168) },   // 18:17 EXTRA flash#2 att 168
    {  0, 18, 17,  8,      82, W(  0) },   // 18:17 EXTRA flash#2 rel (82ms)
    {  0, 18, 18,  1,       0, W(231) },   // 18:18 EXTRA flash#1 att 231
    {  0, 18, 18,  2,      51, W(  0) },   // 18:18 EXTRA flash#1 rel (51ms)
    {  0, 18, 18,  5,       0, W(151) },   // 18:18 EXTRA flash#2 att 151
    {  0, 18, 18,  6,      73, W(  0) },   // 18:18 EXTRA flash#2 rel (73ms)
    {  0, 18, 20,  1,       0, W( 12) },   // 18:20+1f FAST att 12
    {  0, 18, 20,  2,      20, W(  0) },   // 18:20+2f rel 12 0 (20ms,FAST)
    {  0, 18, 21,  4,       0, W( 38) },   // 18:21+4f SOFT att 38
    {  0, 18, 21,  5,      42, W(  0) },   // 18:21+5f rel 38 0 (42ms,SOFT)
    {  0, 18, 22,  5,       0, W( 18) },   // 18:22+5f SOFT att 18
    {  0, 18, 22,  6,      20, W(  0) },   // 18:22+6f rel 18 0 (20ms,SOFT)
    {  0, 18, 39,  2,       0, W(  2) },   // 18:39+2f SHARP att 2
    {  0, 18, 39,  3,      20, W(  0) },   // 18:39+3f rel 2 0 (20ms,SHARP)
    {  0, 18, 40,  1,       0, W( 23) },   // 18:40+1f FAST att 23
    {  0, 18, 40,  2,      20, W(  0) },   // 18:40+2f rel 23 0 (20ms,FAST)
    {  0, 18, 41,  6,       0, W(  2) },   // 18:41+6f SOFT att 2
    {  0, 18, 41,  7,      20, W(  0) },   // 18:41+7f rel 2 0 (20ms,SOFT)
    {  0, 18, 42,  6,       0, W( 27) },   // 18:42+6f FAST att 27
    {  0, 18, 42,  7,      20, W(  0) },   // 18:42+7f rel 27 0 (20ms,FAST)
    {  0, 18, 47,  5,       0, W(142) },   // 18:47+5f SOFT att 142
    {  0, 18, 47,  6,     157, W(  0) },   // 18:47+6f rel 142 0 (157ms,SOFT)
    {  0, 18, 51,  4,       0, W( 51) },   // 18:51+4f FAST att 51
    {  0, 18, 51,  6,      34, W(  0) },   // 18:51+6f rel 51 0 (34ms,FAST)
    {  0, 18, 52,  1,       0, W( 86) },   // 18:52+1f SHARP att 86
    {  0, 18, 52,  2,      34, W(  0) },   // 18:52+2f rel 86 0 (34ms,SHARP)
    {  0, 18, 52, 13,       0, W(115) },   // 18:52+13f FAST att 115
    {  0, 18, 52, 14,      76, W(  0) },   // 18:52+14f rel 115 0 (76ms,FAST)
    {  0, 18, 53,  3,       0, W( 58) },   // 18:53+3f SHARP att 58
    {  0, 18, 53,  4,      23, W(  0) },   // 18:53+4f rel 58 0 (23ms,SHARP)
    {  0, 18, 54,  0,       0, W(101) },   // 18:54+0f SHARP att 101
    {  0, 18, 54,  1,      40, W(  0) },   // 18:54+1f rel 101 0 (40ms,SHARP)
    {  0, 18, 54, 13,       0, W( 58) },   // 18:54+13f SHARP att 58
    {  0, 18, 54, 15,      23, W(  0) },   // 18:54+15f rel 58 0 (23ms,SHARP)
    {  0, 18, 55,  0,       0, W(  7) },   // 18:55+0f FAST att 7
    {  0, 18, 55,  1,      20, W(  0) },   // 18:55+1f rel 7 0 (20ms,FAST)
    {  0, 19,  0,  1,       0, W( 29) },   // 19:00+1f FAST att 29
    {  0, 19,  0,  2,      20, W(  0) },   // 19:00+2f rel 29 0 (20ms,FAST)
    {  0, 19,  1,  1,       0, W(  9) },   // 19:01+1f SHARP att 9
    {  0, 19,  1,  2,      20, W(  0) },   // 19:01+2f rel 9 0 (20ms,SHARP)
    {  0, 19,  7,  0,       0, W( 37) },   // 19:07+0f SHARP att 37
    {  0, 19,  7,  1,      20, W(  0) },   // 19:07+1f rel 37 0 (20ms,SHARP)
    {  0, 19,  8,  6,       0, W( 61) },   // 19:08+6f FAST att 61
    {  0, 19,  8,  8,      40, W(  0) },   // 19:08+8f rel 61 0 (40ms,FAST)
    {  0, 19,  9,  2,       0, W(194) },   // 19:09 EXTRA flash#1 att 194
    {  0, 19,  9,  3,      72, W(  0) },   // 19:09 EXTRA flash#1 rel (72ms)
    {  0, 19,  9,  4,       0, W( 11) },   // 19:09+4f SOFT att 11
    {  0, 19,  9,  4,       0, W(242) },   // 19:09 EXTRA flash#2 att 242
    {  0, 19,  9,  5,      20, W(  0) },   // 19:09+5f rel 11 0 (20ms,SOFT)
    {  0, 19,  9,  5,      40, W(  0) },   // 19:09 EXTRA flash#2 rel (40ms)
    {  0, 19, 11,  0,       0, W( 40) },   // 19:11+0f SOFT att 40
    {  0, 19, 11,  2,      44, W(  0) },   // 19:11+2f rel 40 0 (44ms,SOFT)
    {  0, 19, 19,  3,       0, W( 21) },   // 19:19+3f FAST att 21
    {  0, 19, 19,  5,      20, W(  0) },   // 19:19+5f rel 21 0 (20ms,FAST)
    {  0, 19, 25,  3,       0, W(  2) },   // 19:25+3f FAST att 2
    {  0, 19, 25,  4,      20, W(  0) },   // 19:25+4f rel 2 0 (20ms,FAST)
    {  0, 19, 28,  5,       0, W( 80) },   // 19:28+5f SOFT att 80
    {  0, 19, 28,  6,      88, W(  0) },   // 19:28+6f rel 80 0 (88ms,SOFT)
    {  0, 19, 29,  1,       0, W( 31) },   // 19:29+1f FAST att 31
    {  0, 19, 29,  2,      20, W(  0) },   // 19:29+2f rel 31 0 (20ms,FAST)
    {  0, 19, 31,  4,       0, W( 12) },   // 19:31+4f SOFT att 12
    {  0, 19, 31,  6,      20, W(  0) },   // 19:31+6f rel 12 0 (20ms,SOFT)
    {  0, 19, 32,  0,       0, W( 30) },   // 19:32+0f SHARP att 30
    {  0, 19, 32,  1,      20, W(  0) },   // 19:32+1f rel 30 0 (20ms,SHARP)
    {  0, 19, 33,  5,       0, W(109) },   // 19:33+5f FAST att 109
    {  0, 19, 33,  7,      72, W(  0) },   // 19:33+7f rel 109 0 (72ms,FAST)
    {  0, 19, 33, 19,       0, W(144) },   // 19:33+19f SHARP att 144
    {  0, 19, 33, 21,      57, W(  0) },   // 19:33+21f rel 144 0 (57ms,SHARP)
    {  0, 19, 34,  5,       0, W( 19) },   // 19:34+5f SHARP att 19
    {  0, 19, 34,  6,      20, W(  0) },   // 19:34+6f rel 19 0 (20ms,SHARP)
    {  0, 19, 39,  5,       0, W( 38) },   // 19:39+5f FAST att 38
    {  0, 19, 39,  7,      25, W(  0) },   // 19:39+7f rel 38 0 (25ms,FAST)
    {  0, 19, 40,  6,       0, W( 16) },   // 19:40+6f FAST att 16
    {  0, 19, 40,  8,      20, W(  0) },   // 19:40+8f rel 16 0 (20ms,FAST)
    {  0, 19, 41,  5,       0, W(  2) },   // 19:41+5f SHARP att 2
    {  0, 19, 41,  6,      20, W(  0) },   // 19:41+6f rel 2 0 (20ms,SHARP)
    {  0, 19, 42,  4,       0, W( 25) },   // 19:42+4f FAST att 25
    {  0, 19, 42,  5,      20, W(  0) },   // 19:42+5f rel 25 0 (20ms,FAST)
    {  0, 19, 43,  5,       0, W(  2) },   // 19:43+5f SOFT att 2
    {  0, 19, 43,  6,      20, W(  0) },   // 19:43+6f rel 2 0 (20ms,SOFT)
    {  0, 19, 44,  2,       0, W( 29) },   // 19:44+2f SHARP att 29
    {  0, 19, 44,  3,      20, W(  0) },   // 19:44+3f rel 29 0 (20ms,SHARP)
    {  0, 19, 45,  1,       0, W(  4) },   // 19:45+1f SOFT att 4
    {  0, 19, 45,  2,      20, W(  0) },   // 19:45+2f rel 4 0 (20ms,SOFT)
    {  0, 19, 46,  3,       0, W( 47) },   // 19:46+3f FAST att 47
    {  0, 19, 46,  4,      31, W(  0) },   // 19:46+4f rel 47 0 (31ms,FAST)
    {  0, 19, 47,  5,       0, W( 70) },   // 19:47+5f FAST att 70
    {  0, 19, 47,  6,      46, W(  0) },   // 19:47+6f rel 70 0 (46ms,FAST)
    {  0, 19, 50,  2,       0, W( 10) },   // 19:50+2f FAST att 10
    {  0, 19, 50,  3,      20, W(  0) },   // 19:50+3f rel 10 0 (20ms,FAST)
    {  0, 19, 51,  1,       0, W( 50) },   // 19:51+1f FAST att 50
    {  0, 19, 51,  3,      33, W(  0) },   // 19:51+3f rel 50 0 (33ms,FAST)
    {  0, 19, 52,  5,       0, W(131) },   // 19:52+5f SOFT att 131
    {  0, 19, 52,  6,     145, W(  0) },   // 19:52+6f rel 131 0 (145ms,SOFT)
    {  0, 19, 52, 14,       0, W(149) },   // 19:52+14f SHARP att 149
    {  0, 19, 52, 15,      59, W(  0) },   // 19:52+15f rel 149 0 (59ms,SHARP)
    {  0, 19, 52, 26,       0, W( 99) },   // 19:52+26f SOFT att 99
    {  0, 19, 52, 27,     110, W(  0) },   // 19:52+27f rel 99 0 (110ms,SOFT)
    {  0, 19, 53,  1,       0, W(160) },   // 19:53+1f SHARP att 160
    {  0, 19, 53,  2,      64, W(  0) },   // 19:53+2f rel 160 0 (64ms,SHARP)
    {  0, 19, 53, 10,       0, W(142) },   // 19:53+10f FAST att 142
    {  0, 19, 53, 12,      94, W(  0) },   // 19:53+12f rel 142 0 (94ms,FAST)
    {  0, 19, 53, 24,       0, W(111) },   // 19:53+24f FAST att 111
    {  0, 19, 53, 26,      74, W(  0) },   // 19:53+26f rel 111 0 (74ms,FAST)
    {  0, 19, 54,  3,       0, W( 90) },   // 19:54+3f SOFT att 90
    {  0, 19, 54,  5,     100, W(  0) },   // 19:54+5f rel 90 0 (100ms,SOFT)
    {  0, 19, 54, 13,       0, W(124) },   // 19:54+13f SOFT att 124
    {  0, 19, 54, 14,     137, W(  0) },   // 19:54+14f rel 124 0 (137ms,SOFT)
    {  0, 19, 55,  2,       0, W( 63) },   // 19:55+2f FAST att 63
    {  0, 19, 55,  3,      42, W(  0) },   // 19:55+3f rel 63 0 (42ms,FAST)
    {  0, 19, 56,  5,       0, W( 14) },   // 19:56+5f SOFT att 14
    {  0, 19, 56,  6,      20, W(  0) },   // 19:56+6f rel 14 0 (20ms,SOFT)
    {  0, 19, 57,  6,       0, W( 64) },   // 19:57+6f SHARP att 64
    {  0, 19, 57,  7,      25, W(  0) },   // 19:57+7f rel 64 0 (25ms,SHARP)
    {  0, 19, 57, 18,       0, W( 82) },   // 19:57+18f SHARP att 82
    {  0, 19, 57, 20,      32, W(  0) },   // 19:57+20f rel 82 0 (32ms,SHARP)
    {  0, 20,  0,  0,       0, W(  9) },   // 20:00+0f SOFT att 9
    {  0, 20,  0,  1,      20, W(  0) },   // 20:00+1f rel 9 0 (20ms,SOFT)
    {  0, 20,  1,  3,       0, W( 36) },   // 20:01+3f FAST att 36
    {  0, 20,  1,  4,      24, W(  0) },   // 20:01+4f rel 36 0 (24ms,FAST)
    {  0, 20,  2,  6,       0, W( 16) },   // 20:02+6f SHARP att 16
    {  0, 20,  2,  8,      20, W(  0) },   // 20:02+8f rel 16 0 (20ms,SHARP)
    {  0, 20,  4,  2,       0, W(  2) },   // 20:04+2f SHARP att 2
    {  0, 20,  4,  4,      20, W(  0) },   // 20:04+4f rel 2 0 (20ms,SHARP)
    {  0, 20,  5,  3,       0, W( 24) },   // 20:05+3f FAST att 24
    {  0, 20,  5,  4,      20, W(  0) },   // 20:05+4f rel 24 0 (20ms,FAST)
    {  0, 20,  6,  0,       0, W(  3) },   // 20:06+0f FAST att 3
    {  0, 20,  6,  1,      20, W(  0) },   // 20:06+1f rel 3 0 (20ms,FAST)
    {  0, 20,  7,  3,       0, W( 29) },   // 20:07+3f FAST att 29
    {  0, 20,  7,  5,      20, W(  0) },   // 20:07+5f rel 29 0 (20ms,FAST)
    {  0, 20,  9,  1,       0, W(  7) },   // 20:09+1f SHARP att 7
    {  0, 20,  9,  3,      20, W(  0) },   // 20:09+3f rel 7 0 (20ms,SHARP)
    {  0, 20, 12,  4,       0, W( 33) },   // 20:12+4f FAST att 33
    {  0, 20, 12,  5,      22, W(  0) },   // 20:12+5f rel 33 0 (22ms,FAST)
    {  0, 20, 13,  1,       0, W( 83) },   // 20:13+1f SHARP att 83
    {  0, 20, 13,  2,      33, W(  0) },   // 20:13+2f rel 83 0 (33ms,SHARP)
    {  0, 20, 13,  9,       0, W(102) },   // 20:13+9f SOFT att 102
    {  0, 20, 13, 11,     113, W(  0) },   // 20:13+11f rel 102 0 (113ms,SOFT)
    {  0, 20, 20,  3,       0, W( 17) },   // 20:20+3f SOFT att 17
    {  0, 20, 20,  5,      20, W(  0) },   // 20:20+5f rel 17 0 (20ms,SOFT)
    {  0, 20, 21,  3,       0, W( 43) },   // 20:21+3f SHARP att 43
    {  0, 20, 21,  5,      20, W(  0) },   // 20:21+5f rel 43 0 (20ms,SHARP)
    {  0, 20, 22,  6,       0, W( 71) },   // 20:22+6f SHARP att 71
    {  0, 20, 22,  7,      28, W(  0) },   // 20:22+7f rel 71 0 (28ms,SHARP)
    {  0, 20, 22, 18,       0, W( 90) },   // 20:22+18f FAST att 90
    {  0, 20, 22, 19,      60, W(  0) },   // 20:22+19f rel 90 0 (60ms,FAST)
    {  0, 20, 23,  2,       0, W( 71) },   // 20:23+2f SHARP att 71
    {  0, 20, 23,  4,      28, W(  0) },   // 20:23+4f rel 71 0 (28ms,SHARP)
    {  0, 20, 24,  5,       0, W( 92) },   // 20:24+5f FAST att 92
    {  0, 20, 24,  7,      61, W(  0) },   // 20:24+7f rel 92 0 (61ms,FAST)
    {  0, 20, 24, 18,       0, W( 52) },   // 20:24+18f SHARP att 52
    {  0, 20, 24, 19,      20, W(  0) },   // 20:24+19f rel 52 0 (20ms,SHARP)
    {  0, 20, 25,  2,       0, W( 75) },   // 20:25+2f SOFT att 75
    {  0, 20, 25,  3,      83, W(  0) },   // 20:25+3f rel 75 0 (83ms,SOFT)
    {  0, 20, 26,  6,       0, W( 43) },   // 20:26+6f SHARP att 43
    {  0, 20, 26,  8,      20, W(  0) },   // 20:26+8f rel 43 0 (20ms,SHARP)
    {  0, 20, 27,  0,       0, W( 71) },   // 20:27+0f SHARP att 71
    {  0, 20, 27,  1,      28, W(  0) },   // 20:27+1f rel 71 0 (28ms,SHARP)
    {  0, 20, 28,  0,       0, W(  5) },   // 20:28+0f FAST att 5
    {  0, 20, 28,  2,      20, W(  0) },   // 20:28+2f rel 5 0 (20ms,FAST)
    {  0, 20, 30,  3,       0, W( 27) },   // 20:30+3f FAST att 27
    {  0, 20, 30,  5,      20, W(  0) },   // 20:30+5f rel 27 0 (20ms,FAST)
    {  0, 20, 31,  0,       0, W( 73) },   // 20:31+0f SHARP att 73
    {  0, 20, 31,  2,      29, W(  0) },   // 20:31+2f rel 73 0 (29ms,SHARP)
    {  0, 20, 32,  5,       0, W( 52) },   // 20:32+5f FAST att 52
    {  0, 20, 32,  6,      34, W(  0) },   // 20:32+6f rel 52 0 (34ms,FAST)
    {  0, 20, 33,  5,       0, W( 73) },   // 20:33+5f FAST att 73
    {  0, 20, 33,  7,      48, W(  0) },   // 20:33+7f rel 73 0 (48ms,FAST)
    {  0, 20, 34,  6,       0, W(131) },   // 20:34+6f FAST att 131
    {  0, 20, 34,  7,      87, W(  0) },   // 20:34+7f rel 131 0 (87ms,FAST)
    {  0, 20, 34, 11,       0, W( 96) },   // 20:34+11f SHARP att 96
    {  0, 20, 34, 12,      38, W(  0) },   // 20:34+12f rel 96 0 (38ms,SHARP)
    {  0, 20, 35,  2,       0, W(116) },   // 20:35+2f SHARP att 116
    {  0, 20, 35,  4,      46, W(  0) },   // 20:35+4f rel 116 0 (46ms,SHARP)
    {  0, 20, 35,  9,       0, W( 67) },   // 20:35+9f FAST att 67
    {  0, 20, 35, 10,      44, W(  0) },   // 20:35+10f rel 67 0 (44ms,FAST)
    {  0, 20, 36,  0,       0, W( 21) },   // 20:36+0f SHARP att 21
    {  0, 20, 36,  1,      20, W(  0) },   // 20:36+1f rel 21 0 (20ms,SHARP)
    {  0, 20, 37,  1,       0, W(  2) },   // 20:37+1f FAST att 2
    {  0, 20, 37,  2,      20, W(  0) },   // 20:37+2f rel 2 0 (20ms,FAST)
    {  0, 20, 38,  2,       0, W( 36) },   // 20:38+2f SHARP att 36
    {  0, 20, 38,  3,      20, W(  0) },   // 20:38+3f rel 36 0 (20ms,SHARP)
    {  0, 20, 39,  6,       0, W(100) },   // 20:39+6f FAST att 100
    {  0, 20, 39,  8,      66, W(  0) },   // 20:39+8f rel 100 0 (66ms,FAST)
    {  0, 20, 40,  6,       0, W( 35) },   // 20:40+6f SHARP att 35
    {  0, 20, 40,  8,      20, W(  0) },   // 20:40+8f rel 35 0 (20ms,SHARP)
    {  0, 20, 41,  5,       0, W( 60) },   // 20:41+5f SOFT att 60
    {  0, 20, 41,  7,      66, W(  0) },   // 20:41+7f rel 60 0 (66ms,SOFT)
    {  0, 20, 41, 14,       0, W( 80) },   // 20:41+14f SHARP att 80
    {  0, 20, 41, 16,      32, W(  0) },   // 20:41+16f rel 80 0 (32ms,SHARP)
    {  0, 20, 42,  4,       0, W( 35) },   // 20:42+4f SOFT att 35
    {  0, 20, 42,  6,      38, W(  0) },   // 20:42+6f rel 35 0 (38ms,SOFT)
    {  0, 20, 43,  5,       0, W( 53) },   // 20:43+5f SHARP att 53
    {  0, 20, 43,  6,      21, W(  0) },   // 20:43+6f rel 53 0 (21ms,SHARP)
    {  0, 20, 44,  1,       0, W( 79) },   // 20:44+1f FAST att 79
    {  0, 20, 44,  3,      52, W(  0) },   // 20:44+3f rel 79 0 (52ms,FAST)
    {  0, 20, 45,  4,       0, W(102) },   // 20:45+4f FAST att 102
    {  0, 20, 45,  6,      68, W(  0) },   // 20:45+6f rel 102 0 (68ms,FAST)
    {  0, 20, 45, 11,       0, W( 61) },   // 20:45+11f SHARP att 61
    {  0, 20, 45, 12,      24, W(  0) },   // 20:45+12f rel 61 0 (24ms,SHARP)
    {  0, 20, 46,  3,       0, W( 38) },   // 20:46+3f FAST att 38
    {  0, 20, 46,  5,      25, W(  0) },   // 20:46+5f rel 38 0 (25ms,FAST)
    {  0, 20, 47,  5,       0, W( 57) },   // 20:47+5f SHARP att 57
    {  0, 20, 47,  6,      22, W(  0) },   // 20:47+6f rel 57 0 (22ms,SHARP)
    {  0, 20, 48,  6,       0, W( 80) },   // 20:48+6f FAST att 80
    {  0, 20, 48,  8,      53, W(  0) },   // 20:48+8f rel 80 0 (53ms,FAST)
    {  0, 20, 48, 18,       0, W( 53) },   // 20:48+18f SHARP att 53
    {  0, 20, 48, 19,      21, W(  0) },   // 20:48+19f rel 53 0 (21ms,SHARP)
    {  0, 20, 49,  4,       0, W(101) },   // 20:49+4f SHARP att 101
    {  0, 20, 49,  6,      40, W(  0) },   // 20:49+6f rel 101 0 (40ms,SHARP)
    {  0, 20, 49, 12,       0, W( 70) },   // 20:49+12f SHARP att 70
    {  0, 20, 49, 13,      28, W(  0) },   // 20:49+13f rel 70 0 (28ms,SHARP)
    {  0, 20, 50,  1,       0, W( 33) },   // 20:50+1f SOFT att 33
    {  0, 20, 50,  2,      36, W(  0) },   // 20:50+2f rel 33 0 (36ms,SOFT)
    {  0, 20, 51,  4,       0, W( 51) },   // 20:51+4f SHARP att 51
    {  0, 20, 51,  5,      20, W(  0) },   // 20:51+5f rel 51 0 (20ms,SHARP)
    {  0, 20, 52,  1,       0, W( 79) },   // 20:52+1f SHARP att 79
    {  0, 20, 52,  2,      31, W(  0) },   // 20:52+2f rel 79 0 (31ms,SHARP)
    {  0, 20, 53,  3,       0, W( 54) },   // 20:53+3f SHARP att 54
    {  0, 20, 53,  5,      21, W(  0) },   // 20:53+5f rel 54 0 (21ms,SHARP)
    {  0, 20, 53, 16,       0, W( 75) },   // 20:53+16f SOFT att 75
    {  0, 20, 53, 18,      83, W(  0) },   // 20:53+18f rel 75 0 (83ms,SOFT)
    {  0, 20, 54,  4,       0, W( 93) },   // 20:54+4f SOFT att 93
    {  0, 20, 54,  5,     103, W(  0) },   // 20:54+5f rel 93 0 (103ms,SOFT)
    {  0, 20, 55,  6,       0, W( 47) },   // 20:55+6f FAST att 47
    {  0, 20, 55,  7,      31, W(  0) },   // 20:55+7f rel 47 0 (31ms,FAST)
    {  0, 20, 56,  3,       0, W( 67) },   // 20:56+3f FAST att 67
    {  0, 20, 56,  4,      44, W(  0) },   // 20:56+4f rel 67 0 (44ms,FAST)
    {  0, 20, 57,  2,       0, W( 97) },   // 20:57+2f FAST att 97
    {  0, 20, 57,  3,      64, W(  0) },   // 20:57+3f rel 97 0 (64ms,FAST)
    {  0, 20, 57, 16,       0, W( 72) },   // 20:57+16f FAST att 72
    {  0, 20, 57, 18,      48, W(  0) },   // 20:57+18f rel 72 0 (48ms,FAST)
    {  0, 20, 58,  6,       0, W( 41) },   // 20:58+6f SOFT att 41
    {  0, 20, 58,  7,      45, W(  0) },   // 20:58+7f rel 41 0 (45ms,SOFT)
    {  0, 20, 59,  1,       0, W(203) },   // 20:59 EXTRA flash#1 att 203
    {  0, 20, 59,  2,      61, W(  0) },   // 20:59 EXTRA flash#1 rel (61ms)
    {  0, 20, 59,  3,       0, W( 66) },   // 20:59+3f FAST att 66
    {  0, 20, 59,  4,      44, W(  0) },   // 20:59+4f rel 66 0 (44ms,FAST)
    {  0, 20, 59,  6,       0, W(162) },   // 20:59 EXTRA flash#2 att 162
    {  0, 20, 59,  7,     101, W(  0) },   // 20:59 EXTRA flash#2 rel (101ms)
    {  0, 20, 59,  9,       0, W(207) },   // 20:59 EXTRA flash#3 att 207
    {  0, 20, 59, 10,      49, W(  0) },   // 20:59 EXTRA flash#3 rel (49ms)
    {  0, 20, 59, 11,       0, W(152) },   // 20:59 EXTRA flash#4 att 152
    {  0, 20, 59, 12,      80, W(  0) },   // 20:59 EXTRA flash#4 rel (80ms)
    {  0, 21,  0,  0,       0, W(  8) },   // 21:00+0f SHARP att 8
    {  0, 21,  0,  2,      20, W(  0) },   // 21:00+2f rel 8 0 (20ms,SHARP)
    {  0, 21,  6,  1,       0, W( 29) },   // 21:06+1f SHARP att 29
    {  0, 21,  6,  2,      20, W(  0) },   // 21:06+2f rel 29 0 (20ms,SHARP)
    {  0, 21,  8,  0,       0, W(  7) },   // 21:08+0f SHARP att 7
    {  0, 21,  8,  2,      20, W(  0) },   // 21:08+2f rel 7 0 (20ms,SHARP)
    {  0, 21,  9,  4,       0, W( 26) },   // 21:09+4f SHARP att 26
    {  0, 21,  9,  6,      20, W(  0) },   // 21:09+6f rel 26 0 (20ms,SHARP)
    {  0, 21, 19,  2,       0, W(  5) },   // 21:19+2f FAST att 5
    {  0, 21, 19,  3,      20, W(  0) },   // 21:19+3f rel 5 0 (20ms,FAST)
    {  0, 21, 20,  1,       0, W( 32) },   // 21:20+1f SOFT att 32
    {  0, 21, 20,  2,      35, W(  0) },   // 21:20+2f rel 32 0 (35ms,SOFT)
    {  0, 21, 21,  6,       0, W( 10) },   // 21:21+6f SHARP att 10
    {  0, 21, 21,  7,      20, W(  0) },   // 21:21+7f rel 10 0 (20ms,SHARP)
    {  0, 21, 22,  5,       0, W( 60) },   // 21:22+5f SHARP att 60
    {  0, 21, 22,  6,      24, W(  0) },   // 21:22+6f rel 60 0 (24ms,SHARP)
    {  0, 21, 23,  6,       0, W( 11) },   // 21:23+6f SHARP att 11
    {  0, 21, 23,  7,      20, W(  0) },   // 21:23+7f rel 11 0 (20ms,SHARP)
    {  0, 21, 24,  0,       0, W( 91) },   // 21:24+0f FAST att 91
    {  0, 21, 24,  1,      60, W(  0) },   // 21:24+1f rel 91 0 (60ms,FAST)
    {  0, 21, 25,  1,       0, W( 19) },   // 21:25+1f FAST att 19
    {  0, 21, 25,  2,      20, W(  0) },   // 21:25+2f rel 19 0 (20ms,FAST)
    {  0, 21, 29,  4,       0, W( 38) },   // 21:29+4f SHARP att 38
    {  0, 21, 29,  5,      20, W(  0) },   // 21:29+5f rel 38 0 (20ms,SHARP)
    {  0, 21, 30,  0,       0, W(114) },   // 21:30+0f FAST att 114
    {  0, 21, 30,  1,      76, W(  0) },   // 21:30+1f rel 114 0 (76ms,FAST)
    {  0, 21, 30,  9,       0, W( 95) },   // 21:30+9f SHARP att 95
    {  0, 21, 30, 11,      38, W(  0) },   // 21:30+11f rel 95 0 (38ms,SHARP)
    {  0, 21, 30, 21,       0, W(127) },   // 21:30+21f FAST att 127
    {  0, 21, 30, 22,      84, W(  0) },   // 21:30+22f rel 127 0 (84ms,FAST)
    {  0, 21, 31,  4,       0, W(107) },   // 21:31+4f FAST att 107
    {  0, 21, 31,  5,      71, W(  0) },   // 21:31+5f rel 107 0 (71ms,FAST)
    {  0, 21, 31,  9,       0, W(140) },   // 21:31+9f SHARP att 140
    {  0, 21, 31, 10,      56, W(  0) },   // 21:31+10f rel 140 0 (56ms,SHARP)
    {  0, 21, 31, 14,       0, W( 85) },   // 21:31+14f FAST att 85
    {  0, 21, 31, 16,      56, W(  0) },   // 21:31+16f rel 85 0 (56ms,FAST)
    {  0, 21, 32,  4,       0, W(111) },   // 21:32+4f SOFT att 111
    {  0, 21, 32,  6,     123, W(  0) },   // 21:32+6f rel 111 0 (123ms,SOFT)
    {  0, 21, 32, 12,       0, W( 80) },   // 21:32+12f SHARP att 80
    {  0, 21, 32, 14,      32, W(  0) },   // 21:32+14f rel 80 0 (32ms,SHARP)
    {  0, 21, 33,  0,       0, W(109) },   // 21:33+0f SHARP att 109
    {  0, 21, 33,  1,      43, W(  0) },   // 21:33+1f rel 109 0 (43ms,SHARP)
    {  0, 21, 33, 10,       0, W( 72) },   // 21:33+10f SHARP att 72
    {  0, 21, 33, 11,      28, W(  0) },   // 21:33+11f rel 72 0 (28ms,SHARP)
    {  0, 21, 34,  5,       0, W( 96) },   // 21:34+5f SHARP att 96
    {  0, 21, 34,  6,      38, W(  0) },   // 21:34+6f rel 96 0 (38ms,SHARP)
    {  0, 21, 35,  3,       0, W(131) },   // 21:35+3f SHARP att 131
    {  0, 21, 35,  4,      52, W(  0) },   // 21:35+4f rel 131 0 (52ms,SHARP)
    {  0, 21, 36,  1,       0, W( 32) },   // 21:36+1f FAST att 32
    {  0, 21, 36,  2,      21, W(  0) },   // 21:36+2f rel 32 0 (21ms,FAST)
    {  0, 21, 37,  6,       0, W( 73) },   // 21:37+6f SOFT att 73
    {  0, 21, 37,  8,      81, W(  0) },   // 21:37+8f rel 73 0 (81ms,SOFT)
    {  0, 21, 37, 16,       0, W( 54) },   // 21:37+16f SHARP att 54
    {  0, 21, 37, 17,      21, W(  0) },   // 21:37+17f rel 54 0 (21ms,SHARP)
    {  0, 21, 38,  1,       0, W( 74) },   // 21:38+1f FAST att 74
    {  0, 21, 38,  3,      49, W(  0) },   // 21:38+3f rel 74 0 (49ms,FAST)
    {  0, 21, 39,  3,       0, W(119) },   // 21:39+3f SHARP att 119
    {  0, 21, 39,  5,      47, W(  0) },   // 21:39+5f rel 119 0 (47ms,SHARP)
    {  0, 21, 40,  3,       0, W(141) },   // 21:40+3f SHARP att 141
    {  0, 21, 40,  5,      56, W(  0) },   // 21:40+5f rel 141 0 (56ms,SHARP)
    {  0, 21, 40,  9,       0, W(121) },   // 21:40+9f FAST att 121
    {  0, 21, 40, 10,      80, W(  0) },   // 21:40+10f rel 121 0 (80ms,FAST)
    {  0, 21, 40, 21,       0, W( 87) },   // 21:40+21f SOFT att 87
    {  0, 21, 40, 22,      96, W(  0) },   // 21:40+22f rel 87 0 (96ms,SOFT)
    {  0, 21, 41,  0,       0, W( 12) },   // 21:41+0f FAST att 12
    {  0, 21, 41,  2,      20, W(  0) },   // 21:41+2f rel 12 0 (20ms,FAST)
    {  0, 21, 42,  4,       0, W( 70) },   // 21:42+4f SOFT att 70
    {  0, 21, 42,  6,      77, W(  0) },   // 21:42+6f rel 70 0 (77ms,SOFT)
    {  0, 21, 43,  6,       0, W( 90) },   // 21:43+6f FAST att 90
    {  0, 21, 43,  7,      60, W(  0) },   // 21:43+7f rel 90 0 (60ms,FAST)
    {  0, 21, 43, 20,       0, W(129) },   // 21:43+20f SHARP att 129
    {  0, 21, 43, 21,      51, W(  0) },   // 21:43+21f rel 129 0 (51ms,SHARP)
    {  0, 21, 44,  2,       0, W(102) },   // 21:44+2f SHARP att 102
    {  0, 21, 44,  3,      40, W(  0) },   // 21:44+3f rel 102 0 (40ms,SHARP)
    {  0, 21, 45,  5,       0, W( 56) },   // 21:45+5f FAST att 56
    {  0, 21, 45,  7,      37, W(  0) },   // 21:45+7f rel 56 0 (37ms,FAST)
    {  0, 21, 46,  1,       0, W( 17) },   // 21:46+1f FAST att 17
    {  0, 21, 46,  2,      20, W(  0) },   // 21:46+2f rel 17 0 (20ms,FAST)
    {  0, 21, 47,  6,       0, W( 38) },   // 21:47+6f SOFT att 38
    {  0, 21, 47,  7,      42, W(  0) },   // 21:47+7f rel 38 0 (42ms,SOFT)
    {  0, 21, 48,  0,       0, W( 98) },   // 21:48+0f SOFT att 98
    {  0, 21, 48,  2,     108, W(  0) },   // 21:48+2f rel 98 0 (108ms,SOFT)
    {  0, 21, 48, 13,       0, W(122) },   // 21:48+13f FAST att 122
    {  0, 21, 48, 14,      81, W(  0) },   // 21:48+14f rel 122 0 (81ms,FAST)
    {  0, 21, 48, 24,       0, W( 90) },   // 21:48+24f SHARP att 90
    {  0, 21, 48, 25,      36, W(  0) },   // 21:48+25f rel 90 0 (36ms,SHARP)
    {  0, 21, 49,  1,       0, W(150) },   // 21:49+1f SHARP att 150
    {  0, 21, 49,  2,      60, W(  0) },   // 21:49+2f rel 150 0 (60ms,SHARP)
    {  0, 21, 50,  3,       0, W( 69) },   // 21:50+3f FAST att 69
    {  0, 21, 50,  5,      46, W(  0) },   // 21:50+5f rel 69 0 (46ms,FAST)
    {  0, 21, 51,  5,       0, W(  5) },   // 21:51+5f FAST att 5
    {  0, 21, 51,  7,      20, W(  0) },   // 21:51+7f rel 5 0 (20ms,FAST)
    {  0, 21, 54,  2,       0, W( 88) },   // 21:54+2f SOFT att 88
    {  0, 21, 54,  3,      97, W(  0) },   // 21:54+3f rel 88 0 (97ms,SOFT)
    {  0, 21, 54, 11,       0, W( 68) },   // 21:54+11f SOFT att 68
    {  0, 21, 54, 12,      75, W(  0) },   // 21:54+12f rel 68 0 (75ms,SOFT)
    {  0, 21, 55,  1,       0, W(  4) },   // 21:55+1f FAST att 4
    {  0, 21, 55,  2,      20, W(  0) },   // 21:55+2f rel 4 0 (20ms,FAST)
    // ====================================================================
    // FADE OUT
    // ====================================================================

    {  0, 23,  8,  0,       0, W(193) },   // 23:08 EXTRA flash#1 att 193
    {  0, 23,  8,  1,      61, W(  0) },   // 23:08 EXTRA flash#1 rel (61ms)
    {  0, 23, 22,  2,       0, W(159) },   // 23:22 EXTRA flash#1 att 159
    {  0, 23, 22,  3,     110, W(  0) },   // 23:22 EXTRA flash#1 rel (110ms)
    {  0, 23, 22,  4,       0, W(193) },   // 23:22 EXTRA flash#2 att 193
    {  0, 23, 22,  5,     101, W(  0) },   // 23:22 EXTRA flash#2 rel (101ms)
    {  0, 23, 52,  2,       0, W(227) },   // 23:52 EXTRA flash#1 att 227
    {  0, 23, 52,  3,      95, W(  0) },   // 23:52 EXTRA flash#1 rel (95ms)
    {  0, 23, 52,  5,       0, W(156) },   // 23:52 EXTRA flash#2 att 156
    {  0, 23, 52,  6,      52, W(  0) },   // 23:52 EXTRA flash#2 rel (52ms)
    {  0, 23, 52,  9,       0, W(202) },   // 23:52 EXTRA flash#3 att 202
    {  0, 23, 52, 10,     110, W(  0) },   // 23:52 EXTRA flash#3 rel (110ms)
    {  0, 23, 52, 13,       0, W(234) },   // 23:52 EXTRA flash#4 att 234
    {  0, 23, 52, 14,      76, W(  0) },   // 23:52 EXTRA flash#4 rel (76ms)
    // ====================================================================
    // FIRE ZONE B — 24:34–30:28
    // ====================================================================

    {  0, 24, 34,  4,       0, W(135) },   // 24:34+4f SHARP att 135
    {  0, 24, 34,  6,      54, W(  0) },   // 24:34+6f rel 135 0 (54ms,SHARP)
    {  0, 24, 34,  9,       0, W(102) },   // 24:34+9f SHARP att 102
    {  0, 24, 34, 10,      40, W(  0) },   // 24:34+10f rel 102 0 (40ms,SHARP)
    {  0, 24, 35,  1,       0, W(129) },   // 24:35+1f SHARP att 129
    {  0, 24, 35,  3,      51, W(  0) },   // 24:35+3f rel 129 0 (51ms,SHARP)
    {  0, 24, 36,  0,       0, W(156) },   // 24:36 EXTRA flash#1 att 156
    {  0, 24, 36,  1,      90, W(  0) },   // 24:36 EXTRA flash#1 rel (90ms)
    {  0, 24, 36,  5,       0, W(196) },   // 24:36 EXTRA flash#2 att 196
    {  0, 24, 36,  6,      40, W(  0) },   // 24:36 EXTRA flash#2 rel (40ms)
    {  0, 25,  2,  3,       0, W(  6) },   // 25:02+3f SOFT att 6
    {  0, 25,  2,  5,      20, W(  0) },   // 25:02+5f rel 6 0 (20ms,SOFT)
    {  0, 25, 18,  1,       0, W( 33) },   // 25:18+1f FAST att 33
    {  0, 25, 18,  2,      22, W(  0) },   // 25:18+2f rel 33 0 (22ms,FAST)
    {  0, 25, 52,  2,       0, W( 15) },   // 25:52+2f SOFT att 15
    {  0, 25, 52,  3,      20, W(  0) },   // 25:52+3f rel 15 0 (20ms,SOFT)
    {  0, 26,  3,  6,       0, W(  2) },   // 26:03+6f SHARP att 2
    {  0, 26,  3,  7,      20, W(  0) },   // 26:03+7f rel 2 0 (20ms,SHARP)
    {  0, 26,  4,  6,       0, W( 23) },   // 26:04+6f SHARP att 23
    {  0, 26,  4,  8,      20, W(  0) },   // 26:04+8f rel 23 0 (20ms,SHARP)
    {  0, 26,  5,  3,       0, W( 49) },   // 26:05+3f SHARP att 49
    {  0, 26,  5,  4,      20, W(  0) },   // 26:05+4f rel 49 0 (20ms,SHARP)
    {  0, 26,  8,  0,       0, W(  5) },   // 26:08+0f SOFT att 5
    {  0, 26,  8,  1,      20, W(  0) },   // 26:08+1f rel 5 0 (20ms,SOFT)
    {  0, 26, 17,  6,       0, W( 30) },   // 26:17+6f FAST att 30
    {  0, 26, 17,  7,      20, W(  0) },   // 26:17+7f rel 30 0 (20ms,FAST)
    {  0, 26, 18,  0,       0, W( 50) },   // 26:18+0f SOFT att 50
    {  0, 26, 18,  2,      55, W(  0) },   // 26:18+2f rel 50 0 (55ms,SOFT)
    {  0, 26, 19,  4,       0, W( 77) },   // 26:19+4f SHARP att 77
    {  0, 26, 19,  5,      30, W(  0) },   // 26:19+5f rel 77 0 (30ms,SHARP)
    {  0, 26, 20,  6,       0, W( 43) },   // 26:20+6f FAST att 43
    {  0, 26, 20,  7,      28, W(  0) },   // 26:20+7f rel 43 0 (28ms,FAST)
    {  0, 26, 21,  4,       0, W( 71) },   // 26:21+4f FAST att 71
    {  0, 26, 21,  5,      47, W(  0) },   // 26:21+5f rel 71 0 (47ms,FAST)
    {  0, 26, 22,  2,       0, W( 48) },   // 26:22+2f FAST att 48
    {  0, 26, 22,  4,      32, W(  0) },   // 26:22+4f rel 48 0 (32ms,FAST)
    {  0, 26, 23,  4,       0, W( 66) },   // 26:23+4f FAST att 66
    {  0, 26, 23,  6,      44, W(  0) },   // 26:23+6f rel 66 0 (44ms,FAST)
    {  0, 26, 24,  5,       0, W( 89) },   // 26:24+5f SHARP att 89
    {  0, 26, 24,  6,      35, W(  0) },   // 26:24+6f rel 89 0 (35ms,SHARP)
    {  0, 26, 24, 10,       0, W( 71) },   // 26:24+10f SHARP att 71
    {  0, 26, 24, 12,      28, W(  0) },   // 26:24+12f rel 71 0 (28ms,SHARP)
    {  0, 26, 25,  6,       0, W( 34) },   // 26:25+6f FAST att 34
    {  0, 26, 25,  7,      22, W(  0) },   // 26:25+7f rel 34 0 (22ms,FAST)
    {  0, 26, 26,  6,       0, W( 13) },   // 26:26+6f SHARP att 13
    {  0, 26, 26,  7,      20, W(  0) },   // 26:26+7f rel 13 0 (20ms,SHARP)
    {  0, 26, 27,  1,       0, W(  2) },   // 26:27+1f SHARP att 2
    {  0, 26, 27,  2,      20, W(  0) },   // 26:27+2f rel 2 0 (20ms,SHARP)
    {  0, 26, 28,  2,       0, W(101) },   // 26:28+2f SOFT att 101
    {  0, 26, 28,  3,     112, W(  0) },   // 26:28+3f rel 101 0 (112ms,SOFT)
    {  0, 26, 29,  2,       0, W(  8) },   // 26:29+2f FAST att 8
    {  0, 26, 29,  3,      20, W(  0) },   // 26:29+3f rel 8 0 (20ms,FAST)
    {  0, 26, 30,  3,       0, W( 61) },   // 26:30+3f FAST att 61
    {  0, 26, 30,  4,      40, W(  0) },   // 26:30+4f rel 61 0 (40ms,FAST)
    {  0, 26, 31,  5,       0, W( 95) },   // 26:31+5f SHARP att 95
    {  0, 26, 31,  7,      38, W(  0) },   // 26:31+7f rel 95 0 (38ms,SHARP)
    {  0, 26, 31, 17,       0, W( 57) },   // 26:31+17f SHARP att 57
    {  0, 26, 31, 18,      22, W(  0) },   // 26:31+18f rel 57 0 (22ms,SHARP)
    {  0, 26, 32,  4,       0, W( 34) },   // 26:32+4f SHARP att 34
    {  0, 26, 32,  6,      20, W(  0) },   // 26:32+6f rel 34 0 (20ms,SHARP)
    {  0, 26, 35,  5,       0, W( 98) },   // 26:35+5f FAST att 98
    {  0, 26, 35,  6,      65, W(  0) },   // 26:35+6f rel 98 0 (65ms,FAST)
    {  0, 26, 35, 16,       0, W( 80) },   // 26:35+16f FAST att 80
    {  0, 26, 35, 17,      53, W(  0) },   // 26:35+17f rel 80 0 (53ms,FAST)
    {  0, 26, 37,  3,       0, W( 26) },   // 26:37+3f FAST att 26
    {  0, 26, 37,  5,      20, W(  0) },   // 26:37+5f rel 26 0 (20ms,FAST)
    {  0, 26, 38,  0,       0, W( 53) },   // 26:38+0f SOFT att 53
    {  0, 26, 38,  2,      58, W(  0) },   // 26:38+2f rel 53 0 (58ms,SOFT)
    {  0, 26, 39,  0,       0, W( 32) },   // 26:39+0f FAST att 32
    {  0, 26, 39,  2,      21, W(  0) },   // 26:39+2f rel 32 0 (21ms,FAST)
    {  0, 26, 40,  3,       0, W(  4) },   // 26:40+3f FAST att 4
    {  0, 26, 40,  4,      20, W(  0) },   // 26:40+4f rel 4 0 (20ms,FAST)
    {  0, 26, 42,  2,       0, W( 23) },   // 26:42+2f FAST att 23
    {  0, 26, 42,  3,      20, W(  0) },   // 26:42+3f rel 23 0 (20ms,FAST)
    {  0, 26, 43,  3,       0, W( 44) },   // 26:43+3f FAST att 44
    {  0, 26, 43,  4,      29, W(  0) },   // 26:43+4f rel 44 0 (29ms,FAST)
    {  0, 26, 44,  3,       0, W( 62) },   // 26:44+3f SHARP att 62
    {  0, 26, 44,  4,      24, W(  0) },   // 26:44+4f rel 62 0 (24ms,SHARP)
    {  0, 26, 45,  5,       0, W( 30) },   // 26:45+5f SOFT att 30
    {  0, 26, 45,  7,      33, W(  0) },   // 26:45+7f rel 30 0 (33ms,SOFT)
    {  0, 26, 46,  2,       0, W( 58) },   // 26:46+2f SOFT att 58
    {  0, 26, 46,  3,      64, W(  0) },   // 26:46+3f rel 58 0 (64ms,SOFT)
    {  0, 26, 47,  4,       0, W( 24) },   // 26:47+4f SOFT att 24
    {  0, 26, 47,  5,      26, W(  0) },   // 26:47+5f rel 24 0 (26ms,SOFT)
    {  0, 26, 48,  3,       0, W( 86) },   // 26:48+3f FAST att 86
    {  0, 26, 48,  4,      57, W(  0) },   // 26:48+4f rel 86 0 (57ms,FAST)
    {  0, 26, 48,  8,       0, W(107) },   // 26:48+8f FAST att 107
    {  0, 26, 48,  9,      71, W(  0) },   // 26:48+9f rel 107 0 (71ms,FAST)
    {  0, 26, 49,  2,       0, W( 66) },   // 26:49+2f SOFT att 66
    {  0, 26, 49,  3,      73, W(  0) },   // 26:49+3f rel 66 0 (73ms,SOFT)
    {  0, 26, 49, 13,       0, W( 48) },   // 26:49+13f FAST att 48
    {  0, 26, 49, 14,      32, W(  0) },   // 26:49+14f rel 48 0 (32ms,FAST)
    {  0, 26, 50,  5,       0, W( 70) },   // 26:50+5f FAST att 70
    {  0, 26, 50,  7,      46, W(  0) },   // 26:50+7f rel 70 0 (46ms,FAST)
    {  0, 26, 50, 18,       0, W(115) },   // 26:50+18f FAST att 115
    {  0, 26, 50, 19,      76, W(  0) },   // 26:50+19f rel 115 0 (76ms,FAST)
    {  0, 26, 51,  6,       0, W( 62) },   // 26:51+6f SHARP att 62
    {  0, 26, 51,  7,      24, W(  0) },   // 26:51+7f rel 62 0 (24ms,SHARP)
    {  0, 26, 52,  2,       0, W( 36) },   // 26:52+2f SHARP att 36
    {  0, 26, 52,  3,      20, W(  0) },   // 26:52+3f rel 36 0 (20ms,SHARP)
    {  0, 26, 53,  2,       0, W( 56) },   // 26:53+2f SHARP att 56
    {  0, 26, 53,  3,      22, W(  0) },   // 26:53+3f rel 56 0 (22ms,SHARP)
    {  0, 26, 54,  4,       0, W( 26) },   // 26:54+4f SHARP att 26
    {  0, 26, 54,  6,      20, W(  0) },   // 26:54+6f rel 26 0 (20ms,SHARP)
    {  0, 26, 55,  6,       0, W( 51) },   // 26:55+6f SHARP att 51
    {  0, 26, 55,  8,      20, W(  0) },   // 26:55+8f rel 51 0 (20ms,SHARP)
    {  0, 26, 56,  4,       0, W( 75) },   // 26:56+4f SOFT att 75
    {  0, 26, 56,  5,      83, W(  0) },   // 26:56+5f rel 75 0 (83ms,SOFT)
    {  0, 26, 56, 15,       0, W( 50) },   // 26:56+15f FAST att 50
    {  0, 26, 56, 16,      33, W(  0) },   // 26:56+16f rel 50 0 (33ms,FAST)
    {  0, 26, 57,  1,       0, W( 71) },   // 26:57+1f FAST att 71
    {  0, 26, 57,  2,      47, W(  0) },   // 26:57+2f rel 71 0 (47ms,FAST)
    {  0, 26, 58,  6,       0, W( 42) },   // 26:58+6f FAST att 42
    {  0, 26, 58,  7,      28, W(  0) },   // 26:58+7f rel 42 0 (28ms,FAST)
    {  0, 26, 59,  1,       0, W( 64) },   // 26:59+1f SOFT att 64
    {  0, 26, 59,  2,      71, W(  0) },   // 26:59+2f rel 64 0 (71ms,SOFT)
    {  0, 26, 59, 13,       0, W( 38) },   // 26:59+13f FAST att 38
    {  0, 26, 59, 14,      25, W(  0) },   // 26:59+14f rel 38 0 (25ms,FAST)
    {  0, 27,  0,  0,       0, W( 75) },   // 27:00+0f SHARP att 75
    {  0, 27,  0,  1,      30, W(  0) },   // 27:00+1f rel 75 0 (30ms,SHARP)
    {  0, 27,  0,  6,       0, W( 55) },   // 27:00+6f FAST att 55
    {  0, 27,  0,  7,      36, W(  0) },   // 27:00+7f rel 55 0 (36ms,FAST)
    {  0, 27,  1,  5,       0, W( 77) },   // 27:01+5f SOFT att 77
    {  0, 27,  1,  6,      85, W(  0) },   // 27:01+6f rel 77 0 (85ms,SOFT)
    {  0, 27,  2,  2,       0, W( 56) },   // 27:02+2f FAST att 56
    {  0, 27,  2,  4,      37, W(  0) },   // 27:02+4f rel 56 0 (37ms,FAST)
    {  0, 27,  2, 12,       0, W( 77) },   // 27:02+12f FAST att 77
    {  0, 27,  2, 14,      51, W(  0) },   // 27:02+14f rel 77 0 (51ms,FAST)
    {  0, 27,  3,  0,       0, W( 24) },   // 27:03+0f FAST att 24
    {  0, 27,  3,  1,      20, W(  0) },   // 27:03+1f rel 24 0 (20ms,FAST)
    {  0, 27,  5,  2,       0, W(  6) },   // 27:05+2f FAST att 6
    {  0, 27,  5,  4,      20, W(  0) },   // 27:05+4f rel 6 0 (20ms,FAST)
    {  0, 27,  6,  0,       0, W( 26) },   // 27:06+0f SOFT att 26
    {  0, 27,  6,  1,      28, W(  0) },   // 27:06+1f rel 26 0 (28ms,SOFT)
    {  0, 27,  7,  2,       0, W(  2) },   // 27:07+2f FAST att 2
    {  0, 27,  7,  4,      20, W(  0) },   // 27:07+4f rel 2 0 (20ms,FAST)
    {  0, 27,  8,  3,       0, W( 21) },   // 27:08+3f FAST att 21
    {  0, 27,  8,  4,      20, W(  0) },   // 27:08+4f rel 21 0 (20ms,FAST)
    {  0, 27,  9,  6,       0, W(  3) },   // 27:09+6f FAST att 3
    {  0, 27,  9,  8,      20, W(  0) },   // 27:09+8f rel 3 0 (20ms,FAST)
    {  0, 27, 12,  4,       0, W( 51) },   // 27:12+4f SHARP att 51
    {  0, 27, 12,  5,      20, W(  0) },   // 27:12+5f rel 51 0 (20ms,SHARP)
    {  0, 27, 13,  5,       0, W(  3) },   // 27:13+5f SHARP att 3
    {  0, 27, 13,  7,      20, W(  0) },   // 27:13+7f rel 3 0 (20ms,SHARP)
    {  0, 27, 14,  0,       0, W( 21) },   // 27:14+0f SHARP att 21
    {  0, 27, 14,  2,      20, W(  0) },   // 27:14+2f rel 21 0 (20ms,SHARP)
    {  0, 27, 15,  1,       0, W( 44) },   // 27:15+1f FAST att 44
    {  0, 27, 15,  3,      29, W(  0) },   // 27:15+3f rel 44 0 (29ms,FAST)
    {  0, 27, 16,  5,       0, W( 74) },   // 27:16+5f SOFT att 74
    {  0, 27, 16,  6,      82, W(  0) },   // 27:16+6f rel 74 0 (82ms,SOFT)
    {  0, 27, 17,  2,       0, W( 24) },   // 27:17+2f FAST att 24
    {  0, 27, 17,  3,      20, W(  0) },   // 27:17+3f rel 24 0 (20ms,FAST)
    {  0, 27, 18,  5,       0, W(  2) },   // 27:18+5f FAST att 2
    {  0, 27, 18,  6,      20, W(  0) },   // 27:18+6f rel 2 0 (20ms,FAST)
    {  0, 27, 22,  4,       0, W( 21) },   // 27:22+4f SOFT att 21
    {  0, 27, 22,  5,      23, W(  0) },   // 27:22+5f rel 21 0 (23ms,SOFT)
    {  0, 27, 26,  5,       0, W(  2) },   // 27:26+5f FAST att 2
    {  0, 27, 26,  7,      20, W(  0) },   // 27:26+7f rel 2 0 (20ms,FAST)
    {  0, 27, 29,  1,       0, W( 27) },   // 27:29+1f FAST att 27
    {  0, 27, 29,  2,      20, W(  0) },   // 27:29+2f rel 27 0 (20ms,FAST)
    {  0, 27, 30,  1,       0, W(  2) },   // 27:30+1f FAST att 2
    {  0, 27, 30,  2,      20, W(  0) },   // 27:30+2f rel 2 0 (20ms,FAST)
    {  0, 27, 31,  2,       0, W( 20) },   // 27:31+2f FAST att 20
    {  0, 27, 31,  3,      20, W(  0) },   // 27:31+3f rel 20 0 (20ms,FAST)
    {  0, 27, 33,  6,       0, W(  2) },   // 27:33+6f SHARP att 2
    {  0, 27, 33,  7,      20, W(  0) },   // 27:33+7f rel 2 0 (20ms,SHARP)
    {  0, 27, 34,  4,       0, W( 31) },   // 27:34+4f FAST att 31
    {  0, 27, 34,  5,      20, W(  0) },   // 27:34+5f rel 31 0 (20ms,FAST)
    {  0, 27, 35,  5,       0, W( 56) },   // 27:35+5f SOFT att 56
    {  0, 27, 35,  6,      62, W(  0) },   // 27:35+6f rel 56 0 (62ms,SOFT)
    {  0, 27, 36,  3,       0, W( 36) },   // 27:36+3f SHARP att 36
    {  0, 27, 36,  4,      20, W(  0) },   // 27:36+4f rel 36 0 (20ms,SHARP)
    {  0, 27, 37,  5,       0, W( 57) },   // 27:37+5f FAST att 57
    {  0, 27, 37,  6,      38, W(  0) },   // 27:37+6f rel 57 0 (38ms,FAST)
    {  0, 27, 38,  5,       0, W( 35) },   // 27:38+5f SHARP att 35
    {  0, 27, 38,  7,      20, W(  0) },   // 27:38+7f rel 35 0 (20ms,SHARP)
    {  0, 27, 39,  1,       0, W( 12) },   // 27:39+1f SHARP att 12
    {  0, 27, 39,  2,      20, W(  0) },   // 27:39+2f rel 12 0 (20ms,SHARP)
    {  0, 27, 44,  3,       0, W( 35) },   // 27:44+3f SHARP att 35
    {  0, 27, 44,  5,      20, W(  0) },   // 27:44+5f rel 35 0 (20ms,SHARP)
    {  0, 27, 45,  5,       0, W(  8) },   // 27:45+5f FAST att 8
    {  0, 27, 45,  6,      20, W(  0) },   // 27:45+6f rel 8 0 (20ms,FAST)
    {  0, 27, 51,  4,       0, W( 33) },   // 27:51+4f FAST att 33
    {  0, 27, 51,  5,      22, W(  0) },   // 27:51+5f rel 33 0 (22ms,FAST)
    {  0, 27, 56,  4,       0, W(  7) },   // 27:56+4f SOFT att 7
    {  0, 27, 56,  6,      20, W(  0) },   // 27:56+6f rel 7 0 (20ms,SOFT)
    {  0, 27, 57,  6,       0, W( 33) },   // 27:57+6f FAST att 33
    {  0, 27, 57,  7,      22, W(  0) },   // 27:57+7f rel 33 0 (22ms,FAST)
    {  0, 28,  8,  2,       0, W( 64) },   // 28:08+2f SHARP att 64
    {  0, 28,  8,  3,      25, W(  0) },   // 28:08+3f rel 64 0 (25ms,SHARP)
    {  0, 28, 23,  4,       0, W( 13) },   // 28:23+4f SHARP att 13
    {  0, 28, 23,  6,      20, W(  0) },   // 28:23+6f rel 13 0 (20ms,SHARP)
    {  0, 28, 24,  2,       0, W( 31) },   // 28:24+2f SHARP att 31
    {  0, 28, 24,  3,      20, W(  0) },   // 28:24+3f rel 31 0 (20ms,SHARP)
    {  0, 28, 25,  3,       0, W( 12) },   // 28:25+3f FAST att 12
    {  0, 28, 25,  4,      20, W(  0) },   // 28:25+4f rel 12 0 (20ms,FAST)
    {  0, 28, 26,  6,       0, W( 31) },   // 28:26+6f SHARP att 31
    {  0, 28, 26,  7,      20, W(  0) },   // 28:26+7f rel 31 0 (20ms,SHARP)
    {  0, 28, 27,  3,       0, W(  7) },   // 28:27+3f SOFT att 7
    {  0, 28, 27,  4,      20, W(  0) },   // 28:27+4f rel 7 0 (20ms,SOFT)
    {  0, 28, 28,  3,       0, W( 27) },   // 28:28+3f SHARP att 27
    {  0, 28, 28,  5,      20, W(  0) },   // 28:28+5f rel 27 0 (20ms,SHARP)
    {  0, 28, 30,  6,       0, W(  9) },   // 28:30+6f FAST att 9
    {  0, 28, 30,  7,      20, W(  0) },   // 28:30+7f rel 9 0 (20ms,FAST)
    {  0, 28, 32,  5,       0, W( 28) },   // 28:32+5f SOFT att 28
    {  0, 28, 32,  6,      31, W(  0) },   // 28:32+6f rel 28 0 (31ms,SOFT)
    {  0, 28, 33,  3,       0, W(  8) },   // 28:33+3f SHARP att 8
    {  0, 28, 33,  4,      20, W(  0) },   // 28:33+4f rel 8 0 (20ms,SHARP)
    {  0, 28, 37,  4,       0, W( 28) },   // 28:37+4f FAST att 28
    {  0, 28, 37,  6,      20, W(  0) },   // 28:37+6f rel 28 0 (20ms,FAST)
    {  0, 28, 38,  5,       0, W( 78) },   // 28:38+5f SHARP att 78
    {  0, 28, 38,  7,      31, W(  0) },   // 28:38+7f rel 78 0 (31ms,SHARP)
    {  0, 28, 38, 10,       0, W( 99) },   // 28:38+10f SHARP att 99
    {  0, 28, 38, 11,      39, W(  0) },   // 28:38+11f rel 99 0 (39ms,SHARP)
    {  0, 28, 39,  2,       0, W( 14) },   // 28:39+2f FAST att 14
    {  0, 28, 39,  4,      20, W(  0) },   // 28:39+4f rel 14 0 (20ms,FAST)
    {  0, 28, 42,  4,       0, W(  2) },   // 28:42+4f SHARP att 2
    {  0, 28, 42,  5,      20, W(  0) },   // 28:42+5f rel 2 0 (20ms,SHARP)
    {  0, 28, 43,  4,       0, W( 21) },   // 28:43+4f FAST att 21
    {  0, 28, 43,  5,      20, W(  0) },   // 28:43+5f rel 21 0 (20ms,FAST)
    {  0, 28, 45,  3,       0, W(  2) },   // 28:45+3f SHARP att 2
    {  0, 28, 45,  4,      20, W(  0) },   // 28:45+4f rel 2 0 (20ms,SHARP)
    {  0, 28, 46,  3,       0, W( 20) },   // 28:46+3f FAST att 20
    {  0, 28, 46,  5,      20, W(  0) },   // 28:46+5f rel 20 0 (20ms,FAST)
    {  0, 28, 47,  3,       0, W(  2) },   // 28:47+3f FAST att 2
    {  0, 28, 47,  4,      20, W(  0) },   // 28:47+4f rel 2 0 (20ms,FAST)
    {  0, 28, 48,  6,       0, W( 29) },   // 28:48+6f FAST att 29
    {  0, 28, 48,  8,      20, W(  0) },   // 28:48+8f rel 29 0 (20ms,FAST)
    {  0, 28, 49,  1,       0, W( 10) },   // 28:49+1f SOFT att 10
    {  0, 28, 49,  3,      20, W(  0) },   // 28:49+3f rel 10 0 (20ms,SOFT)
    {  0, 28, 50,  3,       0, W( 35) },   // 28:50+3f FAST att 35
    {  0, 28, 50,  5,      23, W(  0) },   // 28:50+5f rel 35 0 (23ms,FAST)
    {  0, 28, 51,  2,       0, W( 60) },   // 28:51+2f SHARP att 60
    {  0, 28, 51,  3,      24, W(  0) },   // 28:51+3f rel 60 0 (24ms,SHARP)
    {  0, 28, 52,  5,       0, W( 39) },   // 28:52+5f SOFT att 39
    {  0, 28, 52,  6,      43, W(  0) },   // 28:52+6f rel 39 0 (43ms,SOFT)
    {  0, 28, 53,  2,       0, W( 12) },   // 28:53+2f FAST att 12
    {  0, 28, 53,  4,      20, W(  0) },   // 28:53+4f rel 12 0 (20ms,FAST)
    {  0, 28, 54,  1,       0, W(  2) },   // 28:54+1f SOFT att 2
    {  0, 28, 54,  2,      20, W(  0) },   // 28:54+2f rel 2 0 (20ms,SOFT)
    {  0, 28, 55,  5,       0, W( 21) },   // 28:55+5f SHARP att 21
    {  0, 28, 55,  6,      20, W(  0) },   // 28:55+6f rel 21 0 (20ms,SHARP)
    {  0, 28, 56,  5,       0, W( 46) },   // 28:56+5f SOFT att 46
    {  0, 28, 56,  7,      51, W(  0) },   // 28:56+7f rel 46 0 (51ms,SOFT)
    {  0, 28, 57,  0,       0, W( 24) },   // 28:57+0f SOFT att 24
    {  0, 28, 57,  1,      26, W(  0) },   // 28:57+1f rel 24 0 (26ms,SOFT)
    {  0, 28, 58,  5,       0, W( 42) },   // 28:58+5f SHARP att 42
    {  0, 28, 58,  6,      20, W(  0) },   // 28:58+6f rel 42 0 (20ms,SHARP)
    {  0, 28, 59,  5,       0, W( 21) },   // 28:59+5f SHARP att 21
    {  0, 28, 59,  6,      20, W(  0) },   // 28:59+6f rel 21 0 (20ms,SHARP)
    {  0, 29,  0,  4,       0, W(  3) },   // 29:00+4f FAST att 3
    {  0, 29,  0,  6,      20, W(  0) },   // 29:00+6f rel 3 0 (20ms,FAST)
    {  0, 29,  1,  2,       0, W( 87) },   // 29:01+2f FAST att 87
    {  0, 29,  1,  4,      58, W(  0) },   // 29:01+4f rel 87 0 (58ms,FAST)
    {  0, 29,  1, 15,       0, W(144) },   // 29:01+15f SHARP att 144
    {  0, 29,  1, 16,      57, W(  0) },   // 29:01+16f rel 144 0 (57ms,SHARP)
    {  0, 29,  2,  0,       0, W( 45) },   // 29:02+0f FAST att 45
    {  0, 29,  2,  1,      30, W(  0) },   // 29:02+1f rel 45 0 (30ms,FAST)
    {  0, 29,  3,  2,       0, W( 96) },   // 29:03+2f FAST att 96
    {  0, 29,  3,  3,      64, W(  0) },   // 29:03+3f rel 96 0 (64ms,FAST)
    {  0, 29,  3, 11,       0, W( 63) },   // 29:03+11f SOFT att 63
    {  0, 29,  3, 13,      70, W(  0) },   // 29:03+13f rel 63 0 (70ms,SOFT)
    {  0, 29,  4,  0,       0, W( 15) },   // 29:04+0f FAST att 15
    {  0, 29,  4,  1,      20, W(  0) },   // 29:04+1f rel 15 0 (20ms,FAST)
    {  0, 29,  5,  3,       0, W( 34) },   // 29:05+3f SOFT att 34
    {  0, 29,  5,  5,      37, W(  0) },   // 29:05+5f rel 34 0 (37ms,SOFT)
    {  0, 29, 10,  1,       0, W( 12) },   // 29:10+1f FAST att 12
    {  0, 29, 10,  3,      20, W(  0) },   // 29:10+3f rel 12 0 (20ms,FAST)
    {  0, 29, 11,  0,       0, W( 31) },   // 29:11+0f SHARP att 31
    {  0, 29, 11,  1,      20, W(  0) },   // 29:11+1f rel 31 0 (20ms,SHARP)
    {  0, 29, 12,  1,       0, W( 60) },   // 29:12+1f SOFT att 60
    {  0, 29, 12,  2,      66, W(  0) },   // 29:12+2f rel 60 0 (66ms,SOFT)
    {  0, 29, 13,  2,       0, W( 40) },   // 29:13+2f SHARP att 40
    {  0, 29, 13,  4,      20, W(  0) },   // 29:13+4f rel 40 0 (20ms,SHARP)
    {  0, 29, 14,  5,       0, W( 58) },   // 29:14+5f SHARP att 58
    {  0, 29, 14,  6,      23, W(  0) },   // 29:14+6f rel 58 0 (23ms,SHARP)
    {  0, 29, 15,  6,       0, W( 84) },   // 29:15+6f SOFT att 84
    {  0, 29, 15,  8,      93, W(  0) },   // 29:15+8f rel 84 0 (93ms,SOFT)
    {  0, 29, 15, 12,       0, W( 57) },   // 29:15+12f SOFT att 57
    {  0, 29, 15, 13,      63, W(  0) },   // 29:15+13f rel 57 0 (63ms,SOFT)
    {  0, 29, 16,  1,       0, W(115) },   // 29:16+1f SHARP att 115
    {  0, 29, 16,  2,      46, W(  0) },   // 29:16+2f rel 115 0 (46ms,SHARP)
    {  0, 29, 16, 11,       0, W( 96) },   // 29:16+11f SHARP att 96
    {  0, 29, 16, 12,      38, W(  0) },   // 29:16+12f rel 96 0 (38ms,SHARP)
    {  0, 29, 17,  4,       0, W( 70) },   // 29:17+4f SHARP att 70
    {  0, 29, 17,  6,      28, W(  0) },   // 29:17+6f rel 70 0 (28ms,SHARP)
    {  0, 29, 18,  2,       0, W( 33) },   // 29:18+2f SHARP att 33
    {  0, 29, 18,  4,      20, W(  0) },   // 29:18+4f rel 33 0 (20ms,SHARP)
    {  0, 29, 19,  3,       0, W( 66) },   // 29:19+3f FAST att 66
    {  0, 29, 19,  4,      44, W(  0) },   // 29:19+4f rel 66 0 (44ms,FAST)
    {  0, 29, 20,  1,       0, W(101) },   // 29:20+1f FAST att 101
    {  0, 29, 20,  3,      67, W(  0) },   // 29:20+3f rel 101 0 (67ms,FAST)
    {  0, 29, 20,  7,       0, W( 75) },   // 29:20+7f FAST att 75
    {  0, 29, 20,  8,      50, W(  0) },   // 29:20+8f rel 75 0 (50ms,FAST)
    {  0, 29, 21,  6,       0, W(103) },   // 29:21+6f FAST att 103
    {  0, 29, 21,  7,      68, W(  0) },   // 29:21+7f rel 103 0 (68ms,FAST)
    {  0, 29, 22,  4,       0, W( 63) },   // 29:22+4f FAST att 63
    {  0, 29, 22,  6,      42, W(  0) },   // 29:22+6f rel 63 0 (42ms,FAST)
    {  0, 29, 22, 12,       0, W( 38) },   // 29:22+12f FAST att 38
    {  0, 29, 22, 13,      25, W(  0) },   // 29:22+13f rel 38 0 (25ms,FAST)
    {  0, 29, 23,  1,       0, W( 61) },   // 29:23+1f FAST att 61
    {  0, 29, 23,  2,      40, W(  0) },   // 29:23+2f rel 61 0 (40ms,FAST)
    {  0, 29, 24,  0,       0, W(102) },   // 29:24+0f SOFT att 102
    {  0, 29, 24,  1,     113, W(  0) },   // 29:24+1f rel 102 0 (113ms,SOFT)
    {  0, 29, 24, 11,       0, W( 76) },   // 29:24+11f FAST att 76
    {  0, 29, 24, 12,      50, W(  0) },   // 29:24+12f rel 76 0 (50ms,FAST)
    {  0, 29, 25,  6,       0, W( 45) },   // 29:25+6f FAST att 45
    {  0, 29, 25,  7,      30, W(  0) },   // 29:25+7f rel 45 0 (30ms,FAST)
    {  0, 29, 26,  5,       0, W( 72) },   // 29:26+5f SHARP att 72
    {  0, 29, 26,  6,      28, W(  0) },   // 29:26+6f rel 72 0 (28ms,SHARP)
    {  0, 29, 27,  6,       0, W( 47) },   // 29:27+6f SHARP att 47
    {  0, 29, 27,  8,      20, W(  0) },   // 29:27+8f rel 47 0 (20ms,SHARP)
    {  0, 29, 27, 17,       0, W( 79) },   // 29:27+17f FAST att 79
    {  0, 29, 27, 18,      52, W(  0) },   // 29:27+18f rel 79 0 (52ms,FAST)
    {  0, 29, 31,  5,       0, W( 50) },   // 29:31+5f SOFT att 50
    {  0, 29, 31,  6,      55, W(  0) },   // 29:31+6f rel 50 0 (55ms,SOFT)
    {  0, 29, 32,  0,       0, W( 31) },   // 29:32+0f SHARP att 31
    {  0, 29, 32,  1,      20, W(  0) },   // 29:32+1f rel 31 0 (20ms,SHARP)
    {  0, 29, 37,  5,       0, W( 57) },   // 29:37+5f FAST att 57
    {  0, 29, 37,  7,      38, W(  0) },   // 29:37+7f rel 57 0 (38ms,FAST)
    {  0, 29, 42,  0,       0, W( 33) },   // 29:42+0f FAST att 33
    {  0, 29, 42,  1,      22, W(  0) },   // 29:42+1f rel 33 0 (22ms,FAST)
    {  0, 29, 43,  5,       0, W( 73) },   // 29:43+5f FAST att 73
    {  0, 29, 43,  7,      48, W(  0) },   // 29:43+7f rel 73 0 (48ms,FAST)
    {  0, 29, 43, 19,       0, W( 49) },   // 29:43+19f SHARP att 49
    {  0, 29, 43, 20,      20, W(  0) },   // 29:43+20f rel 49 0 (20ms,SHARP)
    {  0, 29, 44,  4,       0, W(101) },   // 29:44+4f FAST att 101
    {  0, 29, 44,  5,      67, W(  0) },   // 29:44+5f rel 101 0 (67ms,FAST)
    {  0, 29, 45,  4,       0, W( 83) },   // 29:45+4f SHARP att 83
    {  0, 29, 45,  5,      33, W(  0) },   // 29:45+5f rel 83 0 (33ms,SHARP)
    {  0, 29, 45, 17,       0, W(145) },   // 29:45+17f SHARP att 145
    {  0, 29, 45, 18,      58, W(  0) },   // 29:45+18f rel 145 0 (58ms,SHARP)
    {  0, 29, 46,  1,       0, W( 91) },   // 29:46+1f SOFT att 91
    {  0, 29, 46,  2,     101, W(  0) },   // 29:46+2f rel 91 0 (101ms,SOFT)
    {  0, 29, 47,  4,       0, W( 51) },   // 29:47+4f FAST att 51
    {  0, 29, 47,  5,      34, W(  0) },   // 29:47+5f rel 51 0 (34ms,FAST)
    {  0, 29, 48,  5,       0, W(  5) },   // 29:48+5f SHARP att 5
    {  0, 29, 48,  6,      20, W(  0) },   // 29:48+6f rel 5 0 (20ms,SHARP)
    {  0, 29, 54,  5,       0, W( 26) },   // 29:54+5f FAST att 26
    {  0, 29, 54,  6,      20, W(  0) },   // 29:54+6f rel 26 0 (20ms,FAST)
    {  0, 29, 55,  1,       0, W( 48) },   // 29:55+1f SHARP att 48
    {  0, 29, 55,  3,      20, W(  0) },   // 29:55+3f rel 48 0 (20ms,SHARP)
    {  0, 29, 56,  4,       0, W( 74) },   // 29:56+4f SOFT att 74
    {  0, 29, 56,  5,      82, W(  0) },   // 29:56+5f rel 74 0 (82ms,SOFT)
    {  0, 29, 57,  5,       0, W( 19) },   // 29:57+5f FAST att 19
    {  0, 29, 57,  7,      20, W(  0) },   // 29:57+7f rel 19 0 (20ms,FAST)
    {  0, 30,  0,  0,       0, W( 44) },   // 30:00+0f FAST att 44
    {  0, 30,  0,  1,      29, W(  0) },   // 30:00+1f rel 44 0 (29ms,FAST)
    {  0, 30,  1,  5,       0, W( 76) },   // 30:01+5f FAST att 76
    {  0, 30,  1,  6,      50, W(  0) },   // 30:01+6f rel 76 0 (50ms,FAST)
    {  0, 30,  1, 13,       0, W( 56) },   // 30:01+13f SHARP att 56
    {  0, 30,  1, 14,      22, W(  0) },   // 30:01+14f rel 56 0 (22ms,SHARP)
    {  0, 30,  2,  4,       0, W( 78) },   // 30:02+4f SHARP att 78
    {  0, 30,  2,  6,      31, W(  0) },   // 30:02+6f rel 78 0 (31ms,SHARP)
    {  0, 30,  3,  5,       0, W( 56) },   // 30:03+5f SHARP att 56
    {  0, 30,  3,  7,      22, W(  0) },   // 30:03+7f rel 56 0 (22ms,SHARP)
    {  0, 30,  4,  0,       0, W( 10) },   // 30:04+0f FAST att 10
    {  0, 30,  4,  1,      20, W(  0) },   // 30:04+1f rel 10 0 (20ms,FAST)
    {  0, 30, 13,  1,       0, W( 29) },   // 30:13+1f FAST att 29
    {  0, 30, 13,  3,      20, W(  0) },   // 30:13+3f rel 29 0 (20ms,FAST)
    {  0, 30, 14,  5,       0, W( 94) },   // 30:14+5f SHARP att 94
    {  0, 30, 14,  6,      37, W(  0) },   // 30:14+6f rel 94 0 (37ms,SHARP)
    {  0, 30, 14, 18,       0, W( 74) },   // 30:14+18f FAST att 74
    {  0, 30, 14, 19,      49, W(  0) },   // 30:14+19f rel 74 0 (49ms,FAST)
    {  0, 30, 15,  3,       0, W( 92) },   // 30:15+3f SOFT att 92
    {  0, 30, 15,  4,     102, W(  0) },   // 30:15+4f rel 92 0 (102ms,SOFT)
    {  0, 30, 16,  4,       0, W( 16) },   // 30:16+4f FAST att 16
    {  0, 30, 16,  5,      20, W(  0) },   // 30:16+5f rel 16 0 (20ms,FAST)
    {  0, 30, 22,  6,       0, W( 37) },   // 30:22+6f FAST att 37
    {  0, 30, 22,  7,      24, W(  0) },   // 30:22+7f rel 37 0 (24ms,FAST)
    {  0, 30, 23,  1,       0, W( 57) },   // 30:23+1f FAST att 57
    {  0, 30, 23,  2,      38, W(  0) },   // 30:23+2f rel 57 0 (38ms,FAST)
    {  0, 30, 24,  3,       0, W(100) },   // 30:24+3f FAST att 100
    {  0, 30, 24,  4,      66, W(  0) },   // 30:24+4f rel 100 0 (66ms,FAST)
    {  0, 30, 24, 17,       0, W(126) },   // 30:24+17f SOFT att 126
    {  0, 30, 24, 19,     140, W(  0) },   // 30:24+19f rel 126 0 (140ms,SOFT)
    {  0, 30, 25,  5,       0, W( 52) },   // 30:25+5f SHARP att 52
    {  0, 30, 25,  6,      20, W(  0) },   // 30:25+6f rel 52 0 (20ms,SHARP)
    {  0, 30, 25, 17,       0, W( 74) },   // 30:25+17f SHARP att 74
    {  0, 30, 25, 18,      29, W(  0) },   // 30:25+18f rel 74 0 (29ms,SHARP)
    {  0, 30, 26,  0,       0, W( 27) },   // 30:26+0f FAST att 27
    {  0, 30, 26,  1,      20, W(  0) },   // 30:26+1f rel 27 0 (20ms,FAST)
    {  0, 30, 27,  0,       0, W(  2) },   // 30:27+0f SOFT att 2
    {  0, 30, 27,  1,      20, W(  0) },   // 30:27+1f rel 2 0 (20ms,SOFT)
    // ====================================================================
    // FADE OUT
    // ====================================================================

    {  0, 31, 38,  0,       0, W(  0) },   // hard black at end

};  // END SABER_CUE_LIST

#undef W

static const uint16_t SABER_CUE_COUNT =
    sizeof(SABER_CUE_LIST) / sizeof(SABER_CUE_LIST[0]);

