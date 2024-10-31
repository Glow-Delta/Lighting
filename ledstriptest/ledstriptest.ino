#include <FastLED.h>

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 100
#define LED_STRIP WS2812B

CRGB* leds;

CRGB yellow = CRGB(255, 163, 3);
CRGB cyan = CRGB(0, 255, 255);
CRGB pink = CRGB(252, 3, 94);
CRGB blue = CRGB(0, 0, 255);
CRGB purple = CRGB(128, 0, 128);
CRGB red = CRGB(255, 0, 0);
CRGB orange = CRGB(255, 165, 0);

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

int currentSensorCount = 0;  // Store current sensor count
int previousSensorCount = 0;  // Store previous sensor count

void setup() {
  leds = new CRGB[NUM_LEDS];
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  Serial.begin(9600);  // Initialize serial communication
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // Read input until newline
    previousSensorCount = currentSensorCount;      // Store previous sensor count
    currentSensorCount = input.toInt();            // Update current sensor count
  }

  if (currentSensorCount == 0) {
    IdleAnimation();
  } else if (currentSensorCount == 1) {
    TransitionToThirdStage(previousSensorCount);
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

void TransitionToThirdStage(int previousCount) {
  if ((millis() - lightTimer) > 33) {
    static int shift = 0;
    static unsigned long lastUpdate = 0;
    int animationSpeed = 2;

    CRGB colors[3] = { cyan, pink, yellow };
    int numColors = sizeof(colors) / sizeof(colors[0]);

    for (int s = 0; s < 4; s++) {
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

        // Apply gradual transition if sensor count changes
        if (currentSensorCount != previousCount) {
          // Here you can adjust how quickly you transition the colors
          leds[ledIndex] = blend(leds[ledIndex], color, 32); // Adjust the blend factor for speed
        } else {
          leds[ledIndex] = color;
        }
      }
    }
    FastLED.show();
    shift = (shift + animationSpeed) % sections[0].end;
    lightTimer = millis();
  }
}
