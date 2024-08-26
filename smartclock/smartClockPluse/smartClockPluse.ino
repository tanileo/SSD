#include "SmartClock.h"

SmartClock smartClock;

void setup() {
  Serial.begin(115200);
  smartClock.init();  //smartClockから初期化メゾッドを呼び出し
  /*mode一覧
    Mode::CLOCK
    Mode::TEMPERATURE
    Mode::TIMER
    Mode::CHINCHIRO
    Mode::SLOT
    Mode::FLASH_CALCULATION
  */
  smartClock.mode1 = Mode::CLOCK;
  smartClock.mode2 = Mode::TIMER;
  smartClock.mode3 = Mode::TEMPERATURE;
  smartClock.mode4 = Mode::CHINCHIRO;
}

void loop() {
  smartClock.run();  //smartClockからrunメゾッドを呼び出し
}