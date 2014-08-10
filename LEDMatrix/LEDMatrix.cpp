/*
LEDMatrix V2 class by Aaron Liddiment (c) 2014

Used by my TextScroller class and uses the 
FastLED v2.1 library by Daniel Garcia and Mark Kriegsmann.
Inspiration for some of the Matrix functions by Stefan Petrick

Written & tested on a Teensy 3.1
*/

#include <FastLED.h>
#include <LEDMatrix.h>


cLEDMatrixBase::cLEDMatrixBase()
{
}

int cLEDMatrixBase::Size()
{
  return(m_Width * m_Height);
}

struct CRGB* cLEDMatrixBase::operator[](int n)
{
  return(&m_LED[n]);
}

struct CRGB& cLEDMatrixBase::operator()(int16_t x, int16_t y)
{
  if ( (x >= 0) && (x < m_Width) && (y >= 0) && (y < m_Height))
    return(m_LED[mXY(x, y)]);
  else
    return(m_OutOfBounds);
}

void cLEDMatrixBase::HorizontalMirror(bool FullHeight)
{
  int ty, y, x, xx;

  if (FullHeight)
    ty = m_Height - 1;
  else
    ty = (m_Height / 2);
  for (y=ty; y>=0; --y)
  {
    for (x=(m_Width/2)-1,xx=((m_Width+1)/2); x>=0; --x,++xx)
      m_LED[mXY(xx, y)] = m_LED[mXY(x, y)];
  }
}


void cLEDMatrixBase::VerticalMirror()
{
  int y, yy, x;

  for (y=(m_Height/2)-1,yy=((m_Height+1)/2); y>=0; --y,++yy)
  {
    for (x=m_Width-1; x>=0; --x)
      m_LED[mXY(x, yy)] = m_LED[mXY(x, y)];
  }
}


void cLEDMatrixBase::QuadrantMirror()
{
  HorizontalMirror(false);
  VerticalMirror();
}


void cLEDMatrixBase::QuadrantRotateMirror()
{
  int MaxXY, MidXY, x, y, src;

  if (m_Width > m_Height)
    MaxXY = m_Height;
  else
    MaxXY = m_Width;
  MidXY = (MaxXY / 2);
  MaxXY--;
  for (x=MidXY-(MaxXY%2); x>=0; --x)
  {
    for (y=MidXY-(MaxXY%2); y>=0; --y)
    {
      src = mXY(x, y);
      m_LED[mXY(MidXY + y, MidXY - (MaxXY % 2) - x)] = m_LED[src];
      m_LED[mXY(MaxXY - x, MaxXY - y)] = m_LED[src];
      m_LED[mXY(MidXY - (MaxXY % 2) - y, MidXY + x)] = m_LED[src];
    }
  }
}


void cLEDMatrixBase::TriangleTopMirror(bool FullHeight)
{
  int MaxXY, x, y;

  if (m_Width > m_Height)
    MaxXY = m_Height - 1;
  else
    MaxXY = m_Width - 1;
  if (! FullHeight)
    MaxXY /= 2;
  for (y=1; y<=MaxXY; ++y)
  {
    for (x=0; x<y; ++x)
      m_LED[mXY(y,x)] = m_LED[mXY(x,y)];
  }
}


void cLEDMatrixBase::TriangleBottomMirror(bool FullHeight)
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
    for (x=MaxXY-y-1,yy=y+1; x>=0; --x,++yy)
      m_LED[mXY(xx, yy)] = m_LED[mXY(x, y)];
  }
}


void cLEDMatrixBase::QuadrantTopTriangleMirror()
{
  TriangleTopMirror(false);
  QuadrantMirror();
}


void cLEDMatrixBase::QuadrantBottomTriangleMirror()
{
  TriangleBottomMirror(false);
  QuadrantMirror();
}
