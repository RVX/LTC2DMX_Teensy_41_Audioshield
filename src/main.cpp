#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TeensyDMX.h>

#include "ltc_decoder.h"
#include "dmx_controller.h"
#include "cues.h"
#include "../include/config.h"
#ifdef ENABLE_DISPLAY
#include "../include/display.h"
#endif

namespace teensydmx = ::qindesign::teensydmx;

// -----------------------------------------------------------------------------
// Audio graph
// -----------------------------------------------------------------------------

AudioInputI2S          audioIn;
AudioRecordQueue       recordQueue;
AudioSynthWaveformSine testTone;
AudioMixer4            mixerL, mixerR;
AudioOutputI2S         audioOut;
AudioControlSGTL5000   sgtl5000;
AudioAnalyzePeak       peakL;

AudioConnection patchIn_Q(audioIn,  0, recordQueue, 0);
AudioConnection patchPk_L(audioIn,  0, peakL,       0);
AudioConnection patchIn_L(audioIn,  0, mixerL,      0);
AudioConnection patchIn_R(audioIn,  1, mixerR,      0);
AudioConnection patchTn_L(testTone, 0, mixerL,      1);
AudioConnection patchTn_R(testTone, 0, mixerR,      1);
AudioConnection patchMx_L(mixerL,   0, audioOut,    0);
AudioConnection patchMx_R(mixerR,   0, audioOut,    1);

// -----------------------------------------------------------------------------
// DMX + LTC
// -----------------------------------------------------------------------------

teensydmx::Sender dmxSender{DMX_SERIAL_PORT};
LTCDecoder        ltcDecoder;
DMXController     dmxCtrl{dmxSender, CUE_LIST, CUE_COUNT, LTC_FRAMERATE};
static bool       s_codecFailed = false;
#ifdef ENABLE_DISPLAY
LTCDisplay        ltcDisplay;
#endif

// -----------------------------------------------------------------------------
// SD cue buffer + live composition state
// -----------------------------------------------------------------------------

static DMXCue   sdCues[256];
static uint16_t sdCueCount = 0;

static uint8_t  liveI    = 0;      // live intensity output (0-255)
static uint8_t  liveS    = 0;      // live strobe    output (0-255)
static bool     liveMode = true;   // true = manual serial control
static bool     tcMute   = true;   // true = suppress TC printing (default ON)

static struct { uint8_t h,m,s,f; bool valid; } lastTC = {0,0,0,0,false};

struct RecMark { uint8_t h,m,s,f, intensity, strobe; uint16_t fadeMs; };
static RecMark  recBuf[512];
static uint16_t recCount = 0;

// -----------------------------------------------------------------------------
// Non-blocking DMX test state machine (driven by loop())
// Avoids blocking delay() calls that kill USB serial on Teensy 4.1.
// -----------------------------------------------------------------------------
static struct {
    uint8_t  step;      // 0 = idle
    uint32_t stepStart; // millis() when current step began
} dmxTest = {0, 0};

static void dmxTestStart() {
    tcMute = true;
    Serial.println("=== DMX TEST \u2014 cycling channels ===");
    Serial.println("Wiring: Teensy TX1 (pin1) -> module TXD");
    Serial.println("        module A -> XLR pin 3 (Data+)");
    Serial.println("        module B -> XLR pin 2 (Data-)");
    Serial.println("        module GND -> XLR pin 1 (GND)");
    Serial.println();
    for (uint16_t ch = 1; ch <= 7; ch++) dmxSender.set(ch, 0);
    dmxTest.step = 1;
    dmxTest.stepStart = millis();
}

