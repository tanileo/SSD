// #pragma once
#ifndef TemperatureHumidity_h
#define TemperatureHumidity_h

#include <Arduino.h>
#include <Wire.h>
#include "pico/stdlib.h"
#include <Arduino.h>

class TemperatureHumidity {
public:
    TemperatureHumidity();
    void init();
    bool update(repeating_timer_t *rt);
    bool initUpdate();
    static bool staticUpdate(repeating_timer_t *rt);
    inline int getTemp() const{return temp;}
    inline int getHum() const{return hum;}
    static TemperatureHumidity* getInstance() {
        return instance;
    }
    int state;

private:
    // int temp_num2, temp_num1, hum_num2, hum_num1;
    static TemperatureHumidity* instance;
    int temp, hum;
    int nDevices = 0;
    const int SDA_PIN = 20;
    const int SCL_PIN = 21;
    const int SENSOR_ADDR = 0x38;
    const int INTERVAL = 10000;
    bool readSensor();
};

#endif