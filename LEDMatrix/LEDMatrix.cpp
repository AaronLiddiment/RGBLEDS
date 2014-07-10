/*
LEDMatrix class by Aaron Liddiment (c) 2014

Used by my TextScroller class and uses the 
FastLED v2.1 library by Daniel Garcia and Mark Kriegsmann.

Written & tested on a Teensy 3.1
*/

#include <FastLED.h>
#include <LEDMatrix.h>


cLEDMatrix::cLEDMatrix(uint16_t Width, uint16_t Height, bool ZigZag, CRGB *LAry)
{
  m_Width = Width;
  m_Height = Height;
  m_XMax = Width - 1;
  m_ZigZag = ZigZag;
  m_MatrixLeds = LAry;
}


void cLEDMatrix::HorizontalMirror(bool FullHeight)
{
	int i, x, y;

	if (FullHeight)
		y = m_Height - 1;
	else
		y = (m_Height / 2);
	for (i=(y*m_Width); y>=0; y--,i-=m_Width)
	{
		for (x=(m_Width/2)-1; x>=0; x--)
		{
			if ((m_ZigZag) && (y % 2))
				m_MatrixLeds[i + x] = m_MatrixLeds[i + m_XMax - x];
			else
				m_MatrixLeds[i + m_XMax - x] = m_MatrixLeds[i + x];
		}
	}
}


void cLEDMatrix::VerticalMirror()
{
	int i, x, offset;

	for (offset=((m_Height-1)*m_Width),i=0; offset>0; offset-=(m_Width*2),i+=m_Width)
	{
		for (x=m_Width-1; x>=0; x--)
		{
			if (m_Height % 2)
				m_MatrixLeds[i + offset + x] = m_MatrixLeds[i + x];
			else
			{
				if (i % 2)
					m_MatrixLeds[i + offset + x] = m_MatrixLeds[i + m_XMax - x];
				else
					m_MatrixLeds[i + offset + m_XMax - x] = m_MatrixLeds[i + x];
			}
		}
	}
}


void cLEDMatrix::QuadrantMirror()
{
	HorizontalMirror(false);
	VerticalMirror();
}


struct CRGB &cLEDMatrix::MatrixXY(int16_t x, int16_t y)
{
  if ( (x >= 0) && (x < m_Width) && (y >= 0) && (y < m_Height))
  {
	if ((m_ZigZag) && (y % 2))
      return(m_MatrixLeds[(((y + 1) * m_Width) - 1) - x]);
    else
      return(m_MatrixLeds[(y * m_Width) + x]);
  }
  else
    return(m_OutOfBounds);
}
