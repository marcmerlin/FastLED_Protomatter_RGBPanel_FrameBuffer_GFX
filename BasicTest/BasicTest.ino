#include "matrixconfig.h"

void setup() {
  matrix_setup();
  Serial.println("Matrix Setup Done");
  
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0 < 3000)) delay(10);
  
}

uint32_t lastUpdate = 0;

void loop() {
  EVERY_N_SECONDS(1) {
    Serial.println("Loop Running");
  }

  if (millis() - lastUpdate >= 33) { // ~30 FPS UI refresh
    lastUpdate = millis();

    // 1. Clear CRGB Framebuffer
    matrix->fillScreen(0);
    
    // 2. Draw using Adafruit_GFX primitives
    matrix->drawRect(0, 0, WIDTH, HEIGHT, matrix->Color(255, 255, 255));
    matrix->setCursor(6, 10);
    matrix->setTextColor(matrix->Color(0, 255, 0));
    matrix->setTextSize(1);
    matrix->print("GFX OK");

    // 3. Draw directly into FastLED CRGB buffer (typically you don't do this
    // but it's supported if you have old FastLED matrix code mapping 2D
    // coordinates in FastLED's 1D array
    leds[matrix->XY(WIDTH / 2,     HEIGHT / 2)]     = CRGB::Red;
    leds[matrix->XY(WIDTH / 2 + 1, HEIGHT / 2)]     = CRGB::Blue;
    leds[matrix->XY(WIDTH / 2,     HEIGHT / 2 + 1)] = CRGB::Green;
    leds[matrix->XY(WIDTH / 2 + 1, HEIGHT / 2 + 1)] = CRGB::White;
    
    // 4. Push composited frame
    matrix->show();
  }
}
