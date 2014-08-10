/*
TextScroller V2 class by Aaron Liddiment (c) 2014

Uses my LEDMatrix class and especially the 
FastLED v2.1 library by Daniel Garcia and Mark Kriegsmann.

Written & tested on a Teensy 3.1
Even the basic examples need 19k rom & 5k ram 
*/

#include <FastLED.h>
#include <LEDMatrix.h>
#include <TextScroller.h>

#define  BACKGND_MASK    (BACKGND_ERASE | BACKGND_LEAVE | BACKGND_DIMMING)
#define  CHAR_MASK       (CHAR_UP | CHAR_DOWN | CHAR_LEFT | CHAR_RIGHT)
#define  SCROLL_MASK     (SCROLL_LEFT | SCROLL_RIGHT | SCROLL_UP | SCROLL_DOWN)
#define  COLR_MASK       (COLR_RGB | COLR_HSV | COLR_SINGLE | COLR_GRAD | COLR_CHAR | COLR_AREA | COLR_VERT | COLR_HORI | COLR_EMPTY | COLR_DIMMING)

#define  UC_BACKGND_ERASE      0x80
#define  UC_BACKGND_LEAVE      0x81
#define  UC_BACKGND_DIMMING    0x82
#define  UC_FRAME_RATE         0x84
#define  UC_DELAY_FRAMES       0x85

#define  UC_SCROLL_LEFT        0x90
#define  UC_SCROLL_RIGHT       0x91
#define  UC_SCROLL_UP          0x92
#define  UC_SCROLL_DOWN        0x93

#define  UC_CHAR_UP            0xa0
#define  UC_CHAR_DOWN          0xa1
#define  UC_CHAR_LEFT          0xa2
#define  UC_CHAR_RIGHT         0xa3

#define  UC_RGB                0xc0
#define  UC_HSV                0xc1
#define  UC_RGB_CV             0xc2
#define  UC_HSV_CV             0xc3
#define  UC_RGB_AV             0xc6
#define  UC_HSV_AV             0xc7
#define  UC_RGB_CH             0xca
#define  UC_HSV_CH             0xcb
#define  UC_RGB_AH             0xce
#define  UC_HSV_AH             0xcf
#define  UC_COLR_EMPTY         0xd0
#define  UC_COLR_DIMMING       0xe0


void cTextScroller::SetFont(uint8_t FontW, uint8_t FontH, uint8_t ChBase, uint8_t ChUpper, const uint8_t *FontData)
{
  m_FontWidth = FontW;
  m_FontHeight = FontH;
  m_FontBase = ChBase;
  m_FontUpper = ChUpper;
  m_FontData = FontData;
}


void cTextScroller::Init(cLEDMatrixBase *Matrix, uint16_t Width, uint16_t Height, int16_t OriginX, int16_t OriginY)
{
  m_Matrix = Matrix;
  m_XMin = OriginX;
  m_YMin = OriginY;
  m_XMax = OriginX + Width - 1;
  m_YMax = OriginY + Height - 1;
  m_Options = (BACKGND_ERASE | CHAR_UP | SCROLL_LEFT | COLR_RGB);
  m_Col1[0] = m_Col1[1] = m_Col1[2] = 255;
  m_LastDelayTP = m_DelayCounter = 0;
  m_FrameRate = 0;
  Initialised = true;
}


void cTextScroller::SetBackgroundMode(uint16_t Options, uint8_t Dimming)
{
  m_Options = (m_Options & (~BACKGND_MASK)) | (Options & BACKGND_MASK);
	if ((m_Options & BACKGND_MASK) == BACKGND_DIMMING)
		m_BackDim = Dimming;
}


void cTextScroller::SetScrollDirection(uint16_t Options)
{
  m_Options = (m_Options & (~SCROLL_MASK)) | (Options & SCROLL_MASK);
}


void cTextScroller::SetTextDirection(uint16_t Options)
{
  m_Options = (m_Options & (~CHAR_MASK)) | (Options & CHAR_MASK);
}


void cTextScroller::SetTextColrOptions(uint16_t Options, uint8_t ColA1, uint8_t ColA2, uint8_t ColA3, uint8_t ColB1, uint8_t ColB2, uint8_t ColB3)
{
  m_Options = (m_Options & (~COLR_MASK)) | (Options & COLR_MASK);
  if ((m_Options & COLR_EMPTY) != COLR_EMPTY)
  {
	  if ((m_Options & COLR_DIMMING) == COLR_DIMMING)
	  	m_ColDim = ColA1;
	  else
  	{
		  if ((m_Options & COLR_GRAD) == COLR_GRAD)
		  {
		  	m_Col2[0] = ColB1;
		  	m_Col2[1] = ColB2;
		  	m_Col2[2] = ColB3;
		  }
	  	m_Col1[0] = ColA1;
	  	m_Col1[1] = ColA2;
	  	m_Col1[2] = ColA3;
  	}
  }
}