static void dmxTestTick() {
    if (dmxTest.step == 0) return;
    uint32_t e = millis() - dmxTest.stepStart;
    switch (dmxTest.step) {
    case 1: // all-off 300ms
        if (e >= 300) {
            Serial.println("[1] White ramp");
            dmxSender.set(DMX_CH_MASTER, 255);
            dmxSender.set(DMX_CH_MODE,   0);
            dmxSender.set(DMX_CH_SPEED,  0);
            dmxSender.set(DMX_CH_STROBE, 0);
            dmxSender.set(DMX_CH_RED,    0);
            dmxSender.set(DMX_CH_GREEN,  0);
            dmxSender.set(DMX_CH_BLUE,   0);
            dmxTest.step = 2; dmxTest.stepStart = millis();
        }
        break;
    case 2: { // white ramp ~170ms
        uint8_t v = (uint8_t)((e / 10) * 15); if (v > 255) v = 255;
        dmxSender.set(DMX_CH_RED, v); dmxSender.set(DMX_CH_GREEN, v); dmxSender.set(DMX_CH_BLUE, v);
        if (e >= 170) {
            dmxSender.set(DMX_CH_RED, 255); dmxSender.set(DMX_CH_GREEN, 255); dmxSender.set(DMX_CH_BLUE, 255);
            dmxTest.step = 3; dmxTest.stepStart = millis();
        }
        break;
    }
    case 3: // hold white 800ms
        if (e >= 800) {
            Serial.println("[2] RED");
            dmxSender.set(DMX_CH_GREEN, 0); dmxSender.set(DMX_CH_BLUE, 0);
            dmxTest.step = 4; dmxTest.stepStart = millis();
        }
        break;
    case 4: // hold red 700ms
        if (e >= 700) {
            Serial.println("[3] GREEN");
            dmxSender.set(DMX_CH_RED, 0); dmxSender.set(DMX_CH_GREEN, 255);
            dmxTest.step = 5; dmxTest.stepStart = millis();
        }
        break;
    case 5: // hold green 700ms
        if (e >= 700) {
            Serial.println("[4] BLUE");
            dmxSender.set(DMX_CH_GREEN, 0); dmxSender.set(DMX_CH_BLUE, 255);
            dmxTest.step = 6; dmxTest.stepStart = millis();
        }
        break;
    case 6: // hold blue 700ms
        if (e >= 700) {
            Serial.println("[5] WHITE+STROBE");
            dmxSender.set(DMX_CH_RED, 255); dmxSender.set(DMX_CH_GREEN, 255);
            dmxSender.set(DMX_CH_STROBE, 128);
            dmxTest.step = 7; dmxTest.stepStart = millis();
        }
        break;
    case 7: // strobe 1000ms
        if (e >= 1000) {
            dmxSender.set(DMX_CH_STROBE, 0);
            Serial.println("[6] Blackout");
            for (uint16_t ch = 1; ch <= 7; ch++) dmxSender.set(ch, 0);
            dmxTest.step = 8; dmxTest.stepStart = millis();
        }
        break;
    case 8: // blackout 300ms then done
        if (e >= 300) {
            Serial.println("=== DMX TEST DONE ===");
            Serial.println("If NO light: check wiring, DMX address (d001), XLR polarity");
            Serial.println("If light but wrong color: check channel mode (7-ch)");
            while (recordQueue.available()) recordQueue.freeBuffer();
            ltcDecoder.resync();
            tcMute = false;
            dmxTest.step = 0;
        }
        break;
    }
}

// -----------------------------------------------------------------------------

static uint8_t clamp8(int v) { return (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v); }

static void applyLive() {
    dmxSender.set(DMX_CH_MASTER,  255);      // master always full
    dmxSender.set(DMX_CH_RED,     liveI);    // R = G = B for white
    dmxSender.set(DMX_CH_GREEN,   liveI);
    dmxSender.set(DMX_CH_BLUE,    liveI);
    dmxSender.set(DMX_CH_STROBE,  liveS);    // 0-3 = off, 4-255 = strobe
    dmxSender.set(DMX_CH_MODE,    0);        // manual RGB mode
    dmxSender.set(DMX_CH_SPEED,   0);
}

static int markCmp(const void* a, const void* b) {
    auto* ra = (const RecMark*)a;
    auto* rb = (const RecMark*)b;
    uint32_t ta = ((uint32_t)ra->h*3600+ra->m*60+ra->s)*LTC_FRAMERATE+ra->f;
    uint32_t tb = ((uint32_t)rb->h*3600+rb->m*60+rb->s)*LTC_FRAMERATE+rb->f;
    return ta > tb ? 1 : ta < tb ? -1 : 0;
}

