#include "Display.h"

Display::Display(): lastTime(0), light(true) {}


void Display::init() {
    Serial.println("Display init");
    int i;
    for(i=0; i<7; i++){
      pinMode(segPin[i], OUTPUT);
    }
    for(i=0; i<4; i++){
      pinMode(nodePin[i], OUTPUT);
    }
    pinMode(coronPin, OUTPUT);
    clear();
}

void Display::printChinchiro(int num3, int num2, int num1, bool angle){
  int i,j,k,l, flow;
  int num[3] = {num3, num2, num1}; 

  for(i=0; i<3; i++){
    if(angle){
      flow = i;
      l = i + 1;
    }else{
      flow = 2 - i;
      l = i;
    }
    digitalWrite(nodePin[l], 1);
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
    digitalWrite(nodePin[l], 0);
  }
}

void Display::printPluseAndMainuse(int number, bool angle){
  int i,j,k,l, flow, keta, isMainuse = 0;
  if(number < 0){
    number = number * -1;
    isMainuse = 1;
  }
  int num3 = (number % 1000) / 100;
  int num2 = (number % 100) / 10;
  int num1 = number % 10;
  int num[3] = {num3, num2, num1};

  if(num3 == 0 && num2 == 0 && num1 == 0){
    keta = 1;
  }else if(num3 == 0 && num2 == 0){
    keta = 1;
  }else if(num3 == 0){
    keta = 2;
  }else{
    keta = 3;
  }

  if(keta == 3){
    if(isMainuse == 1){
      if(angle){
        digitalWrite(nodePin[0], 1);
        digitalWrite(segPin[6], 0);
        delay(1);
        digitalWrite(nodePin[0], 0);
        digitalWrite(segPin[6], 1);
      }else{
        digitalWrite(nodePin[3], 1);
        digitalWrite(segPin[6], 0);
        delay(1);
        digitalWrite(nodePin[3], 0);
        digitalWrite(segPin[6], 1);
      }
    }
    for(i=0; i<3; i++){
      if(angle){
        flow = i;
        l = i + 1;
      }else{
        flow = 2 - i;
        l = i;
      }
      digitalWrite(nodePin[l], 1);
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
      digitalWrite(nodePin[l], 0);
    }
  }else if(keta == 2){
    if(isMainuse == 1){
      if(angle){
        digitalWrite(nodePin[1], 1);
        digitalWrite(segPin[6], 0);
        delay(1);
        digitalWrite(nodePin[1], 0);
        digitalWrite(segPin[6], 1);
      }else{
        digitalWrite(nodePin[2], 1);
        digitalWrite(segPin[6], 0);
        delay(1);
        digitalWrite(nodePin[2], 0);
        digitalWrite(segPin[6], 1);
      }
    }
    for(i=0; i<2; i++){
      if(angle){
        flow = i + 1;
        l = i + 2;
      }else{
        flow = 3 - i;
        l = i;
      }
      digitalWrite(nodePin[l], 1);
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
      digitalWrite(nodePin[l], 0);
    }
  }else if(keta == 1){
    if(isMainuse == 1){
      if(angle){
        digitalWrite(nodePin[2], 1);
        digitalWrite(segPin[6], 0);
        delay(1);
        digitalWrite(nodePin[2], 0);
        digitalWrite(segPin[6], 1);
      }else{
        digitalWrite(nodePin[1], 1);
        digitalWrite(segPin[6], 0);
        delay(1);
        digitalWrite(nodePin[1], 0);
        digitalWrite(segPin[6], 1);
      }
    }

    if(angle){
      l = 3;
    }else{
      l = 0;
    }
    digitalWrite(nodePin[l], 1);
    for(j=0; j<7; j++){
      if(angle){
        digitalWrite(segPin[j], !bitRead(patternTRUE[num[2]], j));
      }else{
        digitalWrite(segPin[j], !bitRead(patternFALSE[num[2]], j));
      }
    }
    delay(1);
    for(k=0; k<7; k++){
      digitalWrite(segPin[k], 1);
    }
    digitalWrite(nodePin[l], 0);
  }
}

