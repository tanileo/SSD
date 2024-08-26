#include "Timer.h"

Timer::Timer() : remainingSeconds(0), lastUpdateTime(0), minutes(3), seconds(0), state(0), stop(false) {}

void Timer::start() {
    state = 2;
    remainingSeconds = minutes * 60 + seconds;
    lastUpdateTime = now();
}

int Timer::update() {
    if (remainingSeconds > 0 && !stop) {
        Serial.println("TIMER update");
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= 1000) {  // 1秒 = 1000ミリ秒
            remainingSeconds--;
            lastUpdateTime = currentTime;
            if(remainingSeconds == 0){
                state = 3;
            }
            return true;
        }
        return true;
    }
    return false;
}