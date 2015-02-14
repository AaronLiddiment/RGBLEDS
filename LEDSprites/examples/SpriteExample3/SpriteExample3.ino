#include <FastLED.h>

#include <LEDMatrix.h>
#include <LEDSprites.h>

// Change the next 6 defines to match your matrix type and size

#define LED_PIN        2
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   80
#define MATRIX_HEIGHT  10
#define MATRIX_TYPE    HORIZONTAL_MATRIX

cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;


#define SQUARE_WIDTH    7
#define SQUARE_HEIGHT   7
const uint8_t SquareData[] = 
{
  B8_2BIT(11111110),
  B8_2BIT(12222210),
  B8_2BIT(12333210),
  B8_2BIT(12333210),
  B8_2BIT(12333210),
  B8_2BIT(12222210),
  B8_2BIT(11111110)
};
const uint8_t SquareMask[] = 
{
  B8_2BIT(11111110),
  B8_2BIT(11111110),
  B8_2BIT(11111110),
  B8_2BIT(11111110),
  B8_2BIT(11111110),
  B8_2BIT(11111110),
  B8_2BIT(11111110)
};
cLEDSprites Sprites(&leds);

#define MAX_SQUARES  8
#define NUM_COLS  3
cSprite Shape[MAX_SQUARES];
struct CRGB ColTabs[MAX_SQUARES][NUM_COLS];
int NumSquares;


void setup()
{
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds[0], leds.Size());
  FastLED.setBrightness(64);
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

  int16_t x = MATRIX_WIDTH - SQUARE_WIDTH - 1, y = 0;
  int8_t xc = -1, yc = 1;
  NumSquares = 0;
  while ((NumSquares < MAX_SQUARES) && (x < (MATRIX_WIDTH - SQUARE_WIDTH)))
  {
    for (int i=0; i<NUM_COLS; i++)
      ColTabs[NumSquares][i] = CHSV(NumSquares * 32, 255, 127 + (i * 64));
    Shape[NumSquares].Setup(SQUARE_WIDTH, SQUARE_HEIGHT, SquareData, 1, _2BIT, ColTabs[NumSquares], SquareMask);
    Shape[NumSquares].SetPositionFrameMotionOptions(x, y, 0, 0, xc, 2, 0, 0, SPRITE_DETECT_EDGE | SPRITE_X_KEEPIN);
    Sprites.AddSprite(&Shape[NumSquares]);
    ++NumSquares;
    x += (((SQUARE_WIDTH * 5) / 3) * xc);
    if (x <= 0)
    {
      x = abs(x);
      xc = +1;
    }
    y += yc;
    if ( (y == 0) || (y == (MATRIX_HEIGHT - SQUARE_HEIGHT)) )
      yc = 0 - yc;
  }
}


void loop()
{
  FastLED.clear();
  Sprites.UpdateSprites();
  for (int i=0; i<NumSquares; i++)
  {
    if (Shape[i].GetFlags() & SPRITE_EDGE_X_MAX)
      Sprites.ChangePriority(&Shape[i], SPR_BACK);
    else if (Shape[i].GetFlags() & SPRITE_EDGE_X_MIN)
      Sprites.ChangePriority(&Shape[i], SPR_FRONT);
  }
  Sprites.RenderSprites();
  FastLED.show();
  delay(25);
}
