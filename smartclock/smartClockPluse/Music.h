#ifndef MUSIC_H
#define MUSIC_H

#include <Arduino.h>

class Music {
public:
    Music();
    void init();
    void playOdeToJoy();
    void playFurElise();
    void playGreensleeves();
    void playSymphonyNo5();
    void playEineKleineNachtmusik();
    void playFourSeasons();
    void playCanonInD();
    void playNewWorldSymphony();

private:
    const int SPEAKER_PIN = 16;
    void playNote(int frequency, int duration);
};

#endif