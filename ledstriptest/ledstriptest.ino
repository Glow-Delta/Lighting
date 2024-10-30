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
byte greenIntensity = 255;

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
  IdleAnimation();
  SecondStage();
  FirstStage();
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
    greenIntensity = max(greenIntensity - 85, 0);

    for (int step = maxStep; step >= 0; step--) {
      for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = CRGB(255, greenIntensity, 0);
        leds[sections[s].end - step] = CRGB(255, greenIntensity, 0);
      }
      FastLED.show();
    }
    lightTimer = millis();

    if (greenIntensity == 0) {
      greenIntensity = 255;
    }
  }
}

void SecondStage() {
  if ((millis() - lightTimer) > 33) {
    static int shift = 0;
    static unsigned long lastUpdate = 0;
    int animationSpeed = 2;

    CRGB colors[3] = { cyan, pink, yellow };
    int numColors = sizeof(colors) / sizeof(colors[0]);

    for (int s = 0; s < 4; s++) {
      int sectionLength = sections[s].end - sections[s].start + 1;

      for (int pos = 0; pos < sectionLength; pos++) {
        int ledIndex;

        // Calculate the LED index for each section
        if (s % 2 == 0) {
          ledIndex = sections[s].start + pos;
        } else {
          ledIndex = sections[s].end - pos;
        }

        // Calculate the color for this shifted position in the gradient
        int gradientPos = map((pos + shift) % sectionLength, 0, sectionLength - 1, 0, 255);
        CRGB color;

        // Determine the color based on the position in the gradient
        if (gradientPos < 128) {
          color = blend(colors[0], colors[1], gradientPos * 2);  // Cyan to Pink
        } else {
          color = blend(colors[1], colors[2], (gradientPos - 128) * 2);  // Pink to Yellow
        }

        leds[ledIndex] = color;
      }
    }
    FastLED.show();
    shift = (shift + animationSpeed) % sections[0].end;
    lightTimer = millis();
  }
}
