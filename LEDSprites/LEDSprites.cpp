/*
LEDSprites V4 class by Aaron Liddiment (c) 2015

Inspiration came from my old C64 days :)

FastLED v3.1 library by Daniel Garcia and Mark Kriegsmann.
Written & tested on a Teensy 3.1 using Arduino V1.0.5r2 & teensyduino V1.20
*/

#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDSprites.h>

cSprite::cSprite(uint8_t Width, uint8_t Height, const uint8_t *Data, uint8_t NumFrames, SpriteNumBits_t BitsPixel, const struct CRGB *ColTable, const uint8_t *Mask)
{
  if (Data)
    Setup(Width, Height, Data, NumFrames, BitsPixel, ColTable, Mask);
}

void cSprite::Setup(uint8_t Width, uint8_t Height, const uint8_t *Data, uint8_t NumFrames, SpriteNumBits_t BitsPixel, const struct CRGB *ColTable, const uint8_t *Mask)
{
  m_Width = Width;
  m_Height = Height;
  m_Data = Data;
  m_NumFrames = NumFrames;
  m_BitsPixel = (uint8_t)BitsPixel;
  m_ColTable = ColTable;
  m_Mask = Mask;
  m_PrevSprite = m_NextSprite = NULL;
  m_X = m_Y = 0;
  m_Frame = m_FrameRate = m_XRate = m_YRate = m_CounterFrame = m_CounterX = m_CounterY = m_Options = m_Flags = 0;
  m_XChange = m_YChange = 0;
  m_NumCols = (1 << m_BitsPixel) - 1;
  m_MaskSize = ((m_Width + 7) / 8) * m_Height;
  m_FrameSize = m_MaskSize * m_BitsPixel;
}

void cSprite::Update()
{
  if (m_CounterFrame > 0)
  {
    if ((--m_CounterFrame) == 0)
    {
      m_CounterFrame = m_FrameRate;
      m_Frame++;
      if (m_Frame >= m_NumFrames)
        m_Frame = 0;
    }
  }
  if (m_CounterX > 0)
  {
    if ((--m_CounterX) == 0)
    {
      m_CounterX = m_XRate;
      if (m_Options & SPRITE_X_KEEPIN)
      {
        if ( ((m_Flags & SPRITE_EDGE_X_MIN) && (m_XChange < 0)) || ((m_Flags & SPRITE_EDGE_X_MAX) && (m_XChange > 0)) )
          m_XChange = 0 - m_XChange;
      }
      m_X += m_XChange;
    }
  }
  if (m_CounterY > 0)
  {
    if ((--m_CounterY) == 0)
    {
      m_CounterY = m_YRate;
      if (m_Options & SPRITE_Y_KEEPIN)
      {
        if ( ((m_Flags & SPRITE_EDGE_Y_MIN) && (m_YChange < 0)) || ((m_Flags & SPRITE_EDGE_Y_MAX) && (m_YChange > 0)) )
          m_YChange = 0 - m_YChange;
      }
      m_Y += m_YChange;
    }
  }
}