void cTextScroller::SetText(unsigned char *Txt, uint16_t TxtSize)
{
  m_pText = Txt;
  m_pSize = TxtSize;
  m_TextPos = m_EOLtp = m_XBitPos = m_YBitPos = 0;
  m_LastDelayTP = 0;
  Initialised = true;
}


void cTextScroller::DecodeOptions(uint16_t *tp, uint16_t *opt, uint8_t *backDim, uint8_t *col1, uint8_t *col2, uint8_t *colDim)
{
  switch (m_pText[*tp])
  {
    case UC_BACKGND_ERASE:
    case UC_BACKGND_LEAVE:
      *opt = (*opt & (~BACKGND_MASK)) | ((uint16_t)m_pText[*tp] & BACKGND_MASK);
      break;
    case UC_BACKGND_DIMMING:
      *opt = (*opt & (~BACKGND_MASK)) | ((uint16_t)m_pText[*tp] & BACKGND_MASK);
      *backDim = (uint8_t)m_pText[*tp + 1];
      *tp += 1;
      break;
    case UC_FRAME_RATE:
      if ((m_XBitPos == 0) && (m_YBitPos == 0))
        m_FrameRate = (uint8_t)m_pText[*tp + 1];
      *tp += 1;
      break;
    case UC_DELAY_FRAMES:
      if ( ((m_XBitPos == 0) && (m_YBitPos == 0)) && (m_LastDelayTP != *tp) )
      {
        m_LastDelayTP = *tp;
        m_DelayCounter = (m_pText[*tp + 1] << 8) + m_pText[*tp + 2];
      }
      *tp += 2;
      break;
    case UC_CHAR_UP:
    case UC_CHAR_DOWN:
    case UC_CHAR_LEFT:
    case UC_CHAR_RIGHT:
      *opt = (*opt & (~CHAR_MASK)) | (((uint16_t)m_pText[*tp] << 2) & CHAR_MASK);
      break;
    case UC_SCROLL_LEFT:
    case UC_SCROLL_RIGHT:
    case UC_SCROLL_UP:
    case UC_SCROLL_DOWN:
      *opt = (*opt & (~SCROLL_MASK)) | (((uint16_t)m_pText[*tp] << 4) & SCROLL_MASK);
      break;
    case UC_RGB:
    case UC_HSV:
    case UC_RGB_CV:
    case UC_HSV_CV:
    case UC_RGB_AV:
    case UC_HSV_AV:
    case UC_RGB_CH:
    case UC_HSV_CH:
    case UC_RGB_AH:
    case UC_HSV_AH:
    case UC_COLR_EMPTY:
    case UC_COLR_DIMMING:
      *opt = (*opt & (~COLR_MASK)) | (((uint16_t)m_pText[*tp] << 6) & COLR_MASK);
      if ((*opt & COLR_EMPTY) != COLR_EMPTY)
      {
        if ((*opt & COLR_DIMMING) == COLR_DIMMING)
        {
          *colDim = (uint8_t)m_pText[*tp + 1];
          *tp += 1;
        }
        else
        {
          col1[0] = m_pText[*tp + 1];
          col1[1] = m_pText[*tp + 2];
          col1[2] = m_pText[*tp + 3];
          *tp += 3;
          if ((*opt & COLR_GRAD) == COLR_GRAD)
          {
            col2[0] = m_pText[*tp + 1];
            col2[1] = m_pText[*tp + 2];
            col2[2] = m_pText[*tp + 3];
            *tp += 3;
          }
        }
      }
      break;
   }
}