static void cmdExport() {
    qsort(recBuf, recCount, sizeof(RecMark), markCmp);
    Serial.println();
    Serial.println("# ===== copy below into /CUES.CSV on the SD card =====");
    Serial.println("# HH:MM:SS:FF, fadeMs, ch:val, ch:val, ...");
    bool hasStart = recCount > 0 && recBuf[0].h == 0 && recBuf[0].m == 0
                    && recBuf[0].s == 0 && recBuf[0].f == 0;
    if (!hasStart)
        Serial.printf("00:00:00:00, 0, %u:255, %u:0, %u:0, %u:0, %u:0, %u:0, %u:0\n",
                      DMX_CH_MASTER, DMX_CH_RED, DMX_CH_GREEN, DMX_CH_BLUE,
                      DMX_CH_STROBE, DMX_CH_MODE, DMX_CH_SPEED);
    for (uint16_t i = 0; i < recCount; i++) {
        const RecMark& r = recBuf[i];
        Serial.printf("%02u:%02u:%02u:%02u, %u, %u:255, %u:%u, %u:%u, %u:%u, %u:%u, %u:0, %u:0\n",
                      r.h, r.m, r.s, r.f, r.fadeMs,
                      DMX_CH_MASTER,
                      DMX_CH_RED,    r.intensity,
                      DMX_CH_GREEN,  r.intensity,
                      DMX_CH_BLUE,   r.intensity,
                      DMX_CH_STROBE, r.strobe,
                      DMX_CH_MODE,   DMX_CH_SPEED);
    }
    Serial.println("# ===== end CUES.CSV =====");
    Serial.println();
}

static void processCmd(const char* cmd) {
    if (!cmd[0]) return;
    char c = cmd[0];  const char* a = cmd+1;  while (*a==' ') a++;
    if      (c=='+') { liveI=clamp8(liveI+10); applyLive(); Serial.printf("I=%u\n",liveI); }
    else if (c=='-') { liveI=clamp8(liveI-10); applyLive(); Serial.printf("I=%u\n",liveI); }
    else if (c==']') { liveI=clamp8(liveI+ 1); applyLive(); Serial.printf("I=%u\n",liveI); }
    else if (c=='[') { liveI=clamp8(liveI- 1); applyLive(); Serial.printf("I=%u\n",liveI); }
    else if (c=='.') { liveS=clamp8(liveS+10); applyLive(); Serial.printf("S=%u\n",liveS); }
    else if (c==',') { liveS=clamp8(liveS-10); applyLive(); Serial.printf("S=%u\n",liveS); }
    else if (c=='i'||c=='I') { liveI=clamp8(atoi(a)); applyLive(); Serial.printf("I=%u\n",liveI); }
    else if (c=='s'||c=='S') { liveS=clamp8(atoi(a)); applyLive(); Serial.printf("S=%u\n",liveS); }
    else if (c=='f'||c=='F') { liveI=255; liveS=0; applyLive(); Serial.println("Full"); }
    else if (c=='b'||c=='B') { liveI=  0; liveS=0; applyLive(); Serial.println("Blackout"); }
    else if (c=='m'||c=='M') {
        if (!lastTC.valid) { Serial.println("No LTC -- play video first"); return; }
        if (recCount>=512)  { Serial.println("Buffer full"); return; }
        RecMark& r=recBuf[recCount++];
        r.h=lastTC.h; r.m=lastTC.m; r.s=lastTC.s; r.f=lastTC.f;
        r.intensity=liveI; r.strobe=liveS;
        r.fadeMs=(*a)?(uint16_t)atoi(a):0;
        Serial.printf("Mark %u: %02u:%02u:%02u:%02u  I=%u S=%u fade=%ums\n",
                      recCount,r.h,r.m,r.s,r.f,r.intensity,r.strobe,r.fadeMs);
    }
    else if (c=='u'||c=='U') {
        if (recCount){recCount--;Serial.printf("Undo -- %u marks\n",recCount);}
        else Serial.println("Nothing to undo");
    }
    else if (c=='c'||c=='C') { recCount=0; Serial.println("Marks cleared"); }
    else if (c=='l'||c=='L') {
        Serial.printf("%u marks:\n",recCount);
        for (uint16_t i=0;i<recCount;i++) {
            const RecMark& r=recBuf[i];
            Serial.printf("  [%3u] %02u:%02u:%02u:%02u  I=%3u S=%3u  fade=%ums\n",
                          i+1,r.h,r.m,r.s,r.f,r.intensity,r.strobe,r.fadeMs);
        }
    }
    else if (c=='e'||c=='E') { cmdExport(); }
    else if (c=='p'||c=='P') {
        liveMode=!liveMode;
        if (liveMode){Serial.println(">> LIVE MODE"); applyLive();}
        else{Serial.println(">> PLAY MODE -- LTC drives cues"); dmxCtrl.blackout();}
    }
    else if (c=='t'||c=='T') {
        if (dmxTest.step == 0) dmxTestStart();
        else Serial.println("DMX test already running");
    }
    else if (c=='?') {
        Serial.println("LIVE COMPOSE  (single keys instant, i/s/m need Enter)");
        Serial.println("  + / -    intensity +10/-10      ] / [  +1/-1");
        Serial.println("  . / ,    strobe +10/-10");
        Serial.println("  i NNN    set intensity 0-255     s NNN  set strobe");
        Serial.println("  f        full (I=255,S=0)        b      blackout");
        Serial.println("  m NNN    mark cue with NNN ms fade (needs Enter)");
        Serial.println("  u        undo last mark          c      clear all marks");
        Serial.println("  l        list marks              e      export as CUES.CSV");
        Serial.println("  p        toggle LIVE/PLAY mode   ?      help");
        Serial.println("  t        DMX test (cycle R/G/B)  q      mute/unmute TC");
    }
    else { Serial.println("? for help"); }
}

