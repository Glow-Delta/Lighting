#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN        6
#define NUMPIXELS  461

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int section1[] = {0, 116}; // inicio
int section2[] = {117, 231}; // segundo (paralelo)
int section3[] = {232, 345}; // terceiro
int section4[] = {346, 461}; // quarto (paralelo de novo)

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
  pixels.clear();
}

void loop() {
  // testHeartbeat();
  //setTubeColor(pixels.Color(255, 0, 0));
  //givingLife1();
  pixelConcept(pixels.Color(255, 0, 200));
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

// TEST ANIMATION

void StartUpAnimation(){
  testAnimation(pixels.Color(0, 255, 0));
  testAnimation(pixels.Color(255, 0, 0));
  testAnimation(pixels.Color(0, 0, 255));
}

void pixelConcept(uint32_t color) {
  setTubeColor(pixels.Color(0, 0, 30));
  int maxSteps = section1[1] - section1[0];
  for (int step = 0; step <= maxSteps + 40 ; step++) {
    if (section1[0] + step <= section1[1]) {
      pixels.setPixelColor(section1[0] + step, color);
      pixels.setPixelColor(section1[0] + step - 20, pixels.Color(0, 0, 30));
    }

    //if (section2[0] + step <= section2[1]) {
    //  pixels.setPixelColor(section2[1] - step + 30, pixels.Color(0, 0, 30));
    //  pixels.setPixelColor(section2[1] - step + 30, pixels.Color(255, 0, 0));
    //}


    pixels.show();
  }
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


// HEART BEAT ANIMATION TEST

void testHeartbeat() {
  for (int i = 0; i <= 255; i+=5) {
    setTubeColor(pixels.Color(i, i, 189));
  }

  for (int i = 255; i > 0; i-=5) {
    setTubeColor(pixels.Color(i, i, 189));
  }

  
}

// giving the pillar life - yellow to orange transition

void givingLife1() {
   for (int i = 0; i <= 100; i+=5) {
    setTubeColor(pixels.Color(255, 0 + i, 0));
  }

  for (int i = 0; i <= 100; i+=5) {
    setTubeColor(pixels.Color(255, 100 - i, 0));
  }

     for (int i = 0; i <= 100; i+=25) {
    setTubeColor(pixels.Color(255, 0 + i, 0));
  }

  for (int i = 0; i <= 100; i+=25) {
    setTubeColor(pixels.Color(255, 100 - i, 0));
  }
}