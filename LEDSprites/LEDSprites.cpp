/*
LEDSprites V1 class by Aaron Liddiment (c) 2014

Uses the FastLED v2.1 library by Daniel Garcia and Mark Kriegsmann.
Inspiration came from my old C64 days :)

Written & tested on a Teensy 3.1
*/

#include <FastLED.h>
#include <LEDMatrix.h>
#include <LEDSprites.h>

cSprite::cSprite(uint8_t Width, uint8_t Height, const uint8_t *Data, uint8_t NumFrames, SpriteNumBits_t BitsPixel, const struct CRGB *ColTable)
{
  if (Data)
    Setup(Width, Height, Data, NumFrames, BitsPixel, ColTable);
}

void cSprite::Setup(uint8_t Width, uint8_t Height, const uint8_t *Data, uint8_t NumFrames, SpriteNumBits_t BitsPixel, const struct CRGB *ColTable)
{
  m_Width = Width;
  m_Height = Height;
  m_Data = Data;
  m_NumFrames = NumFrames;
  m_ColTable = ColTable;
  m_BitsPixel = (uint8_t)BitsPixel;
  m_PrevSprite = m_NextSprite = NULL;
  m_X = m_Y = 0;
  m_Frame = m_FrameRate = m_XRate = m_YRate = m_CounterFrame = m_CounterX = m_CounterY = m_Flags = 0;
  m_XChange = m_YChange = 0;
  m_NumCols = (1 << m_BitsPixel) - 1;
  m_FrameSize = ((m_Width + 7) / 8) * m_BitsPixel * m_Height;
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
      if ( (m_Flags & SPRITE_X_KEEPIN) && (m_Flags & SPRITE_X_EDGE) && (((m_X < (m_Width / 2)) && (m_XChange < 0)) || ((m_X > (m_Width / 2)) && (m_XChange > 0))) )
        m_XChange = 0 - m_XChange;
      m_X += m_XChange;
    }
  }
  if (m_CounterY > 0)
  {
    if ((--m_CounterY) == 0)
    {
      m_CounterY = m_YRate;
      if ( (m_Flags & SPRITE_Y_KEEPIN) && (m_Flags & SPRITE_Y_EDGE) && (((m_Y < (m_Height / 2)) && (m_YChange < 0)) || ((m_Y > (m_Height / 2)) && (m_YChange > 0))) )
        m_YChange = 0 - m_YChange;
      m_Y += m_YChange;
    }
  }
}

void cSprite::Render(cLEDMatrixBase *Matrix)
{
  m_Flags &= (~SPRITE_XY_FLAGS_MASK);
  if ( ((m_X + m_Width) <= 0) || (m_X >= Matrix->m_Width) )
    m_Flags |= SPRITE_X_OFF;
  if ( ((m_Y + m_Height) <= 0) || (m_Y >= Matrix->m_Height) )
    m_Flags |= SPRITE_Y_OFF;
  if (! (m_Flags & SPRITE_XY_FLAGS_MASK))
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
          {
            (*Matrix)(x, y) = m_ColTable[col - 1];
            if ( (x == 0) || (x == (Matrix->m_Width - 1)) )
              m_Flags |= SPRITE_X_EDGE;
            if ( (y == 0) || (y == (Matrix->m_Height - 1)) )
              m_Flags |= SPRITE_Y_EDGE;
          }
        }
      }
    }
  }
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

void cSprite::SetMotion(int8_t XChange, uint8_t XRate, int8_t YChange, uint8_t YRate, uint8_t Flags)
{
  m_XChange = XChange;
  m_XRate = m_CounterX = XRate;
  m_YChange = YChange;
  m_YRate = m_CounterY = YRate;
  m_Flags = (m_Flags & (~SPRITE_KEEPIN_MASK)) | (Flags & SPRITE_KEEPIN_MASK);
}

void cSprite::SetPositionFrameMotion(int16_t X, int16_t Y, uint8_t Frame, uint8_t FrameRate, int8_t XChange, uint8_t XRate, int8_t YChange, uint8_t YRate, uint8_t Flags)
{
  SetPosition(X, Y);
  SetFrame(Frame, FrameRate);
  SetMotion(XChange, XRate, YChange, YRate, Flags);
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
    tmpSpr->Render(m_Matrix);
    tmpSpr = tmpSpr->m_NextSprite;
  }
}
