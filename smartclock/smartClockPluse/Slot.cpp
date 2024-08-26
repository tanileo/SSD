#include "Slot.h"
#include <Arduino.h>

Slot::Slot(): lastUpdateTime(0), state(0) {
    for (int i = 0; i < 3; i++) {
      long seed = millis() + analogRead(29) + random(-1000, 1000);
      results[i] = random(0, 10);
    }
}

void Slot::spin() {
  int i;
  currentTime = millis();
  if(currentTime - lastUpdateTime >= 150){
    lastUpdateTime = millis();
    switch(state){
      case 1:
        for(i=0; i<4; i++){
          results[i]++;
        }
        if(results[3] == 10){
          results[3] = 0;
        }
        if(results[2] == 10){
          results[2] = 0;
        }
        if(results[1] == 10){
          results[1] = 0;
        }
        if(results[0] == 10){
          results[0] = 0;
        }
        break;
      case 2:
        for(i=0; i<3; i++){
          results[i]++;
        }
        if(results[2] == 10){
          results[2] = 0;
        }
        if(results[1] == 10){
          results[1] = 0;
        }
        if(results[0] == 10){
          results[0] = 0;
        }
        break;
      case 3:
        for(i=0; i<2; i++){
          results[i]++;
        }
        if(results[1] == 10){
          results[1] = 0;
        }
        if(results[0] == 10){
          results[0] = 0;
        }
        break;
      case 4:
        results[0]++;
        if(results[0] == 10){
          results[0] = 0;
        }
        break;
      default:
        break;
    }
  }
  
}