boolean cSprite::Combine(int16_t dx, int16_t dy, cSprite *Src)
{
	// Purely added to make Tetris example! Note that the colour tables have to be the same!
  uint32_t sfm2, sfm1, sfd2, sfd1;
  uint8_t *dstData, *dstMask;

  if (m_BitsPixel != Src->m_BitsPixel)
    return(false);
  else
  {
    uint8_t *SrcData = (uint8_t *)&(Src->m_Data[Src->m_Frame * Src->m_FrameSize]);
    uint8_t *DstData = (uint8_t *)&(m_Data[(m_Frame * m_FrameSize) + ((dx / 8) * m_BitsPixel) + (((m_Height - Src->m_Height) - dy) * ((m_Width + 7) / 8) * m_BitsPixel)]);
    uint8_t *SrcMask = (uint8_t *)&(Src->m_Mask[Src->m_Frame * Src->m_MaskSize]);
    uint8_t *DstMask = (uint8_t *)&(m_Mask[(m_Frame * m_MaskSize) + (dx / 8) + (((m_Height - Src->m_Height) - dy) * ((m_Width + 7) / 8))]);
    int8_t sm = dx % 8;
    int8_t sd = sm * m_BitsPixel;

    for (int k=Src->m_Height; k>0; --k)
    {
      dstData = DstData;
      dstMask = DstMask;
      for (int j=0; j<Src->m_Width; j+=8)
      {
        sfd1 = 0;
        for (int k=m_BitsPixel; k>0; --k)
          sfd1 = (sfd1 << 8) | *SrcData++;
        if (sfd1 != 0)
        {
          sfd2 = sfd1 << ((m_BitsPixel * 8) - sd);
          sfd1 >>= sd;
          for (int i=m_BitsPixel-1; i>=0; --i)
          {
            dstData[i] |= (uint8_t)sfd1;
            sfd1 >>= 8;
            if (sfd2 != 0)
            {
              dstData[m_BitsPixel + i] |= (uint8_t)sfd2;
              sfd2 >>= 8;
            }
          }
        }
        sfm1 = *SrcMask++;
        if (sfm1 != 0)
        {
          sfm2 = sfm1 << (8 - sm);
          sfm1 >>= sm;
          dstMask[0] |= (uint8_t)sfm1;
          if (sfm2 > 0)
            dstMask[1] |= (uint8_t)sfm2;
        }
        dstData = &dstData[m_BitsPixel];
        dstMask++;
      }
      DstData = &DstData[((m_Width + 7) / 8) * m_BitsPixel];
      DstMask = &DstMask[(m_Width + 7) / 8];
    }
    return(true);
  }
}

void cSprite::Render(cLEDMatrixBase *Matrix)
{
  int16_t y = m_Y + m_Height - 1;
  uint8_t *sprframedata = (uint8_t *)&(m_Data[m_Frame * m_FrameSize]);
  for (int i=m_Height; i>0; --i,--y)
  {
    int16_t x = m_X;
    for (int j=0; j<m_Width; j+=8)
    {
      uint32_t sfd = 0;
      for (int k=m_BitsPixel; k>0; --k)
        sfd = (sfd << 8) | *sprframedata++;
      for (int k=min(m_Width-j, 8),sm=m_BitsPixel*7; k>0; --k,sm-=m_BitsPixel,++x)
      {
        uint8_t col = (sfd >> sm) & m_NumCols;
        if (col > 0)
          (*Matrix)(x, y) = m_ColTable[col - 1];
      }
    }
  }
}

void cSprite::EdgeDetect(cLEDMatrixBase *Matrix)
{
  bool onXMatrix = false, onYMatrix = false, onEdgeXMin = false, onEdgeXMax = false, onEdgeYMin = false, onEdgeYMax = false;
  if ((m_X + m_Width) <= 0)
    onEdgeXMin = true;
  else if (m_X >= Matrix->Width())
    onEdgeXMax = true;
  else
  {
    uint8_t *Mask = (uint8_t *)&m_Mask[m_Frame * m_MaskSize];
    uint8_t XCnt = (m_Width + 7) / 8;
    int16_t xoff;
    if (m_X <= 0)
      xoff = max(0, 0 - m_X);
    else
      xoff = max(0, (Matrix->Width() - 1) - m_X);
    uint8_t XBit = xoff % 8;
    uint8_t XMid = xoff / 8;
    for (int16_t y=(m_Y+m_Height)-1; y>=m_Y; --y)
    {
      if ((y >= 0) && (y < Matrix->Height()))
      {
        for (uint8_t x=0; x<XCnt; ++x)
        {
          if ( ((x < XMid) && (Mask[x])) || ((x == XMid) && (Mask[x] & (0xff << (7 - XBit)))) )
          {
            if (m_X <= 0)
              onEdgeXMin = true;
            else
            {
              onXMatrix = true;
              onYMatrix = true;
            }
          }
          if ( ((x == XMid) && (Mask[x] & (0xff >> XBit))) || ((x > XMid) && (Mask[x])) )
          {
            if (m_X <= 0)
            {
              onXMatrix = true;
              onYMatrix = true;
            }
            else
              onEdgeXMax = true;
          }
        }
      }
      Mask = &Mask[XCnt];
    }
  }
  if ((m_Y + m_Height) <= 0)
    onEdgeYMin = true;
  else if (m_Y >= Matrix->Height())
    onEdgeYMax = true;
  else
  {
    uint8_t *Mask = (uint8_t *)&m_Mask[m_Frame * m_MaskSize];
    uint8_t XCnt = (m_Width + 7) / 8;
    for (int16_t y=(m_Y+m_Height)-1; y>=m_Y; --y)
    {
      for (uint8_t x=0; x<XCnt; ++x)
      {
        if (Mask[x])
        {
          if (y <= 0)
            onEdgeYMin = true;
          else if (y >= (Matrix->Height() - 1))
            onEdgeYMax = true;
          if ( (y >= 0) && (y < Matrix->Height()) )
            onYMatrix = true;
        }
      }
      Mask = &Mask[XCnt];
    }
  }
  m_Flags &= (~ (SPRITE_EDGE_MASK | SPRITE_OFF_MASK));
  if (onXMatrix)
  {
    if (onEdgeXMin)
      m_Flags |= SPRITE_EDGE_X_MIN;
    if (onEdgeXMax)
      m_Flags |= SPRITE_EDGE_X_MAX;
  }
  else if ((onEdgeXMin) || (onEdgeXMax))
      m_Flags |= SPRITE_MATRIX_X_OFF;
  if (onYMatrix)
  {
    if (onEdgeYMin)
      m_Flags |= SPRITE_EDGE_Y_MIN;
    if (onEdgeYMax)
      m_Flags |= SPRITE_EDGE_Y_MAX;
  }
  else if ((onEdgeYMin) || (onEdgeYMax))
      m_Flags |= SPRITE_MATRIX_Y_OFF;
}

