#ifndef Speaker_h
#define Speaker_h

#include <Arduino.h>

class Speaker {
  public:
    Speaker();
    void init();
    void playTimerEndSound();
    void playTimerButtonSound();
    void playMaracasSound();
    void noiseMaracas();

  private:
    static const int SPEAKER_PIN = 16;
    void tone(unsigned int frequency, unsigned long duration);
};

#endif