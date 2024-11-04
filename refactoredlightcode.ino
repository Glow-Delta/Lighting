#include <FastLED.h>

#define NUM_LEDS 460
#define LED_PIN 5
#define BRIGHTNESS 190
#define LED_STRIP WS2812B
#define SECTIONS_COUNT 4
#define LEDS_PER_SECTION 60
#define FRAME_TIME 33
#define RING_COUNT 3
#define RING_DELAY 500
#define REPEAT_COUNTER 3
#define DELAY_MS 10

CRGB leds[NUM_LEDS];


// Colors defined with #define to save memory
#define YELLOW CRGB(255, 163, 3)
#define BLUE CRGB(0, 0, 255)
#define PINK CRGB(250, 5, 148)
#define PURPLE CRGB(255, 0, 255)
#define LILAC CRGB(221, 117, 244)

// Brightness and timer variables
unsigned long lightTimer = millis();
unsigned long lastFrameTime = 0;
unsigned long lastCyanPixelTime = 0;
uint8_t baseBrightness = 120;
uint8_t brightnessRange = 80;
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
    { 346, 459 }
};

int currentSensorCount = 0;
int previousSensorCount = 0;

// Setup function
void setup() {
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
}

void loop() {
  SecondStage();
}

// Helper function: Set color to a specific LED range recursively
void setColorRecursive(CRGB color, uint8_t section, int step, bool inward) {
    if ((inward && step < 0) || (!inward && step > LEDS_PER_SECTION / 2)) return;

    leds[sections[section].start + step] = color;
    leds[sections[section].end - step] = color;
    FastLED.show();
    delay(DELAY_MS);

    setColorRecursive(color, section, step + (inward ? -1 : 1), inward);
}

// Idle mode function
void Idle() {
    // Calculate brightness using a sine wave for smooth oscillation
    uint8_t oscillation = sin8(millis() / 10); // Adjust speed as necessary
    uint8_t currentBrightness = baseBrightness + map(oscillation, 0, 255, -brightnessRange, brightnessRange);
    
    // Set the brightness before filling the LEDs with color
    FastLED.setBrightness(currentBrightness);
    
    // Fill the LEDs with blue color
    fill_solid(leds, NUM_LEDS, BLUE);
    
    // Update the LEDs display every FRAME_TIME milliseconds
    if (millis() - lightTimer > FRAME_TIME) {
        FastLED.show();
        lightTimer = millis(); // Reset the timer for the next frame
    }
}


// First stage animation
void FirstStage() {
    if (millis() - lightTimer > FRAME_TIME) {
        int maxStep = (sections[0].end - sections[0].start) / 2;

        // Process each section without using for loop
        setColorRecursive(YELLOW, 0, maxStep, true);
        setColorRecursive(YELLOW, 0, 0, false);
        setColorRecursive(YELLOW, 1, maxStep, true);
        setColorRecursive(YELLOW, 1, 0, false);
        setColorRecursive(YELLOW, 2, maxStep, true);
        setColorRecursive(YELLOW, 2, 0, false);
        setColorRecursive(YELLOW, 3, maxStep, true);
        setColorRecursive(YELLOW, 3, 0, false);

        pulseBrightness(REPEAT_COUNTER);
        fadeToColor(BLUE, 150, 100);
        lightTimer = millis();
    }
}

// Recursive brightness pulsing effect
void pulseBrightness(int repeat) {
    if (repeat <= 0) return;
    adjustBrightness(190, 60, -15);
    adjustBrightness(60, 190, 15);
    pulseBrightness(repeat - 1);
}

void adjustBrightness(int start, int end, int step) {
    if ((step > 0 && start > end) || (step < 0 && start < end)) return;

    FastLED.setBrightness(start);
    FastLED.show();
    delay(30);

    adjustBrightness(start + step, end, step);
}

void fadeToColor(CRGB targetColor, int durationMs, int stepDelayMs) {
    // Avoiding for loop
    uint8_t brightness = 0;
    while (brightness <= 190) {
        fill_solid(leds, NUM_LEDS, targetColor);
        FastLED.setBrightness(brightness);
        FastLED.show();
        delay(stepDelayMs);
        brightness += 5;
    }
}

void spreadFromMiddle(uint8_t section, CRGB color, uint8_t delayMs, int step = 0) {
    int sectionMiddle = sections[section].start + (LEDS_PER_SECTION / 2);
    if (step > LEDS_PER_SECTION / 2) return;

    if ((sectionMiddle + step) <= sections[section].end)
        leds[sectionMiddle + step] = color;
    if ((sectionMiddle - step) >= sections[section].start)
        leds[sectionMiddle - step] = color;

    FastLED.show();
    delay(delayMs);
    spreadFromMiddle(section, color, delayMs, step + 1);
}


