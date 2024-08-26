// #pragma once
#ifndef Slot_h
#define Slot_h

class Slot {
public:
    Slot();
    void spin();
    int getResult(int index) const {
        if (index >= 0 && index < 4) {
            return results[index];
        }
        return -1; // エラー値
    }
    int state;

private:
    static const int SLOT_COUNT = 4;
    int results[SLOT_COUNT];
    
    unsigned long currentTime;
    unsigned long lastUpdateTime;
};

#endif