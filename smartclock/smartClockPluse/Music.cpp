#include "Music.h"

Music::Music() {}

void Music::init() {
    pinMode(SPEAKER_PIN, OUTPUT);
}

void Music::playNote(int frequency, int duration) {
    tone(SPEAKER_PIN, frequency, duration);
    delay(duration);
}

void Music::playOdeToJoy() {
    int melody[] = {330, 330, 349, 392, 392, 349, 330, 294, 262, 262, 294, 330, 330, 294, 294};
    int durations[] = {250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 375, 125};
    
    for (int i = 0; i < 15; i++) {
        playNote(melody[i], durations[i]);
    }
}

void Music::playFurElise() {
    int melody[] = {659, 622, 659, 622, 659, 494, 587, 523, 440, 262, 330, 440, 494, 330, 415, 494, 523, 330, 659, 622};
    int durations[] = {125, 125, 125, 125, 125, 125, 125, 125, 250, 250, 250, 125, 125, 125, 125, 125, 125, 125, 125, 250};
    
    for (int i = 0; i < 20; i++) {
        playNote(melody[i], durations[i]);
    }
}

void Music::playGreensleeves() {
    int melody[] = {392, 440, 392, 330, 294, 330, 349, 294, 262, 294, 330, 294, 330, 349};
    int durations[] = {500, 250, 250, 500, 250, 250, 500, 250, 750, 250, 250, 250, 250, 500};
    
    for (int i = 0; i < 14; i++) {
        playNote(melody[i], durations[i]);
    }
}

void Music::playSymphonyNo5() {
    int melody[] = {392, 392, 392, 311, 466, 466, 466, 370, 392, 392, 392, 311};
    int durations[] = {250, 250, 250, 750, 250, 250, 250, 750, 250, 250, 250, 750};
    
    for (int i = 0; i < 12; i++) {
        playNote(melody[i], durations[i]);
    }
}

void Music::playEineKleineNachtmusik() {
    int melody[] = {392, 392, 392, 392, 587, 587, 587, 587, 523, 523, 523, 523, 392, 392, 392, 392};
    int durations[] = {250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250};
    
    for (int i = 0; i < 16; i++) {
        playNote(melody[i], durations[i]);
    }
}

void Music::playFourSeasons() {
    // "春"のテーマ
    int melody[] = {523, 587, 659, 698, 783, 783, 783, 783, 659, 659, 659, 659, 587, 587, 587, 587};
    int durations[] = {250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250};
    
    for (int i = 0; i < 16; i++) {
        playNote(melody[i], durations[i]);
    }
}

void Music::playCanonInD() {
    int melody[] = {293, 329, 392, 293, 329, 392, 293, 329, 392, 440, 392, 329, 293, 329, 392, 440};
    int durations[] = {250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250};
    
    for (int i = 0; i < 16; i++) {
        playNote(melody[i], durations[i]);
    }
}

void Music::playNewWorldSymphony() {
    int melody[] = {392, 349, 330, 294, 262, 294, 330, 349, 392, 392, 440, 392, 349, 330, 294};
    int durations[] = {500, 250, 250, 250, 750, 250, 250, 250, 500, 250, 250, 250, 250, 250, 750};
    
    for (int i = 0; i < 15; i++) {
        playNote(melody[i], durations[i]);
    }
}