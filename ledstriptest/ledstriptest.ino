#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN        6
#define NUMPIXELS  461

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

float ActivationFloat = 0;
bool AlreadyActivated;

// Variables to manage the transition state
bool transitioning = false;
int transitionStep = 0;
int maxTransitionStep = 255;

int section1[] = {0, 116};
int section2[] = {117, 231};
int section3[] = {232, 345};
int section4[] = {346, 461};

void setup() {
  Serial.begin(9600);

  AlreadyActivated = 0;
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
  pixels.clear();
}

void loop() {
  Serial.print("ActivationFloat: ");
  Serial.println(ActivationFloat);
  Serial.print("AlreadyActivated: ");
  Serial.println(AlreadyActivated);

  if (ActivationFloat == 0) {
    Red();
    //GLOWColor();
    //IdleMode();
  } else {
    if (!AlreadyActivated) {
      TransitionStep();
    } else {
      TouchedMode();
    }
  }
}

void setSectionColor(int section[], uint32_t color) {
  for (int i = section[0]; i <= section[1]; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}

void setTubeColor(uint32_t color) {
  setSectionColor(section1, color); 
  setSectionColor(section2, color);  
  setSectionColor(section3, color);  
  setSectionColor(section4, color);
}

//FINAL ANIMATION
void IdleMode(){
  // Test modes
  //StartUpAnimation();
  //Yellow();
  //rainbowLoop();
  //LavaLampAnimation(pixels.Color(0, 0, 255), pixels.Color(255, 0, 0));
  //testHeartbeat();
}

void TransitionStep() {
  if (!transitioning) {
    transitioning = true;
    transitionStep = 0;
  }

  // Phase 1: Transition from blue (RGB: 0, 255, 255) to green (RGB: 0, 255, 0)
  if (transitionStep <= maxTransitionStep) {
    setTubeColor(pixels.Color(0, transitionStep, 255 - transitionStep));  // Reduce blue, increase green
    transitionStep++;
  } 
  // Phase 2: Transition from green (RGB: 0, 255, 0) to yellow (RGB: 255, 255, 0)
  else if (transitionStep > maxTransitionStep && transitionStep <= maxTransitionStep * 2) {
    int step = transitionStep - maxTransitionStep;
    setTubeColor(pixels.Color(step, 196, 0));  // Increase red to transition to yellow
    transitionStep++;
  } 
  // End transition
  else {
    transitioning = false;
    AlreadyActivated = true;
  }
}

void TouchedMode() {
  if (ActivationFloat == 1) {
    setTubeColor(pixels.Color(255, 196, 0));  // Set color to yellow
  }
}

// TEST ANIMATION
void StartUpAnimation(){
  testAnimation(pixels.Color(0, 255, 0));
  testAnimation(pixels.Color(255, 0, 0));
  testAnimation(pixels.Color(0, 0, 255));
}

void testAnimation(uint32_t color) {
  int maxSteps = max(max(section1[1] - section1[0], section2[1] - section2[0]),
                     max(section3[1] - section3[0], section4[1] - section4[0]));

  for (int step = 0; step <= maxSteps; step++) {
    if (section1[0] + step <= section1[1]) {
      pixels.setPixelColor(section1[0] + step, color);
    }
    
    if (section2[1] - step >= section2[0]) {
      pixels.setPixelColor(section2[1] - step, color);
    }
    
    if (section3[0] + step <= section3[1]) {
      pixels.setPixelColor(section3[0] + step, color);
    }
    
    if (section4[1] - step >= section4[0]) {
      pixels.setPixelColor(section4[1] - step, color);
    }

    pixels.show();
  }
}

void testHeartbeat() {
  for (int i = 0; i <= 255; i++) {
    setTubeColor(pixels.Color(i, i, 255));
  }
 
  for (int i = 255; i > 0; i--) {
    setTubeColor(pixels.Color(i, i, 255));
  }
}

void FlyingLight() {
  testAnimation(pixels.Color(255, 0, 0));
  testAnimation(pixels.Color(255, 0, 0));
}

void Yellow(){
  testAnimation(pixels.Color(255, 255, 0));
}

void Red(){
  testAnimation(pixels.Color(255, 0, 0));
}

void GLOWColor(){
  testAnimation(pixels.Color(255, 0, 255));
}

void LavaLampAnimation(uint32_t color, uint32_t Standardcolor){
  setSectionColor(section2, Standardcolor);  
  setSectionColor(section3, Standardcolor);  
  setSectionColor(section4, Standardcolor);
  int maxSteps = max(max(section1[1] - section1[0], section2[1] - section2[0]),
                     max(section3[1] - section3[0], section4[1] - section4[0]));

  for (int step = 0; step <= maxSteps; step++) {
    if (section1[0] + step <= section1[1]) {
      pixels.setPixelColor(section1[0] + step, color);
    }

    pixels.show();
  }

  testAnimation(pixels.Color(255, 0, 0));
}

void rainbowLoop() {
  int discospeed = 50;

  // Red to Yellow (increase green)
  for (int i = 0; i <= 255; i+=discospeed) {
    setTubeColor(pixels.Color(255, i, 0));
  }

  // Yellow to Green (decrease red)
  for (int i = 255; i >= 0; i-=discospeed) {
    setTubeColor(pixels.Color(i, 255, 0));
  }

  // Green to Cyan (increase blue)
  for (int i = 0; i <= 255; i+=discospeed) {
    setTubeColor(pixels.Color(0, 255, i));
  }

  // Cyan to Blue (decrease green)
  for (int i = 255; i >= 0; i-=discospeed) {
    setTubeColor(pixels.Color(0, i, 255));
  }

  // Blue to Magenta (increase red)
  for (int i = 0; i <= 255; i+=discospeed) {
    setTubeColor(pixels.Color(i, 0, 255));
  }

  // Magenta to Red (decrease blue)
  for (int i = 255; i >= 0; i-=discospeed) {
    setTubeColor(pixels.Color(255, 0, i));
  }
}