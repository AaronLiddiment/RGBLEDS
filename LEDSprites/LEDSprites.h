#ifndef LEDSprites_h
#define LEDSprites_h

#ifndef HEX__(n)
  #define HEX__(n) 0x##n##LU
#endif
#define B8_1B__(x) (uint8_t)(((x&0x10000000LU)>>21)+((x&0x01000000LU)>>18)+((x&0x00100000LU)>>15)+((x&0x00010000LU)>>12)+ \
                    ((x&0x00001000LU)>>9)+((x&0x00000100LU)>>6)+((x&0x00000010LU)>>3)+((x&0x00000001LU)))
#define B8_2B__(x) (uint8_t)(((x&0x30000000LU)>>22)+((x&0x03000000LU)>>20)+((x&0x00300000LU)>>18)+((x&0x00030000LU)>>16)), \
                    (uint8_t)(((x&0x00003000LU)>>6)+((x&0x00000300LU)>>4)+((x&0x00000030LU)>>2)+((x&0x00000003LU)))
#define B8_3B__(x) (uint8_t)(((x&0x70000000LU)>>23)+((x&0x07000000LU)>>22)+((x&0x00700000LU)>>21)), \
                    (uint8_t)(((x&0x00700000LU)>>13)+((x&0x00070000LU)>>12)+((x&0x00007000LU)>>11)+((x&0x00000700LU)>>10)), \
                    (uint8_t)(((x&0x00000700LU)>>2)+((x&0x00000070LU)>>1)+((x&0x00000007LU)))
#define B8_4B__(x) (uint8_t)(((x&0xf0000000LU)>>24)+((x&0x0f000000LU)>>24)), \
                    (uint8_t)(((x&0x00f00000LU)>>16)+((x&0x000f0000LU)>>16)), \
                    (uint8_t)(((x&0x0000f000LU)>>8)+((x&0x00000f00LU)>>8)), \
                    (uint8_t)((x&0x000000f0LU)+(x&0x0000000fLU))

#define B8_1BIT(d)  B8_1B__(HEX__(d))
#define B8_2BIT(d)  B8_2B__(HEX__(d))
#define B8_3BIT(d)  B8_3B__(HEX__(d))
#define B8_4BIT(d)  B8_4B__(HEX__(d))

enum SpriteNumBits_t { _1BIT=1,
                       _2BIT=2,
                       _3BIT=3,
                       _4BIT=4 };

enum SpritePriority_t { SPR_FRONT,
                        SPR_BACK,
                        SPR_FORWARD,
                        SPR_BACKWARD };

// defines for m_Flags
#define SPRITE_EDGE_X_MIN      0x01
#define SPRITE_EDGE_X_MAX      0x02
#define SPRITE_EDGE_Y_MIN      0x04
#define SPRITE_EDGE_Y_MAX      0x08
#define SPRITE_MATRIX_X_OFF    0x10
#define SPRITE_MATRIX_Y_OFF    0x20
#define SPRITE_COLLISION       0x40
#define SPRITE_EDGE_MASK       (SPRITE_EDGE_X_MIN | SPRITE_EDGE_X_MAX | SPRITE_EDGE_Y_MIN | SPRITE_EDGE_Y_MAX)
#define SPRITE_OFF_MASK        (SPRITE_MATRIX_X_OFF | SPRITE_MATRIX_Y_OFF)

// defines for m_Options
#define SPRITE_DETECT_EDGE       0x01
#define SPRITE_DETECT_COLLISION  0x02
#define SPRITE_X_KEEPIN          0x40
#define SPRITE_Y_KEEPIN          0x80
#define SPRITE_KEEPIN_MASK       (SPRITE_X_KEEPIN | SPRITE_Y_KEEPIN)

class cSprite
{
  friend class cLEDSprites;
  public:
    cSprite(uint8_t Width = 0, uint8_t Height = 0, const uint8_t *Data = NULL, uint8_t NumFrames = 0, SpriteNumBits_t BitsPixel = _1BIT, const struct CRGB *ColTable = NULL, const uint8_t *Mask = NULL);
    void Setup(uint8_t Width, uint8_t Height, const uint8_t *Data, uint8_t NumFrames, SpriteNumBits_t BitsPixel, const struct CRGB *ColTable, const uint8_t *Mask = NULL);
    void Update();
    boolean Combine(int16_t dx, int16_t dy, cSprite *Src);
    void Render(cLEDMatrixBase *Matrix);
    void EdgeDetect(cLEDMatrixBase *Matrix);
    void SetPosition(int16_t X, int16_t Y);
    void SetFrame(uint8_t Frame, uint8_t FrameRate = 0);
    void SetMotion(int8_t XChange, uint8_t XRate, int8_t YChange, uint8_t YRate);
    void SetOptions(uint8_t Options);
    void SetPositionFrameMotionOptions(int16_t X, int16_t Y, uint8_t Frame, uint8_t FrameRate, int8_t XChange, uint8_t XRate, int8_t YChange, uint8_t YRate, uint8_t Options = 0);
    uint8_t GetFlags()             { return(m_Flags); };
    uint8_t GetCurrentFrame()      { return(m_Frame); };
    int8_t GetXChange()            { return(m_XChange); };
    int8_t GetYChange()            { return(m_YChange); };
    uint8_t GetXCounter()          { return(m_CounterX); };
    uint8_t GetYCounter()          { return(m_CounterY); };
    void SetXChange(int8_t XC)     { m_XChange = XC; };
    void SetYChange(int8_t YC)     { m_YChange = YC; };
    void SetXCounter(uint8_t Xcnt) { m_CounterX = Xcnt; };
    void SetYCounter(uint8_t Ycnt) { m_CounterY = Ycnt; };
    void IncreaseFrame()           { m_Frame++; if (m_Frame >= m_NumFrames) m_Frame = 0; };
    void DecreaseFrame()           { if (m_Frame == 0) m_Frame = m_NumFrames; m_Frame--; };

  private:
    uint16_t m_MaskSize, m_FrameSize;
    uint8_t m_CounterFrame, m_NumCols, m_Options, m_Flags, m_CounterX, m_CounterY, m_XRate, m_YRate, m_Frame, m_FrameRate;
    int8_t m_XChange, m_YChange;
    cSprite *m_PrevSprite, *m_NextSprite;
  protected:
    uint8_t m_Width, m_Height, m_NumFrames, m_BitsPixel;
    const uint8_t *m_Data, *m_Mask;
    const struct CRGB *m_ColTable;
  public:
    int16_t m_X, m_Y;
};

class cLEDSprites
{
  public:
    cLEDSprites(cLEDMatrixBase *Matrix);
    void AddSprite(cSprite *Spr);
    boolean IsSprite(cSprite *Spr);
    void RemoveSprite(cSprite *Spr);
    void RemoveAllSprites();
    void ChangePriority(cSprite *Spr, SpritePriority_t Priority);
    void UpdateSprites();
    void RenderSprites();
    void DetectCollisions(cSprite *srcSpr = NULL);

  private:
    cSprite *m_SpriteHead;
    cLEDMatrixBase *m_Matrix;
};

#endif
