#ifndef MATRIX_CONFIG
#define MATRIX_CONFIG
#include <FastLED_Protomatter_RGBPanel_FrameBuffer_GFX.h>

#define WIDTH      64
#define HEIGHT     64
#define BIT_DEPTH  4
#define BRIGHTNESS 255

// Hardware pins mapping for Pimoroni Interstate 75 W (RP2350)
uint8_t rgbPins[]  = {0, 1, 2, 3, 4, 5};
uint8_t addrPins[] = {6, 7, 8, 9, 10}; 
uint8_t clockPin   = 11;
uint8_t latchPin   = 12;
uint8_t oePin      = 13;


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// CONFIG STOPS HERE. Everything below is common code to all demos
///
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef LEDMATRIX
// Please use https://github.com/marcmerlin/LEDMatrix/ at least as recent as
// https://github.com/marcmerlin/LEDMatrix/commit/597ce703e924d45b2e676d6558c4c74a8ebc6991
// or https://github.com/Jorgen-VikingGod/LEDMatrix/commit/a11e74c8cd5b933021b6e15eb067280a52691449
// zero copy/no malloc code to work.
#include <LEDMatrix.h>
#endif

// Allocate the 1D CRGB buffer backing FrameBuffer_GFX / FastLED
CRGB leds[WIDTH * HEIGHT];

Adafruit_Protomatter *pm_matrix = nullptr;


/// Please ignore the "TILE" part, for externally managed framebuffers like RGBPanels,
/// the resolution is coded here.
const uint16_t MATRIX_TILE_WIDTH = 64;
const uint16_t MATRIX_TILE_HEIGHT= 64;

// and these are used by LEDMatrix for Neopixel Matrices but do not change from 1 for RGBPanels
const uint8_t MATRIX_TILE_H     = 1;  // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V     = 1;  // number of matrices arranged vertically

// Used by NeoMatrix
const uint16_t mw = MATRIX_TILE_WIDTH *  MATRIX_TILE_H;
const uint16_t mh = MATRIX_TILE_HEIGHT * MATRIX_TILE_V;

// Used by some demos
const uint32_t NUMMATRIX = mw*mh;
const uint32_t NUM_LEDS = NUMMATRIX;

#ifdef LEDMATRIX
// cLEDMatrix creation, don't allocate memory since we will feed it the framebuffer
// created by SmartMatrix at runtime ledmatrix(false)
cLEDMatrix<MATRIX_TILE_WIDTH, -MATRIX_TILE_HEIGHT, HORIZONTAL_MATRIX,
    MATRIX_TILE_H, MATRIX_TILE_V, HORIZONTAL_BLOCKS> ledmatrix(false);
#endif
CRGB *matrixleds = nullptr;

// Neither matrixleds point to anything useful yet because on ESP32 not as much
// RAM is available as static array allocation in global space, as RAM you can
// get via malloc in setup (or even ps_malloc to use PSRAM as needed).
// So we'll call Framebuffer_GFX->newLedsPtr and setvirtdisp to set those pointers correctly
FastLED_Protomatter_RGBPanel_FrameBuffer_GFX *matrix = nullptr;

// ----------------------------------------------------------------------------

void show_free_mem(const char *pre=NULL) {
    Framebuffer_GFX::show_free_mem(pre);
}

void die(const char *mesg) {
    Serial.println(mesg);
    while(1) delay((uint32_t)1); // while 1 loop only triggers watchdog on ESP chips
}

void *mallocordie(const char *varname, uint32_t req, bool psram=true) {
    // If varname starts with @, show debug for the allocation
    void *mem;

#ifndef BOARD_HAS_PSRAM
    psram = false;
#endif
    if (varname[0] == '@') {
        if (psram) Serial.print("PS");
        Serial.print("Malloc ");
        Serial.print(varname);
        Serial.print(" . Requested bytes: ");
        Serial.println(req);
    }
#ifdef ESP32
    if (psram) {
        mem = ps_malloc(req);
    } else {
        mem = calloc(1, req);
    }
#else
    mem = calloc(1, req);
#endif

    if (mem) {
        return mem;
    } else {
        show_free_mem();
        Serial.println("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");
        Serial.print("FATAL: ");
        if (psram) Serial.print("ps_");
        Serial.print("malloc failed for ");
        Serial.print(varname);
        Serial.print(" . Requested bytes: ");
        Serial.println(req);
        Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        while (1); // delay(1);  Adding this seems to cause an ESP32 bug
    }
    return NULL;
}

void matrix_setup(bool initserial=true) {

    if (initserial) {
	Serial.begin(115200);
        Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Serial.begin");
	delay(250);
    }

    Serial.println("Matrix FB Init");
    // ESP32 has more memory available for allocation in setup than in global
    // (if this were a global array), so we use malloc here.
    // https://forum.arduino.cc/index.php?topic=647833
    matrixleds = (CRGB *) mallocordie("matrixleds", sizeof(CRGB) * NUMMATRIX, 1);
    // and then fix the until now NULL pointer in the object.
    Serial.print("Setting Matrix framebuffer to ");
    Serial.print((uint32_t) matrixleds);
    Serial.print(" for ");
    Serial.print(NUMMATRIX);
    Serial.println(" pixels");
    // Now we can finally set the framebuffer pointer (without this, the lib will crash)
    matrix->newLedsPtr(matrixleds);

    // double buffering = true, if you are low on RAM, you can change to false
    pm_matrix = new Adafruit_Protomatter( WIDTH, BIT_DEPTH, 1, rgbPins, sizeof(addrPins), addrPins, clockPin, latchPin, oePin, true );
    if (!pm_matrix) die("Can't allow pm_matrix");

    matrix = new FastLED_Protomatter_RGBPanel_FrameBuffer_GFX(pm_matrix, matrixleds, mw, mh);

    show_free_mem("After matrixleds malloc");
    #ifdef LEDMATRIX
        Serial.println("ledmatrix.SetLEDArray");
        ledmatrix.SetLEDArray(matrixleds);
    #endif

    Serial.println("HUB75 Matrix Init");
    ProtomatterStatus status = pm_matrix->begin();
    if (status != PROTOMATTER_OK) {
      Serial.print("Protomatter failed: ");
      Serial.println((int)status);
      while (1);
    }

    Serial.println("New Protomatter RGBPanel Init Done!");
    matrix->setBrightness(BRIGHTNESS);
}

#endif // MATRIX_CONFIG
