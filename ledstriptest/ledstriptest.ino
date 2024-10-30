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

void setup() {
  leds = new CRGB[NUM_LEDS];
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  static int tempIntSensorCount = -0; // Initialize sensor count

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Read input until newline
    tempIntSensorCount = input.toInt(); // Convert input to integer
  }

  if (tempIntSensorCount == 0) {
    IdleAnimation();
  } else if (tempIntSensorCount > 1 && tempIntSensorCount < 4) {
    FirstStage();
  } else if (tempIntSensorCount >= 4 && tempIntSensorCount < 8) {
    SecondStage();
  } else if (tempIntSensorCount >= 8) {
    SimplisticThirdStage();
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

void SimplisticThirdStage() {
  CRGB colors[6] = { pink, purple, red, orange, yellow, blue };
  int numColors = sizeof(colors) / sizeof(colors[0]);
  static int currentColor = 0;
  const int numSteps = sections[0].end - sections[0].start + 1;

  for (int step = 0; step < numSteps; step++) {
    for (int section = 0; section < 4; section++) {
      int ledIndex = sections[section].start + step;
      if (ledIndex <= sections[section].end) {
        leds[ledIndex] = colors[currentColor];
      }
    }
    FastLED.show();
  }
  if (currentColor < numColors) {
    currentColor++;
  } else {
    currentColor = 0;
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
        int ledIndex = sections[s].end - pos;

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
