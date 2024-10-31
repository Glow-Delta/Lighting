#include <FastLED.h>

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 230
#define LED_STRIP WS2812B

CRGB leds[NUM_LEDS];
CRGB originalColors[NUM_LEDS]; // Array to store original colors

CRGB yellow = CRGB(255, 163, 3); // Yellow color
CRGB blue = CRGB(0, 0, 255); // Blue color
CRGB red = CRGB(221, 117, 244); // Red color
CRGB pink = CRGB(250, 5, 148); // Pink color

unsigned long lightTimer = millis();
bool whitePixelsPresent = false; // Flag to track if any white pixels are present
unsigned long lastWhitePixelTime = 0; // Timer for 0.5-second delay after last white pixel
int ringsDisplayed = 0; // Counter for emitted rings

struct Section {
    int start;
    int end;
};

Section sections[] = {
    {0, 60},
    {61, 120},
    {121, 180},
    {181, 240}
};

void setup() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    Serial.begin(9600); // Initialize serial communication
}

void loop() {
    SecondStage();
}

void SecondStage() {
    if ((millis() - lightTimer) > 33) {
        lightTimer = millis(); // Reset timer to prevent rapid execution
        int maxStep = (sections[0].end - sections[0].start) / 2;

        // Save the current colors (pink and yellow) to the originalColors array
        for (int i = 0; i < NUM_LEDS; i++) {
            originalColors[i] = leds[i];
        }

        if (ringsDisplayed < 3) { // Emit 3 white rings with spacing
            emitWhiteRing();
            ringsDisplayed++;
            delay(300); // Delay to create space between rings
        } else {
            // Start fading to blue after all rings are displayed
            if (lastWhitePixelTime == 0) {
                lastWhitePixelTime = millis();
            }
            if (millis() - lastWhitePixelTime >= 500) {
                fadeToColor(blue, 20);
                lastWhitePixelTime = 0; // Reset for future use
                ringsDisplayed = 0; // Reset for future animation cycles
            }
        }
    }
}

void emitWhiteRing() {
    whitePixelsPresent = false;  // Only reset once at the start of the ring animation
    for (int currentPosition = 0; currentPosition < 60; currentPosition++) {
        // Draw white pixels in a ring in each section
        for (int section = 0; section < 4; section++) {
            int sectionStart = sections[section].start;
            for (int i = 0; i < 7; i++) {
                int ringPosition = (currentPosition + i) % 60;
                leds[sectionStart + ringPosition] = CRGB::White;
            }
        }

        FastLED.show();
        delay(80); // Adjust this delay as needed

        // Restore original colors for pixels in the ring
        for (int section = 0; section < 4; section++) {
            int sectionStart = sections[section].start;
            for (int i = 0; i < 7; i++) {
                int ringPosition = (currentPosition + i) % 60;
                leds[sectionStart + ringPosition] = originalColors[sectionStart + ringPosition];
            }
        }
    }
}

void fadeToColor(CRGB endColor, int delayTime) {
    int steps = 150;  // Increased steps for smooth transition
    for (int fadeValue = 0; fadeValue <= steps; fadeValue++) {
        float progress = (float)fadeValue / steps;

        // Smooth transition calculation
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = blend(originalColors[i], endColor, progress * 255);
        }

        FastLED.show();
        delay(delayTime);
    }
}
