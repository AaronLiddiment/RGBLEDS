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


#define SHAPE_WIDTH    5
#define SHAPE_HEIGHT   5
const uint8_t ShapeData[] = 
{
  B8_1BIT(00100000),
  B8_1BIT(01110000),
  B8_1BIT(11111000),
  B8_1BIT(01110000),
  B8_1BIT(00100000),
};
struct CRGB ColTable[1] = { CRGB(64, 128, 255) };
cLEDSprites Sprites(&leds);
cSprite Shape(SHAPE_WIDTH, SHAPE_HEIGHT, ShapeData, 1, _1BIT, ColTable, ShapeData);


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

  Shape.SetPositionFrameMotionOptions(0/*X*/, 0/*Y*/, 0/*Frame*/, 0/*FrameRate*/, +1/*XChange*/, 1/*XRate*/, +1/*YChange*/, 1/*YRate*/, SPRITE_DETECT_EDGE | SPRITE_X_KEEPIN | SPRITE_Y_KEEPIN);
  Sprites.AddSprite(&Shape);
}


void loop()
{
  FastLED.clear();
  Sprites.UpdateSprites();
  Sprites.RenderSprites();
  FastLED.show();
  delay(50);
}
