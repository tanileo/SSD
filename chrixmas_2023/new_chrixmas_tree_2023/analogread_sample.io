const int photoResistorPin = A0;  // フォトトランジスタのピン
const int ledPins[] = {3, 5, 6, 9, 10, 11};  // LEDのピン

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  int sensorValue = analogRead(photoResistorPin);  // フォトトランジスタの値を読み取る
  int brightness = map(sensorValue, 0, 1023, 0, 255);  // フォトトランジスタの値をLEDの明るさにマップ

  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" - Brightness: ");
  Serial.println(brightness);

  for (int i = 0; i < 6; i++) {
    analogWrite(ledPins[i], brightness);  // 各LEDの明るさを設定
  }

  delay(100);  // ウェイト（適切な値に調整してください）
}
