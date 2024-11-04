#include <FastLED.h>

#define NUM_LEDS 480
#define LED_PIN 6
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

void setup() {
 FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  if (!idleComplete && (millis() - startTime < 8000)) {
    IdleAnimation(); // Run IdleAnimation for the first 5 seconds
  } else {
    idleComplete = true; // Mark idle as complete after 5 seconds
    FirstStage();        // Start FirstStage animation
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
