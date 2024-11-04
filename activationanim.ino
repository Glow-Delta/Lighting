#include <FastLED.h>

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 200

CRGB leds[NUM_LEDS];

bool isAnimating = false; // Flag to check if the animation is currently running

void fadeBrightness() {
    // Prevent re-entry if already animating
    if (isAnimating) {
        return;
    }

    isAnimating = true; // Set the flag to true indicating animation is running

    // Original brightness
    uint8_t originalBrightness = BRIGHTNESS;
    // New brightness
    uint8_t targetBrightness = min(originalBrightness + 30, 255); // Increase by 30, but cap at 255

    // Set brightness to the target
    FastLED.setBrightness(targetBrightness);
    FastLED.show();
    delay(50); // Briefly display the increased brightness

    // Gradually reduce brightness back to original
    for (int i = 0; i <= 30; i++) {
        // Calculate the current brightness based on the elapsed time
        uint8_t currentBrightness = targetBrightness - (targetBrightness - originalBrightness) * i / 30;
        FastLED.setBrightness(currentBrightness);
        FastLED.show();
        delay(5); // Adjusted delay for smoother fading
    }

    // Restore original brightness
    FastLED.setBrightness(originalBrightness);
    FastLED.show();

    isAnimating = false; // Reset the flag after animation completes
}
