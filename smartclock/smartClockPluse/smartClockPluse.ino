#include "SmartClock.h"

SmartClock smartClock;

void setup() {
  Serial.begin(115200);
  smartClock.init();  //smartClockから初期化メゾッドを呼び出し
}

void loop() {
  smartClock.run();  //smartClockからrunメゾッドを呼び出し
}