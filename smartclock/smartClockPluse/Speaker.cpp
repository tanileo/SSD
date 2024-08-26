#include "Speaker.h"

Speaker::Speaker() {}

void Speaker::init() {
    pinMode(SPEAKER_PIN, OUTPUT);
}

void Speaker::playTimerEndSound() {
    // タイマー終了音の例: 高音から低音へ
    tone(1000, 200); // 1000Hz, 200ms
    delay(250);
    tone(800, 200);  // 800Hz, 200ms
    delay(250);
    tone(600, 400);  // 600Hz, 400ms
}

void Speaker::playTimerButtonSound() {
    tone(659, 10);
}

void Speaker::playMaracasSound() {
    for (int i = 0; i < 3; i++) {  // マラカスを3回振る音を再現
        noiseMaracas();
    }
}

void Speaker::tone(unsigned int frequency, unsigned long duration) {
    unsigned long startTime = millis();
    unsigned long halfPeriod = 1000000L / frequency / 2;
    
    while (millis() - startTime < duration) {
        digitalWrite(SPEAKER_PIN, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(SPEAKER_PIN, LOW);
        delayMicroseconds(halfPeriod);
    }
}

void Speaker::noiseMaracas() {
    int duration = random(70, 150);
    int pause = random(10, 50);
    int frequency = random(500, 2000);

    tone(frequency, duration);
    delay(duration + pause);
}