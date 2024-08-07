/****************************************************************************************************************************
  RP2040_RTC_Time.ino

  For RP2040-based boards using internal RTC
  with WiFiNINA, ESP8266/ESP32 WiFi, ESP8266-AT, W5x00, ENC28J60, LAN8742A Ethernet modules/shields

  Built by Khoi Hoang https://github.com/khoih-prog/RP2040_RTC
  Licensed under MIT license
 *****************************************************************************************************************************/

// Important notes: Currently, RP2040-based boards RTC has no battery backup. So the time will be lost when power down
// Therefore, NTP client is necessary to get NTP time and update RTC.

#include "defines.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <Timezone_Generic.h>             // https://github.com/khoih-prog/Timezone_Generic

#include <TimeLib.h>
//////////////////////////////////////////

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
int seg_pin[7] = {12, 10, 13, 7, 5, 14, 11};
int nodePin = {9, 8, 4, 2};
int dpPin = 7;

const byte pattern[10] = {
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


//////////////////////////////////////////

// format and print a time_t value, with a time zone appended.
void printDateTime(time_t t, const char *tz)
{
  char buf[32];
  char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
          hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
  Serial.println(buf);
}

void AngleChange(){
  if(angle){
    angle = true;
  }else{
    angle = false;
  }
}

void ToggleChange(){
  if(toggle){
    toggle = true;
  }else{
    toggle = false;
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print(F("\nStart RP2040_RTC_Time on ")); Serial.println(BOARD_NAME);
  Serial.println(RP2040_RTC_VERSION);
  Serial.println(TIMEZONE_GENERIC_VERSION);

  myTZ = new Timezone(myDST, mySTD);

  // Start the RTC
  rtc_init();

  rtc_set_datetime(&currTime);
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

void clock(){
  rtc_get_datetime(&currTime);

  // Display time from RTC
  DateTime now = DateTime(currTime);

  time_t utc = now.get_time_t();
  time_t local = myTZ->toLocal(utc, &tcr);

  int hour = hour(local);
  int minute = minute(local);


}

void timer(){
  DateTime now = DateTime(currTime);

  time_t utc = now.get_time_t();
  time_t local = myTZ->toLocal(utc, &tcr);

  int hour = hour(local);
  int minute = minute(local);

}

void printNum4(int number){
  int i,j;
  int num[4]; 
  num[3] = number / 1000;
  num[2] = (number % 1000) / 100;
  num[1] = (number % 100) / 10;
  num[0] = number % 10;

  for(i=0; i<4; i++){
    digitalWrite(nodePin[i], 1)
    for(j=0; j<7; j++){
      digitalWrite(segPin[j], !bitRead(pattern[num[i]], j));
    }
    digitalWrite(nodePin[i], 0)
  }
}

void printNum(int num4, int num3, int num2, int num1){
  int i,j;
  int num[4]; 
  num[3] = num4;
  num[2] = num3;
  num[1] = num2;
  num[0] = num1;

  for(i=0; i<4; i++){
    digitalWrite(nodePin[i], 1)
    for(j=0; j<7; j++){
      digitalWrite(segPin[j], !bitRead(pattern[num[i]], j));
    }
    digitalWrite(nodePin[i], 0)
  }
}


void temp(){

}

void humidty(){

}

void slot(){

}

void tintiro(){

}

void loop()
{ 
  // displayRTCTime();
  if(mode == 0 ){
    clock();
  } else if(mode == 1){
    temp();
  } else if(mode == 2){
    slot();
  }else if(mode == 3){
    tintiro();
  }
}