static bool parseTcStr(const char* s,uint8_t& h,uint8_t& m,uint8_t& sc,uint8_t& f) {
    if (strlen(s)<11) return false;
    h=(uint8_t)atoi(s); m=(uint8_t)atoi(s+3);
    sc=(uint8_t)atoi(s+6); f=(uint8_t)atoi(s+9);
    return true;
}

static void loadCuesFromSD() {
    Serial.print("SD: init ... ");
    if (!SD.begin(BUILTIN_SDCARD)) { Serial.println("no card"); return; }
    Serial.println("OK");
    File file = SD.open("/CUES.CSV");
    if (!file) { Serial.println("SD: CUES.CSV not found"); return; }
    sdCueCount = 0;
    char line[160];
    while (file.available() && sdCueCount < 256) {
        uint8_t len = 0;
        while (file.available() && len < 159) {
            char c2=(char)file.read();
            if (c2=='\n') break;
            if (c2!='\r') line[len++]=c2;
        }
        line[len]='\0';
        if (!len||line[0]=='#'||line[0]==';') continue;
        DMXCue& cue=sdCues[sdCueCount];
        memset(&cue,0,sizeof(DMXCue));
        char* p=line;
        if (!parseTcStr(p,cue.hours,cue.minutes,cue.seconds,cue.frames)) continue;
        p+=11; while(*p==' '||*p==',') p++;
        cue.fadeMs=(uint16_t)atoi(p);
        while(*p&&*p!=',') p++;
        if(*p==',') p++;
        while(*p&&cue.numChannels<DMX_MAX_CUE_CHANNELS) {
            while(*p==' ') p++;
            if(!*p) break;
            uint16_t ch=(uint16_t)atoi(p);
            while(*p&&*p!=':') p++;
            if(*p!=':') break;
            p++;
            uint8_t val=(uint8_t)atoi(p);
            if(ch>=1&&ch<=512) {
                cue.channels[cue.numChannels].channel=ch;
                cue.channels[cue.numChannels].value=val;
                cue.numChannels++;
            }
            while(*p&&*p!=',') p++;
            if(*p==',') p++;
        }
        sdCueCount++;
    }
    file.close();
    Serial.printf("SD: %u cues loaded\n",sdCueCount);
}

