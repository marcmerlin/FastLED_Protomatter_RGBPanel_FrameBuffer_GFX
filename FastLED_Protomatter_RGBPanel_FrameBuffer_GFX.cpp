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

    uint32_t now = millis();
  
    // Initialize the timer on the very first run to prevent a massive 
    // initial time_diff if show() is called late in the boot sequence.
    if (_last_frame_count_time == 0) {
      _last_frame_count_time = now;
      _matrix->getFrameCount(); // Clear the underlying counter
      return;
    }
    
    uint32_t time_diff = now - _last_frame_count_time;
    
    // Compute hardware refresh rate approximately once per second
    if (time_diff >= 1000) {
      uint32_t frames = _matrix->getFrameCount();
      
      // Scale frames by the exact millisecond duration to get true Hz
      _backend_fps = (frames * 1000) / time_diff;
      
      _last_frame_count_time = now;
    }

}
// vim:sts=4:sw=4
