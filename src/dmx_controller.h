#pragma once

#include <cstdint>
#include <TeensyDMX.h>

// ─────────────────────────────────────────────────────────────────────────────
// Maximum number of channel/value pairs a single cue can set.
// 16 covers 8 fixtures × 2 channels each.
// ─────────────────────────────────────────────────────────────────────────────
#define DMX_MAX_CUE_CHANNELS  16

// ─────────────────────────────────────────────────────────────────────────────
// One channel/value pair inside a cue
// ─────────────────────────────────────────────────────────────────────────────
struct DMXCueChannel {
    uint16_t channel;   // 1-based DMX channel number
    uint8_t  value;     // 0-255
};

// ─────────────────────────────────────────────────────────────────────────────
// A single lighting cue:
//   - triggered when LTC timecode reaches HH:MM:SS:FF
//   - sets the listed channels (others are left unchanged)
//   - fadeMs > 0 interpolates from current value over that duration
// ─────────────────────────────────────────────────────────────────────────────
struct DMXCue {
    uint8_t        hours;
    uint8_t        minutes;
    uint8_t        seconds;
    uint8_t        frames;
    uint16_t       fadeMs;                          // 0 = instant snap
    DMXCueChannel  channels[DMX_MAX_CUE_CHANNELS];
    uint8_t        numChannels;
};

// ─────────────────────────────────────────────────────────────────────────────
// DMXController
//
// Usage:
//   1. Declare a teensydmx::Sender and pass it in.
//   2. Call begin() in setup().
//   3. Call update(h,m,s,f) every time a new LTC frame arrives.
//   4. Call tick() every loop() iteration to process fades.
// ─────────────────────────────────────────────────────────────────────────────
class DMXController {
public:
    DMXController(qindesign::teensydmx::Sender& sender,
                  const DMXCue* cues,
                  uint16_t      cueCount,
                  uint8_t       framerate);

    void begin();

    // Replace the cue list at runtime (e.g. after loading from SD card)
    void setCues(const DMXCue* cues, uint16_t cueCount);

    // Call with each decoded LTC timecode
    void update(uint8_t h, uint8_t m, uint8_t s, uint8_t f);

    // Call every loop() — processes fade interpolation
    void tick();

    // Force all channels to 0
    void blackout();

private:
    qindesign::teensydmx::Sender& _sender;
    const DMXCue* _cues;
    uint16_t      _cueCount;
    uint8_t       _framerate;

    int16_t  _lastCueIndex;       // index of last applied cue (-1 = none)
    uint32_t _lastTotalFrames;    // previous TC in frames (loop detection)

    // Live DMX state: index = channel (1-512), value = current output level
    uint8_t  _dmxState[513];

    // Per-channel fade state
    struct FadeState {
        uint8_t  startVal;
        uint8_t  targetVal;
        uint32_t startMs;
        uint32_t durationMs;
        bool     active;
    };
    FadeState _fades[513];        // indexed by DMX channel (1-based)

    // Convert HH:MM:SS:FF to total frame count
    uint32_t _toFrames(uint8_t h, uint8_t m, uint8_t s, uint8_t f) const;

    // Find the index of the last cue at or before totalFrames (-1 if none)
    int16_t _findCue(uint32_t totalFrames) const;

    // Apply a cue's channel values (with fade if fadeMs > 0)
    void _applyCue(const DMXCue& cue);

    // Write current _dmxState to the sender
    void _flushChannel(uint16_t channel);
};
