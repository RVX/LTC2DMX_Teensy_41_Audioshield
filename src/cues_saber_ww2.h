#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Cue list — ADJ Saber Spot WW  (Unit 2, fixed wash)
// Julian Charrière · CORRER · Venice 2026
//
//   DMX address 5 · 1-channel mode (CH5 = dimmer, 0–255)
//   Behaviour: constant DMX 150 from start to end of composition.
//   Provides a steady warm-white anchor wash that the envelope-follower
//   Saber (ch7) and the Varytec (ch2) are choreographed against.
// ─────────────────────────────────────────────────────────────────────────────

#include "dmx_controller.h"

#define SABER2_CH  5   // ADJ Saber Spot WW Unit 2 — dimmer channel
#define W2(d)  { {SABER2_CH, (d)} }, 1

static const DMXCue SABER2_CUE_LIST[] = {

    { 0,  0,  0,  0,   0, W2(150) },   // 0:00:00 — on at DMX 150 (constant wash)
    { 0, 31, 38,  0,   0, W2(  0) },   // 31:38   — hard black at end

};  // END SABER2_CUE_LIST

#undef W2

static const uint16_t SABER2_CUE_COUNT =
    sizeof(SABER2_CUE_LIST) / sizeof(SABER2_CUE_LIST[0]);
