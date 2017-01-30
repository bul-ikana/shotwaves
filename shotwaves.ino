#include <LiquidCrystal.h>

#include "FastLED.h"

// Imported tone constants here. Sorry not sorry.

/*************************************************
   Public Constants
 *************************************************/

#define nB0  31
#define nC1  33
#define nCS1 35
#define nD1  37
#define nDS1 39
#define nE1  41
#define nF1  44
#define nFS1 46
#define nG1  49
#define nGS1 52
#define nA1  55
#define nAS1 58
#define nB1  62
#define nC2  65
#define nCS2 69
#define nD2  73
#define nDS2 78
#define nE2  82
#define nF2  87
#define nFS2 93
#define nG2  98
#define nGS2 104
#define nA2  110
#define nAS2 117
#define nB2  123
#define nC3  131
#define nCS3 139
#define nD3  147
#define nDS3 156
#define nE3  165
#define nF3  175
#define nFS3 185
#define nG3  196
#define nGS3 208
#define nA3  220
#define nAS3 233
#define nB3  247
#define nC4  262
#define nCS4 277
#define nD4  294
#define nDS4 311
#define nE4  330
#define nF4  349
#define nFS4 370
#define nG4  392
#define nGS4 415
#define nA4  440
#define nAS4 466
#define nB4  494
#define nC5  523
#define nCS5 554
#define nD5  587
#define nDS5 622
#define nE5  659
#define nF5  698
#define nFS5 740
#define nG5  784
#define nGS5 831
#define nA5  880
#define nAS5 932
#define nB5  988
#define nC6  1047
#define nCS6 1109
#define nD6  1175
#define nDS6 1245
#define nE6  1319
#define nF6  1397
#define nFS6 1480
#define nG6  1568
#define nGS6 1661
#define nA6  1760
#define nAS6 1865
#define nB6  1976
#define nC7  2093
#define nCS7 2217
#define nD7  2349
#define nDS7 2489
#define nE7  2637
#define nF7  2794
#define nFS7 2960
#define nG7  3136
#define nGS7 3322
#define nA7  3520
#define nAS7 3729
#define nB7  3951
#define nC8  4186
#define nCS8 4435
#define nD8  4699
#define nDS8 4978

// MAIN SETUP
#define NUM_TARGETS 8
#define LED_PIN A0
#define INFRA_PIN A1
#define POT_PIN A3
#define MAIN_SPEAKER 3

// COLOR SETUP
#define HUE_VAR 10
#define HUE 0
#define HUE_CHANGES 20
#define HUE_MOVE 5

// Define the array of leds
CRGB leds[NUM_TARGETS];

// Initialize changing vars
int target = 0;
int hue = 0;
int remaining = 3;

// Game status
boolean pulled = false;
boolean loaded = false;

// Set display
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

void setup() {
  // Set LEDS
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_TARGETS);

  // Initialize display
  // set up the LCD's number of columns and rows:
  lcd.begin(8, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("TARGETS");
  lcd.setCursor(0, 1);
  lcd.print(" O O O");

  //Set Random seed
  randomSeed(analogRead(A5));
  Serial.begin(9600);

  //Play intro music
  playIntroMusic();

  //Set first target
  Serial.print("Preset \n");
  setTarget();
  Serial.print("Postset \n");

}

void loop() {

  if (remaining > 0) {

    pulled = isPulled();

    if (pulled && !loaded) //Load weapon
    {
      loaded = true;
      playReloadMusic();
    }
    else if (!pulled && loaded) //Shot weapon
    {
      loaded = false;
      int hit = (analogRead(POT_PIN) - 100) / 30;
      if (hit == target) {
        leds[target] = CRGB::White;
        FastLED.show();
        playHitMusic(hit);
        remaining--;
        switch (remaining) {
          case 2:
            lcd.setCursor(0, 1);
            lcd.print(" X O O");
          break;

          case 1:
            lcd.setCursor(0, 1);
            lcd.print(" X X O");
          break;

          case 0:
            lcd.setCursor(0, 1);
            lcd.print(" X X X");
          break;
        }
      } else {
        playMissMusic(hit);
      }
      
      showHit(hit);

      if (remaining != 0) setTarget();
    }
    // else do nothing

  } else if (remaining == 0) {
    remaining--;
    playGameOverMusic();
  } else {
    hue = (hue + 1) % 255;
    for (int i = 0; i < NUM_TARGETS; i++) {
      int m = 0;
      leds[i] = CHSV( (i * -HUE_MOVE) + hue + (m++ * HUE_VAR), 255, 255);
      FastLED.show();
    }
  }
}

