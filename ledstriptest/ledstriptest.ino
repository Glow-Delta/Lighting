#include <FastLED.h>

#define PIN        6
#define NUMPIXELS  461
CRGB leds[NUMPIXELS];

int section1[] = {0, 116};
int section2[] = {117, 231};
int section3[] = {232, 345};
int section4[] = {346, 460};

void setup() {
  FastLED.addLeds<NEOPIXEL, PIN>(leds, NUMPIXELS);
}

void loop() {
  rainbowLoop();
}

void setSectionColor(int section[], CRGB color) {
  for (int i = section[0]; i <= section[1]; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void setTubeColor(CRGB color) {
  setSectionColor(section1, color); 
  setSectionColor(section2, color);  
  setSectionColor(section3, color);  
  setSectionColor(section4, color);
}

// TEST ANIMATION

void StartUpAnimation() {
  testAnimation(CRGB::Green);
  testAnimation(CRGB::Red);
  testAnimation(CRGB::Blue);
}

void testAnimation(CRGB color) {
  int maxSteps = max(max(section1[1] - section1[0], section2[1] - section2[0]),
                     max(section3[1] - section3[0], section4[1] - section4[0]));

  for (int step = 0; step <= maxSteps; step++) {
    if (section1[0] + step <= section1[1]) {
      leds[section1[0] + step] = color;
    }
    
    if (section2[1] - step >= section2[0]) {
      leds[section2[1] - step] = color;
    }
    
    if (section3[0] + step <= section3[1]) {
      leds[section3[0] + step] = color;
    }
    
    if (section4[1] - step >= section4[0]) {
      leds[section4[1] - step] = color;
    }

    FastLED.show();
  }
}

void rainbowLoop() {
  int discospeed = 50;

  // Red to Yellow (increase green)
  for (int i = 0; i <= 255; i += discospeed) {
    setTubeColor(CRGB(255, i, 0));
  }

  // Yellow to Green (decrease red)
  for (int i = 255; i >= 0; i -= discospeed) {
    setTubeColor(CRGB(i, 255, 0));
  }

  // Green to Cyan (increase blue)
  for (int i = 0; i <= 255; i += discospeed) {
    setTubeColor(CRGB(0, 255, i));
  }

  // Cyan to Blue (decrease green)
  for (int i = 255; i >= 0; i -= discospeed) {
    setTubeColor(CRGB(0, i, 255));
  }

  // Blue to Magenta (increase red)
  for (int i = 0; i <= 255; i += discospeed) {
    setTubeColor(CRGB(i, 0, 255));
  }

  // Magenta to Red (decrease blue)
  for (int i = 255; i >= 0; i -= discospeed) {
    setTubeColor(CRGB(255, 0, i));
  }
}
