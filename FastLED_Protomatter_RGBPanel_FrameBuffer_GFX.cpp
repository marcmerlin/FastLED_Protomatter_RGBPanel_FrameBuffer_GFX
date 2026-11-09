/*-------------------------------------------------------------------------
  Arduino library based on Adafruit_Neomatrix
  by Marc MERLIN <marc_soft@merlins.org>

  You should have received a copy of the GNU Lesser General Public
  License along with NeoMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include "FastLED_Protomatter_RGBPanel_FrameBuffer_GFX.h"

FastLED_Protomatter_RGBPanel_FrameBuffer_GFX::FastLED_Protomatter_RGBPanel_FrameBuffer_GFX(
    Adafruit_Protomatter *matrix, CRGB *leds, uint16_t w, uint16_t h) : Framebuffer_GFX(leds, w, h, NULL) {
    _matrix = matrix;
}

void FastLED_Protomatter_RGBPanel_FrameBuffer_GFX::show() {
    if (!_fb || !_matrix) return;

    uint8_t b_scale = FastLED.getBrightness();
    int16_t w = width();
    int16_t h = height();

    if (b_scale == 255) {
        // Maximum speed: No brightness math
        for (int16_t y = 0; y < h; y++) {
            for (int16_t x = 0; x < w; x++) {
                const CRGB &pixel = _fb[XY(x, y)];
                // Fast 565 conversion 
                uint16_t c = ((pixel.r & 0xF8) << 8) | ((pixel.g & 0xFC) << 3) | (pixel.b >> 3);
                _matrix->drawPixel(x, y, c);
            }
        }
    } else {
        // Fast scaling: Uses scale8 (1-cycle ARM multiply)
        for (int16_t y = 0; y < h; y++) {
            for (int16_t x = 0; x < w; x++) {
                const CRGB &pixel = _fb[XY(x, y)];
                uint8_t r = scale8(pixel.r, b_scale);
                uint8_t g = scale8(pixel.g, b_scale);
                uint8_t b = scale8(pixel.b, b_scale);
                uint16_t c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                _matrix->drawPixel(x, y, c);
            }
        }
    }

    // Page flip
    _matrix->show();
}
// vim:sts=4:sw=4
