#pragma once

// Built-in fallback cue list (2-channel, single address).
// Used only when no CUES.CSV is found on the SD card.
// Compose your real cues live and export with the serial "e" command.
//
// Format: { HH, MM, SS, FF, fadeMs, { {channel, value}, ... }, numChannels }
//   fadeMs = 0  -> instant snap
//   fadeMs > 0  -> linear fade in milliseconds
//   Cues MUST be in ascending timecode order.

#include "dmx_controller.h"
#include "../include/config.h"

static const DMXCue CUE_LIST[] = {

    // 00:00:00:00  Blackout at loop start (snap)
    { 0, 0, 0, 0,
      /*fade*/ 0,
      { {DMX_CH_INTENSITY, 0}, {DMX_CH_STROBE, 0} },
      2 },

    // 00:00:10:00  Slow fade in to 50%
    { 0, 0, 10, 0,
      /*fade*/ 3000,
      { {DMX_CH_INTENSITY, 128}, {DMX_CH_STROBE, 0} },
      2 },

    // 00:31:50:00  Slow fade to blackout before loop point (9 s)
    { 0, 31, 50, 0,
      /*fade*/ 9000,
      { {DMX_CH_INTENSITY, 0}, {DMX_CH_STROBE, 0} },
      2 },

};  // END CUE_LIST

static const uint16_t CUE_COUNT = sizeof(CUE_LIST) / sizeof(CUE_LIST[0]);
