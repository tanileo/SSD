// #pragma once
#ifndef Clock_h
#define Clock_h

#include <Arduino.h>
#include <time.h>
#include <TimeLib.h> 
// #include "rtc_wrapper.h"
// #include <Timezone_Generic.h>

class Clock {
public:
    Clock();
    void init();
    void set();
    void update();
    inline int getHour() const{ return Hour; }
    inline int getMinute() const{ return Minute; }
    inline int getSetHour(int num) const{
      if(num == 1){
        return hour1;
      }else{
       return hour2;
      }
    }
    inline int getSetMinute(int num) const{
      if(num == 1){
        return minute1;
      }else{
       return minute2;
      }
    }
    void setSetHour(int index, int num);
    void setSetMinute(int index, int num);
    inline int getState(){ return state;}
    void setState(int num);

private:
    time_t t;
    int Hour, Minute;
    int hour2, hour1, minute2, minute1;
    int state;
};

#endif