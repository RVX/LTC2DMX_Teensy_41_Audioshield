#include "dmx_controller.h"
#include "../include/config.h"
#include <Arduino.h>
#include <cstring>

// ─────────────────────────────────────────────────────────────────────────────

DMXController::DMXController(qindesign::teensydmx::Sender& sender,
                             const DMXCue* cues,
                             uint16_t      cueCount,
                             uint8_t       framerate)
    : _sender(sender)
    , _cues(cues)
    , _cueCount(cueCount)
    , _framerate(framerate)
    , _lastCueIndex(-1)
    , _lastTotalFrames(0)
{
    memset(_dmxState, 0, sizeof(_dmxState));
    memset(_fades,    0, sizeof(_fades));
}

// ─────────────────────────────────────────────────────────────────────────────

void DMXController::begin()
{
    _sender.begin();
    blackout();
    Serial.println("  DMX sender started on Serial1 (pin 1 TX).");
}

// ─────────────────────────────────────────────────────────────────────────────

void DMXController::update(uint8_t h, uint8_t m, uint8_t s, uint8_t f)
{
    uint32_t totalFrames = _toFrames(h, m, s, f);

    // ── Loop detection ──────────────────────────────────────────────────────
    // If TC jumped backwards significantly, the video looped → reset cue index
    if (_lastCueIndex >= 0 &&
        totalFrames + DMX_LOOP_JUMP_FRAMES < _lastTotalFrames)
    {
        Serial.println("DMX: loop detected — resetting cue engine");
        _lastCueIndex = -1;
    }
    _lastTotalFrames = totalFrames;

    // ── Find and apply new cue ──────────────────────────────────────────────
    int16_t cueIdx = _findCue(totalFrames);

    if (cueIdx != _lastCueIndex && cueIdx >= 0) {
        const DMXCue& nextCue = _cues[cueIdx];

        // Capture current CH1 value BEFORE applying (for direction display)
        uint8_t fromDim = _dmxState[DMX_CH_MASTER];
        uint8_t toDim   = fromDim;
        for (uint8_t i = 0; i < nextCue.numChannels; ++i) {
            if (nextCue.channels[i].channel == DMX_CH_MASTER) {
                toDim = nextCue.channels[i].value;
                break;
            }
        }
        const char* dir = (toDim > fromDim + 5) ? "^UP  "
                        : (toDim + 5 < fromDim) ? "vDIP "
                        :                         "~hld ";
        Serial.printf("[CUE] %3d/%-3u  %02u:%02u:%02u  DIM:%3u->%-3u  %2us %s\n",
                      (int)(cueIdx + 1), (unsigned)_cueCount,
                      (unsigned)nextCue.hours, (unsigned)nextCue.minutes,
                      (unsigned)nextCue.seconds,
                      (unsigned)fromDim, (unsigned)toDim,
                      (unsigned)(nextCue.fadeMs / 1000),
                      dir);

        _lastCueIndex = cueIdx;
        _applyCue(nextCue);
        if (_cueCb) _cueCb((uint16_t)cueIdx, nextCue);
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void DMXController::tick()
{
    uint32_t now = millis();

    for (uint16_t ch = 1; ch <= DMX_UNIVERSE_SIZE; ++ch) {
        FadeState& fade = _fades[ch];
        if (!fade.active) continue;

        uint32_t elapsed = now - fade.startMs;

        if (elapsed >= fade.durationMs) {
            // Fade complete
            _dmxState[ch] = fade.targetVal;
            fade.active    = false;
        } else {
            // Interpolate linearly
            float t = (float)elapsed / (float)fade.durationMs;
            _dmxState[ch] = (uint8_t)(fade.startVal + t * (int16_t)(fade.targetVal - fade.startVal));
        }

        _flushChannel(ch);
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void DMXController::blackout()
{
    memset(_dmxState, 0, sizeof(_dmxState));
    memset(_fades,    0, sizeof(_fades));   // clears all active flags (bool → 0 = false)
    for (uint16_t ch = 1; ch <= DMX_UNIVERSE_SIZE; ++ch) {
        _sender.set(ch, 0);
    }
}

// ─────────────────────────────────────────────────────────────────────────────

uint32_t DMXController::_toFrames(uint8_t h, uint8_t m, uint8_t s, uint8_t f) const
{
    return ((uint32_t)h * 3600u +
            (uint32_t)m * 60u  +
            (uint32_t)s) * _framerate + f;
}

// ─────────────────────────────────────────────────────────────────────────────
// Linear scan (cue list is typically short — < 200 cues).
// Returns the index of the last cue whose timecode <= totalFrames, or -1.
// ─────────────────────────────────────────────────────────────────────────────

int16_t DMXController::_findCue(uint32_t totalFrames) const
{
    int16_t result = -1;
    for (uint16_t i = 0; i < _cueCount; ++i) {
        uint32_t cueFrames = _toFrames(_cues[i].hours,
                                        _cues[i].minutes,
                                        _cues[i].seconds,
                                        _cues[i].frames);
        if (cueFrames <= totalFrames) {
            result = (int16_t)i;
        } else {
            break;  // cues are sorted ascending — stop early
        }
    }
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────

void DMXController::_applyCue(const DMXCue& cue)
{
    uint32_t now = millis();

    for (uint8_t i = 0; i < cue.numChannels; ++i) {
        uint16_t ch  = cue.channels[i].channel;
        uint8_t  val = cue.channels[i].value;
        if (ch == 0 || ch > 512) continue;

        if (cue.fadeMs == 0) {
            // Snap: cancel any in-progress fade, apply immediately
            _fades[ch].active = false;
            _dmxState[ch]     = val;
            _flushChannel(ch);
        } else {
            // Start a new fade from current value
            FadeState& fade   = _fades[ch];
            fade.startVal     = _dmxState[ch];
            fade.targetVal    = val;
            fade.startMs      = now;
            fade.durationMs   = cue.fadeMs;
            fade.active       = true;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────

void DMXController::_flushChannel(uint16_t channel)
{
    _sender.set(channel, _dmxState[channel]);
}

// -----------------------------------------------------------------------------

void DMXController::setCues(const DMXCue* cues, uint16_t cueCount)
{
    _cues         = cues;
    _cueCount     = cueCount;
    _lastCueIndex = -1;   // force re-evaluation from the start
}
