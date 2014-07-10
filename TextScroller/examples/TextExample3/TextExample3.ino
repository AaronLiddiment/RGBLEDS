#include <FastLED.h>

#include <LEDMatrix.h>
#include <TextScroller.h>
#include <FontRobotron.h>

#define LED_PIN        2
#define COLOR_ORDER    GRB
#define CHIPSET        WS2812B

#define MATRIX_WIDTH   68
#define MATRIX_HEIGHT  7
#define MATRIX_ZIGZAG  true

#define NUM_LEDS  (MATRIX_WIDTH * MATRIX_HEIGHT)

CRGB leds[NUM_LEDS];
cLEDMatrix LEDMatrix;

cTextScroller ScrollingMsg1, ScrollingMsg2;
#define MESSAGE_WIDTH   68
#define MESSAGE_HEIGHT  8

const unsigned char PlasmaTxt[] = { EFFECT_BACKGND_LEAVE EFFECT_RGB "\xff\xff\xff" "         F-PLASMA " EFFECT_DELAY_FRAMES "\x01\x2c" "         "
                                    EFFECT_BACKGND_DIMMING "\x40" EFFECT_RGB "\xff\xff\xff" "         F-PLASMA " EFFECT_DELAY_FRAMES "\x01\x2c" "         "
                                    EFFECT_BACKGND_LEAVE EFFECT_COLR_DIMMING "\x10" "         F-PLASMA " EFFECT_DELAY_FRAMES "\x01\x2c" "         "
                                    EFFECT_BACKGND_ERASE EFFECT_COLR_EMPTY "     F-PLASMA " EFFECT_DELAY_FRAMES "\x01\x2c" "     "
                                    EFFECT_BACKGND_ERASE EFFECT_COLR_DIMMING "\x40" "     F-PLASMA " EFFECT_DELAY_FRAMES "\x01\x2c" "     " };
const unsigned char TxtRainbowL[] = { EFFECT_SCROLL_LEFT "     " EFFECT_HSV "\x00\xff\xff" "R" EFFECT_HSV "\x20\xff\xff" "A" EFFECT_HSV "\x40\xff\xff" "I" EFFECT_HSV "\x60\xff\xff" "N" EFFECT_DELAY_FRAMES "\x00\x96" };
const unsigned char TxtRainbowR[] = { EFFECT_SCROLL_RIGHT "     " EFFECT_HSV "\xe0\xff\xff" "S" EFFECT_HSV "\xc0\xff\xff" "W" EFFECT_HSV "\xa0\xff\xff" "O" EFFECT_HSV "\x80\xff\xff" "B" EFFECT_DELAY_FRAMES "\x00\x96" };
const unsigned char TxtRainbowDU[] = { "         " EFFECT_FRAME_RATE "\x06" "  " EFFECT_HSV "\x00\xff\xff" "R" EFFECT_HSV "\x20\xff\xff" "A" EFFECT_HSV "\x40\xff\xff" "I" EFFECT_HSV "\x60\xff\xff" "N" EFFECT_HSV "\xe0\xff\xff" "B" EFFECT_HSV "\xc0\xff\xff" "O"
                                       EFFECT_HSV "\xa0\xff\xff" "W" EFFECT_HSV "\x80\xff\xff" "S" EFFECT_DELAY_FRAMES "\x00\x96" "          " };

uint16_t PlasmaTime, PlasmaShift;

