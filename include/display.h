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

#define DISP_MSG_LINES  3   // number of scrolling message rows

class LTCDisplay {
public:
    LTCDisplay() : _display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET) {}

    // Call once in setup() — returns false if display not found
    bool begin() {
        if (!_display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) return false;
        // Teensy 4.1 supports I2C Fast-mode Plus (1 MHz); Teensy 3.6 tops out
        // at 400 kHz reliably. The -D TEENSY36 flag in platformio.ini selects
        // the safe limit automatically — display refresh is unaffected either way.
#ifdef TEENSY36
        Wire.setClock(400000);
#else
        Wire.setClock(1000000);
#endif
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

    // ── Scrolling message log ─────────────────────────────────────────────────
    void pushMessage(const char* msg) {
        for (uint8_t i = 0; i < DISP_MSG_LINES - 1; ++i)
            memcpy(_msgBuf[i], _msgBuf[i + 1], sizeof(_msgBuf[0]));
        snprintf(_msgBuf[DISP_MSG_LINES - 1], sizeof(_msgBuf[0]), "%s", msg);
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

        // ── Row 1: status indicator + label + level bar
        _display.setTextSize(1);
        if (_ltcOK) {
            _blink = !_blink;
            if (_blink)
                _display.fillTriangle(0, 17, 0, 23, 6, 20, SSD1306_WHITE);
        } else {
            _display.fillRect(0, 17, 7, 7, SSD1306_WHITE);
        }
        _display.setCursor(10, 17);
        _display.print(_ltcOK ? "LTC:OK" : "LTC:--");

        // Level bar
        {
            uint8_t barW = (uint8_t)(_level * 74.0f);
            if (barW > 74) barW = 74;
            _display.drawRect(52, 18, 76, 5, SSD1306_WHITE);
            if (barW > 0)
                _display.fillRect(53, 19, barW, 3, SSD1306_WHITE);
        }

        // divider
        _display.drawFastHLine(0, 25, OLED_WIDTH, SSD1306_WHITE);

        // ── Rows 2-4: event log
        for (uint8_t i = 0; i < DISP_MSG_LINES; ++i) {
            _display.setCursor(0, 27 + i * 9);
            if (_msgBuf[i][0]) {
                _display.print('>');
                _display.print(_msgBuf[i]);
            }
        }

        // divider
        _display.drawFastHLine(0, 54, OLED_WIDTH, SSD1306_WHITE);

        // ── Row 5: stat line
        char stat[22];
        snprintf(stat, sizeof(stat), "m:%u/%u z:%lu c:%.0f%%",
                 _memUsed, _memMax, (unsigned long)_zcErr, _cpu);
        _display.setCursor(0, 56);
        _display.print(stat);

        _display.display();
    }

private:
    Adafruit_SSD1306 _display;

    uint8_t  _hh = 0, _mm = 0, _ss = 0, _ff = 0;
    bool     _dfFlag    = false;
    bool     _tcPresent = false;
    bool     _blink     = false;

    bool     _ltcOK   = false;
    float    _level   = 0.0f;
    uint8_t  _memUsed = 0;
    uint8_t  _memMax  = 0;
    float    _cpu     = 0.0f;
    uint32_t _zcErr   = 0;
    char     _msgBuf[DISP_MSG_LINES][22] = {};
    uint32_t _lastDraw = 0;
};
