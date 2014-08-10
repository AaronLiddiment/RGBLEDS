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
#define SPRITE_X_EDGE          0x01
#define SPRITE_Y_EDGE          0x02
#define SPRITE_X_OFF           0x04
#define SPRITE_Y_OFF           0x08
#define SPRITE_XY_FLAGS_MASK   (SPRITE_X_EDGE | SPRITE_Y_EDGE | SPRITE_X_OFF | SPRITE_Y_OFF)
#define SPRITE_X_KEEPIN        0x80
#define SPRITE_Y_KEEPIN        0x40
#define SPRITE_KEEPIN_MASK     (SPRITE_X_KEEPIN | SPRITE_Y_KEEPIN)

class cSprite
{
  friend class cLEDSprites;
  public:
    cSprite(uint8_t Width = 0, uint8_t Height = 0, const uint8_t *Data = NULL, uint8_t NumFrames = 0, SpriteNumBits_t BitsPixel = _1BIT, const struct CRGB *ColTable = NULL);
    void Setup(uint8_t Width, uint8_t Height, const uint8_t *Data, uint8_t NumFrames, SpriteNumBits_t BitsPixel, const struct CRGB *ColTable);
    void Update();
    void Render(cLEDMatrixBase *Matrix);
    void SetPosition(int16_t X, int16_t Y);
    void SetFrame(uint8_t Frame, uint8_t FrameRate = 0);
    void SetMotion(int8_t XChange, uint8_t XRate, int8_t YChange, uint8_t YRate, uint8_t Flags);
    void SetPositionFrameMotion(int16_t X, int16_t Y, uint8_t Frame, uint8_t FrameRate, int8_t XChange, uint8_t XRate, int8_t YChange, uint8_t YRate, uint8_t Flags = 0);
    uint8_t GetXYFlags() { return(m_Flags & SPRITE_XY_FLAGS_MASK); };

  private:
    uint16_t m_FrameSize;
    uint8_t m_CounterFrame, m_CounterX, m_CounterY, m_NumCols, m_Flags;
    cSprite *m_PrevSprite, *m_NextSprite;
  protected:
    uint8_t m_Width, m_Height, m_NumFrames, m_BitsPixel;
    const uint8_t *m_Data;
    const struct CRGB *m_ColTable;
  public:
    int16_t m_X, m_Y;
    uint8_t m_Frame, m_FrameRate, m_XRate, m_YRate;
    int8_t m_XChange, m_YChange;
};

class cLEDSprites
{
  public:
    cLEDSprites(cLEDMatrixBase *Matrix);
    void AddSprite(cSprite *Spr);
    boolean IsSprite(cSprite *Spr);
    void RemoveSprite(cSprite *Spr);
    void ChangePriority(cSprite *Spr, SpritePriority_t Priority);
    void UpdateSprites();

  private:
    cSprite *m_SpriteHead;
    cLEDMatrixBase *m_Matrix;
};

#endif