void Display::printNum4(int number, bool angle) {
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

void Display::printNum(int num4, int num3, int num2, int num1, bool angle) {
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
void Display::printTemp(int temp, int hum, bool angle){
  int i,j,k, flow;
  int temp2 = temp / 10;
  int temp1 = temp % 10;
  int hum2 = hum / 10;
  int hum1 = hum % 10;
  int num[4] = {temp2, temp1, hum2, hum1};

  for(i=0; i<4; i++){
    if(angle){
      flow = i;
    }else{
      flow = 3 - i;
    }
    if(flow == 0 || flow == 2){
      if(num[flow] == 0){
        continue;
      }
    }
    digitalWrite(nodePin[i], 1);
    for(j=0; j<7; j++){
      if(angle){
        digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
  digitalWrite(coronPin, 1);
  digitalWrite(segPin[0], 0);
  digitalWrite(segPin[1], 0);
  delay(1);
  digitalWrite(coronPin, 0);
  digitalWrite(segPin[0], 1);
  digitalWrite(segPin[1], 1);
}

void Display::printTime(int hour, int minute, bool angle){
  int i,j,k, flow;
  int hour2 = hour / 10;
  int hour1 = hour % 10;
  int minute2 = minute / 10;
  int minute1 = minute % 10;
  int num[4] = {hour2, hour1, minute2, minute1};

  for(i=0; i<4; i++){
    if(angle){
      flow = i;
    }else{
      flow = 3 - i;
    }
    if(flow == 0){
      if(num[flow] == 0){
        continue;
      }
    }
    digitalWrite(nodePin[i], 1);
    for(j=0; j<7; j++){
      if(angle){
        digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
  digitalWrite(coronPin, 1);
  digitalWrite(segPin[0], 0);
  digitalWrite(segPin[1], 0);
  delay(1);
  digitalWrite(coronPin, 0);
  digitalWrite(segPin[0], 1);
  digitalWrite(segPin[1], 1);
}

void Display::printClockSetting(int hour2, int hour1, int minute2, int minute1, bool angle, int state){
  int i,j,k, flow;
  int num[4] = {hour2, hour1, minute2, minute1};

  now = millis();
  if(now - lastTime >= 500){
    lastTime = millis();
    light = !light;
  }

  if(light){
    for(i=0; i<4; i++){
      if(angle){
        flow = i;
      }else{
        flow = 3 - i;
      }
      digitalWrite(nodePin[i], 1);
      for(j=0; j<7; j++){
        if(angle){
          digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
  }else{
    if(state == 0){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 0){
          continue;
        }
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    }else if(state == 1){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 1){
          continue;
        }
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    }else if(state == 2){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 2){
          continue;
        }
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    }else if(state == 3){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 3){
          continue;
        }
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
  }
  digitalWrite(coronPin, 1);
  digitalWrite(segPin[0], 0);
  digitalWrite(segPin[1], 0);
  delay(1);
  digitalWrite(coronPin, 0);
  digitalWrite(segPin[0], 1);
  digitalWrite(segPin[1], 1);
}



void Display::printTimer(int hour, int minute, bool angle, int state){
  int i,j,k, flow;
  int hour2 = hour / 10;
  int hour1 = hour % 10;
  int minute2 = minute / 10;
  int minute1 = minute % 10;
  int num[4] = {hour2, hour1, minute2, minute1};

  now = millis();
  if(now - lastTime >= 500){
    lastTime = millis();
    light = !light;
  }

  if(light){
    for(i=0; i<4; i++){
      if(angle){
        flow = i;
      }else{
        flow = 3 - i;
      }
      if(flow == 0){
        if(num[flow] == 0){
          continue;
        }
      }
      digitalWrite(nodePin[i], 1);
      for(j=0; j<7; j++){
        if(angle){
          digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    digitalWrite(coronPin, 1);
    digitalWrite(segPin[0], 0);
    digitalWrite(segPin[1], 0);
    delay(1);
    digitalWrite(coronPin, 0);
    digitalWrite(segPin[0], 1);
    digitalWrite(segPin[1], 1);
  }else{
    if(state == 0){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 0){
          if(num[flow] == 0){
            continue;
          }
        }
        if(flow == 0 || flow == 1){
          continue;
        }
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
      digitalWrite(coronPin, 1);
      digitalWrite(segPin[0], 0);
      digitalWrite(segPin[1], 0);
      delay(1);
      digitalWrite(coronPin, 0);
      digitalWrite(segPin[0], 1);
      digitalWrite(segPin[1], 1);
    }else if(state == 1){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 0){
          if(num[flow] == 0){
            continue;
          }
        }
        if(flow == 2 || flow == 3){
          continue;
        }
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
      digitalWrite(coronPin, 1);
      digitalWrite(segPin[0], 0);
      digitalWrite(segPin[1], 0);
      delay(1);
      digitalWrite(coronPin, 0);
      digitalWrite(segPin[0], 1);
      digitalWrite(segPin[1], 1);
    }
  }
}

void Display::printFlashSetting(int difficult, int speed, bool angle, int state){
  int i,j,k, flow;
  int difficult2 = difficult / 10;
  int difficult1 = difficult % 10;
  int speed2 = speed / 10;
  int speed1 = speed % 10;
  int num[4] = {difficult2, difficult1, speed2, speed1};

  now = millis();
  if(now - lastTime >= 500){
    lastTime = millis();
    light = !light;
  }

  if(light){
    for(i=0; i<4; i++){
      if(angle){
        flow = i;
      }else{
        flow = 3 - i;
      }
      if(flow == 0 || flow == 2){
        if(num[flow] == 0){
          continue;
        }
      }
      digitalWrite(nodePin[i], 1);
      for(j=0; j<7; j++){
        if(angle){
          digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    digitalWrite(coronPin, 1);
    digitalWrite(segPin[0], 0);
    digitalWrite(segPin[1], 0);
    delay(1);
    digitalWrite(coronPin, 0);
    digitalWrite(segPin[0], 1);
    digitalWrite(segPin[1], 1);
  }else{
    if(state == 0){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 0 || flow == 2){
          if(num[flow] == 0){
            continue;
          }
        }
        if(flow == 0 || flow == 1){
          continue;
        }
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
      digitalWrite(coronPin, 1);
      digitalWrite(segPin[0], 0);
      digitalWrite(segPin[1], 0);
      delay(1);
      digitalWrite(coronPin, 0);
      digitalWrite(segPin[0], 1);
      digitalWrite(segPin[1], 1);
    }else if(state == 1){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 0 || flow == 2){
          if(num[flow] == 0){
            continue;
          }
        }
        if(flow == 2 || flow == 3){
          continue;
        }
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
      digitalWrite(coronPin, 1);
      digitalWrite(segPin[0], 0);
      digitalWrite(segPin[1], 0);
      delay(1);
      digitalWrite(coronPin, 0);
      digitalWrite(segPin[0], 1);
      digitalWrite(segPin[1], 1);
    }
  }
}

void Display::printFlashAnswer(int num4, int num3, int num2, int num1, bool angle, int state){
  int i,j,k, flow;
  int num[4] = {num4, num3, num2, num1};

  now = millis();
  if(now - lastTime >= 500){
    lastTime = millis();
    light = !light;
  }

  if(light){
    for(i=0; i<4; i++){
      if(angle){
        flow = i;
      }else{
        flow = 3 - i;
      }
      if(num[flow] == -1){
        digitalWrite(nodePin[i], 1);
        digitalWrite(segPin[6], 0);
        delay(1);
        digitalWrite(nodePin[i], 0);
        digitalWrite(segPin[6], 1);
      }else{
        digitalWrite(nodePin[i], 1);
        for(j=0; j<7; j++){
          if(angle){
            digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
  }else{
    if(state == 3){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 0){
          continue;
        }
        if(num[flow] == -1){
          digitalWrite(nodePin[i], 1);
          digitalWrite(segPin[6], 0);
          delay(1);
          digitalWrite(nodePin[i], 0);
          digitalWrite(segPin[6], 1);
        }else{
          digitalWrite(nodePin[i], 1);
          for(j=0; j<7; j++){
            if(angle){
              digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    }else if(state == 4){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 1){
          continue;
        }
        if(num[flow] == -1){
          digitalWrite(nodePin[i], 1);
          digitalWrite(segPin[6], 0);
          delay(1);
          digitalWrite(nodePin[i], 0);
          digitalWrite(segPin[6], 1);
        }else{
          digitalWrite(nodePin[i], 1);
          for(j=0; j<7; j++){
            if(angle){
              digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    }else if(state == 5){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 2){
          continue;
        }
        if(num[flow] == -1){
          digitalWrite(nodePin[i], 1);
          digitalWrite(segPin[6], 0);
          delay(1);
          digitalWrite(nodePin[i], 0);
          digitalWrite(segPin[6], 1);
        }else{
          digitalWrite(nodePin[i], 1);
          for(j=0; j<7; j++){
            if(angle){
              digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    }else if(state == 6){
      for(i=0; i<4; i++){
        if(angle){
          flow = i;
        }else{
          flow = 3 - i;
        }
        if(flow == 3){
          continue;
        }
        if(num[flow] == -1){
          digitalWrite(nodePin[i], 1);
          digitalWrite(segPin[6], 0);
          delay(1);
          digitalWrite(nodePin[i], 0);
          digitalWrite(segPin[6], 1);
        }else{
          digitalWrite(nodePin[i], 1);
          for(j=0; j<7; j++){
            if(angle){
              digitalWrite(segPin[j], !bitRead(patternTRUE[num[flow]],j));
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
    }
  }
}

void Display::printO() {
  int i;
  digitalWrite(nodePin[1], 1);
  digitalWrite(segPin[0], 0);
  digitalWrite(segPin[3], 0);
  digitalWrite(segPin[4], 0);
  digitalWrite(segPin[5], 0);
  delay(1);
  digitalWrite(nodePin[1], 0);
  for(i=0; i<7; i++){
    digitalWrite(segPin[i], 1);
  }

  digitalWrite(nodePin[2], 1);
  digitalWrite(segPin[0], 0);
  digitalWrite(segPin[1], 0);
  digitalWrite(segPin[2], 0);
  digitalWrite(segPin[3], 0);
  delay(1);
  digitalWrite(nodePin[2], 0);
  for(i=0; i<7; i++){
    digitalWrite(segPin[i], 1);
  }

}

void Display::printX() {
  int i;
  digitalWrite(nodePin[1], 1);
  digitalWrite(segPin[0], 0);
  digitalWrite(segPin[1], 0);
  digitalWrite(segPin[4], 0);
  digitalWrite(segPin[6], 0);
  delay(1);
  digitalWrite(nodePin[1], 0);
  for(i=0; i<7; i++){
    digitalWrite(segPin[i], 1);
  }

  digitalWrite(nodePin[2], 1);
  digitalWrite(segPin[1], 0);
  digitalWrite(segPin[3], 0);
  digitalWrite(segPin[4], 0);
  digitalWrite(segPin[6], 0);
  delay(1);
  digitalWrite(nodePin[2], 0);
  for(i=0; i<7; i++){
    digitalWrite(segPin[i], 1);
  }

}

void Display::clear() {
    int i;
    for(i=0; i<7; i++){
      digitalWrite(segPin[i], 1);
    }
    for(i=0; i<4; i++){
      digitalWrite(nodePin[i], 0);
    }
    digitalWrite(coronPin, 0);
}