// #pragma once
#ifndef Chinchiro_h
#define Chinchiro_h

#include <time.h>
#include <TimeLib.h> 

class Chinchiro {
public:
    Chinchiro();
    void play();
    void stop();
    bool update();
    inline int getResult() const{return result;}
    inline int getState() const{return state;}
    int getDice(int index) const {
        if (index >= 0 && index < 3) {
            return dice[index];
        }
        return -1;  // または適切なエラー値
    }
    void setState(int num);
private:
    int rollDice();
    int dice[3] = {0, 0, 0};
    int state;
    int result;
    unsigned long currentTime;
    unsigned long lastUpdateTime;
    unsigned long stopTime;
};

#endif