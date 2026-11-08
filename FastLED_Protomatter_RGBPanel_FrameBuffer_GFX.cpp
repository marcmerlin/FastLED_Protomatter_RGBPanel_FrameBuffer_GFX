/*-------------------------------------------------------------------------
  Arduino library based on Adafruit_Neomatrix
  by Marc MERLIN <marc_soft@merlins.org>

  You should have received a copy of the GNU Lesser General Public
  License along with NeoMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include "FastLED_Protomatter_RGBPanel_FrameBuffer_GFX.h"

FastLED_Protomatter_RGBPanel_FrameBuffer_GFX::FastLED_Protomatter_RGBPanel_FrameBuffer_GFX(
    Adafruit_Protomatter *matrix, CRGB *leds, uint16_t w, uint16_t h
) : Framebuffer_GFX(leds, w, h, NULL) {
    _matrix = matrix;
    _leds = leds; 
}

void FastLED_Protomatter_RGBPanel_FrameBuffer_GFX::show() {
    if (!_leds || !_matrix) return;
    
    uint8_t b_scale = FastLED.getBrightness();
    bool scale = (b_scale != 255);

    int16_t w = width();
    int16_t h = height();

    for (int16_t y = 0; y < h; y++) {
        for (int16_t x = 0; x < w; x++) {
            CRGB pixel = _leds[XY(x, y)];
            
            if (scale) {
                pixel.r = scale8(pixel.r, b_scale);
                pixel.g = scale8(pixel.g, b_scale);
                pixel.b = scale8(pixel.b, b_scale);
            }
            
            _matrix->drawPixel(x, y, _matrix->color565(pixel.r, pixel.g, pixel.b));
        }
    }
    
    // Page flip
    _matrix->show();
}
// vim:sts=4:sw=4
