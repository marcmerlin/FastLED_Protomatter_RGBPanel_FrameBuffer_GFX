#include "FastLED_Protomatter_RGBPanel_FrameBuffer_GFX.h"

// Hardware pins mapping for Pimoroni Interstate 75 W (RP2350)
uint8_t rgbPins[]  = {0, 1, 2, 3, 4, 5};
uint8_t addrPins[] = {6, 7, 8, 9, 10}; 
uint8_t clockPin   = 11;
uint8_t latchPin   = 12;
uint8_t oePin      = 13;

#define WIDTH      64
#define HEIGHT     64
#define BIT_DEPTH  4

// Fix Bug 1: Correct parameter order: (WIDTH, BIT_DEPTH, 1, rgbPins, 5, addrPins, ...)
Adafruit_Protomatter matrix(
  WIDTH, 
  BIT_DEPTH, 
  1,
  rgbPins, 
  sizeof(addrPins), 
  addrPins,
  clockPin, 
  latchPin, 
  oePin, 
  true
);

// Allocate the 1D CRGB buffer backing FrameBuffer_GFX / FastLED
CRGB leds[WIDTH * HEIGHT];

// Instantiate glue driver
FastLED_Protomatter_RGBPanel_FrameBuffer_GFX fb_matrix(&matrix, leds, WIDTH, HEIGHT);

void setup() {
  Serial.begin(115200);
  
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0 < 3000)) delay(10);
  
  ProtomatterStatus status = matrix.begin();
  if (status != PROTOMATTER_OK) {
    Serial.print("Protomatter failed: ");
    Serial.println((int)status);
    while (1);
  }

  Serial.println("New Protomatter RGBPanel Init Done!");
  fb_matrix.setBrightness(255);
}

uint32_t lastUpdate = 0;

void loop() {
  EVERY_N_SECONDS(1) {
    Serial.println("Loop Running");
  }

  if (millis() - lastUpdate >= 33) { // ~30 FPS UI refresh
    lastUpdate = millis();

    // 1. Clear CRGB Framebuffer
    fb_matrix.fillScreen(0);
    
    // 2. Draw using Adafruit_GFX primitives
    fb_matrix.drawRect(0, 0, WIDTH, HEIGHT, fb_matrix.Color(255, 255, 255));
    fb_matrix.setCursor(6, 10);
    fb_matrix.setTextColor(fb_matrix.Color(0, 255, 0));
    fb_matrix.setTextSize(1);
    fb_matrix.print("GFX OK");

    // 3. Draw directly into FastLED CRGB buffer
    leds[fb_matrix.XY(WIDTH / 2,     HEIGHT / 2)]     = CRGB::Red;
    leds[fb_matrix.XY(WIDTH / 2 + 1, HEIGHT / 2)]     = CRGB::Blue;
    leds[fb_matrix.XY(WIDTH / 2,     HEIGHT / 2 + 1)] = CRGB::Green;
    leds[fb_matrix.XY(WIDTH / 2 + 1, HEIGHT / 2 + 1)] = CRGB::White;
    
    // 4. Push composited frame
    fb_matrix.show();
  }
}
