// #pragma once
#ifndef Timer_h
#define Timer_h

#include <Arduino.h>
#include <time.h>
#include <TimeLib.h> 

class Timer {
public:
    Timer();
    void start();
    int update();
    int getMinute() const { return remainingSeconds / 60; }
    int getSecond() const { return remainingSeconds % 60; }
    int getMinutes() const { return minutes; }
    int getSeconds() const { return seconds; }

    int minutes;
    int seconds;
    int state;
    bool stop;

private:
    int remainingSeconds;
    unsigned long lastUpdateTime;
};

#endif