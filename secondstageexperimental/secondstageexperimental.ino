#include <FastLED.h>

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 230
#define LED_STRIP WS2812B

CRGB leds[NUM_LEDS];

CRGB yellow = CRGB(255, 163, 3); // Yellow color
CRGB pink = CRGB(250, 5, 148);    // Pink color

int ringSize = NUM_LEDS * 0.15; // 15% of the LEDs
int currentRingPosition = 0; // Position of the pink ring
bool movingUp = true; // Direction of the ring movement

unsigned long lightTimer = millis();

void setup() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    Serial.begin(9600); // Initialize serial communication
}

void loop() {
    // Fading effect from yellow to pink and back
    fadeColorAnimation();
    
    // Create the moving ring after the fading is complete
    movePinkRing();
}

// Function to handle fading between two colors
void fadeColorAnimation() {
    static bool fadingToPink = true;
    static int fadeValue = 0; // Used for fading effect

    if (millis() - lightTimer > 600) { // Adjust speed of fading
        lightTimer = millis();

        // Fade from yellow to pink
        if (fadingToPink) {
            fadeValue++;
            if (fadeValue >= 255) {
                fadeValue = 255; // Limit fade value to 255
                fadingToPink = false; // Start fading back to yellow
            }
        } 
        // Fade back from pink to yellow
        else {
            fadeValue--;
            if (fadeValue <= 0) {
                fadeValue = 0; // Limit fade value to 0
                fadingToPink = true; // Start fading to pink again
            }
        }

        // Apply fading effect to the ring only
        for (int i = 0; i < ringSize; i++) {
            int ledIndex = currentRingPosition + i;
            // Wrap around for the LED index
            if (ledIndex >= NUM_LEDS) {
                ledIndex -= NUM_LEDS;
            }
            leds[ledIndex] = blend(yellow, pink, fadeValue);
        }

        FastLED.show();
    }
}

// Function to create a moving pink ring
void movePinkRing() {
    // Clear LEDs, keep background yellow
    fill_solid(leds, NUM_LEDS, yellow);

    // Set the ring LEDs to pink
    for (int i = 0; i < ringSize; i++) {
        int ledIndex = currentRingPosition + i;
        // Wrap around for the LED index
        if (ledIndex >= NUM_LEDS) {
            ledIndex -= NUM_LEDS;
        }
        leds[ledIndex] = pink;
    }

    FastLED.show();

    // Update the position of the ring
    if (movingUp) {
        currentRingPosition++;
        if (currentRingPosition >= NUM_LEDS) {
            currentRingPosition = 0; // Reset when reaching the top
        }
    } else {
        currentRingPosition--;
        if (currentRingPosition < 0) {
            currentRingPosition = NUM_LEDS - ringSize; // Reset when reaching the bottom
        }
    }

    // Reverse direction at the top and bottom
    if (currentRingPosition == 0 || currentRingPosition == NUM_LEDS - ringSize) {
        movingUp = !movingUp; // Change direction
    }
}