void multipleRingAnimation(uint8_t delayMs, uint8_t ring = 0) {
    if (ring >= RING_COUNT) return;

    static uint8_t ringPositions[RING_COUNT] = {0};
    static bool ringActive[RING_COUNT] = {true, false, false};
    static uint8_t completedRings = 0;

    unsigned long ringStartTime = millis();
    bool activeRings = false;

    if (ringActive[ring]) {
        activeRings = true;
        uint8_t currentPosition = ringPositions[ring];
        uint8_t sectionStart;

        // Process each section without using for loop
        for (int section = 0; section < SECTIONS_COUNT; section++) {
            sectionStart = sections[section].start;
            uint8_t clearPos = (currentPosition - 1) % LEDS_PER_SECTION;

            // Clear previous positions directly from leds array
            int clearPixel;
            for (int offset = -2; offset <= 2; offset++) {
                clearPixel = (clearPos + offset + LEDS_PER_SECTION) % LEDS_PER_SECTION;
                leds[sectionStart + clearPixel] = CRGB::Black; // Clear to black or whatever default color
            }

            // Set current ring pixel
            int ringPixel = (currentPosition % LEDS_PER_SECTION);
            for (int offset = -2; offset <= 2; offset++) {
                int newPixel = (ringPixel + offset + LEDS_PER_SECTION) % LEDS_PER_SECTION;
                leds[sectionStart + newPixel] = PURPLE;
            }
        }

        ringPositions[ring]++;
        if (ringPositions[ring] >= LEDS_PER_SECTION) {
            ringActive[ring] = false;
            completedRings++;
        }
    } else if (ring > 0 && (millis() - ringStartTime >= ring * RING_DELAY)) {
        ringActive[ring] = true;
    }

    if (completedRings < RING_COUNT) {
        FastLED.show();
        delay(delayMs);
        multipleRingAnimation(delayMs, ring + 1);
    } else {
        // No need to restore original colors, just clear or set to default if needed
        fill_solid(leds, NUM_LEDS, CRGB::Black); // or reset to a desired state
        FastLED.show();
    }
}

// Second stage animation
void SecondStage() {
    unsigned long currentTime = millis();
    if (currentTime - lastFrameTime >= FRAME_TIME) {
        lastFrameTime = currentTime;

        // Process each section without using for loop
        setColorRecursive(PINK, 0, LEDS_PER_SECTION / 2, true);
        setColorRecursive(PINK, 0, 0, false);
        spreadFromMiddle(0, YELLOW, DELAY_MS);
        
        setColorRecursive(PINK, 1, LEDS_PER_SECTION / 2, true);
        setColorRecursive(PINK, 1, 0, false);
        spreadFromMiddle(1, YELLOW, DELAY_MS);
        
        setColorRecursive(PINK, 2, LEDS_PER_SECTION / 2, true);
        setColorRecursive(PINK, 2, 0, false);
        spreadFromMiddle(2, YELLOW, DELAY_MS);
        
        setColorRecursive(PINK, 3, LEDS_PER_SECTION / 2, true);
        setColorRecursive(PINK, 3, 0, false);
        spreadFromMiddle(3, YELLOW, DELAY_MS);

        // Directly call multipleRingAnimation without restoring original colors
        multipleRingAnimation(DELAY_MS);

        // Implement color fade effect
        if (currentTime - lastCyanPixelTime >= 500) {
            fadeToColor(BLUE, 150, 20);
            lastCyanPixelTime = currentTime;
        }
    }
}


void ThirdStage(int previousCount) {
    if ((millis() - lightTimer) > FRAME_TIME) {
        static int shift = 0;
        CRGB colors[3] = { CRGB(0, 255, 255), CRGB(252, 3, 94), CRGB(255, 163, 3) };

        // Process each section without using for loop
        for (int s = 0; s < SECTIONS_COUNT; s++) {
            int sectionLength = sections[s].end - sections[s].start + 1;
            int pos = sectionLength - 1;

            while (pos >= 0) {
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
                pos--;
            }
        }
        FastLED.show();
        shift = (shift + 2) % sections[0].end;
        lightTimer = millis();
    }
}

// ActivateResponse without for loop
void ActivateResponseRecursive(uint8_t targetBrightness, uint8_t originalBrightness, int step = 0) {
    if (step > 30) {
        FastLED.setBrightness(originalBrightness);
        FastLED.show();
        isActivating = false;
        return;
    }

    uint8_t currentBrightness = targetBrightness - (targetBrightness - originalBrightness) * step / 30;
    FastLED.setBrightness(currentBrightness);
    FastLED.show();
    delay(5);

    ActivateResponseRecursive(targetBrightness, originalBrightness, step + 1);
}

void ActivateResponse() {
    if (isActivating) return;

    isActivating = true;
    uint8_t originalBrightness = BRIGHTNESS;
    uint8_t targetBrightness = min(originalBrightness + 30, 255);

    FastLED.setBrightness(targetBrightness);
    FastLED.show();
    delay(50);

    ActivateResponseRecursive(targetBrightness, originalBrightness);
}
