#include <Wire.h>

// I2Cピンの定義
const int SDA_PIN = 20;
const int SCL_PIN = 21;

// センサのI2Cアドレス
const int SENSOR_ADDR = 0x38;

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);  // RP2040のシリアル接続を待つ

  Serial.println("I2C Sensor Test (Address 0x38)");

  // I2Cの初期化
  Wire1.setSDA(SDA_PIN);
  Wire1.setSCL(SCL_PIN);
  Wire1.begin();

  // I2Cスキャンを実行
  Serial.println("Scanning I2C bus...");
  byte error, address;
  int nDevices = 0;
  for(address = 1; address < 127; address++) {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("I2C scan done");
  }

  // センサの初期化
  initSensor();
}

void loop() {
  float temperature, humidity;
  if (readSensor(temperature, humidity)) {
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity, 2);
    Serial.println(" %");
  } else {
    Serial.println("Failed to read sensor data");
  }
  
  delay(2000);  // 2秒待機
}

void initSensor() {
  Wire1.beginTransmission(SENSOR_ADDR);
  Wire1.write(0xBE);  // 初期化コマンド
  Wire1.endTransmission();
  delay(10);  // 初期化待機
}

bool readSensor(float &temperature, float &humidity) {
  uint8_t data[7];
  
  // 測定開始コマンドを送信
  Wire1.beginTransmission(SENSOR_ADDR);
  Wire1.write(0xAC);  // 測定トリガーコマンド
  Wire1.write(0x33);
  Wire1.write(0x00);
  Wire1.endTransmission();
  
  delay(80);  // 測定待機時間
  
  // データ読み取り
  Wire1.requestFrom(SENSOR_ADDR, 7);
  if (Wire1.available() != 7) {
    return false;
  }
  
  for (int i = 0; i < 7; i++) {
    data[i] = Wire1.read();
  }
  
  // データの解析
  humidity = ((data[1] << 12) | (data[2] << 4) | (data[3] >> 4)) * 100.0 / 1048576.0;
  temperature = ((((data[3] & 0xF) << 16) | (data[4] << 8) | data[5]) * 200.0 / 1048576.0) - 50.0;
  
  return true;
}