#include <FastLED.h>

#define NUM_LEDS 240
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
    for (int repeat = 0; repeat < 3; repeat++) {
      // Dim from full brightness to 0
      for (int pulse = 255; pulse >= 100; pulse -= 15) {
        FastLED.setBrightness(pulse);
        FastLED.show();
        delay(50);
      }
      // Brighten from 0 back to full brightness
      for (int pulse = 100; pulse <= 255; pulse += 15) {
        FastLED.setBrightness(pulse);
        FastLED.show();
        delay(50);
      }
    }

    // Fade to lilac, then blue
    fadeToColor(yellow, lilac, 20);
    fadeToColor(lilac, blue, 20);

    // Final gradient effect
    finalGradientEffect(); // Duration in milliseconds for gradient effect

    lightTimer = millis();
  }
}

// Function to create the final gradient effect
void finalGradientEffect() {
  int maxStep50 = (sections[0].end - sections[0].start) / 2; // Metade da seção (50%)
  int maxStep75 = (sections[0].end - sections[0].start) * 3 / 4; // 75% da seção

  // Fase 1: Gradiente suave até 50%
  for (int gradStep = 0; gradStep <= maxStep50; gradStep++) {
    for (int s = 0; s < 4; s++) {
      if (s % 2 == 0) {
        leds[sections[s].start + gradStep] = blend(blue, yellow, map(gradStep, 0, maxStep50, 0, 255));
      } else {
        leds[sections[s].end - gradStep] = blend(blue, yellow, map(gradStep, 0, maxStep50, 0, 255));
      }
    }
    FastLED.show();
    delay(20); // Controla a velocidade do efeito
  }

  // Fase 2: Expansão do gradiente até 75%
  for (int gradStep = maxStep50; gradStep <= maxStep75; gradStep++) {
    for (int s = 0; s < 4; s++) {
      if (s % 2 == 0) {
        leds[sections[s].start + gradStep] = blend(blue, yellow, map(gradStep, maxStep50, maxStep75, 255, 150));
      } else {
        leds[sections[s].end - gradStep] = blend(blue, yellow, map(gradStep, maxStep50, maxStep75, 255, 150));
      }
    }
    FastLED.show();
    delay(20);
  }

  // Fase 3: Contração do gradiente de volta a 50%
  for (int gradStep = maxStep75; gradStep >= maxStep50; gradStep--) {
    for (int s = 0; s < 4; s++) {
      if (s % 2 == 0) {
        leds[sections[s].start + gradStep] = blend(blue, yellow, map(gradStep, maxStep75, maxStep50, 150, 255));
      } else {
        leds[sections[s].end - gradStep] = blend(blue, yellow, map(gradStep, maxStep75, maxStep50, 150, 255));
      }
    }
    FastLED.show();
    delay(20);
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
