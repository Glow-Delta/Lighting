#include <FastLED.h>

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 100
#define LED_STRIP WS2812B

CRGB* leds;

CRGB yellow = CRGB(255, 163, 3);
CRGB cyan = CRGB(0, 255, 255);
CRGB pink = CRGB(252, 3, 94);

unsigned long lightTimer = millis();

struct Section {
  int start;
  int end;
};

Section sections[] = {
  { 0, 60 },
  { 61, 120 },
  { 121, 180 },
  { 181, 240 }
};

void setup() {
  leds = new CRGB[NUM_LEDS];
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
}

void loop() {
  int tempIntSensorCount = 2;

  if (tempIntSensorCount == 0) {
    IdleAnimation();
  }
  if (tempIntSensorCount > 1 && tempIntSensorCount < 4) {
    FirstStage();
  }
  if (tempIntSensorCount >= 4 && tempIntSensorCount < 8) {
    SecondStage();
  }
  if (tempIntSensorCount >= 8) {
    ThirdStage();
  }
}

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
    for (int step = maxStep; step >= 0; step--) {
      for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = CRGB(255, 255, 0);
        leds[sections[s].end - step] = CRGB(255, 255, 0);
      }
      delay(20);
      FastLED.show();
    }

    for (int step = maxStep; step >= 0; step--) {
      for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = CRGB(0, 0, 255);
        leds[sections[s].end - step] = CRGB(0, 0, 255);
      }
      delay(20);
      FastLED.show();
    }
    lightTimer = millis();
  }
}

void ThirdStage() {
  if ((millis() - lightTimer) > 33) {
    static int shift = 0;
    static unsigned long lastUpdate = 0;
    int animationSpeed = 2;

    CRGB colors[3] = { cyan, pink, yellow };
    int numColors = sizeof(colors) / sizeof(colors[0]);

    for (int s = 0; s < 4; s++) {
      int sectionLength = sections[s].end - sections[s].start + 1;

      for (int pos = sectionLength - 1; pos >= 0; pos--) {
        int ledIndex;

        /*
        if (s % 2 == 0) {
          ledIndex = sections[s].start + pos;  // Even sections: still from start to end
        } else {
          ledIndex = sections[s].end - (sectionLength - 1 - pos);  // Odd sections: now reverse from end to start
        }
      */

        ledIndex = sections[s].end - pos;

        int gradientPos = map((pos + shift) % sectionLength, 0, sectionLength - 1, 0, 255);
        CRGB color;

        if (gradientPos < 128) {
          color = blend(colors[0], colors[1], gradientPos * 2);
        } else {
          color = blend(colors[1], colors[2], (gradientPos - 128) * 2);
        }

        leds[ledIndex] = color;
      }
    }
    FastLED.show();
    shift = (shift + animationSpeed) % sections[0].end;
    lightTimer = millis();
  }
}

void SecondStage() {
}
