const int photoResistorPin = A0;  // フォトトランジスタのピン
const int ledPins[] = {3, 5, 6, 9, 10, 11};  // LEDのピン
const int fadeInDelay = 30;  // フェードインの遅延時間
const int fadeOutDelay = 10;  // フェードアウトの遅延時間

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  int sensorValue = analogRead(photoResistorPin);  // フォトトランジスタの値を読み取る
  int brightness = map(sensorValue, 0, 1023, 0, 255);  // フォトトランジスタの値をLEDの最大値にマップ

  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" - Max Brightness: ");
  Serial.println(brightness);

  for (int i = 0; i < 6; i++) {
    fadeIn(ledPins[i], brightness);  // フェードイン
    delay(fadeInDelay);
  }

  for (int i = 5; i >= 0; i--) {
    fadeOut(ledPins[i]);  // フェードアウト
    delay(fadeOutDelay);
  }
}

void fadeIn(int pin, int maxBrightness) {
  for (int brightness = 0; brightness <= maxBrightness; brightness++) {
    analogWrite(pin, brightness);
    delay(10);  // 適切な値に調整してください
  }
}

void fadeOut(int pin) {
  for (int brightness = 255; brightness >= 0; brightness--) {
    analogWrite(pin, brightness);
    delay(10);  // 適切な値に調整してください
  }
}
