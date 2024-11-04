#include <FastLED.h>

#define NUM_LEDS 480
#define LED_PIN 6
#define BRIGHTNESS 190
#define LED_STRIP WS2812B
#define SECTIONS_COUNT 4
#define LEDS_PER_SECTION 60
#define FRAME_TIME 33  // ~30fps
#define RING_COUNT 3    // Number of rings
#define RING_DELAY 500  // Delay before starting next ring (milliseconds)
#define repeatCounter 3
#define delayMs 20



CRGB leds[NUM_LEDS];
CRGB originalColors[NUM_LEDS];

// Colors defined with #define to save memory
#define YELLOW CRGB(255, 163, 3)
#define BLUE CRGB(0, 0, 255)
#define PINK CRGB(250, 5, 148)
#define PURPLE CRGB(255, 0, 255)
#define LILAC CRGB(221,117,244)

int amountOfSections = 4;
int currentSensorCount = 0;
int previousSensorCount = 0;

unsigned long lightTimer = millis();
unsigned long startTime = millis();
bool idleComplete = false;
bool isActivating = false;

// Section structure and definitions
struct Section {
    uint8_t start;
    uint8_t end;
};

const Section sections[SECTIONS_COUNT] = {
  { 0, 115 },
  { 116, 230 },
  { 231, 345 },
  { 346, 460 }
};

// Timing variables
unsigned long lastFrameTime = 0;
unsigned long lastCyanPixelTime = 0;

void setup() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
}

void loop() {
    Test();
}

void Test() {
    if ((millis() - lightTimer) > FRAME_TIME) {
        int maxStep = (sections[0].end - sections[0].start) / 2;

        // Recursive yellow filling inward
        fillInward(maxStep);

        // Recursive yellow moving outward
        fillOutward(maxStep);

        // Recursive brightness pulsing effect
        pulseBrightness(repeatCounter);

        // Fade to lilac, then blue
        fadeToColor(LILAC, 150, 20);
        fadeToColor(BLUE, 150, 100);

        lightTimer = millis();
    }
}

void fillInward(int step) {
    if (step < 0) return;

    for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = YELLOW;
        leds[sections[s].end - step] = YELLOW;
    }
    FastLED.show();
    delay(20);

    fillInward(step - 1);
}

void fillOutward(int step) {
    if (step > (sections[0].end - sections[0].start) / 2) return;

    for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = YELLOW;
        leds[sections[s].end - step] = YELLOW;
    }
    FastLED.show();
    delay(20);

    fillOutward(step + 1);
}

void pulseBrightness(int repeat) {
    if (repeat <= 0) return;

    // Dim from full brightness to 0
    adjustBrightness(190, 100, -15);

    // Brighten from 0 back to full brightness
    adjustBrightness(100, 190, 15);

    pulseBrightness(repeat - 1);
}

void adjustBrightness(int start, int end, int step) {
    if ((step > 0 && start > end) || (step < 0 && start < end)) return;

    FastLED.setBrightness(start);
    FastLED.show();
    delay(30);

    adjustBrightness(start + step, end, step);
}

}
