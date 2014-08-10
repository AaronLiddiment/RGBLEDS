#include <FastLED.h>

#include <LEDMatrix.h>

// Change the next 6 defines to match your matrix type and size

#define LED_PIN        2
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   15
#define MATRIX_HEIGHT  15
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

uint8_t hue;
int16_t counter;

void setup()
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(255);
  FastLED.clear(true);
  delay(500);
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Lime);
  delay(1000);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.showColor(CRGB::White);
  delay(1000);
  FastLED.show();

  hue = 0;
  counter = 0;
}


void loop()
{
  int16_t sx, sy, x, y;
  uint8_t h;

  FastLED.clear();
  
  h = hue;
  if (counter < 1125)
  {
    // ** Fill LED's with diagonal stripes
    sx = sy = 0;
    while (sy < MATRIX_HEIGHT)
    {
      x = sx;
      y = sy;
      while ((x >= 0) && (y < MATRIX_HEIGHT))
      {
        leds(x, y) = CHSV(h, 255, 255);
        x--;
        y++;
      }
      h+=16;
      if (sx < (MATRIX_WIDTH - 1))
        sx++;
      else
        sy++;
    }
  }
  else
  {
    // ** Fill LED's with horizontal stripes
    for (y=0; y<MATRIX_HEIGHT; y++)
    {
      for (x=0; x<MATRIX_WIDTH; x++)
        leds(x, y) = CHSV(h, 255, 255);
      h+=16;
    }
  }
  hue+=4;

  if (counter < 125)
    ;
  else if (counter < 375)
    leds.HorizontalMirror();
  else if (counter < 625)
    leds.VerticalMirror();
  else if (counter < 875)
    leds.QuadrantMirror();
  else if (counter < 1125)
    leds.QuadrantRotateMirror();
  else if (counter < 1250)
    ;
  else if (counter < 1500)
    leds.TriangleTopMirror();
  else if (counter < 1750)
    leds.TriangleBottomMirror();
  else if (counter < 2000)
    leds.QuadrantTopTriangleMirror();
  else if (counter < 2250)
    leds.QuadrantBottomTriangleMirror();

  counter++;
  if (counter >= 2250)
    counter = 0;
  FastLED.show();
  delay(20);
}