/// Game status ///

void setTarget() {
  Serial.print("Setting target \n");
  target = random(NUM_TARGETS);
  turnOffLeds();
  blinkLed(target);
}

boolean isPulled() {
  return analogRead(INFRA_PIN) > 100;
}

/// Light Management ///

void turnOffLeds() {
  Serial.print("Leds off \n");
  for (int i = 0; i < NUM_TARGETS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
  }
}

void blinkLed(int target) {
  Serial.print("Blinking \n");
  leds[target] = CRGB::Red;
  FastLED.show();
  delay(500);
  leds[target] = CRGB::Black;
  FastLED.show();
  delay(500);
  leds[target] = CRGB::Red;
  FastLED.show();
  delay(500);
  leds[target] = CRGB::Black;
  FastLED.show();
  delay(500);
  leds[target] = CRGB::Red;
  FastLED.show();
}

void showHit(int target){
  for (int j = 0; j < HUE_CHANGES; j++) {
    int n = target;

    leds[target] = CHSV(HUE, 0, 255);
    FastLED.show();

    for (int i = 0; i < target; i++) {
      leds[i] = CHSV( (j * -HUE_MOVE) + HUE + (n-- * HUE_VAR), 255, 127);
      FastLED.show();
    }
    int m = 0;

    for (int i = target + 1; i < NUM_TARGETS; i++) {
      leds[i] = CHSV( (j * -HUE_MOVE) + HUE + (m++ * HUE_VAR), 255, 127);
      FastLED.show();
    }
    delay(10);
  }
}

/// Music definition ///

// Intro music //
const int IntroLength = 21;

int introMusic[IntroLength] = {
  nE5, nF5, nG5, nA5, nB5, nD6, nB5, nE5, nB5, nA5, nG5, nF5, nE5, nF5, nG5, nA5, nB5, nA5, nG5, 0
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int introDurations[IntroLength] = {
  8, 8, 8, 8, 4, 8, 8, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 4
};

void playIntroMusic() {
  for (int thisNote = 0; thisNote < IntroLength; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int introDuration = 1000 / introDurations[thisNote];
    tone(MAIN_SPEAKER, introMusic[thisNote], introDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = introDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(MAIN_SPEAKER);
  }
}

// Game Over music //
const int OverLength = 17;

int overMusic[OverLength] = {
  nF4, nD4, 0, nC4, 0, nD4, nF4, 0, nD4, 0, nC4, 0, nD4, nF4, nG4, nA4, 0
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int overDurations[OverLength] = {
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2
};

void playGameOverMusic() {
  for (int thisNote = 0; thisNote < OverLength; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int overDuration = 1000 / overDurations[thisNote];
    tone(MAIN_SPEAKER, overMusic[thisNote], overDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = overDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(MAIN_SPEAKER);
  }
}


// Hit music //
const int HitLength = 8;

int hitMusic[HitLength] = {
  nA4, nC5, nD5, nD5, nD5, nG5, nE5, 0
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int hitDurations[HitLength] = {
  8, 6, 16, 16, 16, 6, 4, 2
};

void playHitMusic(int speaker) {
  for (int thisNote = 0; thisNote < HitLength; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int hitDuration = 1000 / hitDurations[thisNote];
    tone(speaker, hitMusic[thisNote], hitDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = hitDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speaker);
  }
}

// Miss music //
const int MissLength = 3;

int missMusic[MissLength] = {
  nF4, nD4, 0
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int missDurations[MissLength] = {
  2, 2, 0
};

void playMissMusic(int speaker) {
  for (int thisNote = 0; thisNote < MissLength; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int missDuration = 1000 / overDurations[thisNote];
    tone(speaker, missMusic[thisNote], missDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = missDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speaker);
  }
}


// Reload music //
const int ReloadLength = 4;

int reloadMusic[ReloadLength] = {
  nC4, nD4, nC3, 0
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int reloadDurations[ReloadLength] = {
  16, 16, 16, 0
};

void playReloadMusic() {
  for (int thisNote = 0; thisNote < MissLength; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int reloadDuration = 1000 / reloadDurations[thisNote];
    tone(MAIN_SPEAKER, reloadMusic[thisNote], reloadDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = reloadDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(MAIN_SPEAKER);
  }
}
