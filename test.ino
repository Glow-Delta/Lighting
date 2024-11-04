#include <FastLED.h>

#define NUM_LEDS 480
#define LED_PIN 5
#define BRIGHTNESS 190
#define LED_STRIP WS2812B
#define SECTIONS_COUNT 4
#define LEDS_PER_SECTION 60
#define FRAME_TIME 33 // ~30fps

CRGB leds[NUM_LEDS];

// Colors defined with #define to save memory
#define BLUE CRGB(0, 0, 255)

unsigned long lightTimer = millis();
uint8_t baseBrightness = 120; // Midpoint brightness
uint8_t brightnessRange = 70;  // Range of brightness oscillation

void setup() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
}

void loop() {
    Test();
}

void Test() {
    // Calculate elapsed time for oscillating brightness
    uint8_t oscillation = sin8(millis() / 10); // Adjust frequency with division
    uint8_t currentBrightness = baseBrightness + map(oscillation, 0, 255, -brightnessRange, brightnessRange);

    // Set and apply the brightness
    FastLED.setBrightness(currentBrightness);

    // Set all LEDs to a color (e.g., BLUE) for demonstration
    fill_solid(leds, NUM_LEDS, BLUE);

    if ((millis() - lightTimer) > FRAME_TIME) {
        FastLED.show();
        lightTimer = millis();
    }
}
