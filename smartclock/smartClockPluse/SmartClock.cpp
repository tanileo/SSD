#include "SmartClock.h"

SmartClock *SmartClock::instance = nullptr;

SmartClock::SmartClock()
  : currentMode(Mode::CLOCK), pastMode(Mode::CLOCK), mode1(Mode::CLOCK), mode2(Mode::CLOCK), mode3(Mode::CLOCK), mode4(Mode::CLOCK), angle(false), pastAngle(false), toggle(false), chattaringTime(0) {
  instance = this;
}

void SmartClock::init() {
  while (!Serial) delay(10);
  display.init();
  tempHumid.init();
  clock.init();
  speaker.init();
  //トグルスイッチ
  pinMode(TGS, INPUT_PULLUP);
  attachInterrupt((TGS), SmartClock::staticChangeMode, CHANGE);

  pinMode(TS1, INPUT_PULLUP);
  attachInterrupt((TS1), SmartClock::staticTS1_Push, FALLING);
  pinMode(TS2, INPUT_PULLUP);
  attachInterrupt((TS2), SmartClock::staticTS2_Push, FALLING);
  pinMode(TS3, INPUT_PULLUP);
  attachInterrupt((TS3), SmartClock::staticTS3_Push, FALLING);

  analogReadResolution(12);
  static repeating_timer_t timer;
  add_repeating_timer_ms(INTERVAL, SmartClock::staticReadAngle, NULL, &timer);
}

void SmartClock::run() {
  switch (currentMode) {
    case Mode::CLOCK:
      clockRun();
      break;
    case Mode::TEMPERATURE:
      tempRun();
      break;
    case Mode::TIMER:
      timerRun();
      break;
    case Mode::CHINCHIRO:
      chinchiroRun();
      break;
    case Mode::SLOT:
      slotRun();
      break;
    case Mode::FLASH_CALCULATION:
      flashRun();
      break;
  }
}

void SmartClock::clockRun() {
    Serial.println("CLOCK");
    switch(clock.getState()){
      case 0:
        display.printClockSetting(clock.getSetHour(2), clock.getSetHour(1), clock.getSetMinute(2), clock.getSetMinute(1), angle, 0);
        break;
      case 1:
        display.printClockSetting(clock.getSetHour(2), clock.getSetHour(1), clock.getSetMinute(2), clock.getSetMinute(1), angle, 1);
        break;
      case 2:
        display.printClockSetting(clock.getSetHour(2), clock.getSetHour(1), clock.getSetMinute(2), clock.getSetMinute(1), angle, 2);
        break;
      case 3:
        display.printClockSetting(clock.getSetHour(2), clock.getSetHour(1), clock.getSetMinute(2), clock.getSetMinute(1), angle, 3);
        break;
      default:
        clock.update();
        display.printTime(clock.getHour(), clock.getMinute(), angle);
        break;
    }
}

void SmartClock::timerRun() {
  Serial.println("TIMER");
  timer.update();
  switch (timer.state) {
    case 0:
      display.printTimer(timer.getMinutes(), timer.getSeconds(), angle, timer.state);
      break;
    case 1:
      display.printTimer(timer.getMinutes(), timer.getSeconds(), angle, timer.state);
      break;
    case 2:
      display.printTime(timer.getMinute(), timer.getSecond(), angle);
      break;
    case 3:
      speaker.playTimerEndSound();
      // display.printTime(0, 0, angle);
      break;
  }
}

void SmartClock::chinchiroRun() {
  // Serial.println("CHINCHIRO");
  chinchiro.update();
  switch (chinchiro.getState()) {
    case 0:
      display.printChinchiro(chinchiro.getDice(0), chinchiro.getDice(1), chinchiro.getDice(2), angle);
      break;
    case 1:
      display.printChinchiro(chinchiro.getDice(0), chinchiro.getDice(1), chinchiro.getDice(2), angle);
      break;
    case 2:
      display.printChinchiro(chinchiro.getDice(0), chinchiro.getDice(1), chinchiro.getDice(2), angle);
      break;
  }
}

void SmartClock::flashRun() {
  // Serial.println("FLASH_CALCULATION");
  switch (flashCalc.getState()) {
    case 0:
      display.printFlashSetting(flashCalc.getDifficult(), flashCalc.getSpeed(),  angle, 0);
      break;
    case 1:
      display.printFlashSetting(flashCalc.getDifficult(), flashCalc.getSpeed(),  angle, 1);
      break;
    case 2:
      flashCalc.update();
      display.printPluseAndMainuse(flashCalc.getShowNumber(), angle);
      break;
    case 3:
      display.printFlashAnswer(flashCalc.getUserAnswer(3), flashCalc.getUserAnswer(2), flashCalc.getUserAnswer(1), flashCalc.getUserAnswer(0), angle, 3);
      break;
    case 4:
      display.printFlashAnswer(flashCalc.getUserAnswer(3), flashCalc.getUserAnswer(2), flashCalc.getUserAnswer(1), flashCalc.getUserAnswer(0), angle, 4);
      break;
    case 5:
      display.printFlashAnswer(flashCalc.getUserAnswer(3), flashCalc.getUserAnswer(2), flashCalc.getUserAnswer(1), flashCalc.getUserAnswer(0), angle, 5);
      break;
    case 6:
      display.printFlashAnswer(flashCalc.getUserAnswer(3), flashCalc.getUserAnswer(2), flashCalc.getUserAnswer(1), flashCalc.getUserAnswer(0), angle, 6);
      break;
    case 7:
      if(flashCalc.checkAnswer()){
        display.printO();
      }else{
        display.printX();
      }
  }
}

