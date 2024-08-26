#include "TemperatureHumidity.h"

TemperatureHumidity* TemperatureHumidity::instance = nullptr;

TemperatureHumidity::TemperatureHumidity(): temp(0), hum(0), state(0) {
  instance = this;
}

void TemperatureHumidity::init() {
  Serial.println("Temp init");

  Wire1.setSDA(SDA_PIN);
  Wire1.setSCL(SCL_PIN);
  Wire1.begin();

  Serial.println("Scanning I2C bus...");
  byte error, address;
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

  Wire1.beginTransmission(SENSOR_ADDR);
  Wire1.write(0xBE);  // 初期化コマンド
  Wire1.endTransmission();
  delay(10);  // 初期化待機

  static repeating_timer_t timer;
  add_repeating_timer_ms(INTERVAL, TemperatureHumidity::staticUpdate, NULL, &timer);
}

bool TemperatureHumidity::update(repeating_timer_t *rt) {
  uint8_t data[7];
  if(state == 1){
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
    hum = ((data[1] << 12) | (data[2] << 4) | (data[3] >> 4)) * 100.0 / 1048576.0;
    temp = ((((data[3] & 0xF) << 16) | (data[4] << 8) | data[5]) * 200.0 / 1048576.0) - 50.0;
    return true;
  }
  return true;
}

bool TemperatureHumidity::initUpdate() {
  uint8_t data[7];
  if(state == 1){
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
    hum = ((data[1] << 12) | (data[2] << 4) | (data[3] >> 4)) * 100.0 / 1048576.0;
    temp = ((((data[3] & 0xF) << 16) | (data[4] << 8) | data[5]) * 200.0 / 1048576.0) - 50.0;
    return true;
  }
  return true;
}

bool TemperatureHumidity::staticUpdate(repeating_timer_t *rt) {
    if (getInstance()) {
        return getInstance()->update(rt);
    }
    return false;
}