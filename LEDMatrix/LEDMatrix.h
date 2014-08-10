#ifndef LEDMatrix_h
#define LEDMatrix_h

enum MatrixType_t { HORIZONTAL_MATRIX,
                    VERTICAL_MATRIX,
                    HORIZONTAL_ZIGZAG_MATRIX,
                    VERTICAL_ZIGZAG_MATRIX };

class cLEDMatrixBase
{
	protected:
		int16_t m_Width, m_Height;
		MatrixType_t m_Type;
		struct CRGB *m_LED;
		struct CRGB m_OutOfBounds;

	public:
		cLEDMatrixBase();
		virtual uint16_t mXY(uint16_t x, uint16_t y)=0;

		struct CRGB *operator[](int n);
		int Size();
		struct CRGB &operator()(int16_t x, int16_t y);

		void HorizontalMirror(bool FullHeight = true);
		void VerticalMirror();
		void QuadrantMirror();
		void QuadrantRotateMirror();
		void TriangleTopMirror(bool FullHeight = true);
		void TriangleBottomMirror(bool FullHeight = true);
		void QuadrantTopTriangleMirror();
		void QuadrantBottomTriangleMirror();
};

template<int16_t Width, int16_t Height, MatrixType_t MType> class cLEDMatrix : public cLEDMatrixBase
{
  private:
    struct CRGB p_LED[(Width * ((Width < 0) * -1 + (Width > 0))) * (Height * ((Height < 0) * -1 + (Height > 0)))];  //Do absolutes of Width & Height
  public:
    cLEDMatrix()
    {
      m_Width = abs(Width);
      m_Height = abs(Height);
      m_Type = MType;
      m_LED = p_LED;
    }
    virtual uint16_t mXY(uint16_t x, uint16_t y)
    {
      if (Width < 0)
        x = (m_Width - 1) - x;
      if (Height < 0)
        y = (m_Height - 1) - y;
      if (MType == HORIZONTAL_MATRIX)
        return((y * m_Width) + x);
      if (MType == VERTICAL_MATRIX)
        return((x * m_Height) + y);
      if (MType == HORIZONTAL_ZIGZAG_MATRIX)
      {
        if (y % 2)
          return((((y + 1) * m_Width) - 1) - x);
        else
          return((y * m_Width) + x);
      }
      if (MType == VERTICAL_ZIGZAG_MATRIX)
      {
        if (x % 2)
          return((((x + 1) * m_Height) - 1) - y);
        else
          return((x * m_Height) + y);
      }
    }
};

#endif