void SmartClock::slotRun(){
  // Serial.println("SLOT");
  slot.spin();
  display.printNum(slot.getResult(3), slot.getResult(2), slot.getResult(1), slot.getResult(0), angle);
}

void SmartClock::tempRun(){
  Serial.println("TEMPERATURE");
  display.printTemp(tempHumid.getTemp(), tempHumid.getHum(), angle);
}

void SmartClock::updateDisplay() {
  // ディスプレイ更新ロジック
  switch (currentMode) {
    case Mode::CLOCK:

      break;
    case Mode::TEMPERATURE:

      break;
    case Mode::TIMER:

      break;
    case Mode::CHINCHIRO:

      break;
    case Mode::SLOT:

      break;
    case Mode::FLASH_CALCULATION:

      break;
  }
}

void SmartClock::handleButtons() {
  // ボタン処理ロジック
  switch (currentMode) {
    case Mode::CLOCK:

      break;
    case Mode::TEMPERATURE:

      break;
    case Mode::TIMER:

      break;
    case Mode::CHINCHIRO:

      break;
    case Mode::SLOT:

      break;
    case Mode::FLASH_CALCULATION:

      break;
  }
}

void SmartClock::changeMode() {
  // モード変更ロジック
  toggle = digitalRead(TGS);
  if (toggle) {
    if (angle) {
      currentMode = mode1;
    } else {
      currentMode = mode2;
    }
  } else {
    if (angle) {
      currentMode = mode3;
    } else {
      currentMode = mode4;
    }
  }
  selectInit();
}

void SmartClock::selectInit() {
  // モード変更ロジック
  if (currentMode != pastMode) {
    pastMode = currentMode;
    switch (currentMode) {
      case Mode::CLOCK:
        break;
      case Mode::TEMPERATURE:
        tempHumid.initUpdate();
        tempHumid.state = 1;
        break;
      case Mode::TIMER:

        break;
      case Mode::CHINCHIRO:

        break;
      case Mode::SLOT:

        break;
      case Mode::FLASH_CALCULATION:

        break;
    }
  }
  return;
}

bool SmartClock::ReadAngleImpl(repeating_timer_t *rt) {
  // 角度読み取りロジック
  int x, z;
  x = analogRead(X_PIN);  //X_PINのアナログ電圧値をxに代入
  // y = analogRead(Y_PIN);  //Y_PINのアナログ電圧値をyに代入
  z = analogRead(Z_PIN);  //Z_PINのアナログ電圧値をzに代入

  // angle1 = atan2(x - 2048, y - 2048)*RAD_TO_DEG;
  // angle2 = atan2(y - 2048, z - 2048)*RAD_TO_DEG;
  float Angle = atan2(z - 2048, x - 2048) * RAD_TO_DEG;

  if (Angle >= 90 || Angle <= -90) {
    angle = true;
  } else {
    angle = false;
  }
  if(angle != pastAngle){
    changeMode();
    pastAngle = angle;
  }
  return (true);
}


void SmartClock::TS1_Push() {
  now = millis();
  if (now - chattaringTime >= 200) {
    chattaringTime = millis();
    switch (currentMode) {
      case Mode::CLOCK:
        switch(clock.getState()){
          case 4:
            music.playOdeToJoy();
            break;
          case 0:
            clock.setSetHour(2, 1);
            break;
          case 1:
            clock.setSetHour(1, 1);
            break;
          case 2:
            clock.setSetMinute(2, 1);
            break;
          case 3:
            clock.setSetMinute(1, 1);
            break;
        }
        break;
      case Mode::TEMPERATURE:

        break;
      case Mode::TIMER:
        // speaker.playTimerButtonSound();
        if (timer.state == 0) {
          timer.minutes++;
          if (timer.minutes == 60) {
            timer.minutes = 0;
          }
        } else if (timer.state == 1) {
          timer.seconds++;
          if (timer.seconds == 60) {
            timer.seconds = 0;
          }
        }
        break;
      case Mode::CHINCHIRO:

        break;
      case Mode::SLOT:

        break;
      case Mode::FLASH_CALCULATION:
        switch (flashCalc.getState()) {
          case 0:
            flashCalc.setDifficult(1);
            break;
          case 1:
            flashCalc.setSpeed(1);
            break;
          case 2:
            break;
          case 3:
            flashCalc.setPluseMainuse();
            break;
          case 4:
            flashCalc.setUserAnswer(2, 1);
            break;
          case 5:
            flashCalc.setUserAnswer(1, 1);
            break;
          case 6:
            flashCalc.setUserAnswer(0, 1);
            break;
        }
        break;
    }
  }
}