void cSprite::SetPosition(int16_t X, int16_t Y)
{
  m_X = X;
  m_Y = Y;
}

void cSprite::SetFrame(uint8_t Frame, uint8_t FrameRate)
{
  m_Frame = max(m_NumFrames - 1, Frame);
  m_FrameRate = m_CounterFrame = FrameRate;
}

void cSprite::SetMotion(int8_t XChange, uint8_t XRate, int8_t YChange, uint8_t YRate)
{
  m_XChange = XChange;
  m_XRate = m_CounterX = XRate;
  m_YChange = YChange;
  m_YRate = m_CounterY = YRate;
}

void cSprite::SetOptions(uint8_t Options)
{
  m_Options = Options;
}

void cSprite::SetPositionFrameMotionOptions(int16_t X, int16_t Y, uint8_t Frame, uint8_t FrameRate, int8_t XChange, uint8_t XRate, int8_t YChange, uint8_t YRate, uint8_t Options)
{
  SetPosition(X, Y);
  SetFrame(Frame, FrameRate);
  SetMotion(XChange, XRate, YChange, YRate);
  SetOptions(Options);
}


cLEDSprites::cLEDSprites(cLEDMatrixBase *Matrix)
{
  m_Matrix = Matrix;
  m_SpriteHead = NULL;
}

void cLEDSprites::AddSprite(cSprite *Spr)
{
  if (m_SpriteHead == NULL)
  {
    m_SpriteHead = Spr;
    Spr->m_PrevSprite = Spr->m_NextSprite = NULL;
  }
  else
  {
    if (IsSprite(Spr) == false)
    {
      cSprite *tmpSpr = m_SpriteHead;
      while (tmpSpr->m_NextSprite != NULL)
        tmpSpr = tmpSpr->m_NextSprite;
      tmpSpr->m_NextSprite = Spr;
      Spr->m_PrevSprite = tmpSpr;
      Spr->m_NextSprite = NULL;
    }
  }
}

boolean cLEDSprites::IsSprite(cSprite *Spr)
{
  cSprite *tmpSpr = m_SpriteHead;
  while ((tmpSpr) && (tmpSpr != Spr))
    tmpSpr = tmpSpr->m_NextSprite;
  if (tmpSpr)
    return(true);
  else
    return(false);
}

void cLEDSprites::RemoveSprite(cSprite *Spr)
{
  if (IsSprite(Spr) == true)
  {
    if (Spr->m_PrevSprite != NULL)
      (Spr->m_PrevSprite)->m_NextSprite = Spr->m_NextSprite;
    if (Spr->m_NextSprite != NULL)
      (Spr->m_NextSprite)->m_PrevSprite = Spr->m_PrevSprite;
    if (m_SpriteHead == Spr)
      m_SpriteHead = Spr->m_NextSprite;
    Spr->m_PrevSprite = Spr->m_NextSprite = NULL;
  }
}

