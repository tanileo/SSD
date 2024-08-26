#include "Chinchiro.h"
#include <Arduino.h>

Chinchiro::Chinchiro() : result(0), lastUpdateTime(0), state(0) {
    for (int i = 0; i < 3; i++) {
        dice[i] = rollDice();
    }
}

void Chinchiro::play() {
    if (state == 0) {
        for (int i = 0; i < 3; i++) {
            dice[i] = rollDice();
        }
        lastUpdateTime = millis();  // 更新時間を初期化
    }
    setState(1);  // プレイ状態に変更
}

void Chinchiro::stop() {
    state = 2;
    stopTime = millis();
}

bool Chinchiro::update() {
    if (state == 1) {
        currentTime = millis();
        if (currentTime - lastUpdateTime >= 100) {  // 100ミリ秒ごとに更新
            lastUpdateTime = currentTime;
            for (int i = 0; i < 3; i++) {
                dice[i] = rollDice();
            }
            return true;
        }
    }else if(state == 2){
      currentTime = millis();
      if(currentTime - stopTime >= 2000){
        state = 0;
      }else{
        if (currentTime - lastUpdateTime >= 300) {  // 300ミリ秒ごとに更新
            lastUpdateTime = currentTime;
            for (int i = 0; i < 3; i++) {
                dice[i] = rollDice();
            }
            return true;
        }
      }
        
    }
    return false;
}

int Chinchiro::rollDice() {
    long seed = millis() + analogRead(29);
    randomSeed(seed);
    return random(1, 7);
}

void Chinchiro::setState(int num) {
    state = num;
}