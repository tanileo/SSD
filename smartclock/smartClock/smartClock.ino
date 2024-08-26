#include "defines.h"
#include <RP2040_RTC.h>

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <Timezone_Generic.h>             // https://github.com/khoih-prog/Timezone_Generic

#include <TimeLib.h>
#include <Wire.h>
#include "pico/stdlib.h"
//////////////////////////////////////////

// I2Cピンの定義
const int SDA_PIN = 20;
const int SCL_PIN = 21;

// センサのI2Cアドレス
const int SENSOR_ADDR = 0x38;

//スピーカー
#define SPEAKER 16

//タクトスイッチ
#define TS1 13
#define TS2 14
#define TS3 15

//トグルスイッチ
#define TGS 17

//加速度センサのピン番号
#define X_PIN 27
#define Y_PIN 28
#define Z_PIN 29

//タイマ割り込みの時間間隔
#define INTERVAL_ANGLE (1000)
#define INTERVAL_TEMP (5000)

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240};    //Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};     //Standard time = UTC - 5 hours
Timezone *myTZ;

TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev

//////////////////////////////////////////

/*
typedef struct 
{
  int16_t year;     ///< 0..4095
  int8_t  month;    ///< 1..12, 1 is January
  int8_t  day;      ///< 1..28,29,30,31 depending on month
  int8_t  dotw;     ///< 0..6, 0 is Sunday
  int8_t  hour;     ///< 0..23
  int8_t  min;      ///< 0..59
  int8_t  sec;      ///< 0..59
} datetime_t; 
*/

// Example set at 05:00:00 Fri 21 Jan 2022 UTC or 00:00:00 Fri 21 Jan 2022 EST

datetime_t currTime = { 2024, 7, 6, 5, 5, 0, 0 };
bool angle = false;
bool toggle = false;

//pin設定
int segPin[7] = {10, 8, 11, 5, 3, 12, 9};
int nodePin[4] = {7, 6, 2, 0};
int dpPin = 7;
int mode = 0;

//加速度センサ
int x = 0;  //X_PINの状態を保存
int y = 0;  //Y_PINの状態を保存
int z = 0;  //Z_PINの状態を保存

//タイマ割り込み用

//温度・湿度保持用
int temp_num2 = 0;
int temp_num1 = 0;
int hum_num2 = 0;
int hum_num1 = 0;

//タイマ設定用
int settingHour = 0;
int settingMinute = 0;
int settingSecond= 0;

int secondNum = 0;
int minuteNum = 0;


const byte patternTRUE[10] = {
  B00111111, // 0
  B00000110, // 1
  B01011011, // 2
  B01001111, // 3
  B01100110, // 4
  B01101101, // 5
  B01111101, // 6
  B00100111, // 7
  B01111111, // 8
  B01101111  // 9
};

const byte patternFALSE[10] = {
  B00111111, // 0  GFEDCBA
  B00110000, // 1
  B01011011, // 2
  B01111001, // 3
  B01110100, // 4
  B01101101, // 5
  B01101111, // 6
  B00111100, // 7
  B01111111, // 8
  B01111101  // 9
};

//////////////////////////////////////////
//プロトタイプ宣言
void printDateTime(time_t t, const char *tz);
void displayTime(void);
void displayRTCTime(void);
void setRTCTime(int year, int month, int day, int dayOfWeek, int hour, int min, int sec);
void clock1(void);
void setTimer(void);

//7seg表示
void printNum4(int number);
void printNum(int num4, int num3, int num2, int num1);

//温度・湿度
bool temp(repeating_timer_t *rt);

//ゲーム
// void slot(void):
// void tintiro(void);

//加速度センサ
bool ReadAngle(repeating_timer_t *rt);

//入力
void AngleChange(void);
void ToggleChange(void);
void TS1_Push(void);
void TS2_Push(void);
void TS3_Push(void);

//センサ関係
void initSensor(void);
bool readSensor(float &temperature, float &humidity);


//////////////////////////////////////////

// format and print a time_t value, with a time zone appended.


