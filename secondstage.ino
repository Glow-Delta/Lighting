#include <FastLED.h>

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 200
#define LED_STRIP WS2812B
#define SECTIONS_COUNT 4
#define LEDS_PER_SECTION 60
#define FRAME_TIME 33  // ~30fps

CRGB leds[NUM_LEDS];
CRGB originalColors[NUM_LEDS];

// Colors defined with #define to save memory
#define YELLOW CRGB(255, 163, 3)
#define BLUE CRGB(0, 0, 255)
#define PINK CRGB(250, 5, 148)
#define CYAN CRGB(53, 238, 247)

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

// Animation functions
void saveCurrentColors() {
    memcpy(originalColors, leds, sizeof(leds));
}

void multipleRingAnimation(uint8_t delayMs) {
    const uint8_t totalRings = 6;  // Total rings: 3 cyan + 3 pink
    uint8_t displayedRings = 0;      // Counter for displayed rings
    unsigned long startTime = millis(); // Start time for the fade effect

    while (displayedRings < totalRings) {
        unsigned long currentTime = millis();
        
        // Activate and display rings
        if (currentTime - startTime >= displayedRings * delayMs) {
            CRGB color = (displayedRings < 3) ? CYAN : PINK; // Determine color
            
            for (int section = 0; section < SECTIONS_COUNT; section++) {
                uint8_t sectionStart = sections[section].start;
                uint8_t ringPixel = (displayedRings % LEDS_PER_SECTION);
                
                // Clear previous position (3-pixel-wide)
                for (int offset = -1; offset <= 1; offset++) {
                    int clearPixel = (ringPixel + offset + LEDS_PER_SECTION) % LEDS_PER_SECTION;
                    leds[sectionStart + clearPixel] = originalColors[sectionStart + clearPixel];
                }

                // Set new position (3-pixel-wide)
                for (int offset = -1; offset <= 1; offset++) {
                    int newPixel = (ringPixel + offset + LEDS_PER_SECTION) % LEDS_PER_SECTION;
                    leds[sectionStart + newPixel] = color;
                }
            }
            displayedRings++; // Increment the counter after displaying the ring
        }
        
        // Fade to blue after 4 seconds
        if (currentTime - startTime >= 4000) {
            for (int section = 0; section < SECTIONS_COUNT; section++) {
                uint8_t sectionStart = sections[section].start;
                for (int pixel = 0; pixel < LEDS_PER_SECTION; pixel++) {
                    leds[sectionStart + pixel] = BLUE; // Set all LEDs to blue
                }
            }
            break; // Exit the loop after fading to blue
        }

        FastLED.show();
        delay(delayMs);
    }
}

void SecondStage() {
    unsigned long currentTime = millis();
    if (currentTime - lastFrameTime >= FRAME_TIME) {
        lastFrameTime = currentTime;

        // Placeholder for other animations if needed
        // saveCurrentColors();
        multipleRingAnimation(80);
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
