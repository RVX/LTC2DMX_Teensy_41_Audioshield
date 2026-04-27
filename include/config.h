#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// Audio Shield (SGTL5000) Configuration
// ─────────────────────────────────────────────────────────────────────────────

// Input source: AUDIO_INPUT_LINEIN or AUDIO_INPUT_MIC
#define AUDIO_INPUT_SOURCE    AUDIO_INPUT_LINEIN

// Line-in level: 0 (3.12 Vpp) to 15 (0.24 Vpp), default 5 (~1.33 Vpp)
// Lowered to 0 for maximum sensitivity.
#define AUDIO_LINEIN_LEVEL    0

// Headphone / line-out volume: 0.0 – 1.0
#define AUDIO_VOLUME          0.5f

// Number of AudioMemory blocks (each block = AUDIO_BLOCK_SAMPLES * 2 bytes)
// Increase if you see audio glitches in the serial monitor.
#define AUDIO_MEMORY_BLOCKS   32

// ─────────────────────────────────────────────────────────────────────────────
// LTC Decoder Configuration
// ─────────────────────────────────────────────────────────────────────────────

// Expected LTC frame rate: 24 | 25 | 2997 (29.97 drop) | 30
// Override by defining LTC_FRAMERATE before including this file
// (e.g. in cues_albedo.h for 25fps, cues_control_burn.h for 30fps).
#ifndef LTC_FRAMERATE
#define LTC_FRAMERATE         25
#endif

// Nominal samples per LTC bit at the configured frame rate.
// 44117.64706f is the Teensy audio engine's exact sample rate (AUDIO_SAMPLE_RATE_EXACT).
// LTC frame = 80 bits; samples per frame = sample_rate / LTC_FRAMERATE
// samples per bit = samples per frame / 80
#define LTC_SAMPLES_PER_BIT   (44117.64706f / LTC_FRAMERATE / 80.0f)

// Zero-crossing detection threshold (0.0 – 1.0, relative to full-scale)
// Raise if noise triggers false crossings; lower if signal is weak.
#define LTC_ZC_THRESHOLD      0.001f

// ─────────────────────────────────────────────────────────────────────────────
// Debug / Serial
// ─────────────────────────────────────────────────────────────────────────────

#define SERIAL_BAUD_RATE      115200

// Uncomment to print raw decoded bits to Serial
// #define DEBUG_LTC_BITS

// Uncomment to print audio peak levels to Serial
// #define DEBUG_AUDIO_LEVEL

// ─────────────────────────────────────────────────────────────────────────────
// Runtime Diagnostics
// ─────────────────────────────────────────────────────────────────────────────

// Milliseconds of silence before reporting LTC LOST
#define LTC_LOSS_TIMEOUT_MS    1000

// Interval (ms) between [STAT] heartbeat lines
#define HEARTBEAT_INTERVAL_MS  5000

// Comment out to disable the periodic [STAT] heartbeat line
#define DEBUG_HEARTBEAT

// ─────────────────────────────────────────────────────────────────────────────
// SSD1306 Display
// ─────────────────────────────────────────────────────────────────────────────

// Display refresh interval — derived from LTC_FRAMERATE so one display update
// per LTC frame regardless of 24 / 25 / 29.97 / 30.
#define DISPLAY_REFRESH_MS  (1000 / LTC_FRAMERATE)

// Comment out to disable the OLED display entirely
#define ENABLE_DISPLAY

// ─────────────────────────────────────────────────────────────────────────────
// DMX Output Configuration
// ─────────────────────────────────────────────────────────────────────────────

// Serial port used for DMX output via MAX485.
// Teensy 4.1 pin 1 = TX1  → MAX485 DI
// Wire MAX485:  VCC=3.3V  GND=GND  DE=3.3V  RE=GND  A=XLR3  B=XLR2  GND=XLR1
#define DMX_SERIAL_PORT       Serial1

// Total number of DMX channels to transmit (universe size, max 512)
#define DMX_UNIVERSE_SIZE     2

// ── Fixture channel map ────────────────────────────────────────────────────
// Varytec LED Theater Spot 50 3200K — 2-channel DMX mode
// All 4 fixtures share the same DMX start address (address 1).
//
//   CH 1 = Master dimmer   ( 0 = off, 255 = full intensity )
//   CH 2 = Strobe          ( 0 = off, 4-255 = slow→fast )
//
// Set fixture to "2 CH" mode via its front-panel menu:
//   Menu → Mode → 2 CH → Enter
// Set DMX address to 001 on every fixture.
#define DMX_BASE_ADDR         1
#define DMX_CH_MASTER        (DMX_BASE_ADDR)
#define DMX_CH_STROBE        (DMX_BASE_ADDR + 1)

// Alias for live-compose intensity command
#define DMX_CH_INTENSITY      DMX_CH_MASTER

// Loop detection threshold (frames) — 5 seconds at the active frame rate
#define DMX_LOOP_JUMP_FRAMES  (LTC_FRAMERATE * 5)

// ── Optional deep LTC diagnostic ─────────────────────────────────────────
// Uncomment to print details of every frame that has valid sync but
// out-of-range BCD (invalid hours/minutes/seconds/frames value).
// This distinguishes bit-level dropout from BCD corruption at 25fps.
// WARNING: can produce many serial lines — use briefly for diagnostics only.
// #define DEBUG_LTC_FRAMES