void setup()
{
  Serial.begin(115200);

  analogReadResolution(12);  //AD変換の分解能を12bitに設定

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    Serial.println("Formatting LittleFS...");
    LittleFS.format();
    if(!LittleFS.begin()){
      Serial.println("LittleFS mount failed even after formatting");
      return;
    }
  }

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


  Serial.print(F("\nStart RP2040_RTC_Time on "));
   Serial.println(BOARD_NAME);
  Serial.println(RP2040_RTC_VERSION);
  Serial.println(TIMEZONE_GENERIC_VERSION);

  myTZ = new Timezone(myDST, mySTD);

  // Start the RTC
  rtc_init();

  rtc_set_datetime(&currTime);

  //7セグ　ピン設定
  int i;
  for(i = 0; i < 7; i++){
    pinMode(segPin[i], OUTPUT);
    digitalWrite(segPin[i], 1);
  }
  for(i = 0; i < 4; i++){
    pinMode(nodePin[i], OUTPUT);
    digitalWrite(nodePin[i], 0);
  }

  //スピーカー
  pinMode(SPEAKER, OUTPUT);
  // tone(SPEAKER, 5000, 10);

  //タクトスイッチ ピン設定　割り込み
  pinMode(TS1, INPUT_PULLUP);
  attachInterrupt(TS1, TS1_Push, FALLING);
  pinMode(TS2, INPUT_PULLUP);
  attachInterrupt(TS2, TS2_Push, FALLING);
  pinMode(TS3, INPUT_PULLUP);
  attachInterrupt(TS3, TS3_Push, FALLING);

  //トグルスイッチ
  pinMode(TGS, INPUT_PULLUP);

  // タイマ割り込み
  static repeating_timer_t timer1;
  add_repeating_timer_ms(INTERVAL_ANGLE, &ReadAngle, NULL, &timer1);
  static repeating_timer_t timer2;
  add_repeating_timer_ms(INTERVAL_TEMP, &temp, NULL, &timer2);

}

void loop()
{
  if(digitalRead(TGS)){
    if(angle){
      printNum(temp_num2, temp_num1, hum_num2, hum_num1);
    } else {
      printNum(1, 2, 3, 4);
    }
  } else {
    if(angle){
      printNum(2, 2, 2, 2);
    } else {
      printNum(5, 6, 7, 8);
    }
  }
  // printNum(1, 1, 1, 1);
}

void printDateTime(time_t t, const char *tz)
{
  char buf[32];
  char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
          hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
  // Serial.println(buf);
}

void displayTime()
{
  rtc_get_datetime(&currTime);

  // Display time from RTC
  DateTime now = DateTime(currTime);

  time_t utc = now.get_time_t();
  time_t local = myTZ->toLocal(utc, &tcr);

  printDateTime(utc, "UTC");
  printDateTime(local, tcr -> abbrev);
}

void displayRTCTime()
{
  static unsigned long displayRTCTime_timeout = 0;

#define DISPLAY_RTC_INTERVAL     10000L

  // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to display frequently.
  if ((millis() > displayRTCTime_timeout) || (displayRTCTime_timeout == 0))
  {
    Serial.println("============================");
    displayTime();

    displayRTCTime_timeout = millis() + DISPLAY_RTC_INTERVAL;
  }
}

void setRTCTime(int year, int month, int day, int dayOfWeek, int hour, int min, int sec){
  // rtc_set_Datetime()
}

void clock1(){
  rtc_get_datetime(&currTime);

  // Display time from RTC
  DateTime now = DateTime(currTime);

  time_t utc = now.get_time_t();
  time_t local = myTZ->toLocal(utc, &tcr);

  // int hour = hour(local);
  // int minute = minute(local);

}

// void setTimer(int minute, int second){
//   DateTime now = DateTime(currTime);

//   time_t utc = now.get_time_t();
//   time_t local = myTZ->toLocal(utc, &tcr);

//   int curHour = hour(local);
//   int curMinute = minute(local);
//   int curSecond = second(local);

//   settingSecond = curSecond + second;
//   settingMinute = curMinute + minute;
//   settingHour = curSecond;

//   if(settingSecond >= 60){
//     settingSecond = settingSecond - 60;
//     settingMinute = settingMinute + 1;

//     if(settingMinute >= 60){
//     settingMinute = settingMinute - 60;
//     settingHour = settingHour + 1;
//   }
//   }

//   if((curMinute + minute) >= 60){
//     settingMinute = settingMinute - 60;
//     settingHour = settingHour + 1;
//   }

// }

// bool timer(repeating_timer_t *rt){
//   if(settingNum = )
//   DateTime now = DateTime(currTime);

