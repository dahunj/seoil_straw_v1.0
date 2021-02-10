#ifndef _SIMPLEDRAW_H
#define _SIMPLEDRAW_H

#include "cv.h"

class CSimpleDraw : public CStatic
{
//Attributes
public:
	CBitmap *m_pbitmap;

	int m_iWinWidth;
	int m_iWinHeight;

// Methods
protected:
	DECLARE_MESSAGE_MAP()

public:
	CSimpleDraw(void);
	~CSimpleDraw(void);

	afx_msg void OnPaint();

	void DrawImage(IplImage *pImage);
	void DrawImage(IplImage *pImage, int nWidth, int nheight);
};

#endif