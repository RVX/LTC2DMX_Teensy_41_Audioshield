#pragma once

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "config.h"

// ─────────────────────────────────────────────────────────────────────────────
// SSD1306 128×64 display layout
//
//  Row 0  (y=0,  h=16)  Timecode — HH:MM:SS:FF size-2
//  Row 1  (y=17, h=7)   Status: blinking ■ + LTC:OK/LTC:-- + input level bar
//  ─ divider (y=25) ─
//  Rows 2-4 (y=27..45)  Event log — 3 most recent events, newest at bottom
//  ─ divider (y=54) ─
//  Row 5  (y=56, h=8)   Stat line: mem / zcErr / cpu
// ─────────────────────────────────────────────────────────────────────────────

#define OLED_WIDTH   128
#define OLED_HEIGHT   64
#define OLED_ADDR    0x3C
#define OLED_RESET   -1     // share Teensy reset

#define DISP_MSG_LINES  1   // number of scrolling message rows

class LTCDisplay {
public:
    LTCDisplay() : _display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET) {}

    // Call once in setup() — returns false if display not found
    bool begin() {
        if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) return false;
        // Bump I2C to 1 MHz (Fast-mode Plus) — Teensy 4.1 supports it;
        // display-only traffic from here on so no need to drop back.
        Wire.setClock(1000000);
        _display.clearDisplay();
        _display.setTextWrap(false);
        _display.display();
        return true;
    }

    // ── Timecode row ─────────────────────────────────────────────────────────
    void setTimecode(uint8_t hh, uint8_t mm, uint8_t ss, uint8_t ff,
                     bool dropFrame, bool present)
    {
        _tcPresent = present;
        _hh = hh; _mm = mm; _ss = ss; _ff = ff; _dfFlag = dropFrame;
    }

    // ── Status bar ────────────────────────────────────────────────────────────
    void setStatus(bool ltcOK, float level,
                   uint8_t memUsed, uint8_t memMax,
                   float cpu, uint32_t zcErr)
    {
        _ltcOK   = ltcOK;
        _level   = level;
        _memUsed = memUsed;
        _memMax  = memMax;
        _cpu     = cpu;
        _zcErr   = zcErr;
    }

    // ── Operation mode + DMX master level ────────────────────────────────────
    void setModeAndMaster(bool live, uint8_t master) {
        _liveMode  = live;
        _dmxMaster = master;
    }

    // ── Live DMX channel values (7 channels, index 0=CH1 .. 6=CH7) ─────────
    void setDMXChannels(const uint8_t* vals) {
        for (uint8_t i = 0; i < 7; ++i) _dmxCh[i] = vals[i];
    }

    // ── Cue position + next cue TC ─────────────────────────────────────────
    void setCueInfo(int16_t curIdx, uint16_t total,
                    uint8_t nh, uint8_t nm, uint8_t ns, uint8_t nf,
                    bool hasNext) {
        _curCueNum = (curIdx >= 0) ? (uint16_t)(curIdx + 1) : 0;
        _cueTot    = total;
        _nextH = nh; _nextM = nm; _nextS = ns; _nextF = nf;
        _hasNext   = hasNext;
    }

    // ── Scrolling message log ─────────────────────────────────────────────────
    void pushMessage(const char* msg) {
        for (uint8_t i = 0; i < DISP_MSG_LINES - 1; ++i)
            memcpy(_msgBuf[i], _msgBuf[i + 1], sizeof(_msgBuf[0]));
        strncpy(_msgBuf[DISP_MSG_LINES - 1], msg, sizeof(_msgBuf[0]) - 1);
        _msgBuf[DISP_MSG_LINES - 1][sizeof(_msgBuf[0]) - 1] = '\0';
    }

    // ── Render — call every loop() ────────────────────────────────────────────
    void update() {
        uint32_t now = millis();
        if (now - _lastDraw < DISPLAY_REFRESH_MS) return;
        _lastDraw = now;

        _display.clearDisplay();

        // ── Row 0: timecode at size 2
        _display.setTextColor(SSD1306_WHITE);
        _display.setTextSize(2);
        {
            if (_tcPresent) {
                char tc[13];
                snprintf(tc, sizeof(tc), "%02u:%02u:%02u%c%02u",
                         (unsigned)(_hh % 24), (unsigned)(_mm % 60),
                         (unsigned)(_ss % 60), _dfFlag ? ';' : ':',
                         (unsigned)(_ff % LTC_FRAMERATE));
                int16_t cx = 0;
                for (uint8_t i = 0; tc[i]; ++i) {
                    _display.setCursor(cx, 0);
                    _display.write((uint8_t)tc[i]);
                    cx += 11;
                }
            } else {
                // Full-width inverted bar — "NO LTC IN"
                _display.fillRect(0, 0, OLED_WIDTH, 16, SSD1306_WHITE);
                _display.setTextSize(2);
                _display.setTextColor(SSD1306_BLACK);
                _display.setCursor((OLED_WIDTH - 9 * 12) / 2, 1);
                _display.print("NO LTC IN");
                _display.setTextColor(SSD1306_WHITE);
            }
        }

        // ── Row 1: icon + LIVE/PLAY + LTC status + level bar
        _display.setTextSize(1);
        if (_ltcOK) {
            _blink = !_blink;
            if (_blink)
                _display.fillTriangle(0, 17, 0, 23, 6, 20, SSD1306_WHITE);
        } else {
            _display.fillRect(0, 17, 7, 7, SSD1306_WHITE);
        }
        // LIVE/PLAY label immediately after icon
        _display.setCursor(10, 17);
        _display.print(_liveMode ? "LIVE " : "PLAY ");
        // LTC status after mode label
        _display.setCursor(40, 17);
        _display.print(_ltcOK ? "LTC:OK" : "LTC:--");

        // Level bar (right portion of row 1)
        {
            uint8_t barW = (uint8_t)(_level * 46.0f);
            if (barW > 46) barW = 46;
            _display.drawRect(80, 18, 46, 5, SSD1306_WHITE);
            if (barW > 0)
                _display.fillRect(81, 19, barW, 3, SSD1306_WHITE);
        }

        // divider
        _display.drawFastHLine(0, 25, OLED_WIDTH, SSD1306_WHITE);

        // ── Row 2: latest event (1 line) — 3px gap after divider
        _display.setCursor(0, 29);
        if (_msgBuf[0][0]) {
            _display.print('>');
            _display.print(_msgBuf[0]);
        }

        // ── Row 3 (y=38): cue position + next cue TC (PLAY mode only)
        if (!_liveMode && _cueTot > 0) {
            char cueRow[32];  // worst-case: "C:256/256 N:23:59:59:29" + null
            if (_hasNext) {
                if (_curCueNum > 0)
                    snprintf(cueRow, sizeof(cueRow), "C:%u/%u N:%02u:%02u:%02u:%02u",
                             (unsigned)_curCueNum, (unsigned)_cueTot,
                             (unsigned)_nextH, (unsigned)_nextM,
                             (unsigned)_nextS, (unsigned)_nextF);
                else
                    snprintf(cueRow, sizeof(cueRow), "C:--/%u N:%02u:%02u:%02u:%02u",
                             (unsigned)_cueTot,
                             (unsigned)_nextH, (unsigned)_nextM,
                             (unsigned)_nextS, (unsigned)_nextF);
            } else {
                snprintf(cueRow, sizeof(cueRow), "C:%u/%u  END OF SHOW",
                         (unsigned)_curCueNum, (unsigned)_cueTot);
            }
            _display.setCursor(0, 38);
            _display.print(cueRow);
        }

        // divider before DMX rows
        _display.drawFastHLine(0, 46, OLED_WIDTH, SSD1306_WHITE);

        // ── DMX row A (y=48): Master + R + G + B in decimal
        {
            char dmxA[22];
            snprintf(dmxA, sizeof(dmxA), "DIM%-3u R%-3u G%-3u B%-3u",
                     (unsigned)_dmxCh[0], (unsigned)_dmxCh[1],
                     (unsigned)_dmxCh[2], (unsigned)_dmxCh[3]);
            _display.setCursor(0, 48);
            _display.print(dmxA);
        }

        // ── DMX row B (y=57): Strobe + Mode-ch + Speed in decimal
        {
            char dmxB[21];
            snprintf(dmxB, sizeof(dmxB), "STR%-3u MOD%-3u SPD%-3u",
                     (unsigned)_dmxCh[4], (unsigned)_dmxCh[5],
                     (unsigned)_dmxCh[6]);
            _display.setCursor(0, 57);
            _display.print(dmxB);
        }

        _display.display();
    }

private:
    Adafruit_SSD1306 _display;

    uint8_t  _hh = 0, _mm = 0, _ss = 0, _ff = 0;
    bool     _dfFlag    = false;
    bool     _tcPresent = false;
    bool     _blink     = false;

    bool     _ltcOK     = false;
    float    _level      = 0.0f;
    uint8_t  _memUsed    = 0;
    uint8_t  _memMax     = 0;
    float    _cpu        = 0.0f;
    uint32_t _zcErr      = 0;
    bool     _liveMode   = true;
    uint8_t  _dmxMaster  = 0;
    uint8_t  _dmxCh[7]   = {};   // CH1..CH7 current output values
    uint16_t _curCueNum  = 0;    // 1-based; 0 = no cue fired yet
    uint16_t _cueTot     = 0;
    uint8_t  _nextH = 0, _nextM = 0, _nextS = 0, _nextF = 0;
    bool     _hasNext    = false;
    char     _msgBuf[DISP_MSG_LINES][22] = {};
    uint32_t _lastDraw = 0;
};
