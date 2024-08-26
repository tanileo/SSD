// #pragma once
#ifndef SmartClock_h
#define SmartClock_h

#include "Display.h"
#include "TemperatureHumidity.h"
#include "Clock.h"
#include "Timer.h"
#include "Chinchiro.h"
#include "Slot.h"
#include "FlashCalculation.h"
#include "Speaker.h"
#include "Music.h"
#include <Arduino.h>
#include <math.h>
#include "pico/stdlib.h"
#include <time.h>
#include <TimeLib.h> 


enum class Mode {
    CLOCK,
    TEMPERATURE,
    TIMER,
    CHINCHIRO,
    SLOT,
    FLASH_CALCULATION
};

class SmartClock {
public:
    SmartClock();
    void init();
    void selectInit();
    void run();
    static void staticChangeMode();
    static void staticTS1_Push();
    static void staticTS2_Push();
    static void staticTS3_Push();
    static bool staticReadAngle(repeating_timer_t *rt);
    static SmartClock* getInstance() {
        return instance;
    }
    void timerRun();
    void chinchiroRun();
    void flashRun();
    void slotRun();
    void tempRun();
    void clockRun();

    Mode mode1;
    Mode mode2;
    Mode mode3;
    Mode mode4;

private:
    Display display;
    TemperatureHumidity tempHumid;
    Clock clock;
    Timer timer;
    Chinchiro chinchiro;
    Slot slot;
    FlashCalculation flashCalc;
    Speaker speaker;
    Music music;

    Mode currentMode;
    Mode pastMode;
    const int TGS = 17;
    const int TS1 = 13;
    const int TS2 = 14;
    const int TS3 = 15;
    const int X_PIN = 27;
    const int Y_PIN = 28;
    const int Z_PIN = 29;
    bool toggle;
    int chattaringTime;
    int now;

    bool angle;
    bool pastAngle;


    const int INTERVAL = 1000;


    static SmartClock* instance;

    void handleButtons();
    void updateDisplay();
    void changeMode();
    void TS1_Push();
    void TS2_Push();
    void TS3_Push();
    bool ReadAngleImpl(repeating_timer_t *rt);

};

#endif