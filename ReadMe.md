cTextScroller Instructions
==========================


Overview
--------
The aim of this class is to provide a flexible way of displaying static/scrolling
text on LED displays using the FastLED library.
On each update it will fully render the visible window rather than shifting and
updating just the new data, this makes it a frame based system allowing text to
be overlaid on other patterns/effects.
There is currently a limit on the font width to 8 pixels but the height is not
limited.



Initialise
----------
In order to use the class you must have the following header files included in your program:-

	#include <FastLED.h>
	#include <LEDMatrix.h>
	#include <TextScroller.h>

You will also need some font data, which due to its size, is generally better to be
placed in a header file. I have provided 2 font header files as a start. These are
FontMatrise.h and FontRobotron.h and will be covered further in the Functions bit.

You must then declare an instance of the cLEDMatrix class as this is used to modify
the LED data according to the matrix dimensions and layout. Personally I use defines
to make this clearer in the code, such as:-

	#define MATRIX_WIDTH   68
	#define MATRIX_HEIGHT  7
	#define MATRIX_ZIGZAG  true
	#define NUM_LEDS       (MATRIX_WIDTH * MATRIX_HEIGHT)

You also have to declare the FastLED LED array, as a pointer to this array is required
by the cLEDMatrix class:-

	CRGB leds[NUM_LEDS];
	cLEDMatrix LEDMatrix(MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_ZIGZAG, leds);

Then finally you can declare the cTextScroller variable:-

	cTextScroller ScrollingMsg;



Functions
=========

SetFont(uint8_t FontW, uint8_t FontH, uint8_t ChBase, uint8_t ChUpper, const uint8_t *FontData)
-----------------------------------------------------------------------------------------------
This function should be called as part of your setup routine and must be called at least once
for each instance of cTextScroller, but may be called at any time to change the font.
I have provided 2 font header files to get you started as mentioned above. Example:-
	#include <FontMatrise.h>
	ScrollingMsg.SetFont(MATRISE_WIDTH, MATRISE_HEIGHT, MATRISE_CHAR_LOW, MATRISE_CHAR_HIGH, MatriseData);


Init(cLEDMatrix *Matrix, int16_t Width, int16_t Height, int16_t OriginX = 0, int16_t OriginY = 0, uint16_t Flags = (BACKGND_ERASE | CHAR_UP | SCROLL_LEFT | COLR_RGB))
----------------------------------------------------------------------------------------------------------------------------------------------------------------------
This function should also be called as part of your setup routine and again must be called at
least once for each instance of cTextScroller.
'Matrix' parameter should be a pointer to your cLEDMatrix instance.
'Width' & 'Height' are the dimensions of the area in which the text will be displayed.
'OriginX' & 'OriginY' are the matrix coordinates of the bottom left corner of the area.
'Flags' allows you to set a few options to start with such as Background mode, Character
direction and Scroll direction. These options are listed in the first part of the TextScoller
header file.
NOTE: The dimensions & origin provided here are independent of the LED Matrix size, they can
be smaller to restrict text to a portion of the display or larger to allow control over where
the text stops when using Delay control codes. The default colour mode is bright white, RGB.
Example:-
	ScrollingMsg.Init(&LEDMatrix, MESSAGE_WIDTH, MESSAGE_HEIGHT, MESSAGE_X, MESSAGE_Y);


SetText(unsigned char *Txt, uint16_t TxtSize)
---------------------------------------------
This function is called to set the unsigned 8 bit array that contains the text and control codes.
The 'TxtSize' parameter is needed as zero values can occur in the array when using control codes.
NOTE: If your text array is statically initialised, use the 'const' keyword in its delcaration
as the compiler will place this in the ROM area rather then RAM.
Example:-
	const unsigned char TxtDemo[] = { "THIS IS A MESSAGE" };
	ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);


int UpdateText()
----------------
This function is called repeatedly to render the text data into the LED matrix. Each call
advances the frame by 1 pixel in the chosen scroll direction.
If the frame has been rendered the return code will be '0'.
If the end of the Text Array has been reached it will return '-1', at this point you will then
need to do another 'SetText' call to reset the text array followed by a call to 'UpdateText'
to render the frame.
Example:-
	if (ScrollingMsg.UpdateText() == -1)
	{
		ScrollingMsg.SetText((unsigned char *)TxtDemo, sizeof(TxtDemo) - 1);
		ScrollingMsg.UpdateText();
	}



Text Array & Control Codes
==========================

The text array obviously contains the text you wish to see displayed but can also contain
control codes which will allow to change coours, character direction, scrolling direction,
frame rate, delays & background mode.
The control codes you can use are defined in the TextScroller.h header file and all start
with "EFFECT_". If there are parameters for a control code they are in hexadecimal.

EFFECT_BACKGND_ERASE
This will change the background mode so that all the '0' bits in the font data cause the
corresponding LED's within the initialised message area to be turned off.

EFFECT_BACKGND_LEAVE
This will change the background mode so that all the '0' bits in the font data will leave
the corresponding LED's unmodified.

EFFECT_BACKGND_DIMMING "\xNN"
This will change the background mode so that all the '0' bits in the font data will have
the corresponding LED's brightness scaled down by NN 256ths using FastLED's nscale8_video.

EFFECT_FRAME_RATE "\xNN"
This sets a rate for the frame animation in that if NN is 04, it will be every 5th call to
UpdateText() that the pixel position is moved. The default Init() value is 0 which means
every call will update the pixel position.