void setup()
{
  int16_t HalfWholeChars, WholeEvenChars;

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
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

  LEDMatrix.LEDArray(MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_ZIGZAG, leds);
  ScrollingMsg1.SetFont(ROBOTRON_WIDTH, ROBOTRON_HEIGHT, ROBOTRON_CHAR_LOW, ROBOTRON_CHAR_HIGH, RobotronData);
  ScrollingMsg2.SetFont(ROBOTRON_WIDTH, ROBOTRON_HEIGHT, ROBOTRON_CHAR_LOW, ROBOTRON_CHAR_HIGH, RobotronData);

  HalfWholeChars = (((MESSAGE_WIDTH / 2) + ROBOTRON_WIDTH) / (ROBOTRON_WIDTH + 1)) * (ROBOTRON_WIDTH + 1);
  ScrollingMsg1.Init(&LEDMatrix, HalfWholeChars, MESSAGE_HEIGHT, (MESSAGE_WIDTH / 2) - HalfWholeChars, 0);
  ScrollingMsg1.SetText((unsigned char *)TxtRainbowL, sizeof(TxtRainbowL) - 1);
  ScrollingMsg2.Init(&LEDMatrix, HalfWholeChars, MESSAGE_HEIGHT, MESSAGE_WIDTH / 2, 0);
  ScrollingMsg2.SetText((unsigned char *)TxtRainbowR, sizeof(TxtRainbowR) - 1);
  while (ScrollingMsg1.UpdateText() == 0)
  {
    ScrollingMsg2.UpdateText();
    FastLED.show();
    delay(20);
  }

  WholeEvenChars = ((MESSAGE_WIDTH + (ROBOTRON_WIDTH * 2) + 1) / ((ROBOTRON_WIDTH + 1) * 2)) * ((ROBOTRON_WIDTH + 1) * 2);
  ScrollingMsg1.Init(&LEDMatrix, WholeEvenChars, (ROBOTRON_HEIGHT + 1) / 2, (MESSAGE_WIDTH - WholeEvenChars) / 2, (ROBOTRON_HEIGHT + 1) / 2, SCROLL_UP);
  ScrollingMsg1.SetText((unsigned char *)TxtRainbowDU, sizeof(TxtRainbowDU) - 1);
  ScrollingMsg2.Init(&LEDMatrix, WholeEvenChars, (ROBOTRON_HEIGHT + 1) / 2, (MESSAGE_WIDTH - WholeEvenChars) / 2, 0, SCROLL_DOWN);
  ScrollingMsg2.SetText((unsigned char *)TxtRainbowDU, sizeof(TxtRainbowDU) - 1);
  while (ScrollingMsg1.UpdateText() == 0)
  {
    ScrollingMsg2.UpdateText();
    FastLED.show();
    delay(20);
  }

  ScrollingMsg1.Init(&LEDMatrix, WholeEvenChars, MESSAGE_HEIGHT, (MESSAGE_WIDTH - WholeEvenChars) / 2, 0);
  ScrollingMsg1.SetText((unsigned char *)PlasmaTxt, sizeof(PlasmaTxt) - 1);

  PlasmaShift = (random8(0, 5) * 32) + 64;
  PlasmaTime = 0;
}


void loop()
{
  uint16_t OldPlasmaTime;

  HuePlasmaFrame(PlasmaTime);
  if (ScrollingMsg1.UpdateText() == -1)
    ScrollingMsg1.SetText((unsigned char *)PlasmaTxt, sizeof(PlasmaTxt) - 1);
  FastLED.show();
  delay(10);
  OldPlasmaTime = PlasmaTime;
  PlasmaTime += PlasmaShift;
  if (OldPlasmaTime > PlasmaTime)
    PlasmaShift = (random8(0, 5) * 32) + 64;
}


void HuePlasmaFrame(uint16_t Time)
{
  #define PLASMA_X_FACTOR  24
  #define PLASMA_Y_FACTOR  24
  int16_t r, h;
  int x, y;

  for (x=0; x<MATRIX_WIDTH; x++)
  {
    for (y=0; y<MATRIX_HEIGHT; y++)
    {
      r = sin16(Time) / 256;
      h = sin16(x * r * PLASMA_X_FACTOR + Time) + cos16(y * (-r) * PLASMA_Y_FACTOR + Time) + sin16(y * x * (cos16(-Time) / 256) / 2);
      LEDMatrix.MatrixXY(x, y) = CHSV((h / 256) + 128, 255, 255);
    }
  }
}

