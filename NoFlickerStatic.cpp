#include "stdafx.h"
#include "memdc.h"
#include "NoFlickerStatic.h"

IMPLEMENT_DYNAMIC(CNoFlickerStatic, CStatic)

CNoFlickerStatic::CNoFlickerStatic(void)
{
	m_Font.CreatePointFont(150, "Arial");
}

CNoFlickerStatic::~CNoFlickerStatic(void)
{

}

BEGIN_MESSAGE_MAP(CNoFlickerStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CNoFlickerStatic::OnPaint()
{
	CPaintDC dc(this);

	CWnd *pWndParent = NULL;
	CFont *pOldFont = NULL;
	CRect rect;

	GetClientRect(&rect);
	DWORD dwColor = GetSysColor(COLOR_3DFACE);
	dc.SetBkColor(dwColor);

	_CMemDC memDC(&dc, &rect);
	
	pOldFont = memDC.SelectObject(&m_Font);

	memDC.DrawText(strText, &rect, DT_SINGLELINE);
	memDC.SelectObject(pOldFont);
}

void CNoFlickerStatic::SetText(const CString strText)
{
	this->strText = strText;
	Invalidate();
}