const int photoResistorPin = 28;  // フォトトランジスタのピン
const int ledPin = 2;  // LEDのピン

void setup() {
  pinMode(ledPin , OUTPUT);
}

void loop() {
  int sensorValue = analogRead(photoResistorPin);  // フォトトランジスタの値を読み取る
  int brightness = map(sensorValue, 0, 1023, 0, 255);  // フォトトランジスタの値をLEDの明るさにマップ

  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" - Brightness: ");
  Serial.println(brightness);

  analogWrite(ledPin , brightness);  // 各LEDの明るさを設定

}
