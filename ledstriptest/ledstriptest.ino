#include <FastLED.h> // 2nd stage - 2nd touch 

#define NUM_LEDS 240
#define LED_PIN 6
#define BRIGHTNESS 100
#define LED_STRIP WS2812B

CRGB leds[NUM_LEDS];

CRGB yellow = CRGB(255, 163, 3);
CRGB cyan = CRGB(0, 255, 255);
CRGB pink = CRGB(252, 3, 94);


void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
}

void loop() {
  // Define the gradient colors from cyan to pink to yellow
  CRGB colors[3] = { cyan, pink, yellow };
  int numColors = sizeof(colors) / sizeof(colors[0]);

  // Number of LEDs per strip
  int stripLength = NUM_LEDS / 4;

  // Fill the strips with the gradient
  for (int pos = 0; pos < stripLength; pos++) {
    for (int strip = 0; strip < 4; strip++) {
      int ledIndex;
      // Calculate the current LED index for each strip
      if (strip % 2 == 0) { // For strips 1 and 3
        ledIndex = strip * stripLength + pos; // Normal order
      } else { // For strips 2 and 4
        ledIndex = strip * stripLength + (stripLength - 1 - pos); // Reverse order
      }

      // Calculate the color for this position in the gradient
      int gradientPos = map(pos, 0, stripLength - 1, 0, 255);
      CRGB color;

      // Determine the color based on the position in the gradient
      if (gradientPos < 128) {
        color = blend(colors[0], colors[1], gradientPos * 2); // Cyan to Pink
      } else {
        color = blend(colors[1], colors[2], (gradientPos - 128) * 2); // Pink to Yellow
      }

      leds[ledIndex] = color; // Set the current LED to the calculated color
    }

    FastLED.show();
    delay(30); // Adjust delay for speed of animation
  }

  // Keep all LEDs on for a while at the end
  delay(800); // Adjust time to keep the LEDs on

  // Infinite loop for color movement
  while (true) {
    for (int shift = 0; shift < stripLength; shift++) {
      for (int strip = 0; strip < 4; strip++) {
        for (int pos = 0; pos < stripLength; pos++) {
          int ledIndex;
          if (strip % 2 == 0) { // For strips 1 and 3
            ledIndex = strip * stripLength + pos; // Normal order
          } else { // For strips 2 and 4
            ledIndex = strip * stripLength + (stripLength - 1 - pos); // Reverse order
          }

          // Calculate the color based on the position shifted
          int gradientPos = map((pos + shift) % stripLength, 0, stripLength - 1, 0, 255);
          CRGB color;

          // Determine the color based on the position in the gradient
          if (gradientPos < 128) {
            color = blend(colors[0], colors[1], gradientPos * 2); // Cyan to Pink
          } else {
            color = blend(colors[1], colors[2], (gradientPos - 128) * 2); // Pink to Yellow
          }

          leds[ledIndex] = color; // Set the current LED to the calculated color
        }
      }

      FastLED.show();
      delay(30); // Adjust delay for speed of color movement
    }
  }
}
