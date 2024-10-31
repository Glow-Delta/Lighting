#include <FastLED.h>

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 100
#define LED_STRIP WS2812B

CRGB leds[NUM_LEDS];

CRGB yellow = CRGB(255, 255, 0); // Yellow color
CRGB blue = CRGB(0, 0, 255);     // Blue color
CRGB red = CRGB(221, 117, 244);      // Red color

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
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  FirstStage();
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

    // Fade to red
    fadeToColor(yellow, red, 20);

    // Fade to blue
    fadeToColor(red, blue, 20);

    // Blue filling inward
    for (int step = maxStep; step >= 0; step--) {
      for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = blue;
        leds[sections[s].end - step] = blue;
      }
      delay(20);
      FastLED.show();
    }

    lightTimer = millis();
  }
}

// Function to fade smoothly from one color to another
void fadeToColor(CRGB startColor, CRGB endColor, int delayTime) {
  for (int fadeValue = 0; fadeValue <= 255; fadeValue += 5) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = blend(startColor, endColor, fadeValue);
    }
    FastLED.show();
    delay(delayTime);
  }
}