EFFECT_DELAY_FRAMES "\xHH\xLL"
This will pause any frame animation for ((HH * 256) + LL) calls to UpdateText().

EFFECT_SCROLL_LEFT
EFFECT_SCROLL_RIGHT
EFFECT_SCROLL_UP
EFFECT_SCROLL_DOWN
This will cause the scroll direction to change. Currently on a change of scroll direction
any exisitng text will be scrolled off the message area before the new direction is
applied. 

EFFECT_CHAR_UP
EFFECT_CHAR_DOWN
EFFECT_CHAR_LEFT
EFFECT_CHAR_RIGHT
This changes the font rendering to be in the chosen direction.

EFFECT_RGB "\xRR\xGG\xBB"
EFFECT_HSV "\xHH\xSS\xVV"
Changes the solid colour of the text using either RGB or HSV method.

EFFECT_RGB_CV "\xRR\xGG\xBB\xRR\xGG\xBB"
EFFECT_HSV_CV "\xHH\xSS\xVV\xHH\xSS\xVV"
Sets a graduated colour range for the text over a single character height.
The first triple is the bottom and the second triple is the top.

EFFECT_RGB_AV "\xRR\xGG\xBB\xRR\xGG\xBB"
EFFECT_HSV_AV "\xHH\xSS\xVV\xHH\xSS\xVV"
Sets a graduated colour range for the text over the message area height.
The first triple is the bottom and the second triple is the top.

EFFECT_RGB_CH "\xRR\xGG\xBB\xRR\xGG\xBB"
EFFECT_HSV_CH "\xHH\xSS\xVV\xHH\xSS\xVV"
Sets a graduated colour range for the text over a single character width.
The first triple is the left side and the second triple is the right side.

EFFECT_RGB_AH "\xRR\xGG\xBB\xRR\xGG\xBB"
EFFECT_HSV_AH "\xHH\xSS\xVV\xHH\xSS\xVV"
Sets a graduated colour range for the text over the message area width.
The first triple is the left edge and the second triple is the right edge.

EFFECT_COLR_EMPTY
Any '1' bits in the font data will leave the corresponding LED's unmodified.
Tip - This is useless with EFFECT_BACKGND_LEAVE !

EFFECT_COLR_DIMMING "\xNN"
Any '1' bits in the font data will have the corresponding LED's brightness
scaled down by NN 256ths using FastLED's nscale8_video.
Tip - Useful only when you have data already in the LED array.


Example:-
const unsigned char TxtDemo[] = { EFFECT_SCROLL_LEFT "            LEFT SCROLL "
                                  EFFECT_SCROLL_RIGHT "            LLORCS THGIR"
                                  EFFECT_SCROLL_DOWN "            SCROLL DOWN             SCROLL DOWN            "
                                  EFFECT_FRAME_RATE "\x04" " SCROLL DOWN            "
                                  EFFECT_FRAME_RATE "\x00" " "
                                  EFFECT_SCROLL_UP "             SCROLL UP               SCROLL UP             "
                                  EFFECT_FRAME_RATE "\x04" "  SCROLL UP             "
                                  EFFECT_FRAME_RATE "\x00" " "
                                  EFFECT_CHAR_UP EFFECT_SCROLL_LEFT "            UP"
                                  EFFECT_CHAR_RIGHT "  RIGHT"
                                  EFFECT_CHAR_DOWN "  DOWN"
                                  EFFECT_CHAR_LEFT "  LEFT"
                                  EFFECT_HSV_CV "\x00\xff\xff\x40\xff\xff" EFFECT_CHAR_UP "           HSV_CV 00-40"
                                  EFFECT_HSV_CH "\x00\xff\xff\x40\xff\xff" "    HSV_CH 00-40"
                                  EFFECT_HSV_AV "\x00\xff\xff\x40\xff\xff" "    HSV_AV 00-40"
                                  EFFECT_HSV_AH "\x00\xff\xff\xff\xff\xff" "    HSV_AH 00-FF"
                                  "           "
                                  EFFECT_HSV "\x00\xff\xff" "R"
                                  EFFECT_HSV "\x20\xff\xff" "A"
                                  EFFECT_HSV "\x40\xff\xff" "I"
                                  EFFECT_HSV "\x60\xff\xff" "N"
                                  EFFECT_HSV "\xe0\xff\xff" "B"
                                  EFFECT_HSV "\xc0\xff\xff" "O"
                                  EFFECT_HSV "\xa0\xff\xff" "W"
                                  EFFECT_HSV "\x80\xff\xff" "S "
                                  EFFECT_DELAY_FRAMES "\x00\x96"
                                  EFFECT_RGB "\xff\xff\xff" };


NOTES
=====
1) Make sure you know how many characters fit across your message area, this will be usefull when planning your display.
2) If you want your display to start without any visible text, insert "display width" of spaces at the start of the array.
3) If your display area is taller than a single character height any partially rendered character at the right hand side
   will be skipped and the rendering will continue with the next whole character on the left hand side a "line" below.
   This is mainly useful for a SCROLL_UP/DOWN display. If multiple lines are required for a SCROLL_LEFT display it is
   neccessary to use two instances of cTextScroller placed one over the other with their own text arrays.
4) If you want left-right readable text when using SCROLL_RIGHT reverse the characters in your text array.
5) To just keep rendering static text, use SetText() & UpdateText() at the same time and ignore point 2).
6) I may in the future change the way a SCROLL direction change waits until all the text is off the display. This will
   make the class more flexible but will need you to add blank spaces before SCROLL changes in existing projects.
