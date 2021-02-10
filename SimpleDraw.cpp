#include "stdafx.h"
#include "SimpleDraw.h"
#include "Extern.h"

BEGIN_MESSAGE_MAP(CSimpleDraw, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CSimpleDraw::CSimpleDraw(void)
{
	m_pbitmap = NULL;
}

CSimpleDraw::~CSimpleDraw(void)
{
	if(m_pbitmap != NULL)
		delete m_pbitmap;
}

void CSimpleDraw::DrawImage(IplImage *pImage)
{
	int bitCnt = pImage->depth * pImage->nChannels;

	CDC *pDC = GetDC();

	if ( pDC == NULL )
	{
		return;
	}

	CDC dcMem;
	CRect rectViewer;

	GetClientRect( rectViewer );

	if ( m_pbitmap ) 
		delete m_pbitmap;
 	
	m_pbitmap = new CBitmap;		
	m_pbitmap->CreateCompatibleBitmap( pDC, rectViewer.Width(), rectViewer.Height() );

	m_iWinWidth = rectViewer.Width();
	m_iWinHeight = rectViewer.Height();

	dcMem.CreateCompatibleDC( pDC );	
	dcMem.SelectObject( m_pbitmap );

	// 비트맵 정보 셋팅
	BITMAPINFO *pBmi;

	if ( bitCnt == 24 )
	{
		pBmi = ( BITMAPINFO* ) malloc( sizeof( BITMAPINFOHEADER ) );
	}
	else if ( bitCnt == 8 )
	{
		pBmi = ( BITMAPINFO* ) malloc( sizeof ( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * 256 );

		for ( int i = 0; i < 256; i++ )
		{
			pBmi->bmiColors[i].rgbRed = i;
			pBmi->bmiColors[i].rgbGreen = i;
			pBmi->bmiColors[i].rgbBlue = i;
			pBmi->bmiColors[i].rgbReserved = 0;
		}
	}

	pBmi->bmiHeader.biBitCount = bitCnt;
	pBmi->bmiHeader.biCompression= 0;
	pBmi->bmiHeader.biHeight = _ShowHeight;
	pBmi->bmiHeader.biPlanes = 1;
	pBmi->bmiHeader.biSizeImage = _ShowWidth * _ShowHeight;
	pBmi->bmiHeader.biSize = 40;
	pBmi->bmiHeader.biWidth = _ShowWidth;
	pBmi->bmiHeader.biXPelsPerMeter = 0;
	pBmi->bmiHeader.biYPelsPerMeter = 0;
	
	StretchDIBits(dcMem.GetSafeHdc(),
				0, 0, rectViewer.Width(), rectViewer.Height(),
				0, 0, rectViewer.Width(), rectViewer.Height(),
				pImage->imageData, 
				pBmi, 
				DIB_RGB_COLORS, 
				SRCCOPY );

	//m_iImageWidth = pImage->width;
	//m_iImageHeight = pImage->height;

	Invalidate(FALSE);
	ReleaseDC(pDC);	

	free(pBmi);
}

void CSimpleDraw::DrawImage(IplImage *pImage, int nWidth, int nHeight)
{
	int bitCnt = pImage->depth * pImage->nChannels;

	CDC *pDC = GetDC();

	if ( pDC == NULL )
	{
		AfxMessageBox( _T("NULL") );
		return;
	}

	CDC dcMem;
	CRect rectViewer;

	GetClientRect(rectViewer);

	if ( m_pbitmap ) 
		delete m_pbitmap;

 	m_pbitmap = new CBitmap;		
	m_pbitmap->CreateCompatibleBitmap(pDC, nWidth, nHeight);

	m_iWinWidth = nWidth;
	m_iWinHeight = nHeight;

	dcMem.CreateCompatibleDC(pDC);	
	dcMem.SelectObject(m_pbitmap);
	
	BITMAPINFO *pBmi;

	if ( bitCnt == 24 )
	{
		pBmi = ( BITMAPINFO* ) malloc( sizeof( BITMAPINFOHEADER ) );
	}
	else if ( bitCnt == 8 )
	{
		pBmi = ( BITMAPINFO* ) malloc( sizeof ( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * 256 );

		for ( int i = 0; i < 256; i++ )
		{
			pBmi->bmiColors[i].rgbRed = i;
			pBmi->bmiColors[i].rgbGreen = i;
			pBmi->bmiColors[i].rgbBlue = i;
			pBmi->bmiColors[i].rgbReserved = 0;
		}
	}

	// 비트맵 정보 셋팅
	pBmi->bmiHeader.biBitCount = bitCnt;
	pBmi->bmiHeader.biCompression= 0;
	pBmi->bmiHeader.biHeight = nHeight;
	pBmi->bmiHeader.biPlanes = 1;
	pBmi->bmiHeader.biSizeImage = nWidth * nHeight;
	pBmi->bmiHeader.biSize = 40;
	pBmi->bmiHeader.biWidth = nWidth;
	pBmi->bmiHeader.biXPelsPerMeter = 0;
	pBmi->bmiHeader.biYPelsPerMeter = 0;

	::StretchDIBits(dcMem.GetSafeHdc(),
				0, 0, nWidth, nHeight,
				0, 0, nWidth, nHeight,
				pImage->imageData,
				pBmi, 
				DIB_RGB_COLORS, 
				SRCCOPY );
	
	//m_iImageWidth = pImage->width;
	//m_iImageHeight = pImage->height;

	Invalidate(TRUE);
	ReleaseDC(pDC);	

	free(pBmi);
}

void CSimpleDraw::OnPaint()
{
	CPaintDC dc(this);
	
	CRect rectViewer;
	GetClientRect(rectViewer);

	CDC dcMem;
	CDC *pDC = GetDC();

	dcMem.CreateCompatibleDC(pDC);
	dcMem.SelectObject(m_pbitmap);

	dc.BitBlt(
		1, 1, 
		rectViewer.Width() - 1, rectViewer.Height() - 1 , 
		&dcMem, 0, 0, SRCCOPY);
	
	dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(rectViewer);
	
	ReleaseDC(pDC);
}