int cTextScroller::UpdateText()
{
	uint8_t bDim, cDim, c1[3], c2[3], xbp;
	int16_t x, y, MinY, MaxY;
	uint16_t opt, tp;

	if (m_TextPos >= m_pSize)
		return(-1);
	uint16_t MfractAV = 65535 / ((m_YMax - m_YMin) + 1);
	uint16_t MfractAH = 65535 / ((m_XMax - m_XMin) + 1);
	if (m_DelayCounter == 0)
	{
		if (Initialised == true)
			Initialised = false;
		else
		{
			if (((m_Options & SCROLL_MASK) == SCROLL_LEFT) || ((m_Options & SCROLL_MASK) == SCROLL_RIGHT))
			{
				m_XBitPos++;
				if ( ((m_Options & CHAR_MASK) == CHAR_UP) || ((m_Options & CHAR_MASK) == CHAR_DOWN) )
				{
					if (m_XBitPos > m_FontWidth)
						m_XBitPos = 0;
				}
				else
				{
					if (m_XBitPos > m_FontHeight)
						m_XBitPos = 0;
				}
				if (m_XBitPos == 0)
					m_EOLtp = m_TextPos + 1;
			}
			else if (((m_Options & SCROLL_MASK) == SCROLL_UP) || ((m_Options & SCROLL_MASK) == SCROLL_DOWN))
			{
				m_YBitPos++;
				if ( ((m_Options & CHAR_MASK) == CHAR_UP) || ((m_Options & CHAR_MASK) == CHAR_DOWN) )
				{
					if (m_YBitPos > m_FontHeight)
						m_YBitPos = 0;
				}
				else
				{
					if (m_YBitPos > m_FontWidth)
						m_YBitPos = 0;
				}
			}
		}
		if ((m_XBitPos == 0) && (m_YBitPos == 0))
		{
			while ( (m_TextPos < m_pSize) && ((m_TextPos < m_EOLtp) || (m_pText[m_TextPos] > m_FontUpper)) )
			{
				if (m_pText[m_TextPos] > m_FontUpper)
					DecodeOptions(&m_TextPos, &m_Options, &m_BackDim, m_Col1, m_Col2, &m_ColDim);
				m_TextPos++;
			}
		}
	}
	if ( ((m_Options & SCROLL_MASK) != SCROLL_RIGHT) && (m_TextPos >= m_pSize) )
		return(-1);
	tp = m_TextPos;
	opt = m_Options;
	bDim = m_BackDim;
	memcpy(c1, m_Col1, sizeof(c1));
	memcpy(c2, m_Col2, sizeof(c2));
	cDim = m_ColDim;
	if ((opt & SCROLL_MASK) == SCROLL_DOWN)
		MinY = m_YMin - m_YBitPos;
	else
		MaxY = m_YMax + m_YBitPos;
	m_EOLtp = 0;
	do
	{
		xbp = m_XBitPos;
		if ((opt & SCROLL_MASK) == SCROLL_RIGHT)
			x = m_XMax;
		else
			x = m_XMin;
		do
		{
			if ( (tp < m_pSize) && ((m_pText[tp] < m_FontBase) || (m_pText[tp] > m_FontUpper)) )
			{
				uint16_t oldopt = opt;
				DecodeOptions(&tp, &opt, &bDim, c1, c2, &cDim);
				tp++;
				if ((oldopt & SCROLL_MASK) != (opt & SCROLL_MASK))
				{
					m_EOLtp = tp;
					tp = m_pSize;
					opt = oldopt;
				}
			}
			else
			{
				uint8_t xbpmax, bf, xgap;
				uint16_t fdo = (m_pText[tp] - m_FontBase) * m_FontHeight;
				if ( ((opt & CHAR_MASK) == CHAR_UP) || ((opt & CHAR_MASK) == CHAR_DOWN) )
				{
					if ((opt & SCROLL_MASK) == SCROLL_DOWN)
						MaxY = MinY + m_FontHeight;
					else
						MinY = MaxY - m_FontHeight;
					xbpmax = m_FontWidth;
					if ((opt & CHAR_MASK) == CHAR_UP)
					{
						if ((opt & SCROLL_MASK) == SCROLL_RIGHT)
							bf = 1 << (xbp - 1);
						else
							bf = 1 << (m_FontWidth - xbp - 1);
						fdo += (m_FontHeight - 1);
					}
					else
					{
						if ((opt & SCROLL_MASK) == SCROLL_RIGHT)
							bf = 1 << (m_FontWidth - xbp);
						else
							bf = 1 << xbp;
					}
				}
				else
				{
					if ((opt & SCROLL_MASK) == SCROLL_DOWN)
						MaxY = MinY + m_FontWidth;
					else
						MinY = MaxY - m_FontWidth;
					xbpmax = m_FontHeight;
					if ((opt & CHAR_MASK) == CHAR_LEFT)
					{
						bf = 1 << (m_FontWidth - 1);
						if ((opt & SCROLL_MASK) == SCROLL_RIGHT)
							fdo += (m_FontHeight - xbp);
						else
							fdo += xbp;
					}
					else
					{
						bf = 0x01;
						if ((opt & SCROLL_MASK) == SCROLL_RIGHT)
							fdo += (xbp - 1);
						else
							fdo += (m_FontHeight - xbp - 1);
					}
				}
				if ( ((opt & SCROLL_MASK) == SCROLL_RIGHT) || (tp >= m_pSize) )
					xgap = 0;
				else
					xgap = xbpmax;
				uint16_t MfractCV = 65535 / (MaxY - MinY);
				uint16_t MfractCH = 65535 / xbpmax;
				y = MinY - 1;
				while (y <= MaxY)
				{
					if ((y >= m_YMin) && (y <= m_YMax))
					{
						if ( (xbp != xgap) && (y >= MinY) && (y < MaxY) && ((m_FontData[fdo] & bf) != 0x00) )
						{
							if ((opt & COLR_MASK) != COLR_EMPTY)
							{
								if ((opt & COLR_MASK) == COLR_DIMMING)
									(*m_Matrix)(x, y).nscale8(cDim);
								else
								{
									uint8_t v[3];
									if ((opt & COLR_GRAD) == COLR_SINGLE)
										memcpy(v, c1, sizeof(v));
									else
									{
										uint16_t fract;
										if ((opt & (COLR_AREA | COLR_HORI)) == (COLR_CHAR | COLR_VERT))
											fract = (y - MinY) * MfractCV;
										else if ((opt & (COLR_AREA | COLR_HORI)) == (COLR_AREA | COLR_VERT))
											fract = (y - m_YMin) * MfractAV;
										else if ((opt & (COLR_AREA | COLR_HORI)) == (COLR_CHAR | COLR_HORI))
											fract = xbp * MfractCH;
										else if ((opt & (COLR_AREA | COLR_HORI)) == (COLR_AREA | COLR_HORI))
											fract = (x - m_XMin) * MfractAH;
										for (int i=0; i<3; i++)
										{
											if (c1[i] <= c2[i])
												v[i] = lerp16by16(c1[i]<<8, c2[i]<<8, fract) >> 8;
											else
												v[i] = lerp16by16(c2[i]<<8, c1[i]<<8, ~fract) >> 8;
										}
									}
									if ((opt & COLR_HSV) == COLR_RGB)
										(*m_Matrix)(x, y) = CRGB(v[0], v[1], v[2]);
									else
										(*m_Matrix)(x, y) = CHSV(v[0], v[1], v[2]);
								}
							}
						}
						else if ((opt & BACKGND_MASK) == BACKGND_ERASE)
							(*m_Matrix)(x, y) = CRGB(0, 0, 0);
						else if ((opt & BACKGND_MASK) == BACKGND_DIMMING)
							(*m_Matrix)(x, y).nscale8(bDim);
					}
					if ((y >= MinY) && (xbp != xgap))
					{
						if ((opt & CHAR_MASK) == CHAR_UP)
							--fdo;
						else if ((opt & CHAR_MASK) == CHAR_DOWN)
							++fdo;
						else if ((opt & CHAR_MASK) == CHAR_LEFT)
							bf >>= 1;
						else if ((opt & CHAR_MASK) == CHAR_RIGHT)
							bf <<= 1;
					}
					++y;
				}
				++xbp;
				if (xbp > xbpmax)
				{
					xbp = 0;
					++tp;
				}
				if ((opt & SCROLL_MASK) == SCROLL_RIGHT)
				{
					--x;
					if ((tp == m_pSize) && (xbp > 0))
						++tp;
				}
				else
				{
					++x;
					if (tp == m_pSize)
						++tp;
				}
			}
		}
		while ((x >= m_XMin) && (x <= m_XMax) && (tp <= m_pSize));
		if (xbp != 0)
			++tp;
		if (m_EOLtp == 0)
			m_EOLtp = tp;
		if ((opt & SCROLL_MASK) == SCROLL_DOWN)
			y = MinY = MaxY + 1;
		else if ((opt & SCROLL_MASK) == SCROLL_UP)
			y = MaxY = MinY - 1;
		else
			y = m_YMin - 1;
	}
	while ((y >= m_YMin) && (y <= m_YMax) && (tp < m_pSize));
	if ((xbp == 0) && (tp < m_pSize) && ((m_pText[tp] < m_FontBase) || (m_pText[tp] > m_FontUpper)) )
		DecodeOptions(&tp, &opt, &bDim, c1, c2, &cDim);
	if (m_DelayCounter > 0)
		m_DelayCounter--;
	else if (m_FrameRate > 0)
		m_DelayCounter = m_FrameRate;
	return(0);
}
