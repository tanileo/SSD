#include "Clock.h"

// Clock::Clock() : hour(0), minute(0), tcr(nullptr), myTZ(myDST, mySTD) {}
Clock::Clock() : Hour(0), Minute(0), hour2(0), hour1(0), minute2(0), minute1(0), state(0) {}

void Clock::init(){
  setTime(0, 0, 0, 14, 9, 2024);
}

void Clock::set(){
  int setHour, setMinute;
  setHour = hour2*10 + hour1;
  setMinute = minute2*10 + minute1;
  setTime(setHour, setMinute, 0, 14, 9, 2024);
}

void Clock::setSetHour(int index, int num) {
    if(index == 1) {
        hour1 = hour1 + num;
        if(hour1 == 10) {
            hour1 = 0;
        } else if(hour1 == -1) {
            hour1 = 9;
        }
    } else {
        hour2 = hour2 + num;
        if(hour2 == 3) {
            hour2 = 0;
        } else if(hour2 == -1) {
            hour2 = 2;
        }
    }
}

void Clock::setSetMinute(int index, int num) {
    if(index == 1) {
        minute1 = minute1 + num;
        if(minute1 == 10) {
            minute1 = 0;
        } else if(minute1 == -1) {
            minute1 = 9;
        }
    } else {
        minute2 = minute2 + num;
        if(minute2 == 6) {
            minute2 = 0;
        } else if(minute2 == -1) {
            minute2 = 5;
        }
    }
}

void Clock::update() {
  Serial.println("Clock update");
  t = now();
  Serial.println(t);
  Hour = hour(t);
  Minute = minute(t);
}

void Clock::setState(int num){
  state = num;
  return;
}
