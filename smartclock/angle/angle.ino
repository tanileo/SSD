#include "pico/stdlib.h"

#define INTERVAL (10)  //タイマ割り込みの時間間隔

#define X_PIN 27
#define Y_PIN 28
#define Z_PIN 29

int x = 0;  //X_PINの状態を保存
int y = 0;  //Y_PINの状態を保存
int z = 0;  //Z_PINの状態を保存

// bool ReadAngle(repeating_timer_t *rt)  //割り込み処理
bool ReadAngle()  
{
  x = analogRead(X_PIN);  //X_PINのアナログ電圧値をxに代入
  y = analogRead(Y_PIN);  //Y_PINのアナログ電圧値をyに代入
  z = analogRead(Z_PIN);  //Z_PINのアナログ電圧値をzに代入
  return (false);  //割り込みを処理
}


void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  //AD変換の分解能を12bitに設定

  // 50ms 毎に計測を行うための割り込みを発生させる。
  static repeating_timer_t timer;
  add_repeating_timer_ms(INTERVAL, &ReadAngle, NULL, &timer);

}

void loop() {
  ReadAngle();
  Serial.print("x: ");
  Serial.println(x);
  Serial.print("y: ");
  Serial.println(y);
  Serial.print("z: ");
  Serial.println(z);
  
}
