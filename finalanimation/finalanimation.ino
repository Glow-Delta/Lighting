//WATCH OUT WITH FOR LOOPS! TRY TO NOT USE THEM AT ALL!!!

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

//HELPER FUNCTION FOR TRANSITIONS
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


void IdleAnimation() {
  if ((millis() - lightTimer) > FRAME_TIME) {
    static byte fade = 0;
    static byte up = 1;

    fade += (up ? 5 : -5);

    if (fade >= 255 || fade <= 0) {
      up = (up == 1) ? 0 : 1;
    }
    fill_solid(leds, NUM_LEDS, CRGB(fade, fade, 255));
    FastLED.show();
    lightTimer = millis();
  }
}


void FirstStage() {
  if ((millis() - lightTimer) > FRAME_TIME) {
    int maxStep = (sections[0].end - sections[0].start) / 2;

    // Yellow filling inward
    for (int step = maxStep; step >= 0; step--) {
      for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = YELLOW;
        leds[sections[s].end - step] = YELLOW;
      }
      delay(20);
      FastLED.show();
    }

    // Yellow moving outward
    for (int step = 0; step <= maxStep; step++) {
      for (int s = 0; s < 4; s++) {
        leds[sections[s].start + step] = YELLOW;
        leds[sections[s].end - step] = YELLOW;
      }
      delay(20);
      FastLED.show();
    }

    // Brightness pulsing effect - repeat 3 times
    for (int repeat = 0; repeat < repeatCounter; repeat++) {
 {
      // Dim from full brightness to 0
      for (int pulse = 190; pulse >= 100; pulse -= 15) {
        FastLED.setBrightness(pulse);
        FastLED.show();
        delay(30);
      }
      // Brighten from 0 back to full brightness
      for (int pulse = 100; pulse <= 190; pulse += 15) {
        FastLED.setBrightness(pulse);
        FastLED.show();
        delay(30);
      }
    }
    // Fade to lilac, then blue
    fadeToColor(LILAC, 150,20);
    fadeToColor(BLUE, 150,100);


    lightTimer = millis();
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

//SECOND STAGE HELPER FUNCTIONS
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
    memcpy(originalColors, leds, sizeof(originalColors)); 
}

void multipleRingAnimation(uint8_t delayMs) {
    uint8_t ringPositions[RING_COUNT] = {0};
    bool ringActive[RING_COUNT] = {true, false, false};
    uint8_t completedRings = 0;  // Counter for completed rings

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
                    for (int offset = -2; offset <= 2; offset++) {  // Wider clear area
                        int clearPixel = (clearPos + offset + LEDS_PER_SECTION) % LEDS_PER_SECTION;
                        leds[sectionStart + clearPixel] = originalColors[sectionStart + clearPixel];
                    }

                    // Set new 5-pixel-wide position
                    int ringPixel = (currentPosition % LEDS_PER_SECTION);
                    for (int offset = -2; offset <= 2; offset++) {  // Thicker ring
                        int newPixel = (ringPixel + offset + LEDS_PER_SECTION) % LEDS_PER_SECTION;
                        leds[sectionStart + newPixel] = PURPLE; 
                    }
                }

                ringPositions[ring]++;
                if (ringPositions[ring] >= LEDS_PER_SECTION) {
                    ringActive[ring] = false; // Ring is completed
                    completedRings++;  // Increment completed rings counter
                }
            } else if (ring > 0 && (currentTime - ringStartTime >= ring * RING_DELAY)) {
                ringActive[ring] = true; // Activate the next ring after the delay
            }
        }

        // Exit the loop if all rings have been completed
        if (completedRings >= RING_COUNT) break;

        FastLED.show();
        delay(delayMs);
    }

    // Restore original colors at the end
    memcpy(leds, originalColors, sizeof(originalColors)); // Use originalColors size for clarity
    FastLED.show();
}


void ThirdStage(int previousCount) {
  if ((millis() - lightTimer) > FRAME_TIME) {
    static int shift = 0;
    static unsigned long lastUpdate = 0;
    CRGB colors[3] = { CRGB(0, 255, 255), CRGB(252, 3, 94), CRGB(255, 163, 3) };

    for (int s = 0; s < amountOfSections; s++) {
      int sectionLength = sections[s].end - sections[s].start + 1;

      for (int pos = sectionLength - 1; pos >= 0; pos--) {
        int ledIndex = sections[s].end - pos;

        int gradientPos = map((pos + shift) % sectionLength, 0, sectionLength - 1, 0, 255);

        CRGB color;
        if (gradientPos < 128) {
          color = blend(colors[0], colors[1], gradientPos * 2);
        } else {
          color = blend(colors[1], colors[2], (gradientPos - 128) * 2);
        }

        if (currentSensorCount != previousCount) {
          leds[ledIndex] = blend(leds[ledIndex], color, 45);
        } else {
          leds[ledIndex] = color;
        }
      }
    }
    FastLED.show();
    shift = (shift + 2) % sections[0].end;
    lightTimer = millis();
  }
}

void ActivateResponse() {
    // Prevent re-entry if already animating
    if (isActivating) {
        return;
    }

    isActivating = true; // Set the flag to true indicating is running

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

    isActivating = false; // Reset the flag after animation completes
}