// -----------------------------------------------------------------------------

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && millis() < 3000) {}
    memset(&lastTC, 0, sizeof(lastTC));
    AudioMemory(AUDIO_MEMORY_BLOCKS);

    // -- Codec -----------------------------------------------------------------
    bool codecOK = sgtl5000.enable();
    s_codecFailed = !codecOK;
    delay(200);
    sgtl5000.inputSelect(AUDIO_INPUT_SOURCE);
    sgtl5000.lineInLevel(AUDIO_LINEIN_LEVEL);
    sgtl5000.volume(AUDIO_VOLUME);
    mixerL.gain(0, 1.0f);  mixerR.gain(0, 1.0f);
    mixerL.gain(1, 0.0f);  mixerR.gain(1, 0.0f);

    // -- SD card: try loading CUES.CSV -----------------------------------------
    loadCuesFromSD();
    if (sdCueCount > 0) {
        dmxCtrl.setCues(sdCues, sdCueCount);
        liveMode = false;
    }

    // -- DMX sender ------------------------------------------------------------
    dmxCtrl.begin();

    // -- Banner ----------------------------------------------------------------
    Serial.println("-----------------------------------------");
    Serial.println("  Teensy 4.1  LTC + DMX  Live Compose");
    Serial.println("-----------------------------------------");
    Serial.printf ("  Codec  : %s\n", codecOK ? "OK" : "FAILED");
    Serial.printf ("  FPS    : %u\n", LTC_FRAMERATE);
    Serial.printf ("  Cues   : %u  (%s)\n",
                   sdCueCount > 0 ? sdCueCount : CUE_COUNT,
                   sdCueCount > 0 ? "SD /CUES.CSV" : "built-in fallback");
    Serial.printf ("  Mode   : %s\n", liveMode ? "LIVE (compose)" : "PLAY (LTC)");
    Serial.println("  TC print: OFF (press q to toggle)");
    Serial.println("  Most keys instant (no Enter needed)");
    Serial.println("  i/s/m + number need Enter");
    Serial.println("-----------------------------------------");

#ifdef ENABLE_DISPLAY
    if (!ltcDisplay.begin()) {
        Serial.println("[!!] DISPLAY not found (0x3C)");
    } else {
        Serial.println("  OLED   : OK (0x3C)");
        ltcDisplay.pushMessage("LTC+DMX Live Compose");
        ltcDisplay.pushMessage(codecOK ? "Codec: OK" : "Codec: FAILED");
        ltcDisplay.update();
    }
#endif

    if (!codecOK) { recordQueue.begin(); return; }

    // -- Quick test tone (L then R, 500ms each) --------------------------------
    testTone.frequency(1000.0f);  testTone.amplitude(0.5f);
    mixerL.gain(1, 0.8f);  mixerR.gain(1, 0.0f);  delay(500);
    mixerL.gain(1, 0.0f);  mixerR.gain(1, 0.8f);  delay(500);
    testTone.amplitude(0.0f);
    mixerL.gain(1, 0.0f);  mixerR.gain(1, 0.0f);

    if (liveMode) applyLive();
    recordQueue.begin();
}

// -----------------------------------------------------------------------------
// Helper: timecode → absolute frame number (for skip / jump detection)
// Drop-frame correction per SMPTE 12M.
// -----------------------------------------------------------------------------

static uint32_t tcToAbsFrames(const LTCTimecode& tc)
{
    uint32_t abs = ((uint32_t)tc.hours * 3600u + tc.minutes * 60u + tc.seconds)
                   * LTC_FRAMERATE + tc.frames;
    if (tc.dropFrame) {
        uint32_t totalMinutes = (uint32_t)tc.hours * 60u + tc.minutes;
        abs -= 2u * (totalMinutes - totalMinutes / 10u);
    }
    return abs;
}

// -----------------------------------------------------------------------------

