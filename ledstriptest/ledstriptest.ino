#include <FastLED.h>

#define NUM_LEDS 460
#define LED_PIN 6
#define BRIGHTNESS 100
#define LED_STRIP WS2812B

CRGB* leds;

unsigned long lightTimer = millis();

struct Section {
  int start;
  int end;
};

Section sections[] = {
  { 0, 115 },
  { 116, 230 },
  { 231, 345 },
  { 346, 460 }
};

int amountOfSections = 4;
int currentSensorCount = 0;
int previousSensorCount = 0;

void setup() {
  leds = new CRGB[NUM_LEDS];
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  Serial.begin(9600);
}

void loop() {
  //Sensors activated (currently just dont by serial read in prototype)
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    //Save previous amount of sensors
    previousSensorCount = currentSensorCount;
    currentSensorCount = input.toInt();
  }

  if (currentSensorCount == 0) {
    IdleAnimation(previousSensorCount);
  } else if (currentSensorCount == 1) {
    ActiveAnimation(previousSensorCount);
  }
}

void IdleAnimation(int previousCount) {
  if ((millis() - lightTimer) > 33) {
    static byte fade = 0;
    static byte up = 1;

    fade += (up ? 5 : -5);

    if (fade >= 255 || fade <= 0) {
      up = (up == 1) ? 0 : 1;
    }

    CRGB idleColor = CRGB(fade, fade, 255);

    if (currentSensorCount != previousCount) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = blend(leds[i], idleColor, 10);
      }
    } else {
      fill_solid(leds, NUM_LEDS, idleColor);
    }

    FastLED.show();
    lightTimer = millis();
  }
}

void ActiveAnimation(int previousCount) {
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
