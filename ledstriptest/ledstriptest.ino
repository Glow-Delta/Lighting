#include <FastLED.h>

#define BRIGHTNESS 100
#define LED_STRIP WS2812B

CRGB leds[NUM_LEDS];

CRGB yellow = CRGB(255, 163, 3);    // Yellow color
CRGB blue = CRGB(0, 0, 255);     // Blue color
CRGB lilac = CRGB(221, 117, 244);      // Lilac environment color

int repeatCounter = 3;
unsigned long lightTimer = millis();
unsigned long startTime = millis();
bool idleComplete = false; // Flag to track idle completion

struct Section {
  int start;
  int end;
};

Section sections[] = {
  { 0, 120 },
  { 121, 240 },
  { 241, 360 },
  { 361, 480 }
};


void IdleAnimation() {
  if ((millis() - lightTimer) > 33) {
    static byte fade = 0;
    static byte up = 1;

    fade += (up ? 5 : -5);

    if (fade >= 255 || fade <= 0) {
      up = (up == 1) ? 0 : 1;
    }
    fill_solid(leds, NUM_LEDS, CRGB(fade, fade, 255));
    FastLED.show();
    lightTimer = millis();
  }
}

void FirstStage() {
  if ((millis() - lightTimer) > 33) {
    int maxStep = (sections[0].end - sections[0].start) / 2;

    // Yellow filling inward
    for (int step = maxStep; step >= 0; step--) {
      for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = yellow;
        leds[sections[s].end - step] = yellow;
      }
      delay(20);
      FastLED.show();
    }

    // Yellow moving outward
    for (int step = 0; step <= maxStep; step++) {
      for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = yellow;
        leds[sections[s].end - step] = yellow;
      }
      delay(20);
      FastLED.show();
    }

    // Brightness pulsing effect - repeat 3 times
    for (int repeat = 0; repeat < 2; repeat++) {
      // Dim from full brightness to 0
      for (int pulse = 255; pulse >= 100; pulse -= 15) {
        FastLED.setBrightness(pulse);
        FastLED.show();
        delay(30);
      }
      // Brighten from 0 back to full brightness
      for (int pulse = 100; pulse <= 255; pulse += 15) {
        FastLED.setBrightness(pulse);
        FastLED.show();
        delay(30);
      }
    }

    // Fade to lilac, then blue
    fadeToColor(yellow, lilac, 20);
    fadeToColor(lilac, blue, 100);


    lightTimer = millis();
  }
}


// Function to fade smoothly from one color to another - between idleAnimation() and FirstStage()
void fadeToColor(CRGB startColor, CRGB endColor, int delayTime) {
  for (int fadeValue = 0; fadeValue <= 255; fadeValue += 5) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = blend(startColor, endColor, fadeValue);
    }
    FastLED.show();
    delay(delayTime);
  }
}

// 
void ThirdStage(int previousCount) {
  if ((millis() - lightTimer) > 33) {
    static int shift = 0;
    static unsigned long lastUpdate = 0;
    CRGB colors[3] = { CRGB(0, 255, 255), CRGB(252, 3, 94), CRGB(255, 163, 3) };

    for (int s = 0; s < amountOfSections; s++) {
      int sectionLength = sections[s].end - sections[s].start + 1;

      for (int pos = sectionLength - 1; pos >= 0; pos--) {
        int ledIndex = sections[s].end - pos;

        int gradientPos = map((pos + shift) % sectionLength, 0, sectionLength - 1, 0, 255);

        CRGB color;
        if (gradientPos < 128) {
          color = blend(colors[0], colors[1], gradientPos * 2);
        } else {
          color = blend(colors[1], colors[2], (gradientPos - 128) * 2);
        }

        if (currentSensorCount != previousCount) {
          leds[ledIndex] = blend(leds[ledIndex], color, 45);
        } else {
          leds[ledIndex] = color;
        }
      }
    }
    FastLED.show();
    shift = (shift + 2) % sections[0].end;
    lightTimer = millis();
  }
}
