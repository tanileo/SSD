// #pragma once
#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <time.h>
#include <TimeLib.h> 

class Display {
public:
    Display();
    void init();
    void printNum4(int number, bool angle);
    void printNum(int num4, int num3, int num2, int num1, bool angle);
    void printTime(int hour, int minute, bool angle);
    void printClockSetting(int hour2, int hour1, int minute2, int minute1, bool angle, int state);
    void printTimer(int hour, int minute, bool angle, int state);
    void printFlashSetting(int difficult, int speed, bool angle, int state);
    void printFlashAnswer(int num4, int num3, int num2, int num1, bool angle, int state);
    void printO();
    void printX();
    void printPluseAndMainuse(int num, bool angle);
    void printTemp(int temp, int hum, bool angle);
    void printChinchiro(int num3, int num2, int num1, bool angle);
    void clear();

private:
    const int segPin[7] = {10, 8, 11, 5, 3, 12, 9};
    const int nodePin[4] = {7, 6, 2, 0};
    const int coronPin = 4;
    static constexpr byte patternTRUE[10] = {
        0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
        0b01101101, 0b01111101, 0b00100111, 0b01111111, 0b01101111
    };
    static constexpr byte patternFALSE[10] = {
        0b00111111, 0b00110000, 0b01011011, 0b01111001, 0b01110100,
        0b01101101, 0b01101111, 0b00111100, 0b01111111, 0b01111101
    };
    int lastTime;
    int now;
    bool light;
};

#endif