void cLEDSprites::RemoveAllSprites()
{
  cSprite *tmpSpr;

  while ((tmpSpr = m_SpriteHead) != NULL)
  {
    m_SpriteHead = tmpSpr->m_NextSprite;
    tmpSpr->m_PrevSprite = tmpSpr->m_NextSprite = NULL;
  }
}

void cLEDSprites::ChangePriority(cSprite *Spr, SpritePriority_t Priority)
{
  if (IsSprite(Spr) == true)
  {
    if (Priority == SPR_FRONT)
    {
      if (Spr->m_NextSprite != NULL)
      {
        cSprite *tmpSpr;
        if (Spr->m_PrevSprite != NULL)
          (Spr->m_PrevSprite)->m_NextSprite = Spr->m_NextSprite;
        else
          m_SpriteHead = Spr->m_NextSprite;
        (Spr->m_NextSprite)->m_PrevSprite = Spr->m_PrevSprite;
        tmpSpr = Spr->m_NextSprite;
        while (tmpSpr->m_NextSprite)
          tmpSpr = tmpSpr->m_NextSprite;
        tmpSpr->m_NextSprite = Spr;
        Spr->m_PrevSprite = tmpSpr;
        Spr->m_NextSprite = NULL;
      }
    }
    else if (Priority == SPR_FORWARD)
    {
      if (Spr->m_NextSprite != NULL)
      {
        if (Spr->m_PrevSprite != NULL)
          (Spr->m_PrevSprite)->m_NextSprite = Spr->m_NextSprite;
        else
          m_SpriteHead = Spr->m_NextSprite;
        (Spr->m_NextSprite)->m_PrevSprite = Spr->m_PrevSprite;
        Spr->m_PrevSprite = Spr->m_NextSprite;
        Spr->m_NextSprite = (Spr->m_NextSprite)->m_NextSprite;
        (Spr->m_PrevSprite)->m_NextSprite = Spr;
        if (Spr->m_NextSprite != NULL)
          (Spr->m_NextSprite)->m_PrevSprite = Spr;
      }
    }
    else if (Priority == SPR_BACKWARD)
    {
      if (Spr->m_PrevSprite != NULL)
      {
          if (Spr->m_NextSprite != NULL)
            (Spr->m_NextSprite)->m_PrevSprite = Spr->m_PrevSprite;
          (Spr->m_PrevSprite)->m_NextSprite = Spr->m_NextSprite;
          Spr->m_NextSprite = Spr->m_PrevSprite;
          Spr->m_PrevSprite = (Spr->m_PrevSprite)->m_PrevSprite;
          (Spr->m_NextSprite)->m_PrevSprite = Spr;
          if (Spr->m_PrevSprite != NULL)
            (Spr->m_PrevSprite)->m_NextSprite = Spr;
          else
            m_SpriteHead = Spr;
      }
    }
    else if (Priority == SPR_BACK)
    {
      if (Spr->m_PrevSprite != NULL)
      {
          if (Spr->m_NextSprite != NULL)
            (Spr->m_NextSprite)->m_PrevSprite = Spr->m_PrevSprite;
          (Spr->m_PrevSprite)->m_NextSprite = Spr->m_NextSprite;
          m_SpriteHead->m_PrevSprite = Spr;
          Spr->m_NextSprite = m_SpriteHead;
          m_SpriteHead = Spr;
          Spr->m_PrevSprite = NULL;
      }
    }
  }
}

void cLEDSprites::UpdateSprites()
{
  cSprite *tmpSpr = m_SpriteHead;
  while (tmpSpr)
  {
    tmpSpr->Update();
    if (tmpSpr->m_Options & SPRITE_DETECT_EDGE)
        tmpSpr->EdgeDetect(m_Matrix);
    tmpSpr = tmpSpr->m_NextSprite;
  }
}

void cLEDSprites::RenderSprites()
{
  cSprite *tmpSpr = m_SpriteHead;
  while (tmpSpr)
  {
    tmpSpr->Render(m_Matrix);
    tmpSpr = tmpSpr->m_NextSprite;
  }
}

