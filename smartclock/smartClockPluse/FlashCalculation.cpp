#include "FlashCalculation.h"
#include <Arduino.h>

FlashCalculation::FlashCalculation() : correctAnswer(0), state(0), difficulty(1), speedLevel(1), lastShowTime(0), nextShowIndex(0) {
    for (int i = 0; i < 5; i++) {
        numbers[i] = 0;
    }
}

void FlashCalculation::init() {
  int i;
  for(i=0; i<4; i++){
    answer[i] = 0;
  }
}

void FlashCalculation::setDifficult(int num){
  difficulty = difficulty + num;
  if(difficulty == 3){
    difficulty = 1;
  }else if(difficulty == 0){
    difficulty = 2;
  }
  return;
}

void FlashCalculation::setSpeed(int num){
  speedLevel = speedLevel + num;
  if(speedLevel == 11){
    speedLevel = 1;
  }else if(speedLevel == 0){
    speedLevel = 10;
  }
  return;
}

void FlashCalculation::generateProblem() {
    correctAnswer = 0;
    nextShowIndex = 0;
    long seed = millis() + analogRead(27) + analogRead(28) + random(-1000, 1000);
    randomSeed(seed);
    switch(difficulty){
      case 1:
        for (int i = 0; i < 5; i++) {
          numbers[i] = random(-9, 10); // 1-99の数字を生成
          correctAnswer += numbers[i];
        }
        Serial.print("correctAnswer");
        Serial.println(correctAnswer);
        break;
      case 2:
        for (int i = 0; i < 5; i++) {
          numbers[i] = random(-99, 100); // 1-99の数字を生成
          correctAnswer += numbers[i];
        }
        Serial.print("correctAnswer");
        Serial.println(correctAnswer);
        break;
      case 3:
        for (int i = 0; i < 5; i++) {
          numbers[i] = random(-999, 1000); // 1-99の数字を生成
          correctAnswer += numbers[i];
        }
        Serial.print("correctAnswer");
        Serial.println(correctAnswer);
        break;
    }
}

void FlashCalculation::update() {
    currentTime = millis();
    if(currentTime - lastShowTime >= ((11- speedLevel)*200)){
      lastShowTime = millis();
      if(nextShowIndex == 5){
        state = 3;
      }else{
        currentShowNumber = numbers[nextShowIndex];
        nextShowIndex++;
        Serial.println(currentShowNumber);
        // delay(100);
      }
    }
}

bool FlashCalculation::checkAnswer() {
    int userAnswer;
    if(answer[3] == -1){
      userAnswer = (answer[0] + answer[1]*10 + answer[2]*100)*-1;
    }else{
      userAnswer = answer[0] + answer[1]*10 + answer[2]*100;
    }
    Serial.print("userAnswer");
    Serial.println(userAnswer);
    Serial.print("correctAnswer");
    Serial.println(correctAnswer);
    
    return userAnswer == correctAnswer;
}

inline int FlashCalculation::getNumber(int index) const {  // ここで関数を定義
    if (index >= 0 && index < 5) {
        return numbers[index];
    }
    return -1; // エラー値
}

void FlashCalculation::setState(int num){
  state = num;
  return;
}
