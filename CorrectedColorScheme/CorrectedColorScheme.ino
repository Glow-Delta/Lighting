#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN        6
#define NUMPIXELS  461

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Define the four sections
int section1[] = {0, 116};
int section2[] = {117, 231};
int section3[] = {232, 345};
int section4[] = {346, 460};

// Code
void setup() {
  Serial.begin(9600);

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  pixels.begin();
  pixels.clear();
}

void loop() {
  RedBeamsConvergeMiddle();
}

void Heartbeat() {
  for (int i = 0; i <= 255; i += 15) {
    setTubeColor(pixels.Color(i, i, 255));
    delay(10);
  }

  for (int i = 255; i >= 0; i -= 15) {
    setTubeColor(pixels.Color(i, i, 255));
    delay(10);
  }

  // Pause between heartbeats
  delay(150);
}

void Transition() { //It's now from blue to yellow
  if (!transitioning) {
    transitioning = true;
    transitionStep = 0;
  }

  // Phase 1: Transition from blue (RGB: 0, 255, 255) to green (RGB: 0, 255, 0)
  if (transitionStep <= maxTransitionStep) {
    setTubeColor(pixels.Color(0, transitionStep, 255 - transitionStep));
    transitionStep++;
  } 
  // Phase 2: Transition from green (RGB: 0, 255, 0) to yellow (RGB: 255, 255, 0)
  else if (transitionStep > maxTransitionStep && transitionStep <= maxTransitionStep * 2) {
    int step = transitionStep - maxTransitionStep;
    setTubeColor(pixels.Color(step, 196, 0));
    transitionStep++;
  } 
  // End transition
  else {
    transitioning = false;
  }
}

void RedBeamsConvergeMiddle() {
  int midPoint1 = (section1[0] + section1[1]) / 2;
  int midPoint2 = (section2[0] + section2[1]) / 2;
  int midPoint3 = (section3[0] + section3[1]) / 2;
  int midPoint4 = (section4[0] + section4[1]) / 2;

  for (int step = 0; step <= (section1[1] - section1[0]) / 2; step++) {
    pixels.setPixelColor(section1[0] + step, pixels.Color(255, 0, 0));
    pixels.setPixelColor(section1[1] - step, pixels.Color(255, 0, 0));

    pixels.setPixelColor(section2[0] + step, pixels.Color(255, 0, 0));
    pixels.setPixelColor(section2[1] - step, pixels.Color(255, 0, 0));

    pixels.setPixelColor(section3[0] + step, pixels.Color(255, 0, 0));
    pixels.setPixelColor(section3[1] - step, pixels.Color(255, 0, 0));

    pixels.setPixelColor(section4[0] + step, pixels.Color(255, 0, 0));
    pixels.setPixelColor(section4[1] - step, pixels.Color(255, 0, 0));

    pixels.show();

    delay(20);
  }
}