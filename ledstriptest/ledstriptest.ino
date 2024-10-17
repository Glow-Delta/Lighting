#include <FastLED.h>

#define PIN        6
#define NUMPIXELS  461
CRGB leds[NUMPIXELS];

bool isRed = true;

void setup() {
  //FastLED initialization takes 12 microseconds
  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUMPIXELS);
}

void loop() {
  fastLedUpdate();
}

void fastLedUpdate() {
  //switch from Blue to Red takes between ~1-2 milliseconds
  if (isRed) {
    fill_solid(leds, NUMPIXELS, CRGB::Blue);
  } else {
    fill_solid(leds, NUMPIXELS, CRGB::Red);
  }
  FastLED.show();
  isRed = !isRed;
}
