#include <FastLED.h>

#define PIN        6
#define NUMPIXELS  461
CRGB leds[NUMPIXELS];

bool isRed = true;

void setup() {

}

void loop() {
  fastLedUpdate();
}

void fastLedUpdate() {
  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUMPIXELS);
  if (isRed) {
    fill_solid(leds, NUMPIXELS, CRGB::Blue);
  } else {
    fill_solid(leds, NUMPIXELS, CRGB::Red);
  }
  FastLED.show();
  isRed = !isRed;
}