void loop()
{
    static bool     ltcPresent    = false;
    static bool     ltcEverSeen   = false;
    static uint32_t lastFrameMs   = 0;
    static uint32_t prevTCFrames  = 0;
    static bool     prevDropFrame = false;
#ifdef DEBUG_HEARTBEAT
    static uint32_t frameSkips = 0;
    static uint32_t tcJumps    = 0;
#endif

    uint32_t now = millis();

    // -- Codec failure: repeated warning, skip all audio processing -----------
    if (s_codecFailed) {
        static uint32_t lastCodecWarn = 0;
        if (now - lastCodecWarn >= 3000) {
            lastCodecWarn = now;
            Serial.println("[!!] CODEC FAILED - check shield / I2C");
#ifdef ENABLE_DISPLAY
            ltcDisplay.pushMessage("!! CODEC FAILED");
#endif
        }
#ifdef ENABLE_DISPLAY
        ltcDisplay.update();
#endif
        return;
    }

    // -- Audio blocks -> LTC decoder -------------------------------------------
    while (recordQueue.available() > 0) {
        const int16_t* block = recordQueue.readBuffer();
        ltcDecoder.processSamples(block, AUDIO_BLOCK_SAMPLES);
        recordQueue.freeBuffer();
    }

    // -- LTC frame decoded -----------------------------------------------------
    if (ltcDecoder.isFrameReady()) {
        const LTCTimecode& tc = ltcDecoder.getTimecode();
        if (tc.valid) {
            // Save for live-compose marks
            lastTC.h=tc.hours; lastTC.m=tc.minutes;
            lastTC.s=tc.seconds; lastTC.f=tc.frames;
            lastTC.valid = true;

            // ── Tier 1: acquired / recovered ─────────────────────────────────
            if (!ltcPresent) {
                Serial.printf(ltcEverSeen
                    ? "[OK] LTC recovered @ %02u:%02u:%02u%c%02u\n"
                    : "[OK] LTC acquired  @ %02u:%02u:%02u%c%02u\n",
                    tc.hours, tc.minutes, tc.seconds,
                    tc.dropFrame ? ';' : ':', tc.frames);
#ifdef ENABLE_DISPLAY
                {
                    char dmsg[22];
                    snprintf(dmsg, sizeof(dmsg), ltcEverSeen ? "LTC recovered" : "LTC acquired");
                    ltcDisplay.pushMessage(dmsg);
                }
#endif
                ltcPresent    = true;
                ltcEverSeen   = true;
                prevDropFrame = tc.dropFrame;
                prevTCFrames  = tcToAbsFrames(tc);
            } else {
                // ── Tier 1: drop-frame flag change ───────────────────────────
                if (tc.dropFrame != prevDropFrame) {
                    Serial.printf("[!!] Drop-frame changed: %s -> %s\n",
                        prevDropFrame ? "ON" : "OFF",
                        tc.dropFrame  ? "ON" : "OFF");
#ifdef ENABLE_DISPLAY
                    {
                        char dmsg[22];
                        snprintf(dmsg, sizeof(dmsg), "DF: %s->%s",
                            prevDropFrame ? "ON" : "OFF",
                            tc.dropFrame  ? "ON" : "OFF");
                        ltcDisplay.pushMessage(dmsg);
                    }
#endif
                    prevDropFrame = tc.dropFrame;
                }

                // ── Tier 1: frame skip / TC jump ─────────────────────────────
                uint32_t curAbs = tcToAbsFrames(tc);
                int32_t  diff   = (int32_t)curAbs - (int32_t)prevTCFrames;
                if (diff < 0 || diff > 10) {
                    Serial.printf("[!!] TC jump   -> %02u:%02u:%02u%c%02u\n",
                        tc.hours, tc.minutes, tc.seconds,
                        tc.dropFrame ? ';' : ':', tc.frames);
#ifdef ENABLE_DISPLAY
                    {
                        char dmsg[22];
                        snprintf(dmsg, sizeof(dmsg), "JUMP->%02u:%02u:%02u:%02u",
                            tc.hours % 24, tc.minutes % 60,
                            tc.seconds % 60, tc.frames % LTC_FRAMERATE);
                        ltcDisplay.pushMessage(dmsg);
                    }
#endif
#ifdef DEBUG_HEARTBEAT
                    tcJumps++;
#endif
                } else if (diff > 1) {
                    Serial.printf("[!!] Frame skip: +%d @ %02u:%02u:%02u%c%02u\n",
                        (int)(diff - 1),
                        tc.hours, tc.minutes, tc.seconds,
                        tc.dropFrame ? ';' : ':', tc.frames);
#ifdef ENABLE_DISPLAY
                    {
                        char dmsg[22];
                        snprintf(dmsg, sizeof(dmsg), "SKIP+%d %02u:%02u:%02u:%02u",
                            (int)(diff-1),
                            (unsigned)(tc.hours % 24),
                            (unsigned)(tc.minutes % 60),
                            (unsigned)(tc.seconds % 60),
                            (unsigned)(tc.frames % 100));
                        ltcDisplay.pushMessage(dmsg);
                    }
#endif
#ifdef DEBUG_HEARTBEAT
                    frameSkips++;
#endif
                }
                prevTCFrames = curAbs;
            }
            lastFrameMs = now;

            // ── Drive cue engine in PLAY mode ────────────────────────────────
            if (!liveMode)
                dmxCtrl.update(tc.hours, tc.minutes, tc.seconds, tc.frames);

            // ── Normal timecode print ────────────────────────────────────────
            if (!tcMute) {
                char buf[20];
                snprintf(buf, sizeof(buf), "%02u:%02u:%02u%c%02u",
                         tc.hours, tc.minutes, tc.seconds,
                         tc.dropFrame ? ';' : ':', tc.frames);
                Serial.println(buf);
            }
#ifdef ENABLE_DISPLAY
            ltcDisplay.setTimecode(tc.hours, tc.minutes, tc.seconds,
                                   tc.frames, tc.dropFrame, true);
#endif
        }
        ltcDecoder.reset();
    }

    // ── Tier 1: LTC lost ─────────────────────────────────────────────────────
    if (ltcPresent && (now - lastFrameMs) > LTC_LOSS_TIMEOUT_MS) {
        Serial.printf("[!!] LTC LOST (%.1fs silence)\n",
            (now - lastFrameMs) / 1000.0f);
        ltcPresent = false;
#ifdef ENABLE_DISPLAY
        ltcDisplay.setTimecode(0,0,0,0,false,false);
        ltcDisplay.pushMessage("!! LTC LOST");
#endif
    }

    // -- Fade tick (PLAY mode only) --------------------------------------------
    if (!liveMode) dmxCtrl.tick();

    // -- Audio level (read once; shared between heartbeat and display) ---------
    static float s_audioLevel = 0.0f;
    if (peakL.available()) s_audioLevel = peakL.read();

#ifdef DEBUG_HEARTBEAT
    // ── Tier 2: periodic stat line ───────────────────────────────────────────
    {
        static uint32_t lastStat = 0;
        if (now - lastStat >= HEARTBEAT_INTERVAL_MS) {
            lastStat = now;

            Serial.printf("[STAT] LTC:%-3s  lvl:%3.0f%%  skips:%-3u  jumps:%-3u  zcErr:%-4u  mem:%2u(pk:%2u)/%u  cpu:%.1f%%\n",
                ltcPresent ? "OK" : "---",
                s_audioLevel * 100.0f,
                frameSkips,
                tcJumps,
                ltcDecoder.getZcResets(),
                AudioMemoryUsage(),
                AudioMemoryUsageMax(),
                AUDIO_MEMORY_BLOCKS,
                AudioProcessorUsage());
            frameSkips = 0;
            tcJumps    = 0;
            ltcDecoder.resetZcResets();
        }
    }
#endif

#ifdef ENABLE_DISPLAY
    {
        ltcDisplay.setStatus(ltcPresent, s_audioLevel,
                             AudioMemoryUsage(), AudioMemoryUsageMax(),
                             AudioProcessorUsage(),
                             ltcDecoder.getZcResets());
        ltcDisplay.update();
    }
#endif

    // -- Non-blocking DMX test tick -------------------------------------------
    dmxTestTick();

    // -- Serial command input --------------------------------------------------
    {
        static char    cmdBuf[32];
        static uint8_t cmdLen = 0;
        while (Serial.available()) {
            char c = (char)Serial.read();
            // Instant single-key commands (no Enter needed)
            if (cmdLen == 0) {
                bool instant = true;
                switch (c) {
                    case 'q': case 'Q':
                        tcMute = !tcMute;
                        Serial.printf("TC print: %s\n", tcMute ? "OFF" : "ON");
                        break;
                    case 't': case 'T': case 'f': case 'F': case 'b': case 'B':
                    case 'p': case 'P': case 'e': case 'E': case 'u': case 'U':
                    case 'c': case 'C': case 'l': case 'L': case '?':
                    case '+': case '-': case ']': case '[': case '.': case ',':
                        { char tb[2] = {c,'\0'}; processCmd(tb); }
                        break;
                    default:
                        instant = false;
                        break;
                }
                if (instant) continue;
            }
            // Multi-char commands (i NNN, s NNN, m NNN) — accumulate until Enter
            if (c == '\n' || c == '\r') {
                if (cmdLen > 0) { cmdBuf[cmdLen]='\0'; processCmd(cmdBuf); cmdLen=0; }
            } else if (cmdLen < 31) {
                cmdBuf[cmdLen++] = c;
            }
        }
    }
}
