
/*--------------------------------------------------------------------
  Arduino library based on Adafruit_Neomatrix
  by Marc MERLIN <marc_soft@merlins.org>

  Original notice and license from Adafruit_Neomatrix:
  NeoMatrix is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  NeoMatrix is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with NeoMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  --------------------------------------------------------------------*/

#ifndef FastLED_Protomatter_RGBPanel_FrameBuffer_GFX_h
#define FastLED_Protomatter_RGBPanel_FrameBuffer_GFX_h

#include <Adafruit_Protomatter.h>
#include <Framebuffer_GFX.h>

class FastLED_Protomatter_RGBPanel_FrameBuffer_GFX : public Framebuffer_GFX {
  public:
    FastLED_Protomatter_RGBPanel_FrameBuffer_GFX(
        Adafruit_Protomatter *matrix,
        CRGB *leds,
        uint16_t w,
        uint16_t h
    );
    
    virtual void show();
    
    void setBrightness(uint8_t b) { FastLED.setBrightness(b); }
    Adafruit_Protomatter *getMatrix() { return _matrix; }
    uint16_t backend_fps() const { return _backend_fps; }

  protected:
    uint32_t _last_frame_count_time = 0;
    uint16_t _backend_fps = 0;

  private:
    Adafruit_Protomatter *_matrix;
};

#endif
// vim:sts=4:sw=4
