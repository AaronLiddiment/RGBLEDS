/*
LEDMatrix class by Aaron Liddiment (c) 2014

Used by my TextScroller class and uses the 
FastLED v2.1 library by Daniel Garcia and Mark Kriegsmann.
Inspiration for some of the Matrix functions by Stefan Petrick

Written & tested on a Teensy 3.1
*/

#include <FastLED.h>
#include <LEDMatrix.h>


inline uint16_t cLEDMatrix::mXY(int16_t x, int16_t y)
{
  if (m_Type & 1)
  { // Vertical Matrix
    if ((m_Type & 2) && (x % 2))
      return((((x + 1) * m_Height) - 1) - y);
    else
      return((x * m_Height) + y);
  }
  else
  { // Horizontal Matrix
    if ((m_Type & 2) && (y % 2))
      return((((y + 1) * m_Width) - 1) - x);
    else
      return((y * m_Width) + x);
  }
}


cLEDMatrix::cLEDMatrix(uint16_t Width, uint16_t Height, MatrixType_t Type, CRGB *LAry)
{
  m_Width = Width;
  m_Height = Height;
  m_Type = Type;
  m_MatrixLeds = LAry;
}


struct CRGB &cLEDMatrix::MatrixXY(int16_t x, int16_t y)
{
  if ( (x >= 0) && (x < m_Width) && (y >= 0) && (y < m_Height))
    return(m_MatrixLeds[mXY(x,y)]);
  else
    return(m_OutOfBounds);
}


void cLEDMatrix::HorizontalMirror(bool FullHeight)
{
  int ty, y, x, xx;

  if (FullHeight)
    ty = m_Height - 1;
  else
    ty = (m_Height / 2);
  for (y=ty; y>=0; y--)
  {
    for (x=(m_Width/2)-1,xx=((m_Width+1)/2); x>=0; x--,xx++)
      m_MatrixLeds[mXY(xx, y)] = m_MatrixLeds[mXY(x, y)];
  }
}


void cLEDMatrix::VerticalMirror()
{
  int y, yy, x;

  for (y=(m_Height/2)-1,yy=((m_Height+1)/2); y>=0; y--,yy++)
  {
    for (x=m_Width-1; x>=0; x--)
      m_MatrixLeds[mXY(x, yy)] = m_MatrixLeds[mXY(x, y)];
  }
}


void cLEDMatrix::QuadrantMirror()
{
  HorizontalMirror(false);
  VerticalMirror();
}


void cLEDMatrix::QuadrantRotateMirror()
{
  int MaxXY, MidXY, x, y, xx, yy, src;

  if (m_Width > m_Height)
    MaxXY = m_Height;
  else
    MaxXY = m_Width;
  MidXY = (MaxXY / 2);
  MaxXY--;
  for (x=MidXY-(MaxXY%2); x>=0; x--)
  {
    for (y=MidXY-(MaxXY%2); y>=0; y--)
    {
      src = mXY(x, y);
      xx = MidXY + y;
      yy = MidXY - (MaxXY % 2) - x;
      m_MatrixLeds[mXY(xx, yy)] = m_MatrixLeds[src];
      xx = MaxXY - x;
      yy = MaxXY - y;
      m_MatrixLeds[mXY(xx, yy)] = m_MatrixLeds[src];
      xx = MidXY - (MaxXY % 2) - y;
      yy = MidXY + x;
      m_MatrixLeds[mXY(xx, yy)] = m_MatrixLeds[src];
    }
  }
}


void cLEDMatrix::TriangleTopMirror(bool FullHeight)
{
  int MaxXY, x, y;

  if (m_Width > m_Height)
    MaxXY = m_Height - 1;
  else
    MaxXY = m_Width - 1;
  if (! FullHeight)
    MaxXY /= 2;
  for (y=1; y<=MaxXY; y++)
  {
    for (x=0; x<y; x++)
      m_MatrixLeds[mXY(y,x)] = m_MatrixLeds[mXY(x,y)];
  }
}


void cLEDMatrix::TriangleBottomMirror(bool FullHeight)
{
  int MaxXY, x, y, xx, yy;

  if (m_Width > m_Height)
    MaxXY = m_Height - 1;
  else
    MaxXY = m_Width - 1;
  if (! FullHeight)
    MaxXY /= 2;
  for (y=0,xx=MaxXY; y<MaxXY; y++,xx--)
  {
    for (x=MaxXY-y-1,yy=y+1; x>=0; x--,yy++)
      m_MatrixLeds[mXY(xx,yy)] = m_MatrixLeds[mXY(x,y)];
  }
}


void cLEDMatrix::QuadrantTopTriangleMirror()
{
  TriangleTopMirror(false);
  QuadrantMirror();
}


void cLEDMatrix::QuadrantBottomTriangleMirror()
{
  TriangleBottomMirror(false);
  QuadrantMirror();
}
