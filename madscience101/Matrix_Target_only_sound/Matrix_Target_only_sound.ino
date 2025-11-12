/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-piezo-buzzer
 */

#include "pitches.h"
#define BUZZZER_PIN  11 // ESP32 pin GPIO18 connected to piezo buzzer

int melody[] = {
  NOTE_C4, NOTE_G3
};

int noteDurations[] = {
  8, 8
};

void play() {
  size_t numNotes = sizeof(melody) / sizeof(melody[0]);
  for (int thisNote = 0; thisNote < numNotes; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZZER_PIN, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZZER_PIN);
  }
}

void setup(){
  for (int i =0; i<10; i++){
    play();
  }
}

void loop() {
}