void cLEDSprites::DetectCollisions(cSprite *srcSpr)
{
  cSprite *colSpr[2];
  bool FirstLoop = true;

  if (srcSpr != NULL)
    colSpr[0] = srcSpr;
  else
    colSpr[0] = m_SpriteHead;
  while (colSpr[0] != NULL)
  {
    if (FirstLoop)
      colSpr[0]->m_Flags &= (~SPRITE_COLLISION);  // Clear collision flag
    if (srcSpr != NULL)
      colSpr[1] = m_SpriteHead;
    else
      colSpr[1] = colSpr[0]->m_NextSprite;
    while (colSpr[1] != NULL)
    {
      while ((colSpr[0] == colSpr[1]) && (colSpr[1] != NULL))
        colSpr[1] = colSpr[1]->m_NextSprite;
      if (colSpr[1] != NULL)
      {
        if (FirstLoop)
          colSpr[1]->m_Flags &= (~SPRITE_COLLISION);  // Clear collision flag
        if ((colSpr[0]->m_Options & SPRITE_DETECT_COLLISION) || (colSpr[1]->m_Options & SPRITE_DETECT_COLLISION))
        {
          bool chkColission = false;
          if ( (colSpr[0]->m_X < (colSpr[1]->m_X + colSpr[1]->m_Width)) && (colSpr[1]->m_X < (colSpr[0]->m_X + colSpr[0]->m_Width))
            && (colSpr[0]->m_Y < (colSpr[1]->m_Y + colSpr[1]->m_Height)) && (colSpr[1]->m_Y < (colSpr[0]->m_Y + colSpr[0]->m_Height)) )
          {
            uint8_t *colMask[2], partX, partY, colXBit;
            int16_t colXCnt, colYCnt;
            uint16_t xoff, yoff;
            if (colSpr[0]->m_X <= colSpr[1]->m_X)
              partX = 0;
            else
              partX = 1;
            if ((colSpr[partX]->m_Y + colSpr[partX]->m_Height) >= (colSpr[1 - partX]->m_Y + colSpr[1 - partX]->m_Height))
              partY = partX;
            else
              partY = 1 - partX;
            xoff = colSpr[1 - partX]->m_X - colSpr[partX]->m_X;
            colMask[partX] = (uint8_t *)&colSpr[partX]->m_Mask[(colSpr[partX]->m_Frame * colSpr[partX]->m_MaskSize) + (xoff / 8)];
            colMask[1 - partX] = (uint8_t *)&colSpr[1 - partX]->m_Mask[(colSpr[1 - partX]->m_Frame * colSpr[1 - partX]->m_MaskSize)];
            colXBit = xoff % 8;
            colXCnt = min(colSpr[partX]->m_Width - xoff, colSpr[1 - partX]->m_Width);
            yoff = (colSpr[partY]->m_Y + colSpr[partY]->m_Height) - (colSpr[1 - partY]->m_Y + colSpr[1 - partY]->m_Height);
            colMask[partY] = (uint8_t *)&colMask[partY][yoff * ((colSpr[partY]->m_Width + 7) / 8)];
            colYCnt = min(colSpr[partY]->m_Height - yoff, colSpr[1 - partY]->m_Height);
            for (; colYCnt>0; --colYCnt)
            {
              uint8_t x = 0;
              int16_t tmpXCnt = colXCnt;
              do
              {
                uint8_t colAData = colMask[partX][x] << colXBit;
                if ((8 - colXBit) < colXCnt)
                  colAData |= colMask[partX][x + 1] >> (8 - colXBit);
                if ((colAData & colMask[1 - partX][x]) != 0x00)
                {
                  chkColission = true;
                  break;
                }
                ++x;
                colXCnt -= 8;
              }
              while (colXCnt > 0);
              if (chkColission)
                break;
              colXCnt = tmpXCnt;
              colMask[0] = (uint8_t *)&colMask[0][(colSpr[0]->m_Width + 7) / 8];
              colMask[1] = (uint8_t *)&colMask[1][(colSpr[1]->m_Width + 7) / 8];
            }
          }
          if (chkColission)
          {
            colSpr[0]->m_Flags |= SPRITE_COLLISION;
            colSpr[1]->m_Flags |= SPRITE_COLLISION;
          }
        }
        colSpr[1] = colSpr[1]->m_NextSprite;
      }
    }
    if (srcSpr != NULL)
      break;
    colSpr[0] = colSpr[0]->m_NextSprite;
    FirstLoop = false;
  }
}
