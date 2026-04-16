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

AudioConnection patchIn_Q(audioIn,  0, recordQueue, 0);
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

// -----------------------------------------------------------------------------
// SD cue buffer + live composition state
// -----------------------------------------------------------------------------

static DMXCue   sdCues[256];
static uint16_t sdCueCount = 0;

static uint8_t  liveI    = 0;      // live intensity output (0-255)
static uint8_t  liveS    = 0;      // live strobe    output (0-255)
static bool     liveMode = true;   // true = manual serial control

static struct { uint8_t h,m,s,f; bool valid; } lastTC = {0,0,0,0,false};

struct RecMark { uint8_t h,m,s,f, intensity, strobe; uint16_t fadeMs; };
static RecMark  recBuf[512];
static uint16_t recCount = 0;

// -----------------------------------------------------------------------------

static uint8_t clamp8(int v) { return (uint8_t)(v < 0 ? 0 : v > 255 ? 255 : v); }

static void applyLive() {
    dmxSender.set(DMX_CH_INTENSITY, liveI);
    dmxSender.set(DMX_CH_STROBE,    liveS);
}

static int markCmp(const void* a, const void* b) {
    auto* ra = (const RecMark*)a;
    auto* rb = (const RecMark*)b;
    uint32_t ta = ((uint32_t)ra->h*3600+ra->m*60+ra->s)*LTC_FRAMERATE+ra->f;
    uint32_t tb = ((uint32_t)rb->h*3600+rb->m*60+rb->s)*LTC_FRAMERATE+rb->f;
    return ta > tb ? 1 : ta < tb ? -1 : 0;
}

static void cmdExport() {
    RecMark sorted[512];
    memcpy(sorted, recBuf, recCount * sizeof(RecMark));
    qsort(sorted, recCount, sizeof(RecMark), markCmp);
    Serial.println();
    Serial.println("# ===== copy below into /CUES.CSV on the SD card =====");
    Serial.println("# HH:MM:SS:FF, fadeMs, ch:val, ch:val");
    bool hasStart = recCount > 0 && sorted[0].h == 0 && sorted[0].m == 0
                    && sorted[0].s == 0 && sorted[0].f == 0;
    if (!hasStart)
        Serial.printf("00:00:00:00, 0, %u:0, %u:0\n", DMX_CH_INTENSITY, DMX_CH_STROBE);
    for (uint16_t i = 0; i < recCount; i++) {
        const RecMark& r = sorted[i];
        Serial.printf("%02u:%02u:%02u:%02u, %u, %u:%u, %u:%u\n",
                      r.h, r.m, r.s, r.f, r.fadeMs,
                      DMX_CH_INTENSITY, r.intensity,
                      DMX_CH_STROBE,    r.strobe);
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
    else if (c=='?') {
        Serial.println("LIVE COMPOSE  ( type command + Enter )");
        Serial.println("  + / -    intensity +10/-10      ] / [  +1/-1");
        Serial.println("  . / ,    strobe +10/-10");
        Serial.println("  i NNN    set intensity 0-255     s NNN  set strobe");
        Serial.println("  f        full (I=255,S=0)        b      blackout");
        Serial.println("  m        mark cue (snap)         m NNN  with NNN ms fade");
        Serial.println("  u        undo last mark          c      clear all marks");
        Serial.println("  l        list marks              e      export as CUES.CSV text");
        Serial.println("  p        toggle LIVE/PLAY mode   ?      help");
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
        while(*p&&*p!=',') p++; if(*p==',') p++;
        while(*p&&cue.numChannels<DMX_MAX_CUE_CHANNELS) {
            while(*p==' ') p++; if(!*p) break;
            uint16_t ch=(uint16_t)atoi(p);
            while(*p&&*p!=':') p++; if(*p!=':') break; p++;
            uint8_t val=(uint8_t)atoi(p);
            if(ch>=1&&ch<=512) {
                cue.channels[cue.numChannels].channel=ch;
                cue.channels[cue.numChannels].value=val;
                cue.numChannels++;
            }
            while(*p&&*p!=',') p++; if(*p==',') p++;
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
    Serial.println("  Type ? for commands");
    Serial.println("-----------------------------------------");

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

void loop()
{
    // -- Audio blocks -> LTC decoder -------------------------------------------
    while (recordQueue.available() > 0) {
        const int16_t* block = recordQueue.readBuffer();
        ltcDecoder.processSamples(block, AUDIO_BLOCK_SAMPLES);
        recordQueue.freeBuffer();
    }

    // -- LTC frame: save TC, drive cue engine in PLAY mode --------------------
    if (ltcDecoder.isFrameReady()) {
        const LTCTimecode& tc = ltcDecoder.getTimecode();
        if (tc.valid) {
            lastTC.h=tc.hours; lastTC.m=tc.minutes;
            lastTC.s=tc.seconds; lastTC.f=tc.frames;
            lastTC.valid = true;
            if (!liveMode)
                dmxCtrl.update(tc.hours, tc.minutes, tc.seconds, tc.frames);
            char buf[20];
            snprintf(buf, sizeof(buf), "%02u:%02u:%02u%c%02u",
                     tc.hours, tc.minutes, tc.seconds,
                     tc.dropFrame ? ';' : ':', tc.frames);
            Serial.println(buf);
        }
        ltcDecoder.reset();
    }

    // -- Fade tick (PLAY mode only) --------------------------------------------
    if (!liveMode) dmxCtrl.tick();

    // -- Serial command input --------------------------------------------------
    {
        static char    cmdBuf[32];
        static uint8_t cmdLen = 0;
        while (Serial.available()) {
            char c = (char)Serial.read();
            if (c == '\n' || c == '\r') {
                if (cmdLen > 0) { cmdBuf[cmdLen]='\0'; processCmd(cmdBuf); cmdLen=0; }
            } else if (cmdLen < 31) {
                cmdBuf[cmdLen++] = c;
            }
        }
    }
}
