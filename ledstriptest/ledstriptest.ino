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

  setTubeColor(pixels.Color(255, 0, 0)); // Red
  delay(5000);
}

void loop() {
  // testHeartbeat();
  
  //givingLife1();
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