void SmartClock::TS2_Push() {
  now = millis();
  if (now - chattaringTime >= 200) {
    chattaringTime = millis();
    switch (currentMode) {
      case Mode::CLOCK:  
        switch(clock.getState()){
          case 4:
            music.playFurElise();
            break;
          case 3:
            clock.setState(4);
            clock.set();
          default:
            clock.setState(clock.getState() + 1);
            break;
        }
        break;
      case Mode::TEMPERATURE:

        break;
      case Mode::TIMER:
        // speaker.playTimerButtonSound();
        switch (timer.state) {
          case 0:
            timer.state = 1;
            break;
          case 1:
            timer.start();
            break;
          case 2:
            timer.stop = !timer.stop;
            break;
          case 3:
            timer.state = 0;
            break;
        }
        break;
      case Mode::CHINCHIRO:
        switch (chinchiro.getState()) {
          case 0:
            chinchiro.play();
            break;
          case 1:
            chinchiro.stop();
            break;
          case 2:
            break;
        }

        break;
      case Mode::SLOT:
        slot.state++;
        if(slot.state == 5){
          slot.state = 0;
        }
        break;
      case Mode::FLASH_CALCULATION:
         switch (flashCalc.getState()) {
          case 0:
            flashCalc.setState(1);
            break;
          case 1:
            flashCalc.setState(2);
            flashCalc.generateProblem();
            break;
          case 2:
            break;
          case 3:
            flashCalc.setState(4);
            break;
          case 4:
            flashCalc.setState(5);
            break;
          case 5:
            flashCalc.setState(6);
            break;
          case 6:
            flashCalc.setState(7);
            break;
          case 7:
            flashCalc.setState(0);
            flashCalc.init();
            break;
        }
        break;
    }
  }
}

void SmartClock::TS3_Push() {
  now = millis();
  if (now - chattaringTime >= 200) {
    chattaringTime = millis();
    switch (currentMode) {
      case Mode::CLOCK:
        // music.playGreensleeves();
        switch(clock.getState()){
          case 4:
            music.playSymphonyNo5();
            break;
          case 0:
            clock.setSetHour(2, -1);
            break;
          case 1:
            clock.setSetHour(1, -1);
            break;
          case 2:
            clock.setSetMinute(2, -1);
            break;
          case 3:
            clock.setSetMinute(1, -1);
            break;
        }
        break;
      case Mode::TEMPERATURE:

        break;
      case Mode::TIMER:
        // speaker.playTimerButtonSound();
        if (timer.state == 0) {
          timer.minutes--;
          if (timer.minutes == -1) {
            timer.minutes = 59;
          }
        } else if (timer.state == 1) {
          timer.seconds--;
          if (timer.seconds == -1) {
            timer.seconds = 59;
          }
        }

        break;
      case Mode::CHINCHIRO:

        break;
      case Mode::SLOT:

        break;
      case Mode::FLASH_CALCULATION:
        switch (flashCalc.getState()) {
          case 0:
            flashCalc.setDifficult(-1);
            break;
          case 1:
            flashCalc.setSpeed(-1);
            break;
          case 2:
            break;
          case 3:
            flashCalc.setPluseMainuse();
            break;
          case 4:
            flashCalc.setUserAnswer(2, -1);
            break;
          case 5:
            flashCalc.setUserAnswer(1, -1);
            break;
          case 6:
            flashCalc.setUserAnswer(0, -1);
            break;
        }
        break;
    }
  }
}

void SmartClock::staticChangeMode() {
  if (SmartClock::getInstance()) {
    SmartClock::getInstance()->changeMode();
  }
}

void SmartClock::staticTS1_Push() {
  if (SmartClock::getInstance()) {
    SmartClock::getInstance()->TS1_Push();
  }
}

void SmartClock::staticTS2_Push() {
  if (SmartClock::getInstance()) {
    SmartClock::getInstance()->TS2_Push();
  }
}

void SmartClock::staticTS3_Push() {
  if (SmartClock::getInstance()) {
    SmartClock::getInstance()->TS3_Push();
  }
}

bool SmartClock::staticReadAngle(repeating_timer_t *rt) {
  if (SmartClock::getInstance()) {
    return SmartClock::getInstance()->ReadAngleImpl(rt);
  }
  return false;
}
