#ifndef LEDMatrix_h
#define LEDMatrix_h

enum MatrixType_t { HORIZONTAL_MATRIX=0,
                    VERTICAL_MATRIX=1,
                    HORIZONTAL_ZIGZAG_MATRIX=2,
                    VERTICAL_ZIGZAG_MATRIX=3 };

class cLEDMatrix
{
  public:
    cLEDMatrix(uint16_t Width, uint16_t Height, MatrixType_t Type, CRGB *LAry);
    struct CRGB &MatrixXY(int16_t x, int16_t y);

    void HorizontalMirror(bool FullHeight = true);
    void VerticalMirror();
    void QuadrantMirror();

    // ** The following functions are only really good with matrix that have the same width and height
    void QuadrantRotateMirror();
    void TriangleTopMirror(bool FullHeight = true);
    void TriangleBottomMirror(bool FullHeight = true);
    void QuadrantTopTriangleMirror();
    void QuadrantBottomTriangleMirror();
  private:
    inline uint16_t mXY(int16_t x, int16_t y) __attribute__((always_inline));

    uint16_t m_Width, m_Height;
    MatrixType_t m_Type;
    CRGB *m_MatrixLeds;
    CRGB m_OutOfBounds;
};

#endif
