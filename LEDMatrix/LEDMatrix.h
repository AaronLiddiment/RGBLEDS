#ifndef LEDMatrix_h
#define LEDMatrix_h

class cLEDMatrix
{
  public:
    void LEDArray(uint16_t Width, uint16_t Height, bool ZigZag, CRGB *LAry);
    void HorizontalMirror(bool FullHeight = true);
    void VerticalMirror();
    void QuadrantMirror();
    struct CRGB &MatrixXY(int16_t x, int16_t y);
  private:
    uint16_t m_Width, m_Height, m_XMax;
    bool m_ZigZag;
    CRGB *m_MatrixLeds;
    CRGB m_OutOfBounds;
};

#endif