//   time_t utc = now.get_time_t();
//   time_t local = myTZ->toLocal(utc, &tcr);

//   int hour = hour(local);
//   int minute = minute(local);
//   int second = second(local);

//   secondNum = settingSecond - second;
//   minuteNum = ((settingHour - hour)*60 + settingMinute) - minute;

//   // printNum4(minutedNum*100+secondNum);
//   return true;

// }

void printNum4(int number){
  int i,j,k, flow;
  int num4 = number / 1000;
  int num3 = (number % 1000) / 100;
  int num2 = (number % 100) / 10;
  int num1 = number % 10;
  int num[4] = {num4, num3, num2, num1};

  for(i=0; i<4; i++){
    if(angle){
      flow = i;
    }else{
      flow = 3 - i;
    }
    digitalWrite(nodePin[i], 1);
    for(j=0; j<7; j++){
      if(angle){
        digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],
         j));
      }else{
        digitalWrite(segPin[j], !bitRead(patternFALSE[num[flow]], j));
      }
      
    }
    delay(1);
    for(k=0; k<7; k++){
      digitalWrite(segPin[k], 1);
    }
    digitalWrite(nodePin[i], 0);
  }
}

void printNum(int num4, int num3, int num2, int num1){
  int i,j,k, flow;
  int num[4] = {num4, num3, num2, num1}; 

  for(i=0; i<4; i++){
    if(angle){
      flow = i;
    }else{
      flow = 3 - i;
    }
    digitalWrite(nodePin[i], 1);
    for(j=0; j<7; j++){
      if(angle){
        digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]], j));
      }else{
        digitalWrite(segPin[j], !bitRead(patternFALSE[num[flow]], j));
      }
    }
    delay(1);
    for(k=0; k<7; k++){
      digitalWrite(segPin[k], 1);
    }
    digitalWrite(nodePin[i], 0);
  }
}


// void humidty(){

// }

// void slot(){

// }

// void tintiro(){

// }

//sensor

void initSensor() {
  Wire1.beginTransmission(SENSOR_ADDR);
  Wire1.write(0xBE);  // 初期化コマンド
  Wire1.endTransmission();
  delay(10);  // 初期化待機
}

void TS1_Push(){
  if(analogRead(TGS)){
    if(angle){
      
    }else{

    }
  }else{
    if(angle){

    }else{

    }
  }
}

void TS2_Push(){
  if(analogRead(TGS)){
    if(angle){
      
    }else{

    }
  }else{
    if(angle){

    }else{

    }
  }
}

void TS3_Push(){
  if(analogRead(TGS)){
    if(angle){
      
    }else{

    }
  }else{
    if(angle){

    }else{

    }
  }
}

bool temp(repeating_timer_t *rt){
  float temperature, humidity;
  if(!angle || !digitalRead(TGS)){
    return true;
  }
  // if (readSensor(temperature, humidity)) {
  //   Serial.print("Temperature: ");
  //   Serial.print(temperature, 2);
  //   Serial.print(" °C, Humidity: ");
  //   Serial.print(humidity, 2);
  //   Serial.println(" %");
  // } else {
  //   Serial.println("Failed to read sensor data");
  // }

  readSensor(temperature, humidity);

  temp_num2 = (int)temperature / 10;
  temp_num1 = (int)temperature % 10;
  hum_num2 = (int)humidity / 10;
  hum_num1 = (int)humidity % 10;

  return true;
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

bool ReadAngle(repeating_timer_t *rt){
  x = analogRead(X_PIN);  //X_PINのアナログ電圧値をxに代入
  // y = analogRead(Y_PIN);  //Y_PINのアナログ電圧値をyに代入
  z = analogRead(Z_PIN);  //Z_PINのアナログ電圧値をzに代入

  // angle1 = atan2(x - 2048, y - 2048)*RAD_TO_DEG;
  // angle2 = atan2(y - 2048, z - 2048)*RAD_TO_DEG;
  float Angle = atan2(z - 2048, x - 2048)*RAD_TO_DEG;

  if(Angle >= 90 || Angle <= -90){
    angle = true;
  }else{
    angle = false;
  }

  // Serial.print("angle1: ");
  // Serial.println(angle1);
  // Serial.print("angle2: ");
  // Serial.println(angle2);
  // Serial.print("angle: ");
  // Serial.println(angle);

  return (true);
}
