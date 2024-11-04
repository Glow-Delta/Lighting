#include <FastLED.h>

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 200
#define LED_STRIP WS2812B
#define SECTIONS_COUNT 4
#define LEDS_PER_SECTION 60
#define FRAME_TIME 33  // ~30fps
#define RING_COUNT 6    // Updated to include 3 pink rings
#define RING_DELAY 500  // Delay before starting next ring (milliseconds)

CRGB leds[NUM_LEDS];
CRGB originalColors[NUM_LEDS];

// Colors defined with #define to save memory
#define YELLOW CRGB(255, 163, 3)
#define BLUE CRGB(0, 0, 255)
#define PINK CRGB(250, 5, 148)
#define CYAN CRGB(53, 238, 247);

// Section structure and definitions
struct Section {
    uint8_t start;
    uint8_t end;
};

const Section sections[SECTIONS_COUNT] = {
    {0, 59},
    {60, 119},
    {120, 179},
    {180, 239}
};

// Timing variables
unsigned long lastFrameTime = 0;
unsigned long lastCyanPixelTime = 0;

// Animation functions
void fillInward(CRGB color, uint8_t delayMs) {
    for (int step = LEDS_PER_SECTION / 2; step >= 0; step--) {
        for (uint8_t s = 0; s < SECTIONS_COUNT; s++) {
            leds[sections[s].start + step] = color;
            leds[sections[s].end - step] = color;
        }
        FastLED.show();
        delay(delayMs);
    }
}

void fillOutward(CRGB color, uint8_t delayMs) {
    for (int step = 0; step <= LEDS_PER_SECTION / 2; step++) {
        for (uint8_t s = 0; s < SECTIONS_COUNT; s++) {
            leds[sections[s].start + step] = color;
            leds[sections[s].end - step] = color;
        }
        FastLED.show();
        delay(delayMs);
    }
}

void spreadFromMiddle(CRGB color, uint8_t delayMs) {
    for (int step = 0; step <= LEDS_PER_SECTION / 2; step++) {
        for (uint8_t s = 0; s < SECTIONS_COUNT; s++) {
            int sectionMiddle = sections[s].start + (LEDS_PER_SECTION / 2);
            if ((sectionMiddle + step) <= sections[s].end)
                leds[sectionMiddle + step] = color;
            if ((sectionMiddle - step) >= sections[s].start)
                leds[sectionMiddle - step] = color;
        }
        FastLED.show();
        delay(delayMs);
    }
}

void saveCurrentColors() {
    memcpy(originalColors, leds, sizeof(leds));
}

void multipleRingAnimation(uint8_t delayMs) {
    uint8_t ringPositions[RING_COUNT] = {0};
    bool ringActive[RING_COUNT] = {true, false, false, false, false, false}; // Updated for 6 rings

    unsigned long ringStartTime = millis(); // Start time for the ring delay

    while (true) {
        bool activeRings = false;
        unsigned long currentTime = millis();

        for (int ring = 0; ring < RING_COUNT; ring++) {
            if (ringActive[ring]) {
                activeRings = true;
                uint8_t currentPosition = ringPositions[ring];

                for (int section = 0; section < SECTIONS_COUNT; section++) {
                    uint8_t sectionStart = sections[section].start;
                    uint8_t clearPos = (currentPosition - 1) % LEDS_PER_SECTION;

                    // Clear the previous position
                    for (int offset = -1; offset <= 1; offset++) {
                        int clearPixel = (clearPos + offset + LEDS_PER_SECTION) % LEDS_PER_SECTION;
                        leds[sectionStart + clearPixel] = originalColors[sectionStart + clearPixel];
                    }

                    // Set new 3-pixel-wide position
                    int ringPixel = (currentPosition % LEDS_PER_SECTION);
                    for (int offset = -1; offset <= 1; offset++) {
                        int newPixel = (ringPixel + offset + LEDS_PER_SECTION) % LEDS_PER_SECTION;

                        // Use different colors for different rings
                        if (ring < 3) {
                            leds[sectionStart + newPixel] = CYAN; // First 3 rings are cyan
                        } else {
                            leds[sectionStart + newPixel] = PINK; // Last 3 rings are pink
                        }
                    }
                }

                ringPositions[ring]++;
                if (ringPositions[ring] >= LEDS_PER_SECTION) {
                    ringActive[ring] = false; // Ring is completed
                }
            } else if (ring > 0 && (currentTime - ringStartTime >= ring * RING_DELAY)) {
                ringActive[ring] = true; // Activate the next ring after the delay
            }
        }

        if (!activeRings) break;

        FastLED.show();
        delay(delayMs);
    }

    // Restore original colors at the end
    memcpy(leds, originalColors, sizeof(leds));
    FastLED.show();
}

void fadeToColor(CRGB endColor, uint8_t steps, uint8_t delayMs) {
    for (uint8_t fadeValue = 0; fadeValue <= steps; fadeValue++) {
        float progress = fadeValue / (float)steps;
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = blend(originalColors[i], endColor, progress * 255);
        }
        FastLED.show();
        delay(delayMs);
    }
}

void SecondStage() {
    unsigned long currentTime = millis();
    if (currentTime - lastFrameTime >= FRAME_TIME) {
        lastFrameTime = currentTime;

        fillInward(PINK, 20);
        fillOutward(PINK, 20);
        spreadFromMiddle(YELLOW, 20);
        delay(20);

        saveCurrentColors();
        multipleRingAnimation(80);

        if (currentTime - lastCyanPixelTime >= 500) {
            fadeToColor(BLUE, 150, 20);
            lastCyanPixelTime = currentTime;
        }
    }
}

void setup() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
}

void loop() {
    SecondStage();
}
