// InspectImage.cpp : implementation file
//

#include "stdafx.h"
//#include "CViewerTest.h"
#include "InspectImage.h"

#include "ViewingImage.h"

#include "../Extern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspectImage

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

extern int _MAXROI;

#define	_GAP	10
#define _IMUL_RAD	10
#define _HALF_RAD	5

#define STRAW_IMUL_TOP		111
#define STRAW_IMUL_MID		112
#define STRAW_IMUL_JABARA	113
#define STRAW_IMUL_CHUCK	114
#define STRAW_IMUL_INSERT	115

CInspectImage::CInspectImage()
{
	m_inspect_ROI = NULL;
	m_bExistObj = NULL;
	m_result_inspect = NULL;
	m_result_Value = NULL;
	m_ROI = NULL;

	if(_MAXROI <= 0)
		return;

	m_inspect_ROI = new INSPECT_ROI[_MAXROI];
	m_bExistObj = new BOOL[_MAXROI];
	m_result_inspect = new RESULT_INSPECT[_MAXROI];
	m_result_Value = new RESULT_INSPVALUE[_MAXROI];
	m_ROI = new CPoint*[_MAXROI];
	for(int i = 0; i < _MAXROI; i++)
		m_ROI[i] = new CPoint[2];

	m_pbitmap = NULL;

	// ��Ȳ �ʱ�ȭ
	status_down = 0;
	status_move = 0;
	m_Loaded = false;
	m_ROIS = 0;
	
	// ��� ROI �ʱ�ȭ
	InitROIs();

	// ��� �˻� �ʱ�ȭ


	m_process = new CStrawProcess;


	m_drawinsepct = false;
	InitOX();

	for (int j = 0;j < _MAXROI; j++)
	{
		m_result_inspect[j].angle[0] = 0.0;
		m_result_inspect[j].angle[1] = 0.0;
		m_result_inspect[j].pok = 0.0;
		m_result_inspect[j].chuck = false;
		m_result_inspect[j].insert = false;
		m_result_inspect[j].maguri = false;
		m_result_inspect[j].jabara = false;
		m_result_inspect[j].imul = false;
		m_result_inspect[j].imul_mid = false;
		m_result_inspect[j].imul_jabara = false;
		m_result_inspect[j].imul_chuck = false;
		m_result_inspect[j].imul_insert = false;
		m_result_inspect[j].etiquette = false;

		m_bExistObj[j] = TRUE;

		memset(m_result_Value[j].Rmaguri, 0, sizeof( int ) * 4);
		memset(m_result_Value[j].Rchuck, 0, sizeof( double ) * 3);
		memset(m_result_Value[j].Rfchuck, 0, sizeof( double) * 2);
		memset(m_result_Value[j].Rinsult, 0, sizeof( double ) * 6);
		memset(m_result_Value[j].Rimul, 0, sizeof( int ) * 7 * 2);
		memset(m_result_Value[j].Rjabara, 0, sizeof( int ) * 4);
		memset(m_result_Value[j].Rpok, 0, sizeof( double ) * 4);
		memset(m_result_Value[j].Rangle, 0, sizeof( double ) * 5);
		memset(m_result_Value[j].REtiquet, 0, sizeof(double) * 4);

		m_result_Value[j].black_num = 0;
		m_result_Value[j].jabara_num = 0;
		m_result_Value[j].jabara_neighbor = 0;
		m_result_Value[j].jabara_distance_stdev = 0.0;
		m_result_Value[j].jabara_gradient_stdev = 0.0;
		m_result_Value[j].jabara_width_stdev = 0.0;

		m_result_Value[j].maguri.topLen = 0;
		m_result_Value[j].maguri.botLen = 0;
		m_result_Value[j].maguri.inputMaxLength = 0;
		m_result_Value[j].maguri.inputMinLength = 0;
		m_result_Value[j].maguri.isAngleOk = FALSE;
		m_result_Value[j].maguri.isSideOk = FALSE;

		m_result_Value[j].mid.isAngleOk = FALSE;
		m_result_Value[j].mid.isHeightOk = FALSE;
		m_result_Value[j].mid.maxHeight = 0;
		m_result_Value[j].mid.minHeight = 0;

		m_result_Value[j].top.isAngleOk = FALSE;
		m_result_Value[j].top.isHeightOk = FALSE;
		m_result_Value[j].top.maxHeight = 0;
		m_result_Value[j].top.minHeight = 0;

		m_result_Value[j].chuck.isAngleOk = FALSE;
		m_result_Value[j].chuck.isHeightOk = FALSE;
		m_result_Value[j].chuck.maxHeight = 0;
		m_result_Value[j].chuck.minHeight = 0;

		m_result_Value[j].insert.isAngleOk = FALSE;
	}

	// ��ü �̹��� ����
	m_nowView = NULL;
	m_resizeView = NULL;
	m_rgbResizeView = NULL;

	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	m_nowView = cvCreateImage(cvSize(_CamWidth,_CamHeight), 8, 1);
	m_resizeView = cvCreateImage(cvSize(_ShowWidth,_ShowHeight), 8, 1);
	m_rgbResizeView = cvCreateImage(cvSize(_ShowWidth,_ShowHeight), 8, 3);

	InitInspectSettingValues();
}

CInspectImage::~CInspectImage()
{
	if(m_inspect_ROI != NULL)
		delete [] m_inspect_ROI;

	if(m_bExistObj != NULL)
		delete [] m_bExistObj;

	if(m_result_inspect != NULL)
		delete [] m_result_inspect;

	if(m_result_Value != NULL)
		delete [] m_result_Value;

	if(m_ROI != NULL)
	{
		for(int i = 0; i < _MAXROI; i++)
			delete [] m_ROI[i];
		delete [] m_ROI;
	}

	// �̹��� ����
	if( m_nowView != NULL )
		cvReleaseImage( &m_nowView );

	if( m_resizeView != NULL )
		cvReleaseImage( &m_resizeView );

	if( m_rgbResizeView != NULL )
		cvReleaseImage( &m_rgbResizeView );

	if( m_pbitmap != NULL)
		delete m_pbitmap;

	// �˻� Ŭ���� �輼

	delete this->m_process;

}


BEGIN_MESSAGE_MAP(CInspectImage, CStatic)
	//{{AFX_MSG_MAP(CInspectImage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspectImage message handlers

void CInspectImage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rectViewer;
	GetClientRect(rectViewer);

	CDC dcMem, * pDC;
	pDC = GetDC();

	CPen pen1(PS_SOLID, 2, RGB(0,0,0));
//	BYTE r, g, b;

	// �̹��� �׸���
	dcMem.CreateCompatibleDC(pDC);
	dcMem.SelectObject(m_pbitmap);
	dc.BitBlt(1,1,rectViewer.Width() - 1, rectViewer.Height() - 1 , &dcMem,0,0,SRCCOPY);
	
	dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(rectViewer);
	
	// Do not call CStatic::OnPaint() for painting messages
	
	// ROI�׸���
	for ( int i = 0; i < _MAXROI; i++ )
	{
		// ���� ROI ���� �ȵǸ� �Ѿ
		if (m_ROI[i][0].x == -1)
			continue;
		if(m_bExistObj[i]==false) 
			continue;
		
		// ROI�� ���
		DrawSquare(pDC,m_ROI[i][0].x, m_ROI[i][0].y,m_ROI[i][1].x,m_ROI[i][1].y,255,255,255,PS_DASHDOT);

		// ������ �׸���->����
		if (m_inspect_ROI[i].maguri[0].x !=-1 && m_inspect_ox.maguri)
			DrawSquare(pDC,m_inspect_ROI[i].maguri[0].x,m_inspect_ROI[i].maguri[0].y,m_inspect_ROI[i].maguri[1].x,m_inspect_ROI[i].maguri[1].y,255,0,0,PS_SOLID);

		// �ڹٶ� �׸��� -> ���
		if (m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.jabara)
			DrawSquare(pDC,m_inspect_ROI[i].jabara[0].x,m_inspect_ROI[i].jabara[0].y,m_inspect_ROI[i].jabara[1].x,m_inspect_ROI[i].jabara[1].y,0,255,0,PS_SOLID);

		// �� �׸��� -> û��
		if (m_inspect_ROI[i].pok[0].x != -1 && m_inspect_ox.pok)
				DrawSquare(pDC,m_inspect_ROI[i].pok[0].x,m_inspect_ROI[i].pok[0].y,m_inspect_ROI[i].pok[1].x,m_inspect_ROI[i].pok[1].y,0,0,255,PS_SOLID);

		// ��� �׸��� ->��+��
		if (m_inspect_ROI[i].chuck[0].x != -1 && m_inspect_ox.chuck)
			DrawSquare(pDC,m_inspect_ROI[i].chuck[0].x,m_inspect_ROI[i].chuck[0].y,m_inspect_ROI[i].chuck[1].x,m_inspect_ROI[i].chuck[1].y,255,255,0,PS_SOLID);

		// ���� �׸��� -> ��+û
		if (m_inspect_ROI[i].insert[0].x != -1 && m_inspect_ox.insert)
			DrawSquare(pDC,m_inspect_ROI[i].insert[0].x,m_inspect_ROI[i].insert[0].y,m_inspect_ROI[i].insert[1].x,m_inspect_ROI[i].insert[1].y,255,0,255,PS_SOLID);

		// �� �׸��� (2��)-> û+��
		for (int j=0;j<2;j++)
		{
			//if (m_inspect_ROI[i].angle[j][0].x != -1 && m_inspect_ox.angle[j])
			//	DrawSquare(pDC,m_inspect_ROI[i].angle[j][0].x,m_inspect_ROI[i].angle[j][0].y,m_inspect_ROI[i].angle[j][1].x,m_inspect_ROI[i].angle[j][1].y,0,255,255,PS_SOLID);

		}

		// ��Ƽ�� �׸���-> ����
		if (m_inspect_ROI[i].etiquette[0].x !=-1 && m_inspect_ox.etiquette)
			DrawSquare(pDC,m_inspect_ROI[i].etiquette[0].x,m_inspect_ROI[i].etiquette[0].y,m_inspect_ROI[i].etiquette[1].x,m_inspect_ROI[i].etiquette[1].y,128,0,255,PS_SOLID);

		// ���� ��Ƽ�� �̹� �׸��� -> ����
		if(m_inspect_ROI[i].imul_etq_left[0].x != -1 && m_inspect_ox.imul_etq_left)
		{
			DrawSquare(pDC,m_inspect_ROI[i].imul_etq_left[0].x,m_inspect_ROI[i].imul_etq_left[0].y,m_inspect_ROI[i].imul_etq_left[1].x,m_inspect_ROI[i].imul_etq_left[1].y,128,0,255,PS_SOLID);
		}

		// ���� ��Ƽ�� �̹� �׸��� -> �Ķ�
		if(m_inspect_ROI[i].imul_etq_right[0].x != -1 && m_inspect_ox.imul_etq_right)
		{
			DrawSquare(pDC,m_inspect_ROI[i].imul_etq_right[0].x,m_inspect_ROI[i].imul_etq_right[0].y,m_inspect_ROI[i].imul_etq_right[1].x,m_inspect_ROI[i].imul_etq_right[1].y,0,0,255,PS_SOLID);
		}

		// �̹� �׸���->ȸ�� (����)
		//if (m_inspect_ROI[i].imul[0].x != -1 && m_inspect_ox.imul)
		//	DrawSquare(pDC,m_inspect_ROI[i].imul[0].x, m_inspect_ROI[i].imul[0].y, m_inspect_ROI[i].imul[1].x, m_inspect_ROI[i].imul[1].y,128,128,128,PS_SOLID);
		

		// �̹� ���
		if ( m_inspect_ox.jabara )
		{
			// �ڹٶ� �ִ� ��� ( ��� )
			if ( m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.imul )
			{
				DrawSquare(
					pDC,
					m_ROI[i][0].x + _GAP,
					m_ROI[i][0].y + _GAP, //+ ( m_ROI[i][1].y - m_ROI[i][0].y ) / 2,
					m_inspect_ROI[i].jabara[0].x - _GAP,
					m_inspect_ROI[i].jabara[1].y - _GAP,
					128, 128, 128,
					PS_SOLID );
			}

			// �ڹٶ� �ִ� ��� ( �ߴ� )
			if (   m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.imul_mid // �ڹٶ� Ȯ��
				&& m_inspect_ROI[i].chuck[0].x != -1  && m_inspect_ox.chuck )  // ��� Ȯ��
			{
				DrawSquare(
					pDC,
					m_inspect_ROI[i].jabara[1].x + _GAP,
					m_ROI[i][0].y + _GAP,  // + ( m_ROI[i][1].y - m_ROI[i][0].y ) / 2,
					m_inspect_ROI[i].chuck[0].x - _GAP,
					m_inspect_ROI[i].chuck[1].y - _GAP,
					128, 128, 128,
					PS_SOLID );
			}
		}
		else
		{
			// �ڹٶ� ���� ��� ( ��� )
			if ( m_inspect_ROI[i].chuck[0].x != -1 && m_inspect_ox.imul && m_inspect_ox.chuck )
			{
				DrawSquare(
					pDC,
					m_ROI[i][0].x + _GAP,
					m_ROI[i][0].y + _GAP, // + ( m_ROI[i][1].y - m_ROI[i][0].y ) / 2,
					m_ROI[i][0].x + ( m_inspect_ROI[i].chuck[0].x - m_ROI[i][0].x ) / 2 - _GAP,
					m_inspect_ROI[i].chuck[1].y - _GAP,
					128, 128, 128,
					PS_SOLID );
			}

			// �ڹٶ� ���� ��� ( �ߴ� )
			if ( m_inspect_ROI[i].chuck[0].x != -1 && m_inspect_ox.imul_mid && m_inspect_ox.chuck )
			{
				DrawSquare(
					pDC,
					m_ROI[i][0].x + ( m_inspect_ROI[i].chuck[0].x - m_ROI[i][0].x ) / 2 + _GAP,
					m_ROI[i][0].y + _GAP,  // + ( m_ROI[i][1].y - m_ROI[i][0].y ) / 2,
					m_inspect_ROI[i].chuck[0].x - _GAP,
					m_inspect_ROI[i].chuck[1].y - _GAP,
					128, 128, 128,
					PS_SOLID );
			}
		}

		
		// �̹� �ڹٶ� 
		if ( m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.imul_jabara && m_inspect_ox.jabara )
		{
			DrawSquare(
				pDC,
				m_inspect_ROI[i].jabara[0].x + _GAP,
				m_inspect_ROI[i].jabara[0].y + _GAP, // + ( m_inspect_ROI[i].jabara[1].y - m_inspect_ROI[i].jabara[0].y ) / 2,
				m_inspect_ROI[i].jabara[1].x - _GAP,
				m_inspect_ROI[i].jabara[1].y - _GAP,
				128, 128, 128,
				PS_SOLID );
		}

		// �̹� ���
		if ( m_inspect_ROI[i].chuck[0].x != -1 && m_inspect_ox.chuck && m_inspect_ox.imul_chuck )
		{
			DrawSquare(
				pDC,
				m_inspect_ROI[i].chuck[0].x + _GAP,
				m_inspect_ROI[i].chuck[0].y + _GAP, // + ( m_inspect_ROI[i].chuck[1].y - m_inspect_ROI[i].chuck[0].y ) / 2,
				m_inspect_ROI[i].chuck[1].x - _GAP,
				m_inspect_ROI[i].chuck[1].y - _GAP,
				128, 128, 128,
				PS_SOLID );
		}

		// �̹� ����
		if ( m_inspect_ROI[i].insert[0].x != -1 && m_inspect_ox.insert && m_inspect_ox.imul_insert )
		{
			int insX2 = m_inspect_ROI[i].insert[0].x + (int)((m_inspect_ROI[i].insert[1].x - m_inspect_ROI[i].insert[0].x) * ( m_inspect_setting.imul_insert_pos / 100.0 ));

			if ( m_inspect_setting.imul_insert_pos == 100 )
				insX2 -= _GAP;

			if ( insX2 < m_inspect_ROI[i].insert[0].x )
				insX2 = m_inspect_ROI[i].insert[0].x;
				
			DrawSquare(
				pDC,
				m_inspect_ROI[i].insert[0].x + _GAP,
				m_inspect_ROI[i].insert[0].y + _GAP,
				insX2,
				m_inspect_ROI[i].insert[1].y - _GAP,
				128, 128, 128,
				PS_SOLID );
		}

		// ��� ���� �׸���
		if(m_inspect_ox.top_angle)
		{
			if (m_inspect_ROI[i].maguri[0].x !=-1 && m_inspect_ox.maguri)
			{
				if(m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.jabara)
				{
					DrawSquare(
						pDC,
						m_inspect_ROI[i].maguri[1].x + 5,
						m_inspect_ROI[i].maguri[0].y + 5,
						m_inspect_ROI[i].jabara[0].x - 5,
						m_inspect_ROI[i].jabara[1].y - 5,
						0, 255, 255, PS_SOLID);
				}
				else if(m_inspect_ROI[i].chuck[0].x != -1, m_inspect_ox.chuck)
				{
					DrawSquare(
						pDC,
						m_inspect_ROI[i].maguri[1].x + 5,
						m_inspect_ROI[i].maguri[0].y + 5,
						(m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].chuck[0].x) / 2 - 5,
						m_inspect_ROI[i].chuck[1].y - 5,
						0, 255, 255, PS_SOLID);
				}
			}
		}

		// �ߴ� ���� �׸���
		if(m_inspect_ox.mid_angle)
		{
			if (m_inspect_ROI[i].chuck[0].x !=-1 && m_inspect_ox.chuck)
			{
				if(m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.jabara)
				{
					DrawSquare(
						pDC,
						m_inspect_ROI[i].jabara[1].x + 5,
						m_inspect_ROI[i].jabara[0].y + 5,
						m_inspect_ROI[i].chuck[0].x - 5,
						m_inspect_ROI[i].chuck[1].y - 5,
						0, 255, 255, PS_SOLID);
				}
				else if(m_inspect_ROI[i].maguri[0].x != -1, m_inspect_ox.maguri)
				{
					DrawSquare(
						pDC,
						(m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].chuck[0].x) / 2 + 5,
						m_inspect_ROI[i].maguri[0].y + 5,
						m_inspect_ROI[i].chuck[0].x - 5,
						m_inspect_ROI[i].chuck[1].y - 5,
						0, 255, 255, PS_SOLID);
				}
			}
		}
	}


	// ��� �׸��� -> �ִ� ��츸
	if (m_drawinsepct)
	{
		// ��� �ִ� ��� ����� OX�� �׸���
		for (int i=0;i<m_ROIS;i++)
		{
			// ���������� �׸���.
			// Exsist ���ϸ� continue;
			if(m_bExistObj[i]==false) continue;

			// ������
			if ( m_inspect_ox.maguri )
			{
				if (m_result_inspect[i].maguri)
				{
					// Oǥ
					DrawCircle(
						pDC,
						(m_inspect_ROI[i].maguri[0].x+m_inspect_ROI[i].maguri[1].x) / 2,
						(m_inspect_ROI[i].maguri[0].y+m_inspect_ROI[i].maguri[1].y) / 2,
						5, 
						255, 0, 0,
						PS_SOLID );
					
				}
				else
				{
					// Xǥ
					DrawLine(
						pDC,
						m_inspect_ROI[i].maguri[0].x,
						m_inspect_ROI[i].maguri[0].y,
						m_inspect_ROI[i].maguri[1].x,
						m_inspect_ROI[i].maguri[1].y,
						255,0,0,PS_SOLID);

					DrawLine(
						pDC,
						m_inspect_ROI[i].maguri[0].x,
						m_inspect_ROI[i].maguri[1].y,
						m_inspect_ROI[i].maguri[1].x,
						m_inspect_ROI[i].maguri[0].y,
						255, 0, 0,
						PS_SOLID );
				}
			}

			// �߰� ���� -------------------------------------------------

			// �̹��� �κ�-------------------------------------------------------
			if ( m_inspect_ox.jabara && m_inspect_ROI[i].jabara[0].x != -1 )
			{
				// �ڹٶ� ���� �� �̹� ���
				if ( m_inspect_ox.imul )
				{
					if ( m_result_inspect[i].imul )
					{
						// O
						DrawCircle(
							pDC, 
							( m_ROI[i][0].x + m_inspect_ROI[i].jabara[0].x) / 2 + _HALF_RAD,
							( m_ROI[i][0].y + m_ROI[i][1].y ) / 2 + _HALF_RAD,
							_IMUL_RAD,
							128, 128, 128,
							PS_SOLID );
					}
					else
					{
						// X
						DrawX(
							pDC,
							m_ROI[i][0].x,
							m_ROI[i][0].y,
							m_inspect_ROI[i].jabara[0].x,
							m_inspect_ROI[i].jabara[1].y,
							128, 128, 128,
							PS_SOLID,
							_GAP );
					}
				}
				
				// �̹� �ߴ�
				if ( m_inspect_ox.imul_mid && m_inspect_ox.chuck )
				{
					if ( m_result_inspect[i].imul_mid )
					{
						// O
						DrawCircle(
							pDC,
							( m_inspect_ROI[i].jabara[1].x + m_inspect_ROI[i].chuck[0].x ) / 2 + _HALF_RAD,
							( m_ROI[i][0].y + m_ROI[i][1].y ) / 2 + _HALF_RAD,
							_IMUL_RAD,
							128, 128, 128,
							PS_SOLID );
					}
					else
					{
						// X
						DrawX(
							pDC,
							m_inspect_ROI[i].jabara[1].x,
							m_ROI[i][0].y,
							m_inspect_ROI[i].chuck[0].x,
							m_ROI[i][1].y,
							128, 128, 128,
							PS_SOLID,
							_GAP );
					}
				}	
			}
			else
			{
				// ����� ������ �˻� �� ��
				if ( m_inspect_ox.chuck && m_inspect_ROI[i].chuck[0].x != -1 )
				{
					// �ڹٶ� ���� �� �̹� ���
					if ( m_inspect_ox.imul )
					{
						if ( m_result_inspect[i].imul )
						{
							DrawCircle(
								pDC,
								m_ROI[i][0].x + (m_ROI[i][0].x + m_inspect_ROI[i].chuck[0].x) / 4 + _HALF_RAD,
								(m_ROI[i][0].y + m_ROI[i][1].y) / 2 + _HALF_RAD,
								_IMUL_RAD,
								128, 128, 128,
								PS_SOLID );
						}
						else
						{
							DrawX(
								pDC,
								m_ROI[i][0].x,
								m_ROI[i][0].y,
								m_ROI[i][0].x + (m_inspect_ROI[i].chuck[0].x - m_ROI[i][0].x ) / 2,
								m_ROI[i][1].y,
								128, 128, 128,
								PS_SOLID,
								_GAP );
						}
					}

					// �ڹٶ� ���� �� �̹� �ߴ�
					if ( m_inspect_ox.imul_mid )
					{
						if ( m_result_inspect[i].imul_mid )
						{
							DrawCircle(
								pDC,
								( m_ROI[i][0].x + m_inspect_ROI[i].chuck[0].x ) * 3 / 4 + _HALF_RAD,
								( m_ROI[i][0].y + m_ROI[i][1].y ) / 2 + _HALF_RAD,
								_IMUL_RAD,
								128, 128, 128,
								PS_SOLID );
						}
						else
						{
							DrawX(
								pDC,
								(m_ROI[i][0].x + m_inspect_ROI[i].chuck[0].x) / 2,
								m_ROI[i][0].y,
								m_inspect_ROI[i].chuck[0].x,
								m_ROI[i][1].y,
								128, 128, 128,
								PS_SOLID,
								_GAP );
						}
					}
				}
			}

			

			// �̹� �ڹٶ� ( �ڹٶ� ���� )
			if ( m_inspect_ox.imul_jabara && m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.jabara )
			{
				if ( m_result_inspect[i].imul_jabara )
				{
					DrawCircle(
						pDC,
						( m_inspect_ROI[i].jabara[0].x + m_inspect_ROI[i].jabara[1].x ) / 2 + _HALF_RAD,
						( m_inspect_ROI[i].jabara[0].y + m_inspect_ROI[i].jabara[1].y ) / 2 + _HALF_RAD,  //+ ( m_ROI[i][1].y - m_ROI[i][0].y ) / 4,
						_IMUL_RAD,
						128, 128, 128,
						PS_SOLID );
				}
				else
				{
					// Xǥ
					DrawX(
						pDC,
						m_inspect_ROI[i].jabara[0].x, m_inspect_ROI[i].jabara[0].y,
						m_inspect_ROI[i].jabara[1].x, m_inspect_ROI[i].jabara[1].y,
						128, 128, 128,
						PS_SOLID,
						_GAP );
				}
			}

			

			// �̹� ��� ( ��� ���� )
			if ( m_inspect_ox.imul_chuck && m_inspect_ox.chuck && m_inspect_ROI[i].chuck[0].x != -1 )
			{
				if ( m_result_inspect[i].imul_chuck )
				{
					// O
					DrawCircle(
						pDC,
						( m_inspect_ROI[i].chuck[0].x + m_inspect_ROI[i].chuck[1].x ) / 2 + _HALF_RAD,
						( m_inspect_ROI[i].chuck[0].y + m_inspect_ROI[i].chuck[1].y ) / 2 + _HALF_RAD,
						_IMUL_RAD,
						128, 128, 128,
						PS_SOLID );
				}
				else
				{
					// X
					DrawX(
						pDC,
						m_inspect_ROI[i].chuck[0].x, m_inspect_ROI[i].chuck[0].y,
						m_inspect_ROI[i].chuck[1].x, m_inspect_ROI[i].chuck[1].y,
						128, 128, 128,
						PS_SOLID,
						_GAP );
				}
			}

			// �̹� ���� ( ���� ���� )
			if ( m_inspect_ox.insert && m_inspect_ROI[i].insert[0].x != -1 && m_inspect_ox.imul_insert )
			{
				if ( m_result_inspect[i].imul_insert )
				{
					// O
					DrawCircle(
						pDC,
						( m_inspect_ROI[i].insert[0].x + m_inspect_ROI[i].insert[1].x ) / 2 + _HALF_RAD,
						( m_inspect_ROI[i].insert[0].y + m_inspect_ROI[i].insert[1].y ) / 2 + _HALF_RAD,
						_IMUL_RAD,
						128, 128, 128,
						PS_SOLID );
				}
				else
				{
					// X
					DrawX(
						pDC,
						m_inspect_ROI[i].insert[0].x, m_inspect_ROI[i].insert[0].y,
						m_inspect_ROI[i].insert[1].x, m_inspect_ROI[i].insert[1].y,
						128, 128, 128,
						PS_SOLID,
						_GAP );
				}
			}

			// �߰� �� ----------------------------------------------

			// �̹� �� ----------------------------------------------

			// �ڹٶ�
			if (m_inspect_ox.jabara)
			{
				if (m_result_inspect[i].jabara)
				{
					// Oǥ
					DrawCircle(pDC,(m_inspect_ROI[i].jabara[0].x+m_inspect_ROI[i].jabara[1].x)/2,(m_inspect_ROI[i].jabara[0].y+m_inspect_ROI[i].jabara[1].y)/2,5,0,255,0,PS_SOLID);
				}
				else
				{
					// Xǥ
					DrawLine(pDC,m_inspect_ROI[i].jabara[0].x,m_inspect_ROI[i].jabara[0].y,m_inspect_ROI[i].jabara[1].x,m_inspect_ROI[i].jabara[1].y,0,255,0,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].jabara[0].x,m_inspect_ROI[i].jabara[1].y,m_inspect_ROI[i].jabara[1].x,m_inspect_ROI[i].jabara[0].y,0,255,0,PS_SOLID);
				}
			}

			// ���
			if (m_inspect_ox.chuck)
			{
				if (m_result_inspect[i].chuck)
				{
					// Oǥ
					DrawCircle(pDC,(m_inspect_ROI[i].chuck[0].x+m_inspect_ROI[i].chuck[1].x)/2,(m_inspect_ROI[i].chuck[0].y+m_inspect_ROI[i].chuck[1].y)/2,5,255,255,0,PS_SOLID);
					
				}
				else
				{
					// Xǥ
					DrawLine(pDC,m_inspect_ROI[i].chuck[0].x,m_inspect_ROI[i].chuck[0].y,m_inspect_ROI[i].chuck[1].x,m_inspect_ROI[i].chuck[1].y,255,255,0,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].chuck[0].x,m_inspect_ROI[i].chuck[1].y,m_inspect_ROI[i].chuck[1].x,m_inspect_ROI[i].chuck[0].y,255,255,0,PS_SOLID);
				}
			}

			// ����
			if (m_inspect_ox.insert)
			{
				if (m_result_inspect[i].insert)
				{
					// Oǥ
					DrawCircle(pDC,(m_inspect_ROI[i].insert[0].x+m_inspect_ROI[i].insert[1].x)/2,(m_inspect_ROI[i].insert[0].y+m_inspect_ROI[i].insert[1].y)/2,5,255,0,255,PS_SOLID);
					
				}
				else
				{
					// Xǥ
					DrawLine(pDC,m_inspect_ROI[i].insert[0].x,m_inspect_ROI[i].insert[0].y,m_inspect_ROI[i].insert[1].x,m_inspect_ROI[i].insert[1].y,255,0,255,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].insert[0].x,m_inspect_ROI[i].insert[1].y,m_inspect_ROI[i].insert[1].x,m_inspect_ROI[i].insert[0].y,255,0,255,PS_SOLID);
				}
			}
			// ��
			if (m_inspect_ox.pok)
			{
				if (m_result_inspect[i].pok != -1.0)
				{
					// Oǥ
					DrawCircle(pDC,(m_inspect_ROI[i].pok[0].x+m_inspect_ROI[i].pok[1].x)/2,(m_inspect_ROI[i].pok[0].y+m_inspect_ROI[i].pok[1].y)/2,5,0,0,255,PS_SOLID);
					
				}
				else
				{
					// Xǥ
					DrawLine(pDC,m_inspect_ROI[i].pok[0].x,m_inspect_ROI[i].pok[0].y,m_inspect_ROI[i].pok[1].x,m_inspect_ROI[i].pok[1].y,0,0,255,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].pok[0].x,m_inspect_ROI[i].pok[1].y,m_inspect_ROI[i].pok[1].x,m_inspect_ROI[i].pok[0].y,0,0,255,PS_SOLID);
				}
			}
			// ��
			for (int j=0;j<2;j++)
			{
				if (m_inspect_ox.angle[j])
				{
					if(m_result_inspect[i].angle[j] >= -900.0)
					{
						// Oǥ
						DrawCircle(pDC,(m_inspect_ROI[i].angle[j][0].x+m_inspect_ROI[i].angle[j][1].x)/2,(m_inspect_ROI[i].angle[j][0].y+m_inspect_ROI[i].angle[j][1].y)/2,5,0,255,255,PS_SOLID);
						
					}
					else
					{
						// Xǥ
						DrawLine(pDC,m_inspect_ROI[i].angle[j][0].x,m_inspect_ROI[i].angle[j][0].y,m_inspect_ROI[i].angle[j][1].x,m_inspect_ROI[i].angle[j][1].y,0,255,255,PS_SOLID);
						DrawLine(pDC,m_inspect_ROI[i].angle[j][0].x,m_inspect_ROI[i].angle[j][1].y,m_inspect_ROI[i].angle[j][1].x,m_inspect_ROI[i].angle[j][0].y,0,255,255,PS_SOLID);
					}
				}
			}
			
			// ��Ƽ��
			if (m_inspect_ox.etiquette)
			{
				if (m_result_inspect[i].etiquette)
				{
					// Oǥ
					DrawCircle(pDC,(m_inspect_ROI[i].etiquette[0].x+m_inspect_ROI[i].etiquette[1].x)/2,(m_inspect_ROI[i].etiquette[0].y+m_inspect_ROI[i].etiquette[1].y)/2,5,128,0,255,PS_SOLID);
				}
				else
				{
					// Xǥ
					DrawLine(
						pDC,
						m_inspect_ROI[i].etiquette[0].x,
						m_inspect_ROI[i].etiquette[0].y,
						m_inspect_ROI[i].etiquette[1].x,
						m_inspect_ROI[i].etiquette[1].y,
						128,0,255,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].etiquette[0].x,m_inspect_ROI[i].etiquette[1].y,m_inspect_ROI[i].etiquette[1].x,m_inspect_ROI[i].etiquette[0].y,128,0,255,PS_SOLID);
				}
			}

			// ��� ����
			if(m_inspect_ox.top_angle)
			{
				if(m_inspect_ox.maguri && m_inspect_ROI[i].maguri[0].x != -1)
				{
					if(m_inspect_ox.jabara && m_inspect_ROI[i].jabara[0].x != -1)
					{
						// O
						if(m_result_inspect[i].angle[0] != -1000)
						{
							DrawCircle(pDC,
								(m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].jabara[0].x) / 2,
								(m_inspect_ROI[i].maguri[0].y + m_inspect_ROI[i].maguri[1].y) / 2,
								5,
								0, 255, 255,
								PS_SOLID);
						}
						// X
						else 
						{
							DrawLine(
								pDC,
								m_inspect_ROI[i].maguri[1].x + 5,
								m_inspect_ROI[i].maguri[0].y + 5,
								m_inspect_ROI[i].jabara[0].x - 5,
								m_inspect_ROI[i].jabara[1].y - 5,
								0, 255, 255,
								PS_SOLID);
							DrawLine(
								pDC,
								m_inspect_ROI[i].jabara[0].x -5,
								m_inspect_ROI[i].jabara[0].y + 5,
								m_inspect_ROI[i].maguri[1].x + 5,
								m_inspect_ROI[i].maguri[1].y - 5,
								0, 255, 255,
								PS_SOLID);
						}
					}
					else if(m_inspect_ox.chuck && m_inspect_ROI[i].chuck[0].x != -1)
					{
						// O
						if(m_result_inspect[i].angle[0] != -1000)
						{
							DrawCircle(pDC,
								(3 * m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].chuck[0].x) / 4,
								(m_inspect_ROI[i].maguri[0].y + m_inspect_ROI[i].maguri[1].y) / 2,
								5,
								0, 255, 255,
								PS_SOLID);
						}
						// X
						else 
						{
							DrawLine(
								pDC,
								m_inspect_ROI[i].maguri[1].x + 5,
								m_inspect_ROI[i].maguri[0].y + 5,
								(m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].chuck[0].x) / 2 - 5,
								m_inspect_ROI[i].chuck[1].y - 5,
								0, 255, 255,
								PS_SOLID);
							DrawLine(
								pDC,
								(m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].chuck[0].x) / 2 - 5,
								m_inspect_ROI[i].chuck[0].y + 5,
								m_inspect_ROI[i].maguri[1].x + 5,
								m_inspect_ROI[i].maguri[1].y - 5,
								0, 255, 255,
								PS_SOLID);
						}
					}
				}
			}

			// �ߴ� ����
			if(m_inspect_ox.mid_angle)
			{
				if(m_inspect_ox.chuck && m_inspect_ROI[i].chuck[0].x != -1)
				{
					if(m_inspect_ox.jabara && m_inspect_ROI[i].jabara[0].x != -1)
					{
						// O
						if(m_result_inspect[i].pok != -1.0)
						{
							DrawCircle(pDC,
								(m_inspect_ROI[i].jabara[1].x + m_inspect_ROI[i].chuck[0].x) / 2,
								(m_inspect_ROI[i].jabara[0].y + m_inspect_ROI[i].jabara[1].y) / 2,
								5,
								0, 255, 255,
								PS_SOLID);
						}
						// X
						else 
						{
							DrawLine(
								pDC,
								m_inspect_ROI[i].jabara[1].x + 5,
								m_inspect_ROI[i].jabara[0].y + 5,
								m_inspect_ROI[i].chuck[0].x - 5,
								m_inspect_ROI[i].chuck[1].y - 5,
								0, 255, 255,
								PS_SOLID);
							DrawLine(
								pDC,
								m_inspect_ROI[i].chuck[0].x -5,
								m_inspect_ROI[i].chuck[0].y + 5,
								m_inspect_ROI[i].jabara[1].x + 5,
								m_inspect_ROI[i].jabara[1].y - 5,
								0, 255, 255,
								PS_SOLID);
						}
					}
					else if(m_inspect_ox.maguri && m_inspect_ROI[i].maguri[0].x != -1)
					{
						// O
						if(m_result_inspect[i].pok != -1.0)
						{
							DrawCircle(pDC,
								m_inspect_ROI[i].maguri[1].x / 4 + m_inspect_ROI[i].chuck[0].x * 3 / 4,
								(m_inspect_ROI[i].chuck[0].y + m_inspect_ROI[i].chuck[1].y) / 2,
								5,
								0, 255, 255,
								PS_SOLID);
						}
						// X
						else 
						{
							DrawLine(
								pDC,
								(m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].chuck[0].x) / 2 + 5,
								m_inspect_ROI[i].maguri[0].y + 5,
								m_inspect_ROI[i].chuck[0].x - 5,
								m_inspect_ROI[i].chuck[1].y - 5,
								0, 255, 255,
								PS_SOLID);
							DrawLine(
								pDC,
								m_inspect_ROI[i].chuck[0].x - 5,
								m_inspect_ROI[i].chuck[0].y + 5,
								(m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].chuck[0].x) / 2 + 5,
								m_inspect_ROI[i].maguri[1].y - 5,
								0, 255, 255,
								PS_SOLID);
						}
					}
				}
			}
		}
	
		m_drawinsepct = false;

	}

	ReleaseDC(pDC);

}

void CInspectImage::DrawImage(CViewingImage *pImage)
{
	CDC *pDC = GetDC();
	CDC dcMem;
	CRect rectViewer;

	GetClientRect(rectViewer);

	if ( m_pbitmap ) delete m_pbitmap;
 	m_pbitmap = new CBitmap;		
	m_pbitmap->CreateCompatibleBitmap(pDC,rectViewer.Width(), rectViewer.Height());

	m_iWinWidth = rectViewer.Width();
	m_iWinHeight = rectViewer.Height();

	dcMem.CreateCompatibleDC(pDC);	
	dcMem.SelectObject(m_pbitmap);

	// �̹����� ��Ʈ�� �׳� �Ѹ���

	// ��Ʈ�� ���� ����
	BITMAPINFO *BmInfo = new BITMAPINFO;
	BmInfo->bmiHeader.biBitCount = 24;
	BmInfo->bmiHeader.biCompression= 0;
	BmInfo->bmiHeader.biHeight = _ShowHeight;
	BmInfo->bmiHeader.biPlanes = 1;
	BmInfo->bmiHeader.biSizeImage = _ShowWidth*_ShowHeight;
	BmInfo->bmiHeader.biSize = 40;
	BmInfo->bmiHeader.biWidth = _ShowWidth;
	BmInfo->bmiHeader.biXPelsPerMeter = 0;
	BmInfo->bmiHeader.biYPelsPerMeter = 0;

	StretchDIBits(dcMem.GetSafeHdc(),
				1, 1, rectViewer.Width()-1, rectViewer.Height()-1,
				0, 0, rectViewer.Width()-1, rectViewer.Height()-1,
				pImage->m_nowView->imageData, BmInfo, DIB_RGB_COLORS, SRCCOPY);

	
	m_iImageWidth = pImage->m_nowView->width;
	m_iImageHeight = pImage->m_nowView->height;


	Invalidate(FALSE);
	ReleaseDC(pDC);	

	delete [] BmInfo;
}

void CInspectImage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(_MAXROI <= 0 || _MAXROI > 9)
		return;

	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	// TODO: Add your message handler code here and/or call default
	
	// ������ : 1 �ڹٶ� : 2 ��� : 3 ���� : 4
	// �� : 5 �� �� : 11 ���� : 12 �̹��� : 20~29
	// ��Ƽ��: 30

	// �׸��� �� ROI�� 100�ڸ��� 
	int now = status_down / 100;
	int stat = status_down % 100;

	double dRatioX = (double)_CamWidth/_ShowWidth;
	double dRatioY = (double)_CamHeight/_ShowHeight;
	point.x = (int)(point.x * dRatioX);
	point.y = (int)(point.y * dRatioY);

	// ROI ������� �˻�
	if (status_down > 0 && status_down < 900)
	{
		if(point.x >= m_ROI[now][1].x)
			point.x = m_ROI[now][1].x - 1;

		if(point.x <= m_ROI[now][0].x)
			point.x = m_ROI[now][0].x + 1;

		if(point.y >= m_ROI[now][1].y)
			point.y = m_ROI[now][1].y - 1;

		if(point.y <= m_ROI[now][0].y)
			point.y = m_ROI[now][0].y + 1;
	}

	// ������ �˻�� �Ǿ��ִ�.
	if (stat == 1 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;

		m_inspect_ROI[now].maguri[0] = point;

		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.maguri[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.maguri[0].y = point.y - m_ROI[0][0].y ;

		}
	}

	// �ڹٶ� �˻�� �Ǿ��ִ�.
	if (stat == 2 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 ������

		m_inspect_ROI[now].jabara[0] = point;
		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.jabara[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.jabara[0].y = point.y - m_ROI[0][0].y ;
		}

	}

	// ��� �˻�� �Ǿ��ִ�.
	if (stat == 3 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;

		// status 100 ������

		m_inspect_ROI[now].chuck[0] = point;
		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.chuck[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.chuck[0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// ���� �˻�� �Ǿ��ִ�.
	if (stat == 4 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 ������

		m_inspect_ROI[now].insert[0] = point;

		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.insert[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.insert[0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// �� �˻�� �Ǿ��ִ�.
	if (stat == 5 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 ������

		m_inspect_ROI[now].pok[0] = point;

		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.pok[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.pok[0].y = point.y - m_ROI[0][0].y ;

		}

	}

	// ������ �˻�� �Ǿ��ִ�.
	if (stat == 11 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 ������

		m_inspect_ROI[now].angle[0][0] = point;

		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.angle[0][0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.angle[0][0].y = point.y - m_ROI[0][0].y ;

		}
	}

	// ������ �˻�� �Ǿ��ִ�.
	if (stat == 12 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 ������

		m_inspect_ROI[now].angle[1][0] = point;
		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.angle[1][0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.angle[1][0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// ���� ��Ƽ�� �̹� �˻�� �Ǿ��ִ�.
	if (stat == 40 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 ������

		m_inspect_ROI[now].imul_etq_left[0] = point;

		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.imul_etq_left[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul_etq_left[0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// ���� ��Ƽ�� �̹� �˻�� �Ǿ��ִ�.
	if (stat == 41 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 ������

		m_inspect_ROI[now].imul_etq_right[0] = point;

		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.imul_etq_right[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul_etq_right[0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// �̹��� �˻�
	//if (status_down > 19 && status_down < 30)
	//{
	//	int now_imul = status_down - 20;
	//	status_move = status_down;
		// 1�� �����̸� Offset�� ����
	//	if (now == 0)
	//	{
	//		m_inspect_offset.imul[0].x = point.x - m_ROI[0][0].x ;
	//		m_inspect_offset.imul[0].y = point.y - m_ROI[0][0].y ;
	//	}
	//}

	// ��Ƽ�� �˻�
	if(stat == 30 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;

		m_inspect_ROI[now].etiquette[0] = point;

		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.etiquette[0].x = point.x - m_ROI[0][0].x;
			m_inspect_offset.etiquette[0].y = point.y - m_ROI[0][0].y;
		}
	}

	// ROI ������ 900~908
	if (status_down >899 && status_down < 1000)
	{
		int roi = status_down - 900;
		status_move = status_down;
		status_down = 0;
		m_ROI[roi][0] = point;
	}

	// ROI ������ ���� 1001~1003 (num��ȣ ����)
	if (status_down >= 1000 && status_down <1000 + _MAXROI)
	{
		// ���� ���� ����� ������ ����
		int nowx, nowy;
		nowx = point.x + abs(m_ROI[0][0].x - m_ROI[0][1].x); 
		nowy = point.y + abs(m_ROI[0][0].y - m_ROI[0][1].y); 
		
		if (nowx > _CamWidth-1)
		{
			point.x = _CamWidth-1-abs(m_ROI[0][0].x - m_ROI[0][1].x); 
		}

		if (nowy > _CamHeight-1)
		{
			point.y = _CamHeight-1-abs(m_ROI[0][0].y - m_ROI[0][1].y); 
		}

		m_ROI[status_down-1000][0] = point;
		m_ROI[status_down-1000][1].x = point.x + abs(m_ROI[0][0].x - m_ROI[0][1].x); 
		m_ROI[status_down-1000][1].y = point.y + abs(m_ROI[0][0].y - m_ROI[0][1].y);
		
		DrawCopyBoxes(status_down-1000);
		Invalidate(FALSE);

		status_move = status_down;
	}

	if (status_down > 1999 && status_down <2004)
	{
		// ���� ���� ����� ������ ����
		int nowx, nowy;
		nowx = point.x + abs(m_ROI2[0].x - m_ROI2[1].x); 
		nowy = point.y + abs(m_ROI2[0].y - m_ROI2[1].y); 
		
		if (nowx > _CamWidth-1)
		{
			point.x = _CamWidth-1-abs(m_ROI2[0].x - m_ROI2[1].x); 
		}
		
		if (nowy > _CamHeight-1)
		{
			point.y = _CamHeight-1-abs(m_ROI2[0].y - m_ROI2[1].y); 
		}

		m_ROI[status_down-2000][0] = point;
		m_ROI[status_down-2000][1].x = point.x + abs(m_ROI2[0].x - m_ROI2[1].x); 
		m_ROI[status_down-2000][1].y = point.y + abs(m_ROI2[0].y - m_ROI2[1].y);

		DrawCopyBoxes(status_down-2000);
		Invalidate(FALSE);

		status_move = status_down;
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void CInspectImage::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(_MAXROI <= 0 || _MAXROI > 9)
	{
		return;
	}

	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
	{
		return;
	}

	// TODO: Add your message handler code here and/or call default
	
	double dRatioX = (double)_CamWidth/_ShowWidth;
	double dRatioY = (double)_CamHeight/_ShowHeight;
	point.x = (int)(point.x * dRatioX);
	point.y = (int)(point.y * dRatioY);

	// ���� ȸ��
	if (status_move < 900)
	{
  		int now = status_move / 100;

		// ���� �ൿ

		// �̹����� ��� �̹��� �����
		if (status_move > 19 && status_move < 30)
		{
			int now_imul = status_move - 20;
		}
		
		// ������ : 1 �ڹٶ� : 2 ��� : 3 ���� : 4
		// �� : 5 �� �� : 11 ���� : 12 �̹��� : 20~29
		// ��Ƽ��: 30

		// ������ �˻�� �Ǿ��ִ�.
		if (status_move == 1 && status_down < 900)
		{
			//m_start = point;
			// status 100 ������
			//m_inspect_ROI[now].maguri = point;]
			if (m_inspect_ROI[now].maguri[0].x > point.x)
			{
				m_inspect_ROI[now].maguri[1].x = m_inspect_ROI[now].maguri[0].x;
				m_inspect_ROI[now].maguri[0].x = point.x;
			}
			if (m_inspect_ROI[now].maguri[0].y > point.y)
			{
				m_inspect_ROI[now].maguri[1].y = m_inspect_ROI[now].maguri[0].y;
				m_inspect_ROI[now].maguri[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.maguri[0].x = m_inspect_ROI[now].maguri[0].x - m_ROI[0][0].x ;
				m_inspect_offset.maguri[0].y = m_inspect_ROI[now].maguri[0].y - m_ROI[0][0].y ;
				m_inspect_offset.maguri[1].x = m_inspect_ROI[now].maguri[1].x - m_ROI[0][0].x ;
				m_inspect_offset.maguri[1].y = m_inspect_ROI[now].maguri[1].y - m_ROI[0][0].y ;
			}

			// �ٸ� ROI �� ������ �ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].maguri[0].x = m_ROI[i][0].x + m_inspect_offset.maguri[0].x;
					m_inspect_ROI[i].maguri[0].y = m_ROI[i][0].y + m_inspect_offset.maguri[0].y;
					m_inspect_ROI[i].maguri[1].x = m_ROI[i][0].x + m_inspect_offset.maguri[1].x;
					m_inspect_ROI[i].maguri[1].y = m_ROI[i][0].y + m_inspect_offset.maguri[1].y;

				}
			}
		}

		// �ڹٶ� �˻�� �Ǿ��ִ�.
		if (status_move == 2 && status_down < 900)
		{
			// status 100 ������

			if (m_inspect_ROI[now].jabara[0].x > point.x)
			{
				m_inspect_ROI[now].jabara[1].x = m_inspect_ROI[now].jabara[0].x;
				m_inspect_ROI[now].jabara[0].x = point.x;
			}
			if (m_inspect_ROI[now].jabara[0].y > point.y)
			{
				m_inspect_ROI[now].jabara[1].y = m_inspect_ROI[now].jabara[0].y;
				m_inspect_ROI[now].jabara[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.jabara[0].x = m_inspect_ROI[now].jabara[0].x - m_ROI[0][0].x ;
				m_inspect_offset.jabara[0].y = m_inspect_ROI[now].jabara[0].y - m_ROI[0][0].y ;
				m_inspect_offset.jabara[1].x = m_inspect_ROI[now].jabara[1].x - m_ROI[0][0].x ;
				m_inspect_offset.jabara[1].y = m_inspect_ROI[now].jabara[1].y - m_ROI[0][0].y ;

			}
			// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].jabara[0].x = m_ROI[i][0].x + m_inspect_offset.jabara[0].x;
					m_inspect_ROI[i].jabara[0].y = m_ROI[i][0].y + m_inspect_offset.jabara[0].y;
					m_inspect_ROI[i].jabara[1].x = m_ROI[i][0].x + m_inspect_offset.jabara[1].x;
					m_inspect_ROI[i].jabara[1].y = m_ROI[i][0].y + m_inspect_offset.jabara[1].y;

				}
			}
		}

		// ��� �˻�� �Ǿ��ִ�.
		if (status_move == 3 && status_down < 900)
		{
			// status 100 ������
			if (m_inspect_ROI[now].chuck[0].x > point.x)
			{
				m_inspect_ROI[now].chuck[1].x = m_inspect_ROI[now].chuck[0].x;
				m_inspect_ROI[now].chuck[0].x = point.x;
			}
			if (m_inspect_ROI[now].chuck[0].y > point.y)
			{
				m_inspect_ROI[now].chuck[1].y = m_inspect_ROI[now].chuck[0].y;
				m_inspect_ROI[now].chuck[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.chuck[0].x = m_inspect_ROI[now].chuck[0].x - m_ROI[0][0].x ;
				m_inspect_offset.chuck[0].y = m_inspect_ROI[now].chuck[0].y - m_ROI[0][0].y ;
				m_inspect_offset.chuck[1].x = m_inspect_ROI[now].chuck[1].x - m_ROI[0][0].x ;
				m_inspect_offset.chuck[1].y = m_inspect_ROI[now].chuck[1].y - m_ROI[0][0].y ;

			}
			// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].chuck[0].x = m_ROI[i][0].x + m_inspect_offset.chuck[0].x;
					m_inspect_ROI[i].chuck[0].y = m_ROI[i][0].y + m_inspect_offset.chuck[0].y;
					m_inspect_ROI[i].chuck[1].x = m_ROI[i][0].x + m_inspect_offset.chuck[1].x;
					m_inspect_ROI[i].chuck[1].y = m_ROI[i][0].y + m_inspect_offset.chuck[1].y;

				}
			}

		}

		// ���� �˻�� �Ǿ��ִ�.
		if (status_move == 4 && status_down < 900)
		{
			// status 100 ������
			if (m_inspect_ROI[now].insert[0].x > point.x)
			{
				m_inspect_ROI[now].insert[1].x = m_inspect_ROI[now].insert[0].x;
				m_inspect_ROI[now].insert[0].x = point.x;
			}
			if (m_inspect_ROI[now].insert[0].y > point.y)
			{
				m_inspect_ROI[now].insert[1].y = m_inspect_ROI[now].insert[0].y;
				m_inspect_ROI[now].insert[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.insert[0].x = m_inspect_ROI[now].insert[0].x - m_ROI[0][0].x ;
				m_inspect_offset.insert[0].y = m_inspect_ROI[now].insert[0].y - m_ROI[0][0].y ;
				m_inspect_offset.insert[1].x = m_inspect_ROI[now].insert[1].x - m_ROI[0][0].x ;
				m_inspect_offset.insert[1].y = m_inspect_ROI[now].insert[1].y - m_ROI[0][0].y ;

			}
			// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].insert[0].x = m_ROI[i][0].x + m_inspect_offset.insert[0].x;
					m_inspect_ROI[i].insert[0].y = m_ROI[i][0].y + m_inspect_offset.insert[0].y;
					m_inspect_ROI[i].insert[1].x = m_ROI[i][0].x + m_inspect_offset.insert[1].x;
					m_inspect_ROI[i].insert[1].y = m_ROI[i][0].y + m_inspect_offset.insert[1].y;

				}
			}

		}

		// �� �˻�� �Ǿ��ִ�.
		if (status_move == 5 && status_down < 900)
		{
			// status 100 ������
			if (m_inspect_ROI[now].pok[0].x > point.x)
			{
				m_inspect_ROI[now].pok[1].x = m_inspect_ROI[now].pok[0].x;
				m_inspect_ROI[now].pok[0].x = point.x;
			}
			if (m_inspect_ROI[now].pok[0].y > point.y)
			{
				m_inspect_ROI[now].pok[1].y = m_inspect_ROI[now].pok[0].y;
				m_inspect_ROI[now].pok[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.pok[0].x = m_inspect_ROI[now].pok[0].x - m_ROI[0][0].x ;
				m_inspect_offset.pok[0].y = m_inspect_ROI[now].pok[0].y - m_ROI[0][0].y ;
				m_inspect_offset.pok[1].x = m_inspect_ROI[now].pok[1].x - m_ROI[0][0].x ;
				m_inspect_offset.pok[1].y = m_inspect_ROI[now].pok[1].y - m_ROI[0][0].y ;

			}
			// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].pok[0].x = m_ROI[i][0].x + m_inspect_offset.pok[0].x;
					m_inspect_ROI[i].pok[0].y = m_ROI[i][0].y + m_inspect_offset.pok[0].y;
					m_inspect_ROI[i].pok[1].x = m_ROI[i][0].x + m_inspect_offset.pok[1].x;
					m_inspect_ROI[i].pok[1].y = m_ROI[i][0].y + m_inspect_offset.pok[1].y;

				}
			}
		}


		// ���� ��Ƽ�� �̹� �˻�� �Ǿ��ִ�.
		if (status_move == 40 && status_down < 900)
		{
			if (m_inspect_ROI[now].imul_etq_left[0].x > point.x)
			{
				m_inspect_ROI[now].imul_etq_left[1].x = m_inspect_ROI[now].imul_etq_left[0].x;
				m_inspect_ROI[now].imul_etq_left[0].x = point.x;
			}
			if (m_inspect_ROI[now].imul_etq_left[0].y > point.y)
			{
				m_inspect_ROI[now].imul_etq_left[1].y = m_inspect_ROI[now].imul_etq_left[0].y;
				m_inspect_ROI[now].imul_etq_left[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.imul_etq_left[0].x = m_inspect_ROI[now].imul_etq_left[0].x - m_ROI[0][0].x ;
				m_inspect_offset.imul_etq_left[0].y = m_inspect_ROI[now].imul_etq_left[0].y - m_ROI[0][0].y ;
				m_inspect_offset.imul_etq_left[1].x = m_inspect_ROI[now].imul_etq_left[1].x - m_ROI[0][0].x ;
				m_inspect_offset.imul_etq_left[1].y = m_inspect_ROI[now].imul_etq_left[1].y - m_ROI[0][0].y ;
			}

			// �ٸ� ROI �� ������ �ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].imul_etq_left[0].x = m_ROI[i][0].x + m_inspect_offset.imul_etq_left[0].x;
					m_inspect_ROI[i].imul_etq_left[0].y = m_ROI[i][0].y + m_inspect_offset.imul_etq_left[0].y;
					m_inspect_ROI[i].imul_etq_left[1].x = m_ROI[i][0].x + m_inspect_offset.imul_etq_left[1].x;
					m_inspect_ROI[i].imul_etq_left[1].y = m_ROI[i][0].y + m_inspect_offset.imul_etq_left[1].y;

				}
			}
		}





		// ���� ��Ƽ�� �̹� �˻�� �Ǿ��ִ�.
		if (status_move == 41 && status_down < 900)
		{
			if (m_inspect_ROI[now].imul_etq_right[0].x > point.x)
			{
				m_inspect_ROI[now].imul_etq_right[1].x = m_inspect_ROI[now].imul_etq_right[0].x;
				m_inspect_ROI[now].imul_etq_right[0].x = point.x;
			}
			if (m_inspect_ROI[now].imul_etq_right[0].y > point.y)
			{
				m_inspect_ROI[now].imul_etq_right[1].y = m_inspect_ROI[now].imul_etq_right[0].y;
				m_inspect_ROI[now].imul_etq_right[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.imul_etq_right[0].x = m_inspect_ROI[now].imul_etq_right[0].x - m_ROI[0][0].x ;
				m_inspect_offset.imul_etq_right[0].y = m_inspect_ROI[now].imul_etq_right[0].y - m_ROI[0][0].y ;
				m_inspect_offset.imul_etq_right[1].x = m_inspect_ROI[now].imul_etq_right[1].x - m_ROI[0][0].x ;
				m_inspect_offset.imul_etq_right[1].y = m_inspect_ROI[now].imul_etq_right[1].y - m_ROI[0][0].y ;
			}

			// �ٸ� ROI �� ������ �ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].imul_etq_right[0].x = m_ROI[i][0].x + m_inspect_offset.imul_etq_right[0].x;
					m_inspect_ROI[i].imul_etq_right[0].y = m_ROI[i][0].y + m_inspect_offset.imul_etq_right[0].y;
					m_inspect_ROI[i].imul_etq_right[1].x = m_ROI[i][0].x + m_inspect_offset.imul_etq_right[1].x;
					m_inspect_ROI[i].imul_etq_right[1].y = m_ROI[i][0].y + m_inspect_offset.imul_etq_right[1].y;
				}
			}
		}





		// ������ �˻�� �Ǿ��ִ�.
		if (status_move == 11 && status_down < 900)
		{
			// status 100 ������
			if (m_inspect_ROI[now].angle[0][0].x > point.x)
			{
				m_inspect_ROI[now].angle[0][1].x = m_inspect_ROI[now].angle[0][0].x;
				m_inspect_ROI[now].angle[0][0].x = point.x;
			}
			if (m_inspect_ROI[now].angle[0][0].y > point.y)
			{
				m_inspect_ROI[now].angle[0][1].y = m_inspect_ROI[now].angle[0][0].y;
				m_inspect_ROI[now].angle[0][0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.angle[0][0].x = m_inspect_ROI[now].angle[0][0].x - m_ROI[0][0].x ;
				m_inspect_offset.angle[0][0].y = m_inspect_ROI[now].angle[0][0].y - m_ROI[0][0].y ;
				m_inspect_offset.angle[0][1].x = m_inspect_ROI[now].angle[0][1].x - m_ROI[0][0].x ;
				m_inspect_offset.angle[0][1].y = m_inspect_ROI[now].angle[0][1].y - m_ROI[0][0].y ;

			}
			// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].angle[0][0].x = m_ROI[i][0].x + m_inspect_offset.angle[0][0].x;
					m_inspect_ROI[i].angle[0][0].y = m_ROI[i][0].y + m_inspect_offset.angle[0][0].y;
					m_inspect_ROI[i].angle[0][1].x = m_ROI[i][0].x + m_inspect_offset.angle[0][1].x;
					m_inspect_ROI[i].angle[0][1].y = m_ROI[i][0].y + m_inspect_offset.angle[0][1].y;

				}
			}

		}



		// ������ �˻�� �Ǿ��ִ�.
		if (status_move == 12 && status_down < 900)
		{
			// status 100 ������
			if (m_inspect_ROI[now].angle[1][0].x > point.x)
			{
				m_inspect_ROI[now].angle[1][1].x = m_inspect_ROI[now].angle[1][0].x;
				m_inspect_ROI[now].angle[1][0].x = point.x;
			}
			if (m_inspect_ROI[now].angle[1][0].y > point.y)
			{
				m_inspect_ROI[now].angle[1][1].y = m_inspect_ROI[now].angle[1][0].y;
				m_inspect_ROI[now].angle[1][0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.angle[1][0].x = m_inspect_ROI[now].angle[1][0].x - m_ROI[0][0].x ;
				m_inspect_offset.angle[1][0].y = m_inspect_ROI[now].angle[1][0].y - m_ROI[0][0].y ;
				m_inspect_offset.angle[1][1].x = m_inspect_ROI[now].angle[1][1].x - m_ROI[0][0].x ;
				m_inspect_offset.angle[1][1].y = m_inspect_ROI[now].angle[1][1].y - m_ROI[0][0].y ;

			}
			// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].angle[1][0].x = m_ROI[i][0].x + m_inspect_offset.angle[1][0].x;
					m_inspect_ROI[i].angle[1][0].y = m_ROI[i][0].y + m_inspect_offset.angle[1][0].y;
					m_inspect_ROI[i].angle[1][1].x = m_ROI[i][0].x + m_inspect_offset.angle[1][1].x;
					m_inspect_ROI[i].angle[1][1].y = m_ROI[i][0].y + m_inspect_offset.angle[1][1].y;

				}
			}

		}

		// �̹��� �˻�
		if (status_down > 19 && status_down < 30)
		{
			int now_imul = status_down - 20;
			status_move = status_down;

			if (m_inspect_ROI[now].imul[0].x > point.x)
			{
				m_inspect_ROI[now].imul[0].x = point.x;
			}
			if (m_inspect_ROI[now].imul[0].y > point.y)
			{
				m_inspect_ROI[now].imul[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.imul[0].x = m_inspect_ROI[now].imul[0].x - m_ROI[0][0].x ;
				m_inspect_offset.imul[0].y = m_inspect_ROI[now].imul[0].y - m_ROI[0][0].y ;
				m_inspect_offset.imul[1].x = m_inspect_ROI[now].imul[1].x - m_ROI[0][0].x ;
				m_inspect_offset.imul[1].y = m_inspect_ROI[now].imul[1].y - m_ROI[0][0].y ;

			}
			// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].imul[0].x = m_ROI[i][0].x + m_inspect_offset.imul[0].x;
					m_inspect_ROI[i].imul[0].y = m_ROI[i][0].y + m_inspect_offset.imul[0].y;
					m_inspect_ROI[i].imul[1].x = m_ROI[i][0].x + m_inspect_offset.imul[1].x;
					m_inspect_ROI[i].imul[1].y = m_ROI[i][0].y + m_inspect_offset.imul[1].y;

				}
			}
		}

		// ��Ƽ�� �˻�� �Ǿ��ִ�.
		if (status_move == 30 && status_down < 900)
		{
			// status 100 ������
			if (m_inspect_ROI[now].etiquette[0].x > point.x)
			{
				m_inspect_ROI[now].etiquette[1].x = m_inspect_ROI[now].etiquette[0].x;
				m_inspect_ROI[now].etiquette[0].x = point.x;
			}
			if (m_inspect_ROI[now].etiquette[0].y > point.y)
			{
				m_inspect_ROI[now].etiquette[1].y = m_inspect_ROI[now].etiquette[0].y;
				m_inspect_ROI[now].etiquette[0].y = point.y;
			}
			// 1�� �����̸� Offset�� ����
			if (now == 0)
			{
				m_inspect_offset.etiquette[0].x = m_inspect_ROI[now].etiquette[0].x - m_ROI[0][0].x ;
				m_inspect_offset.etiquette[0].y = m_inspect_ROI[now].etiquette[0].y - m_ROI[0][0].y ;
				m_inspect_offset.etiquette[1].x = m_inspect_ROI[now].etiquette[1].x - m_ROI[0][0].x ;
				m_inspect_offset.etiquette[1].y = m_inspect_ROI[now].etiquette[1].y - m_ROI[0][0].y ;

			}
			// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
			for (int i = 1; i < _MAXROI; i++)
			{
				if (m_ROI[i][0].x != -1)
				{
					m_inspect_ROI[i].etiquette[0].x = m_ROI[i][0].x + m_inspect_offset.etiquette[0].x;
					m_inspect_ROI[i].etiquette[0].y = m_ROI[i][0].y + m_inspect_offset.etiquette[0].y;
					m_inspect_ROI[i].etiquette[1].x = m_ROI[i][0].x + m_inspect_offset.etiquette[1].x;
					m_inspect_ROI[i].etiquette[1].y = m_ROI[i][0].y + m_inspect_offset.etiquette[1].y;

				}
			}

		}


		status_move = 0;
	//--------------------------------------------------------------------
	}
	if (status_move >= 900 && status_move <1000)
	{
		int now = status_move - 900;
		
		// ���� �ൿ
		status_move = 0;
	}

	if (status_move >= 1000 && status_move < 1010)
	{
		int now = status_move - 1000;
		
		// ���� �ൿ
		status_move = 0;


	}

	if (status_move > 1999 && status_move < 2010)
	{
		int now = status_move-2000;
		status_move = 0;
	}

	status_down = 0;

	CStatic::OnLButtonUp(nFlags, point);
}

void CInspectImage::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(_MAXROI <= 0 || _MAXROI > 9)
	{
		return;
	}
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
	{
		return;
	}

	// �׸��� �� ROI�� 100�ڸ��� 
	double dRatioX = (double)_CamWidth/_ShowWidth;
	double dRatioY = (double)_CamHeight/_ShowHeight;
	point.x = (int)(point.x * dRatioX);
	point.y = (int)(point.y * dRatioY);

 	int now = status_move / 100;
	int stat = status_move % 100;

	// ROI ������� �˻�
	if (status_move >0 && status_move <900)
	{
		if (point.x >= m_ROI[now][1].x)
		{
			point.x = m_ROI[now][1].x - 1;
		}
		if (point.y >= m_ROI[now][1].y)
		{
			point.y = m_ROI[now][1].y - 1;
		}
		if (point.x <= m_ROI[now][0].x)
		{
			point.x = m_ROI[now][0].x + 1;
		}
		if (point.y <= m_ROI[now][0].y)
		{
			point.y = m_ROI[now][0].y + 1;
		}
	}


	// ������ �̵���
	if (stat == 1 && status_move < 900)
	{
		//m_now = point;
		//m_inspect_ROI[now].maguri.height = m_inspect_ROI[now];
		m_inspect_ROI[now].maguri[1] = point;

		if (now == 0)
		{
			m_inspect_offset.maguri[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.maguri[1].y = point.y - m_ROI[0][0].y ;

		}

		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].maguri[0].x = m_ROI[i][0].x + m_inspect_offset.maguri[0].x;
				m_inspect_ROI[i].maguri[0].y = m_ROI[i][0].y + m_inspect_offset.maguri[0].y;
				m_inspect_ROI[i].maguri[1].x = m_ROI[i][0].x + m_inspect_offset.maguri[1].x;
				m_inspect_ROI[i].maguri[1].y = m_ROI[i][0].y + m_inspect_offset.maguri[1].y;

			}
		}

		Invalidate(FALSE);
	}

	// ROI ������ 900~903
	if (status_move >899 && status_move < 1000)
	{
		int roi = status_move - 900;
		
		m_ROI[roi][1] = point;



		Invalidate(FALSE);
	}

	// �ڹٶ� �˻�� �Ǿ��ִ�.
	if (stat == 2 && status_move < 900)
	{
		// status 100 ������

		m_inspect_ROI[now].jabara[1] = point;

		if (now == 0)
		{
			m_inspect_offset.jabara[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.jabara[1].y = point.y - m_ROI[0][0].y ;

		}
		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].jabara[0].x = m_ROI[i][0].x + m_inspect_offset.jabara[0].x;
				m_inspect_ROI[i].jabara[0].y = m_ROI[i][0].y + m_inspect_offset.jabara[0].y;
				m_inspect_ROI[i].jabara[1].x = m_ROI[i][0].x + m_inspect_offset.jabara[1].x;
				m_inspect_ROI[i].jabara[1].y = m_ROI[i][0].y + m_inspect_offset.jabara[1].y;

			}
		}

		Invalidate(FALSE);

	}

	// ��� �˻�� �Ǿ��ִ�.
	if (stat == 3 && status_move < 900)
	{
		// status 100 ������

		m_inspect_ROI[now].chuck[1] = point;

		if (now == 0)
		{
			m_inspect_offset.chuck[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.chuck[1].y = point.y - m_ROI[0][0].y ;

		}

		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].chuck[0].x = m_ROI[i][0].x + m_inspect_offset.chuck[0].x;
				m_inspect_ROI[i].chuck[0].y = m_ROI[i][0].y + m_inspect_offset.chuck[0].y;
				m_inspect_ROI[i].chuck[1].x = m_ROI[i][0].x + m_inspect_offset.chuck[1].x;
				m_inspect_ROI[i].chuck[1].y = m_ROI[i][0].y + m_inspect_offset.chuck[1].y;

			}
		}


		Invalidate(FALSE);
	}

	// ���� �˻�� �Ǿ��ִ�.
	if (stat == 4 && status_move < 900)
	{
		// status 100 ������

		m_inspect_ROI[now].insert[1] = point;

		if (now == 0)
		{
			m_inspect_offset.insert[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.insert[1].y = point.y - m_ROI[0][0].y ;

		}
		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].insert[0].x = m_ROI[i][0].x + m_inspect_offset.insert[0].x;
				m_inspect_ROI[i].insert[0].y = m_ROI[i][0].y + m_inspect_offset.insert[0].y;
				m_inspect_ROI[i].insert[1].x = m_ROI[i][0].x + m_inspect_offset.insert[1].x;
				m_inspect_ROI[i].insert[1].y = m_ROI[i][0].y + m_inspect_offset.insert[1].y;

			}
		}



		Invalidate(FALSE);
	}

	// �� �˻�� �Ǿ��ִ�.
	if (stat == 5 && status_move < 900)
	{
		// status 100 ������

		m_inspect_ROI[now].pok[1] = point;
		if (now == 0)
		{
			m_inspect_offset.pok[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.pok[1].y = point.y - m_ROI[0][0].y ;

		}

		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].pok[0].x = m_ROI[i][0].x + m_inspect_offset.pok[0].x;
				m_inspect_ROI[i].pok[0].y = m_ROI[i][0].y + m_inspect_offset.pok[0].y;
				m_inspect_ROI[i].pok[1].x = m_ROI[i][0].x + m_inspect_offset.pok[1].x;
				m_inspect_ROI[i].pok[1].y = m_ROI[i][0].y + m_inspect_offset.pok[1].y;

			}
		}

		Invalidate(FALSE);
	}

	// ������ �˻�� �Ǿ��ִ�.
	if (stat == 11 && status_move < 900)
	{
		// status 100 ������

		m_inspect_ROI[now].angle[0][1] = point;
		if (now == 0)
		{
			m_inspect_offset.angle[0][1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.angle[0][1].y = point.y - m_ROI[0][0].y ;

		}

		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].angle[0][0].x = m_ROI[i][0].x + m_inspect_offset.angle[0][0].x;
				m_inspect_ROI[i].angle[0][0].y = m_ROI[i][0].y + m_inspect_offset.angle[0][0].y;
				m_inspect_ROI[i].angle[0][1].x = m_ROI[i][0].x + m_inspect_offset.angle[0][1].x;
				m_inspect_ROI[i].angle[0][1].y = m_ROI[i][0].y + m_inspect_offset.angle[0][1].y;

			}
		}

		Invalidate(FALSE);
	}

	// ������ �˻�� �Ǿ��ִ�.
	if (stat == 12 && status_move < 900)
	{
		// status 100 ������

		m_inspect_ROI[now].angle[1][1] = point;
		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.angle[1][1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.angle[1][1].y = point.y - m_ROI[0][0].y ;

		}
		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].angle[1][0].x = m_ROI[i][0].x + m_inspect_offset.angle[1][0].x;
				m_inspect_ROI[i].angle[1][0].y = m_ROI[i][0].y + m_inspect_offset.angle[1][0].y;
				m_inspect_ROI[i].angle[1][1].x = m_ROI[i][0].x + m_inspect_offset.angle[1][1].x;
				m_inspect_ROI[i].angle[1][1].y = m_ROI[i][0].y + m_inspect_offset.angle[1][1].y;

			}
		}
		Invalidate(FALSE);
	}

	// �̹��� �˻�
	if (status_move > 19 && status_move < 30)
	{
		int now_imul = status_move - 20;

		m_inspect_ROI[now].imul[1] = point;
		
		// 1�� �����̸� Offset�� ����
		if (now == 0)
		{
			m_inspect_offset.imul[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul[1].y = point.y - m_ROI[0][0].y ;
		}
		
		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].imul[0].x = m_ROI[i][0].x + m_inspect_offset.imul[0].x;
				m_inspect_ROI[i].imul[0].y = m_ROI[i][0].y + m_inspect_offset.imul[0].y;
				m_inspect_ROI[i].imul[1].x = m_ROI[i][0].x + m_inspect_offset.imul[1].x;
				m_inspect_ROI[i].imul[1].y = m_ROI[i][0].y + m_inspect_offset.imul[1].y;
			}
		}
		Invalidate(FALSE);
	}


	// ��Ƽ�� �̵���
	if (stat == 30 && status_move < 900)
	{
		m_inspect_ROI[now].etiquette[1] = point;

		if (now == 0)
		{
			m_inspect_offset.etiquette[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.etiquette[1].y = point.y - m_ROI[0][0].y ;

		}

		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].etiquette[0].x = m_ROI[i][0].x + m_inspect_offset.etiquette[0].x;
				m_inspect_ROI[i].etiquette[0].y = m_ROI[i][0].y + m_inspect_offset.etiquette[0].y;
				m_inspect_ROI[i].etiquette[1].x = m_ROI[i][0].x + m_inspect_offset.etiquette[1].x;
				m_inspect_ROI[i].etiquette[1].y = m_ROI[i][0].y + m_inspect_offset.etiquette[1].y;
			}
		}

		Invalidate(FALSE);
	}

	// ���� ��Ƽ�� �̹� �̵���
	if (stat == 40 && status_move < 900)
	{
		m_inspect_ROI[now].imul_etq_left[1] = point;

		if (now == 0)
		{
			m_inspect_offset.imul_etq_left[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul_etq_left[1].y = point.y - m_ROI[0][0].y ;

		}

		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].imul_etq_left[0].x = m_ROI[i][0].x + m_inspect_offset.imul_etq_left[0].x;
				m_inspect_ROI[i].imul_etq_left[0].y = m_ROI[i][0].y + m_inspect_offset.imul_etq_left[0].y;
				m_inspect_ROI[i].imul_etq_left[1].x = m_ROI[i][0].x + m_inspect_offset.imul_etq_left[1].x;
				m_inspect_ROI[i].imul_etq_left[1].y = m_ROI[i][0].y + m_inspect_offset.imul_etq_left[1].y;
			}
		}

		Invalidate(FALSE);
	}

	// ���� ��Ƽ�� �̹� �̵���
	if (stat == 41 && status_move < 900)
	{
		m_inspect_ROI[now].imul_etq_right[1] = point;

		if (now == 0)
		{
			m_inspect_offset.imul_etq_right[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul_etq_right[1].y = point.y - m_ROI[0][0].y ;

		}

		// ���� �ٸ� ROI������ �ִٸ� �װ͵� �������ش�.
		for (int i = 1; i < _MAXROI; i++)
		{
			if (m_ROI[i][0].x != -1)
			{
				m_inspect_ROI[i].imul_etq_right[0].x = m_ROI[i][0].x + m_inspect_offset.imul_etq_right[0].x;
				m_inspect_ROI[i].imul_etq_right[0].y = m_ROI[i][0].y + m_inspect_offset.imul_etq_right[0].y;
				m_inspect_ROI[i].imul_etq_right[1].x = m_ROI[i][0].x + m_inspect_offset.imul_etq_right[1].x;
				m_inspect_ROI[i].imul_etq_right[1].y = m_ROI[i][0].y + m_inspect_offset.imul_etq_right[1].y;
			}
		}

		Invalidate(FALSE);
	}


	// ROI ������ ���� 1001~1003 (num��ȣ ����)
	if (status_move > 1000 && status_move < 1000 + _MAXROI)
	{

		// ���� ���� ����� ������ ����
		int nowx, nowy;
		nowx = point.x + abs(m_ROI[0][0].x - m_ROI[0][1].x); 
		nowy = point.y + abs(m_ROI[0][0].y - m_ROI[0][1].y); 
		if (nowx > _CamWidth-1)
		{
			point.x = _CamWidth-1-abs(m_ROI[0][0].x - m_ROI[0][1].x); 
		}
		
		if (nowy > _CamHeight-1)
		{
			point.y = _CamHeight-1-abs(m_ROI[0][0].y - m_ROI[0][1].y); 
		}

		if (point.x < 1)
		{
			point.x = 1;
		}
		if (point.y < 1)
		{
			point.y = 1;
		}

		m_ROI[status_move-1000][0] = point;
		m_ROI[status_move-1000][1].x = point.x + abs(m_ROI[0][0].x - m_ROI[0][1].x); 
		m_ROI[status_move-1000][1].y = point.y + abs(m_ROI[0][0].y - m_ROI[0][1].y);

		DrawCopyBoxes(status_move-1000);

		Invalidate(FALSE);
	}
	else if (status_move==1000)	// 1�� ROI �����ϱ�. (2�� ROI ������ �����)
	{
		// ���� ���� ����� ������ ����
		int nowx, nowy;
		nowx = point.x + abs(m_ROI[1][0].x - m_ROI[1][1].x); 
		nowy = point.y + abs(m_ROI[1][0].y - m_ROI[1][1].y); 
		if (nowx > _CamWidth-1)
			point.x = _CamWidth-1-abs(m_ROI[1][0].x - m_ROI[1][1].x); 
		if (nowy > _CamHeight-1)
			point.y = _CamHeight-1-abs(m_ROI[1][0].y - m_ROI[1][1].y); 

		if (point.x < 1)
			point.x = 1;
		if (point.y < 1)
			point.y = 1;

		m_ROI[status_move-1000][0] = point;
		m_ROI[status_move-1000][1].x = point.x + abs(m_ROI[1][0].x - m_ROI[1][1].x); 
		m_ROI[status_move-1000][1].y = point.y + abs(m_ROI[1][0].y - m_ROI[1][1].y);

		DrawCopyBoxes(status_move-1000);

		Invalidate(FALSE);
	}

	// ROI2 ������ ���� 2000~1003 (num��ȣ ����)
	if (status_move > 1999 && status_move <2004)
	{

		// ���� ���� ����� ������ ����
		int nowx, nowy;
		nowx = point.x + abs(m_ROI2[0].x - m_ROI2[1].x); 
		nowy = point.y + abs(m_ROI2[0].y - m_ROI2[1].y); 
		if (nowx > _CamWidth-1)
			point.x = _CamWidth-1-abs(m_ROI2[0].x - m_ROI2[1].x); 
		if (nowy > _CamHeight-1)
			point.y = _CamHeight-1-abs(m_ROI2[0].y - m_ROI2[1].y); 

		if (point.x < 1)
			point.x = 1;
		if (point.y < 1)
			point.y = 1;

		m_ROI[status_move-2000][0] = point;
		m_ROI[status_move-2000][1].x = point.x + abs(m_ROI2[0].x - m_ROI2[1].x); 
		m_ROI[status_move-2000][1].y = point.y + abs(m_ROI2[0].y - m_ROI2[1].y);

		DrawCopyBoxes(status_move-2000);

		Invalidate(FALSE);
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CInspectImage::DrawSquare(CDC *pDC, int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b, int nPenStyle)
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	// �簢�� �׸���
	double dRatioX = (double)_ShowWidth/_CamWidth;
	double dRatioY = (double)_ShowHeight/_CamHeight;
	x1 = (int)(x1 * dRatioX);
	y1 = (int)(y1 * dRatioY);
	x2 = (int)(x2 * dRatioX);
	y2 = (int)(y2 * dRatioY);

	CPen jcpPen, *ppnMemo;
	if(!jcpPen.CreatePen(nPenStyle, 1, RGB(r, g, b))) return;
	ppnMemo = pDC->SelectObject(&jcpPen);

	POINT pCoord[] = {{x1, y1}, {x2, y1}, {x2, y2}, {x1, y2}, {x1, y1}};
	pDC->Polyline(pCoord,5);

	pDC->SelectObject(ppnMemo);
}

void CInspectImage::DrawX(CDC *pDC, int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b, int nPenStyle, int gap)
{
	DrawLine(
		pDC,
		x1 + gap,
		y1 + gap,
		x2 - gap,
		y2 - gap,
		r, g, b,
		nPenStyle );

	DrawLine(
		pDC,
		x2 - gap,
		y1 + gap,
		x1 + gap,
		y2 - gap,
		r, g, b,
		nPenStyle );
}

void CInspectImage::DrawLine(CDC *pDC, int x1, int y1, int x2, int y2, BYTE R, BYTE G, BYTE B, int nPenStyle)
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	// �� �׸���
	double dRatioX = (double)_ShowWidth/_CamWidth;
	double dRatioY = (double)_ShowHeight/_CamHeight;
	x1 = (int)(x1 * dRatioX);
	y1 = (int)(y1 * dRatioY);
	x2 = (int)(x2 * dRatioX);
	y2 = (int)(y2 * dRatioY);

	CPen jcpPen, *ppnMemo;
	if(!jcpPen.CreatePen(nPenStyle, 1, RGB(R,G,B))) return;
	ppnMemo = pDC->SelectObject(&jcpPen);

	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);

	pDC->SelectObject(ppnMemo);
}

void CInspectImage::InitROIs()
{
	// ��� ROI �ʱ�ȭ
	int i;
	for (i = 0; i < _MAXROI; i++)
	{
		m_ROI[i][0].x = -1;
		m_ROI[i][1].x = -1;
		m_ROI[i][0].y = -1;
		m_ROI[i][1].y = -1;
	}

	for (int k = 0; k < _MAXROI; k++)
	{
		for (i = 0; i < 2; i++)
		{
			m_inspect_ROI[k].maguri[i].x = -1;
			m_inspect_ROI[k].maguri[i].y = -1;
			m_inspect_ROI[k].chuck[i].x = -1;
			m_inspect_ROI[k].chuck[i].y = -1;
			m_inspect_ROI[k].jabara[i].x = -1;
			m_inspect_ROI[k].jabara[i].y = -1;
			m_inspect_ROI[k].insert[i].x = -1;
			m_inspect_ROI[k].insert[i].y = -1;
			m_inspect_ROI[k].pok[i].x = -1;
			m_inspect_ROI[k].pok[i].y = -1;
			
			m_inspect_ROI[k].imul[i].x = -1;
			m_inspect_ROI[k].imul[i].y = -1;

			m_inspect_ROI[k].etiquette[i].x = -1;
			m_inspect_ROI[k].etiquette[i].y = -1;
		
			m_inspect_ROI[k].imul_etq_left[i].x = -1;
			m_inspect_ROI[k].imul_etq_left[i].y = -1;

			m_inspect_ROI[k].imul_etq_right[i].x = -1;
			m_inspect_ROI[k].imul_etq_right[i].y = -1;

			for (int j=0;j<2;j++)
			{
				m_inspect_ROI[k].angle[j][i].x = -1;
				m_inspect_ROI[k].angle[j][i].y = -1;
				
			}
		}
	}
}

bool CInspectImage::SetInspectionROI(int roi, int type)
{
	if(_MAXROI <= 0)
		return false;

	// ROI �������ֱ�
	if (m_ROI[roi][0].x == -1)
		return false;

	int status = 0;
	status = roi * 100;

	status += type;
	
	status_down = status;

	return true;
}

void CInspectImage::InitOX()
{
	m_inspect_ox.angle[0] = false;
	m_inspect_ox.angle[1] = false;
	m_inspect_ox.chuck = false;
	m_inspect_ox.insert = false;
	m_inspect_ox.jabara = false;
	m_inspect_ox.maguri = false;
	m_inspect_ox.pok = false;
	m_inspect_ox.etiquette = false;

	m_inspect_ox.imul = false;
	m_inspect_ox.imul_mid = false;
	m_inspect_ox.imul_jabara = FALSE;
	m_inspect_ox.imul_chuck = FALSE;
	m_inspect_ox.imul_insert = FALSE;
	m_inspect_ox.imul_etq_left = FALSE;
	m_inspect_ox.imul_etq_right = FALSE;

	m_inspect_ox.maguri_angle = FALSE;
	m_inspect_ox.mid_angle = FALSE;
	m_inspect_ox.top_angle = FALSE;
	m_inspect_ox.chuck_angle = FALSE;
	m_inspect_ox.insert_angle = FALSE;
	m_inspect_ox.maguri_side = FALSE;

	return;
}

void CInspectImage::CopyROI(int num)
{
	// ROI�� �����Ѵ�. ���� �˻� �������� �ٸ� ���뿡���� �����ϱ�
	// num�� ������ 1~3����. 0�� �����̹Ƿ� �н�

	if (num < 1 || num > _MAXROI - 1)
	{
		AfxMessageBox("ROI ������ �߸��Ǿ����ϴ�!");
		return;
	}

	// �˻��ϱ�� �� ��� �͵鿡 ���� �˻� ���� ������ 0������ �Ǿ��־�� ��.

	bool ok = true;

	if (m_inspect_ox.chuck && m_inspect_ROI[0].chuck[0].x == -1)
		ok = false;
	if (m_inspect_ox.insert && m_inspect_ROI[0].insert[0].x == -1)
		ok = false;
	if (m_inspect_ox.jabara && m_inspect_ROI[0].jabara[0].x == -1)
		ok = false;
	if (m_inspect_ox.maguri && m_inspect_ROI[0].maguri[0].x == -1)
		ok = false;
	if (m_inspect_ox.etiquette && m_inspect_ROI[0].etiquette[0].x == -1)
		ok = false;

	if (!ok)
	{
		AfxMessageBox("��� �˻� ������ �����ϼ���!");
		return;
	}

	AfxMessageBox("ȭ�鿡 Ŭ���Ͽ� ���ϴ� �������� Drag&Drop�Ͽ� �ű⼼��!\n(��ۯ�������������)");
	status_down = 1000 + num;
	// 1001~3 -> �����
	m_now_copying_ROI = num;

	// ���� ����
	if (m_ROIS < num+1)
		m_ROIS = num+1;

	return;

}

void CInspectImage::DrawCopyBoxes(int num)
{
	// ROI ������ ��, �˻� ���� �׷��ֱ�

	// ��
	if(m_inspect_ox.pok)
	{
		m_inspect_ROI[num].pok[0].x = m_ROI[num][0].x+m_inspect_offset.pok[0].x;
		m_inspect_ROI[num].pok[1].x = m_ROI[num][0].x+m_inspect_offset.pok[1].x;
		m_inspect_ROI[num].pok[0].y = m_ROI[num][0].y+m_inspect_offset.pok[0].y;
		m_inspect_ROI[num].pok[1].y = m_ROI[num][0].y+m_inspect_offset.pok[1].y;
	}

	// ������
	if (m_inspect_ox.maguri)
	{
		m_inspect_ROI[num].maguri[0].x = m_ROI[num][0].x+m_inspect_offset.maguri[0].x;
		m_inspect_ROI[num].maguri[1].x = m_ROI[num][0].x+m_inspect_offset.maguri[1].x;
		m_inspect_ROI[num].maguri[0].y = m_ROI[num][0].y+m_inspect_offset.maguri[0].y;
		m_inspect_ROI[num].maguri[1].y = m_ROI[num][0].y+m_inspect_offset.maguri[1].y;

	}

	// �ڹٶ�
	if (m_inspect_ox.jabara)
	{
		m_inspect_ROI[num].jabara[0].x = m_ROI[num][0].x+m_inspect_offset.jabara[0].x;
		m_inspect_ROI[num].jabara[1].x = m_ROI[num][0].x+m_inspect_offset.jabara[1].x;
		m_inspect_ROI[num].jabara[0].y = m_ROI[num][0].y+m_inspect_offset.jabara[0].y;
		m_inspect_ROI[num].jabara[1].y = m_ROI[num][0].y+m_inspect_offset.jabara[1].y;

	}

	// ���
	if (m_inspect_ox.chuck)
	{
		m_inspect_ROI[num].chuck[0].x = m_ROI[num][0].x+m_inspect_offset.chuck[0].x;
		m_inspect_ROI[num].chuck[1].x = m_ROI[num][0].x+m_inspect_offset.chuck[1].x;
		m_inspect_ROI[num].chuck[0].y = m_ROI[num][0].y+m_inspect_offset.chuck[0].y;
		m_inspect_ROI[num].chuck[1].y = m_ROI[num][0].y+m_inspect_offset.chuck[1].y;


	}
	
	// ����
	if (m_inspect_ox.insert)
	{
		m_inspect_ROI[num].insert[0].x = m_ROI[num][0].x+m_inspect_offset.insert[0].x;
		m_inspect_ROI[num].insert[1].x = m_ROI[num][0].x+m_inspect_offset.insert[1].x;
		m_inspect_ROI[num].insert[0].y = m_ROI[num][0].y+m_inspect_offset.insert[0].y;
		m_inspect_ROI[num].insert[1].y = m_ROI[num][0].y+m_inspect_offset.insert[1].y;


	}

	// �°�
	if (m_inspect_ox.angle[0])
	{
		m_inspect_ROI[num].angle[0][0].x = m_ROI[num][0].x+m_inspect_offset.angle[0][0].x;
		m_inspect_ROI[num].angle[0][1].x = m_ROI[num][0].x+m_inspect_offset.angle[0][1].x;
		m_inspect_ROI[num].angle[0][0].y = m_ROI[num][0].y+m_inspect_offset.angle[0][0].y;
		m_inspect_ROI[num].angle[0][1].y = m_ROI[num][0].y+m_inspect_offset.angle[0][1].y;


	}

	// �찢
	if (m_inspect_ox.angle[1])
	{
		m_inspect_ROI[num].angle[1][0].x = m_ROI[num][0].x+m_inspect_offset.angle[1][0].x;
		m_inspect_ROI[num].angle[1][1].x = m_ROI[num][0].x+m_inspect_offset.angle[1][1].x;
		m_inspect_ROI[num].angle[1][0].y = m_ROI[num][0].y+m_inspect_offset.angle[1][0].y;
		m_inspect_ROI[num].angle[1][1].y = m_ROI[num][0].y+m_inspect_offset.angle[1][1].y;

	}

	// �̹�
	if (m_inspect_ox.imul)
	{
		m_inspect_ROI[num].imul[0].x = m_ROI[num][0].x+m_inspect_offset.imul[0].x;
		m_inspect_ROI[num].imul[1].x = m_ROI[num][0].x+m_inspect_offset.imul[1].x;
		m_inspect_ROI[num].imul[0].y = m_ROI[num][0].y+m_inspect_offset.imul[0].y;
		m_inspect_ROI[num].imul[1].y = m_ROI[num][0].y+m_inspect_offset.imul[1].y;
	}

	// ��Ƽ��
	if(m_inspect_ox.etiquette)
	{
		m_inspect_ROI[num].etiquette[0].x = m_ROI[num][0].x+m_inspect_offset.etiquette[0].x;
		m_inspect_ROI[num].etiquette[1].x = m_ROI[num][0].x+m_inspect_offset.etiquette[1].x;
		m_inspect_ROI[num].etiquette[0].y = m_ROI[num][0].y+m_inspect_offset.etiquette[0].y;
		m_inspect_ROI[num].etiquette[1].y = m_ROI[num][0].y+m_inspect_offset.etiquette[1].y;
	}

	// ���� ��Ƽ�� �̹�
	if(m_inspect_ox.imul_etq_left)
	{
		m_inspect_ROI[num].imul_etq_left[0].x = m_ROI[num][0].x+m_inspect_offset.imul_etq_left[0].x;
		m_inspect_ROI[num].imul_etq_left[1].x = m_ROI[num][0].x+m_inspect_offset.imul_etq_left[1].x;
		m_inspect_ROI[num].imul_etq_left[0].y = m_ROI[num][0].y+m_inspect_offset.imul_etq_left[0].y;
		m_inspect_ROI[num].imul_etq_left[1].y = m_ROI[num][0].y+m_inspect_offset.imul_etq_left[1].y;
	}

	// ���� ��Ƽ�� �̹�
	if(m_inspect_ox.imul_etq_right)
	{
		m_inspect_ROI[num].imul_etq_right[0].x = m_ROI[num][0].x+m_inspect_offset.imul_etq_right[0].x;
		m_inspect_ROI[num].imul_etq_right[1].x = m_ROI[num][0].x+m_inspect_offset.imul_etq_right[1].x;
		m_inspect_ROI[num].imul_etq_right[0].y = m_ROI[num][0].y+m_inspect_offset.imul_etq_right[0].y;
		m_inspect_ROI[num].imul_etq_right[1].y = m_ROI[num][0].y+m_inspect_offset.imul_etq_right[1].y;
	}
}

bool CInspectImage::LoadImage(LPCSTR filename)
{
	// ���� �ҷ�����
	// 1024x768�� �÷��̿��� �ȴ�.
	if( _CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024 )
		return false;

	IplImage *temp;
	temp = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
	
	bool ok = true;
	// ũ��, ���� �� �׽�Ʈ
	if (temp->width != _CamWidth)
		ok = false;
	if (temp->height != _CamHeight)
		ok = false;
	if (temp->nChannels != 1)
		ok = false;

	// �� �������� �̹��� ����
	if ( ok )
	{
		cvCopy( temp, m_nowView );

		cvResize( m_nowView, m_resizeView );

		cvCvtColor( m_resizeView, m_rgbResizeView, CV_GRAY2RGB );

		cvFlip( m_rgbResizeView, m_rgbResizeView );

		DrawImage( m_rgbResizeView, m_rgbResizeView->width, m_rgbResizeView->height );
	}

	// ������
	cvReleaseImage(&temp);

	return ok;
}

bool CInspectImage::SetROI(int num)
{
	if (!m_Loaded)
	{
		AfxMessageBox("�ϴ� �̹������� �ҷ����ÿ�!\n(��������)");
		return false;
	}

	// ROI ���� ����
	status_down = 900+num;
	return true;
}

void CInspectImage::SetROIS(int num)
{
	// ROI ���� ����
	
	m_ROIS = num;

	return;

}

int CInspectImage::DoInspection()
{
	// �˻� ����
	int i, ReturnResult=0;
	int arrIdx = 0;
	


	m_process->ClearInspResult();


	for (i = 0; i < m_ROIS; i++)
	{
		// ROi ���� ���� Ȯ��
		if (m_ROI[i][0].x == -1)
		{
			AfxMessageBox("���� ROI���� �� �����ϼ���!");
			return -1;
		}

		// �˻� ���� ���� ȣ����
		// �˻��ϱ�� �� ��� �͵鿡 ���� �˻� ���� ������ 0������ �Ǿ��־�� ��.

		bool ok = true;

		// * �̹�
		//   + �ֺ������� ������
		// * ����
		//   + �ֺ������� ������
		// * ��
		//   + ����
		// * ��, �� ����
		//   + ����
		if (m_inspect_ox.chuck && m_inspect_ROI[i].chuck[0].x == -1)
			ok = false;
		if (m_inspect_ox.insert && m_inspect_ROI[i].insert[0].x == -1)
			ok = false;
		if (m_inspect_ox.jabara && m_inspect_ROI[i].jabara[0].x == -1)
			ok = false;
		if (m_inspect_ox.maguri && m_inspect_ROI[i].maguri[0].x == -1)
			ok = false;
		if (m_inspect_ox.etiquette && m_inspect_ROI[i].etiquette[0].x == -1)
			ok = false;

		if (!ok)
		{
			AfxMessageBox("�˻��ϱ�� �� �˻� ������ ��� �����Ͻÿ�!");
			return -1;
		}
	}

	// �˻� �ǽ� �� ������ �ѱ��
	m_process->m_CannyLow = m_inspect_setting.m_CannyLow;
	m_process->m_CannyHigh = m_inspect_setting.m_CannyHigh;
	m_process->m_CannyApp = m_inspect_setting.m_CannyApp;
	m_process->m_Magunus_Slide = m_inspect_setting.m_Magunus_Slide;
	m_process->m_Magunus_Slide2 = m_inspect_setting.m_Magunus_Slide2;
	m_process->m_Magunus_SlideLength = m_inspect_setting.m_Magunus_SlideLength;
	m_process->m_minChuck = cvRound(m_inspect_setting.m_minChuck / m_inspect_setting.resol);
	m_process->m_maxChuck = cvRound(m_inspect_setting.m_maxChuck / m_inspect_setting.resol);
	m_process->m_Slide = m_inspect_setting.m_Slide;
	m_process->m_SlideLength = m_inspect_setting.m_SlideLength;

	// ���� �� �ʱ�ȭ.
	memset(m_process->m_nResultData,0,sizeof(int) *20);
	memset(m_process->m_dResultData,0,sizeof(double) *20);


	// �̹��� ���� ������
	// �̹��� ����
	//bool result = m_process->SetColorImage(m_nowView);
	bool result = m_process->SetMonoImage(m_nowView);
	//bool bfindstraw=false;

	if (!result)
	{
		AfxMessageBox("�̹��� ���� ����!");
		return -1;
	}

	// ��� �˻翡�� ���� ����....
	m_process->m_jabara.slide_up = 0.01;
	m_process->m_jabara.up_slide = 0.01;
	m_process->m_jabara.slide_down = 0.01;
	m_process->m_jabara.down_slide = 0.01;
 
	// �˻� ���� m_ROIS
	for (i=0;i<m_ROIS;i++)
	{
		CvRect area;
		CvRect maguri_rect = cvRect(m_inspect_ROI[i].maguri[0].x,
						  m_inspect_ROI[i].maguri[0].y,
						  abs(m_inspect_ROI[i].maguri[0].x - m_inspect_ROI[i].maguri[1].x),
						  abs(m_inspect_ROI[i].maguri[0].y - m_inspect_ROI[i].maguri[1].y));

		CvRect Ettiquet_Right_rect = cvRect(m_inspect_ROI[i].imul_etq_right[0].x,
						  m_inspect_ROI[i].imul_etq_right[0].y,
						  abs(m_inspect_ROI[i].imul_etq_right[0].x - m_inspect_ROI[i].imul_etq_right[1].x),
						  abs(m_inspect_ROI[i].imul_etq_right[0].y - m_inspect_ROI[i].imul_etq_right[1].y));

		CvRect Ettiquet_Left_rect = cvRect(m_inspect_ROI[i].imul_etq_left[0].x,
						  m_inspect_ROI[i].imul_etq_left[0].y,
						  abs(m_inspect_ROI[i].imul_etq_left[0].x - m_inspect_ROI[i].imul_etq_left[1].x),
						  abs(m_inspect_ROI[i].imul_etq_left[0].y - m_inspect_ROI[i].imul_etq_left[1].y));

		bool ox;
		if(m_process->m_trans) // ����
		{
			CvRect partialArea = cvRect(m_inspect_ROI[i].insert[0].x,
						  m_inspect_ROI[i].insert[0].y,
						  abs(m_inspect_ROI[i].insert[0].x - m_inspect_ROI[i].insert[1].x),
						  abs(m_inspect_ROI[i].insert[0].y - m_inspect_ROI[i].insert[1].y));

			CvRect totalArea = cvRect(m_ROI[i][0].x,
					  m_ROI[i][0].y,
					  abs(m_ROI[i][0].x - m_ROI[i][1].x),
					  abs(m_ROI[i][0].y - m_ROI[i][1].y));

			int isExist = m_process->StrawExistanceCheckForTrans(partialArea, totalArea, true);
			if(isExist == TRUE)
				ox = true;
			else if(isExist == FALSE)
				ox = false;
			else if(isExist == -1)
			{
				m_result_inspect[i].insert = false;
				continue;
			}

			//ox = m_process->StrawExistanceCheckForTrans(partialArea, totalArea, true);	
		}
		else // �Ϲ�
		{
			area = cvRect(m_ROI[i][0].x,
					  m_ROI[i][0].y,
					  abs(m_ROI[i][0].x - m_ROI[i][1].x),
					  abs(m_ROI[i][0].y - m_ROI[i][1].y));
			 ox = m_process->StrawExistanceCheck(area, 1);
		}
		
		// ���밡 ���ų� ������ �˻��� ��� continue
		if (!ox)
		{
			m_bExistObj[i]= false;
			continue;
		}
		else 
		{
			m_bExistObj[i] = true;
		}

		// ������ �˻�
		if(m_inspect_ox.maguri)
		{
			m_process->SetROI(m_inspect_ROI[i].maguri[0].x,
						  m_inspect_ROI[i].maguri[0].y,
						  abs(m_inspect_ROI[i].maguri[0].x - m_inspect_ROI[i].maguri[1].x),
						  abs(m_inspect_ROI[i].maguri[0].y - m_inspect_ROI[i].maguri[1].y));

			result = m_process->InspectMaguri(true);
			if(result == false) ReturnResult++;
			m_result_inspect[i].maguri = result;

			if(m_inspect_ox.maguri_side)
			{
				result = m_process->InspectMaguriSide(true);
				if(result == false) 
					ReturnResult++;

				if(m_result_inspect[i].maguri) 
					m_result_inspect[i].maguri = result;
				else 
					m_result_inspect[i].maguri = false;
			}
			
			if(m_inspect_ox.maguri_angle)
			{
				result = m_process->InspectMaguriAngle(true);
				if(result == false)
					ReturnResult++;

				if(m_result_inspect[i].maguri)
					m_result_inspect[i].maguri = result;
				else
					m_result_inspect[i].maguri = false;
			}
		}

		// ��� ���� �˻�
		if(m_inspect_ox.top_angle && m_inspect_ox.maguri)
		{
			if(m_inspect_ox.jabara)
			{
				m_process->SetROI(
					m_inspect_ROI[i].maguri[1].x,
					m_inspect_ROI[i].maguri[0].y,
					abs(m_inspect_ROI[i].maguri[1].x - m_inspect_ROI[i].jabara[0].x),
					abs(m_inspect_ROI[i].maguri[0].y - m_inspect_ROI[i].maguri[1].y));

				result = m_process->InspectTopAngle(true);
				if(result == false)
				{
					ReturnResult++;
					m_result_inspect[i].angle[0] = -1000;
				}
				else
					m_result_inspect[i].angle[0] = true;

			}
			else if(m_inspect_ox.chuck)
			{
				m_process->SetROI(
					m_inspect_ROI[i].maguri[1].x,
					m_inspect_ROI[i].maguri[0].y,
					abs(m_inspect_ROI[i].maguri[1].x - m_inspect_ROI[i].chuck[0].x) / 2,
					abs(m_inspect_ROI[i].maguri[0].y - m_inspect_ROI[i].maguri[1].y));
				result = m_process->InspectTopAngle(true);
				if(result == false)
				{
					ReturnResult++;
					m_result_inspect[i].angle[0] = -1000;
				}
				else
					m_result_inspect[i].angle[0] = true;
			}
		}

		// �ߴ� ���� �˻�
		if(m_inspect_ox.mid_angle && m_inspect_ox.chuck)
		{
			if(m_inspect_ox.jabara)
			{
				m_process->SetROI(
					m_inspect_ROI[i].jabara[1].x,
					m_inspect_ROI[i].jabara[0].y,
					abs(m_inspect_ROI[i].jabara[1].x - m_inspect_ROI[i].chuck[0].x) - g_midNotInspArea,
					abs(m_inspect_ROI[i].jabara[0].y - m_inspect_ROI[i].jabara[1].y));

 				result = m_process->InspectMidAngle(true);
				if(result == false)
				{
					ReturnResult++;
					//m_result_inspect[i].angle[1] = -1000;
					m_result_inspect[i].pok = -1.0;
				}
				else
					//m_result_inspect[i].angle[1] = true;
					m_result_inspect[i].pok = true;
			}
			else if(m_inspect_ox.maguri)
			{
				m_process->SetROI(
					(m_inspect_ROI[i].maguri[1].x + m_inspect_ROI[i].chuck[0].x) / 2,
					m_inspect_ROI[i].maguri[0].y,
					abs(m_inspect_ROI[i].maguri[1].x - m_inspect_ROI[i].chuck[0].x) / 2 - g_midNotInspArea,
					abs(m_inspect_ROI[i].maguri[0].y - m_inspect_ROI[i].maguri[1].y));
				
				result = m_process->InspectMidAngle(true);
				if(result == false)
				{
					ReturnResult++;
					//m_result_inspect[i].angle[1] = -1000;
					m_result_inspect[i].pok = -1.0;
				}
				else
					//m_result_inspect[i].angle[1] = true;
					m_result_inspect[i].pok = true;
			}
		}

		// ��� �˻�
		if (m_inspect_ox.chuck)
		{
			area = cvRect(m_inspect_ROI[i].chuck[0].x,
						  m_inspect_ROI[i].chuck[0].y,
						  abs(m_inspect_ROI[i].chuck[0].x - m_inspect_ROI[i].chuck[1].x),
						  abs(m_inspect_ROI[i].chuck[0].y - m_inspect_ROI[i].chuck[1].y));

			result = m_process->CheckChuck(
				area,
				m_inspect_setting.resol,
				true );

			if(result==false) ReturnResult++; // Error �� ���. 
			m_result_inspect[i].chuck = result;

			// ��� ���� �˻�
			if(m_inspect_ox.chuck_angle)
			{
				m_process->SetROI(
					area.x + g_chuckLeft, 
					area.y, 
					area.width - g_chuckLeft, 
					area.height);

				result = m_process->InspectChuckAngle(true);
				if(result == false) ReturnResult++;
				if(m_result_inspect[i].chuck != false)
					m_result_inspect[i].chuck = result;
			}
		}

		// ���� �˻�
		if (m_inspect_ox.insert)
		{
			area = cvRect(m_inspect_ROI[i].insert[0].x,
						  m_inspect_ROI[i].insert[0].y,
						  abs(m_inspect_ROI[i].insert[0].x - m_inspect_ROI[i].insert[1].x),
						  abs(m_inspect_ROI[i].insert[0].y - m_inspect_ROI[i].insert[1].y));

			result = m_process->CheckInsert(area,1, true, m_inspect_setting.m_Slide, m_inspect_setting.m_SlideLength ,m_inspect_setting.stabbed_insert_notinspect, m_inspect_setting.stabbed_insert_pix);
			if(result==false) ReturnResult++; // Error �� ���. 
			m_result_inspect[i].insert = result;

			
			// ���� ����
			/*
			if(m_inspect_ox.insert_angle)
			{
				m_process->SetROI(area.x, area.y, area.width, area.height);
				result = m_process->InspectInsertAngle(true);
				if(result == false) ReturnResult++;
				if(m_result_inspect[i].insert != false)
					m_result_inspect[i].insert = result;
			}
			*/
			
		}

		int abc = 0;

		if(result == true)
		{
			// ���� ����
			if(m_inspect_ox.insert_angle)
			{
				area = cvRect(m_inspect_ROI[i].insert[0].x,
					m_inspect_ROI[i].insert[0].y,
					abs(m_inspect_ROI[i].insert[0].x - m_inspect_ROI[i].insert[1].x),
					abs(m_inspect_ROI[i].insert[0].y - m_inspect_ROI[i].insert[1].y));

				result = m_process->CheckInsertStabbed(area,1, true, m_inspect_setting.m_Slide, m_inspect_setting.m_SlideLength ,m_inspect_setting.stabbed_insert_notinspect, m_inspect_setting.stabbed_insert_pix);
				if(result==false) ReturnResult++; // Error �� ���. 
				m_result_inspect[i].insert = result;

			}
		}

		
		
		// �ڹٶ� �˻�
		if ( m_inspect_ox.jabara )
		{
			area = cvRect(m_inspect_ROI[i].jabara[0].x,
						  m_inspect_ROI[i].jabara[0].y,
						  abs(m_inspect_ROI[i].jabara[0].x - m_inspect_ROI[i].jabara[1].x),
						  abs(m_inspect_ROI[i].jabara[0].y - m_inspect_ROI[i].jabara[1].y));


			result = m_process->InspectJabara(
				area,									  // ROI
				m_inspect_setting.jabara_num,			  // �ڹٶ� ����
				m_inspect_setting.jabara_margin_of_error, // �ڹٶ� ���� ��� ����
				10,										  // ������ ������Ʈ ũ��
				m_inspect_setting.jabara_recognize,		  // ������׷����� threshold�� ���� ���� n% �ȼ�
				m_inspect_setting.jabara_distance_stdev,  // �ڹٶ� ���� �Ÿ� ǥ������
				m_inspect_setting.jabara_width_stdev,	  // �ڹٶ� �β� ǥ������
				true									  // ��� ����
				);

			if( result == true && m_inspect_ox.jabara_edge )
			{
				result = m_process->InspectJabaraEdge(
					area,									  // ROI
					m_inspect_setting.jabara_num,			  // �ڹٶ� ����
					m_inspect_setting.jabara_margin_of_error, // �ڹٶ� ���� ��� ����
					m_inspect_setting.jabara_gradient_stdev,  // ���� ǥ������
					m_inspect_setting.jabara_neighbor,	      // �� ���� min, max ����
					true								      // ��� ����
					);
			}

			if(result==false) ReturnResult++; // Error �� ���.
			m_result_inspect[i].jabara = result;
		}

		// �̹� �˻� ---------------------------------
		
		// �̹� �˻� ������ �ϳ��� ������ ��� ���� ��ü�� ���� Edge ���ϱ�
		if ( m_inspect_ox.imul || m_inspect_ox.imul_chuck || m_inspect_ox.imul_mid ||
			m_inspect_ox.imul_insert || m_inspect_ox.imul_jabara )
		{
			// ���� �ϳ��� ���� ROI
			area = cvRect( m_ROI[i][0].x,
						   m_ROI[i][0].y,
						   abs( m_ROI[i][0].x - m_ROI[i][1].x ),
						   abs( m_ROI[i][0].y - m_ROI[i][1].y ) );

			m_process->OtsuThreshold(
				area, 
				m_process->m_OriginalImage, 
				true );
			
			//m_process->GetStrawEdge( area );
		}

		// �̹� ��� �˻�
		if ( m_inspect_ox.imul_chuck && m_inspect_ox.chuck )
		{
			area = cvRect(m_inspect_ROI[i].chuck[0].x,
						  m_inspect_ROI[i].chuck[0].y,
						  abs(m_inspect_ROI[i].chuck[0].x - m_inspect_ROI[i].chuck[1].x),
						  abs(m_inspect_ROI[i].chuck[0].y - m_inspect_ROI[i].chuck[1].y));

			result = m_process->InspectBlackPoint(
				i,
				area,									// ROI ( ������� )
				m_inspect_setting.imul_chuck_mask_cols,										// ��� ����ũ ũ��(Cols)
				m_inspect_setting.imul_chuck_mask,		// ��� ����ũ ũ��(Rows)
				m_inspect_setting.imul_chuck_threshold, // ��� threshold
				m_inspect_setting.chuck_pixsize,		// ��� �̹� ũ��
				true,									// ��� ����
				_T("���"), STRAW_IMUL_CHUCK,
				area,
				m_inspect_setting.imul_chuck_mask_cols,
				1,
				1
				);

			if ( result == false ) ReturnResult++;
			m_result_inspect[i].imul_chuck = result;
		}

		// �̹� ���� �˻�
		if ( m_inspect_ox.imul_insert && m_inspect_ox.insert )
		{
			area = cvRect(m_inspect_ROI[i].insert[0].x,
						  m_inspect_ROI[i].insert[0].y,
						  (int)(abs(m_inspect_ROI[i].insert[0].x - m_inspect_ROI[i].insert[1].x) * ( m_inspect_setting.imul_insert_pos / 100.0 )),
						  (int)abs(m_inspect_ROI[i].insert[0].y - m_inspect_ROI[i].insert[1].y));

			result = m_process->InspectBlackPoint(
				i,
				area,									 // ROI ( ���� ���� )
				1,										 // ���� ����ũ ũ��(Cols)
				m_inspect_setting.imul_insert_mask,		 // ���� ����ũ ũ��(Rows)
				m_inspect_setting.imul_insert_threshold, // ���� threshold
				m_inspect_setting.insert_pixsize,		 // ��� �̹� ũ��
				true,									 // ��� ����
				_T("����"), STRAW_IMUL_INSERT,
				area,
				m_inspect_setting.imul_insert_threshold,
				1,
				1
				);

			if ( result == false ) ReturnResult++;
			m_result_inspect[i].imul_insert = result;
		}

		CvRect topArea = cvRect( 0, 0, 0, 0 );
		CvRect midArea = cvRect( 0, 0, 0, 0 );

		// �̹� �ڹٶ� �˻�
		if ( m_inspect_ox.jabara )
		{
			if ( m_inspect_ox.imul_jabara )
			{
				area = cvRect( m_inspect_ROI[i].jabara[0].x,
						       m_inspect_ROI[i].jabara[0].y,
						       abs( m_inspect_ROI[i].jabara[0].x - m_inspect_ROI[i].jabara[1].x ),
						       abs( m_inspect_ROI[i].jabara[0].y - m_inspect_ROI[i].jabara[1].y ) );

				result = m_process->InspectBlackPoint(
					i,
					area,							         // ROI ( �ڹٶ� ���� )
					1,										 // �ڹٶ� ����ũ ũ��(Cols)
					m_inspect_setting.imul_jabara_mask,		 // �ڹٶ� ����ũ ũ��(Rows)
					m_inspect_setting.imul_jabara_threshold, // �ڹٶ� threshold
					m_inspect_setting.jabara_pixsize,		 // ��� �̹� ũ��
					true,									 // ��� ����
					_T("�ڹٶ�"), STRAW_IMUL_JABARA,
					area,
					m_inspect_setting.imul_jabara_threshold,
					1,
					1
					);

				if ( result == false ) ReturnResult++;
				m_result_inspect[i].imul_jabara = result;
			}

			// �̹� ��� ROI ( ���� �� ~ �ڹٶ� ���� )
			if ( m_inspect_ox.imul )
			{
				topArea = cvRect( m_ROI[i][0].x,
								  m_ROI[i][0].y,
								  abs( m_ROI[i][0].x - m_inspect_ROI[i].jabara[0].x ),
								  abs( m_ROI[i][0].y - m_inspect_ROI[i].jabara[1].y ) );
				
			}

			// �̹� �ߴ� ROI ( �ڹٶ� ������ ~ ��� ���� )
			if ( m_inspect_ox.imul_mid )
			{
				if ( m_inspect_ox.chuck )
				{
					midArea = cvRect( m_inspect_ROI[i].jabara[1].x,
									  m_inspect_ROI[i].jabara[0].y,
									  abs( m_inspect_ROI[i].jabara[1].x - m_inspect_ROI[i].chuck[0].x ),
									  abs( m_inspect_ROI[i].jabara[0].y - m_inspect_ROI[i].chuck[1].y ) );
				}
			}
		}
		// �ڹٶ� �˻� �� �� ���
		else
		{
			if ( m_inspect_ox.chuck )
			{
				// (���� �� ~ ��� ����) / 2
				int halfWidth = abs( m_ROI[i][0].x - m_inspect_ROI[i].chuck[0].x ) / 2;

				// ���� �� ~ �߰�
				if ( m_inspect_ox.imul )
				{
					topArea = cvRect( m_ROI[i][0].x,
									  m_ROI[i][0].y,
									  halfWidth,
									  abs( m_ROI[i][0].y - m_ROI[i][1].y ) );
				}

				// �߰� ~ ��� ����
				if ( m_inspect_ox.imul_mid )
				{
					midArea = cvRect( m_ROI[i][0].x + halfWidth,
									  m_ROI[i][0].y,
									  halfWidth,
									  abs( m_ROI[i][0].y - m_ROI[i][1].y ) );
				}
			}
		}

		// �̹� ��� �˻�
		if ( m_inspect_ox.imul )
		{
			result = m_process->InspectBlackPoint(
				i,
				topArea,							  // ROI
				m_inspect_setting.imul_top_mask_cols, // ��� ����ũ ũ��(Cols)
				m_inspect_setting.imul_top_mask,	  // ��� ����ũ ũ��(Rows)
				m_inspect_setting.imul_top_threshold, // ��� threshold
				m_inspect_setting.pixsize,			  // ��� �̹� ũ��
				true,								  // ��� ����
				_T("���"), STRAW_IMUL_TOP,
				maguri_rect,
				m_inspect_setting.imul_top_persent,
				m_inspect_setting.imul_top_size,
				m_inspect_setting.imul_mid_notinspect
				);

			if ( result == false ) ReturnResult++;
			m_result_inspect[i].imul = result;
		}
		
		// �̹� �ߴ� �˻�
		if ( m_inspect_ox.imul_mid )
		{
			result = m_process->InspectBlackPoint(
				i,
				midArea,						      // ROI
				1,									  // �ߴ� ����ũ ũ��(Cols)
				m_inspect_setting.imul_mid_mask,	  // �ߴ� ����ũ ũ��(Rows)
				m_inspect_setting.imul_mid_threshold, // �ߴ� threshold
				m_inspect_setting.mid_pixsize,		  // ��� �̹� ũ��
				true,								  // ��� ����
				_T("�ߴ�"), STRAW_IMUL_MID,
				Ettiquet_Right_rect,
				m_inspect_setting.imul_mid_persent,
				m_inspect_setting.imul_mid_size,
				m_inspect_setting.imul_mid_notinspect
				);

			if ( result == false ) ReturnResult++;
			m_result_inspect[i].imul_mid = result;
		}

//		if( m_process->m_StrawEdge != NULL)
//		{
//			delete [] m_process->m_StrawEdge;
//			m_process->m_StrawEdge = NULL;
//		}

		// ----------------------------------------------


		// ��Ƽ�� �˻�
		if ( m_inspect_ox.etiquette )
		{
			area = cvRect(
				m_inspect_ROI[i].etiquette[0].x,
				m_inspect_ROI[i].etiquette[0].y,
				abs(m_inspect_ROI[i].etiquette[0].x - m_inspect_ROI[i].etiquette[1].x),
				abs(m_inspect_ROI[i].etiquette[0].y - m_inspect_ROI[i].etiquette[1].y)
				);

			result = m_process->InspectEtiquette(
				area,
				m_inspect_setting.etiquette_size,
				m_inspect_setting.etiquette_recognize,
				m_inspect_setting.etiquette_position,
				true
				);

			if(result == false) ReturnResult++;
			m_result_inspect[i].etiquette = result;
		}
		
		// ��ġ�� ���� �ٸ� �˻� ���
		memcpy(
			&m_result_Value[arrIdx++],
			&( m_process->m_ResultValue ),
			sizeof(m_process->m_ResultValue) );
	}

	m_drawinsepct = true;
	Invalidate(FALSE);

	m_process->ErrorFileClose();

	return ReturnResult; //Error ���� Return.
}

void CInspectImage::DrawCircle(CDC *pDC, int x, int y, int radius, BYTE R, BYTE G, BYTE B, int nPenStyle)
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	// �� �׸���
	double dRatioX = (double)_ShowWidth/_CamWidth;
	double dRatioY = (double)_ShowHeight/_CamHeight;
	x = (int)(x * dRatioX);
	y = (int)(y * dRatioY);

	CPen jcpPen, *ppnMemo;
	if(!jcpPen.CreatePen(nPenStyle, 1, RGB(R,G,B))) return;
	ppnMemo = pDC->SelectObject(&jcpPen);

	pDC->MoveTo(x-radius,y-radius);
	pDC->AngleArc(x-radius,y-radius,radius,(float)0.0,(float)359.9);

	pDC->SelectObject(ppnMemo);
}

//////////////////////////////////////////////////////////////////////
// CShowImage Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CShowingImage

CShowingImage::CShowingImage()
{
}

CShowingImage::~CShowingImage()
{
}


BEGIN_MESSAGE_MAP(CShowingImage, CStatic)
	//{{AFX_MSG_MAP(CShowingImage)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CInspectImage::DrawImage(IplImage *pImage)
{
	CDC *pDC = GetDC();
	CDC dcMem;
	CRect rectViewer;

	

	GetClientRect(rectViewer);
//	m_csImage.Lock();
	if ( m_pbitmap ) delete m_pbitmap;
 	m_pbitmap = new CBitmap;		
	m_pbitmap->CreateCompatibleBitmap(pDC,rectViewer.Width(), rectViewer.Height());

	m_iWinWidth = rectViewer.Width();
	m_iWinHeight = rectViewer.Height();
// 	m_iWinHeight = 768;
// 	m_iWinWidth = 1024;

	dcMem.CreateCompatibleDC(pDC);	
	dcMem.SelectObject(m_pbitmap);


//	pImage->Draw(&dcMem, 1, 1, rectViewer.Width() - 1, rectViewer.Height() - 1);

	// �̹����� ��Ʈ�� �׳� �Ѹ���

	// ��Ʈ�� ���� ����
	BITMAPINFO *BmInfo = new BITMAPINFO;
	BmInfo->bmiHeader.biBitCount = 24;
	BmInfo->bmiHeader.biCompression= 0;
	BmInfo->bmiHeader.biHeight = _ShowHeight;
	BmInfo->bmiHeader.biPlanes = 1;
	BmInfo->bmiHeader.biSizeImage = _ShowWidth*_ShowHeight;
	BmInfo->bmiHeader.biSize = 40;
	BmInfo->bmiHeader.biWidth = _ShowWidth;
	BmInfo->bmiHeader.biXPelsPerMeter = 0;
	BmInfo->bmiHeader.biYPelsPerMeter = 0;


//	StretchDIBits(pDC->GetSafeHdc(),
// 				0, 0, 1024, 768,
// 				0, 0, 1024, 768,
//				pImage->m_nowView->imageData, BmInfo, DIB_RGB_COLORS, SRCCOPY);

	StretchDIBits(dcMem.GetSafeHdc(),
				0, 0, rectViewer.Width(), rectViewer.Height(),
				0, 0, rectViewer.Width(), rectViewer.Height(),
				pImage->imageData, BmInfo, DIB_RGB_COLORS, SRCCOPY);

	
	m_iImageWidth = pImage->width;
	m_iImageHeight = pImage->height;


	Invalidate(FALSE);
	ReleaseDC(pDC);	

	delete [] BmInfo;
}
//--------------------------------------------------------------------
void CInspectImage::DrawImage()
{
	// ��ü �׸� �׸���
	CDC *pDC = GetDC();
	CDC dcMem;
	CRect rectViewer;

	GetClientRect(rectViewer);

	if ( m_pbitmap ) delete m_pbitmap;
 	m_pbitmap = new CBitmap;		
	m_pbitmap->CreateCompatibleBitmap(pDC,rectViewer.Width(), rectViewer.Height());

	m_iWinWidth = rectViewer.Width();
	m_iWinHeight = rectViewer.Height();

	dcMem.CreateCompatibleDC(pDC);	
	dcMem.SelectObject(m_pbitmap);

	// �̹����� ��Ʈ�� �׳� �Ѹ���

	// ��Ʈ�� ���� ����
	BITMAPINFO *BmInfo = (BITMAPINFO*) malloc( sizeof( BITMAPINFOHEADER ) + sizeof(RGBQUAD) * 256 );
	
	for ( int i = 0; i < 256; i++ )
	{
		BmInfo->bmiColors[i].rgbRed = i;
		BmInfo->bmiColors[i].rgbGreen = i;
		BmInfo->bmiColors[i].rgbBlue = i;
		BmInfo->bmiColors[i].rgbReserved = 0;
	}

	BmInfo->bmiHeader.biBitCount = 8;
	BmInfo->bmiHeader.biCompression= 0;
	BmInfo->bmiHeader.biHeight = _ShowHeight;
	BmInfo->bmiHeader.biPlanes = 1;
	BmInfo->bmiHeader.biSizeImage = _ShowWidth*_ShowHeight;
	BmInfo->bmiHeader.biSize = 40;
	BmInfo->bmiHeader.biWidth = _ShowWidth;
	BmInfo->bmiHeader.biXPelsPerMeter = 0;
	BmInfo->bmiHeader.biYPelsPerMeter = 0;

	StretchDIBits(dcMem.GetSafeHdc(),
				0, 0, rectViewer.Width(), rectViewer.Height(),
				0, 0, rectViewer.Width(), rectViewer.Height(),
				m_nowView->imageData, BmInfo, DIB_RGB_COLORS, SRCCOPY);

	m_iImageWidth = m_nowView->width;
	m_iImageHeight = m_nowView->height;


	Invalidate(FALSE);
	ReleaseDC(pDC);	

	free( BmInfo );
}
//--------------------------------------------------------------------
void CInspectImage::DrawImage(IplImage *pImage, int nwidht, int nheight)
{
	int bitCnt = pImage->depth * pImage->nChannels;

	CDC *pDC = GetDC();
	CDC dcMem;
	CRect rectViewer;

	GetClientRect(rectViewer);

	if ( m_pbitmap ) delete m_pbitmap;
 	m_pbitmap = new CBitmap;		
	m_pbitmap->CreateCompatibleBitmap(pDC,nwidht, nheight);

	m_iWinWidth = nwidht;
	m_iWinHeight = nheight;

	dcMem.CreateCompatibleDC(pDC);	
	dcMem.SelectObject(m_pbitmap);

	// �̹����� ��Ʈ�� �׳� �Ѹ���

	// ��Ʈ�� ���� ����
	BITMAPINFO *BmInfo;

	if ( bitCnt == 24 )
	{
		BmInfo = (BITMAPINFO*) malloc( sizeof( BITMAPINFOHEADER ) );
	}
	else if ( bitCnt == 8 )
	{
		BmInfo = (BITMAPINFO*) malloc( sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * 256 );

		for ( int i = 0; i < 256; i++ )
		{
			BmInfo->bmiColors[i].rgbRed = i;
			BmInfo->bmiColors[i].rgbGreen = i;
			BmInfo->bmiColors[i].rgbBlue = i;
			BmInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	BmInfo->bmiHeader.biBitCount = bitCnt;
	BmInfo->bmiHeader.biCompression= 0;
	BmInfo->bmiHeader.biHeight = nheight;
	BmInfo->bmiHeader.biPlanes = 1;
	BmInfo->bmiHeader.biSizeImage = nwidht*nheight;
	BmInfo->bmiHeader.biSize = 40;
	BmInfo->bmiHeader.biWidth = nwidht;
	BmInfo->bmiHeader.biXPelsPerMeter = 0;
	BmInfo->bmiHeader.biYPelsPerMeter = 0;

	::StretchDIBits(dcMem.GetSafeHdc(),
				0, 0, nwidht, nheight,
				0, 0, nwidht, nheight,
				pImage->imageData, BmInfo, DIB_RGB_COLORS, SRCCOPY);
		
	m_iImageWidth = pImage->width;
	m_iImageHeight = pImage->height;

	Invalidate(FALSE);
	ReleaseDC(pDC);	

	free( BmInfo );
}

CInspImage::CInspImage()
{

}

CInspImage::~CInspImage()
{

}

void CInspectImage::CopyROI2(int num)
{
	// ROI�� �����Ѵ�. ���� �˻� �������� �ٸ� ���뿡���� �����ϱ�
	// num�� ������ 1~3����. 0�� �����̹Ƿ� �н�

	if(num==10)
	{
		// �˻��ϱ�� �� ��� �͵鿡 ���� �˻� ���� ������ 0������ �Ǿ��־�� ��.
		num = 0;
		bool ok = true;

		if (m_inspect_ox.chuck && m_inspect_ROI[0].chuck[0].x == -1)
			ok = false;
		if (m_inspect_ox.insert && m_inspect_ROI[0].insert[0].x == -1)
			ok = false;
		if (m_inspect_ox.jabara && m_inspect_ROI[0].jabara[0].x == -1)
			ok = false;
		if (m_inspect_ox.maguri && m_inspect_ROI[0].maguri[0].x == -1)
			ok = false;
		if (m_inspect_ox.etiquette && m_inspect_ROI[0].etiquette[0].x == -1)
			ok = false;
		if (m_inspect_ox.imul_etq_left && m_inspect_ROI[0].imul_etq_left[0].x == -1)
			ok = false;
		if (m_inspect_ox.imul_etq_right && m_inspect_ROI[0].imul_etq_right[0].x == -1)
			ok = false;


		if (!ok)
		{
			AfxMessageBox("��� �˻� ������ �����ϼ���!");
			return;
		}

		// ���� Drag&Drop����...
		AfxMessageBox("ȭ�鿡 Ŭ���Ͽ� ���ϴ� �������� Drag&Drop�Ͽ� �ű⼼��!");
		status_down = 1000 + num;
		// 1001~3 -> �����
		m_now_copying_ROI = num;

		// ���� ����
		if (m_ROIS < num+1)
			m_ROIS = num+1;
		return;
	}
	else if (num < 0 || num > _MAXROI - 1 )
	{
		AfxMessageBox("ROI ������ �߸��Ǿ����ϴ�!");
		return;
	}
	// ���� Drag&Drop����...
	AfxMessageBox("ȭ�鿡 Ŭ���Ͽ� ���ϴ� �������� Drag&Drop�Ͽ� �ű⼼��!");
	status_down = 2000 + num;
	// 1001~3 -> �����
	m_now_copying_ROI = num;

	// ���� ����
	if (m_ROIS < num+1)
		m_ROIS = num+1;

	// 2õ����

	return;
}

void CInspectImage::SetNewParameter(CString strpath)
{
	FILE *in = fopen(strpath,"r");
	int threshold, data[30];
	float fdata[10];
	char line[512];

	INSP_PARAM ReadParam;

	if (in == NULL)
	{
		threshold = 120;
		data[0] = 200;
		AfxMessageBox("������ �����ϴ�.");
		return;
	}
	else
	{
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&threshold); ReadParam.nData[0] = threshold;
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[0]);	ReadParam.nData[4] = data[0];

	//Etiquette Data
	fgets(line,512,in);
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[1]);	ReadParam.nData[6] = data[1];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[2]);	ReadParam.nData[7] = data[2];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[3]);	
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[4]);	
	ReadParam.fData[0] = data[4] + data[3]*0.01;

	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[5]);	ReadParam.nData[8] = data[5];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[6]);	ReadParam.nData[9] = data[6];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[7]);	ReadParam.nData[10] = data[7];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[8]);	ReadParam.nData[11] = data[8];
	// Imul Offset
	fgets(line,512,in);
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[9]);	ReadParam.nData[12] = data[9];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[10]);	ReadParam.nData[13] = data[10];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[11]);	ReadParam.nData[14] = data[11];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[12]);	ReadParam.nData[15] = data[12];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[13]);	ReadParam.nData[16] = data[13];

	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[14]);	ReadParam.nData[17] = data[14];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[15]);	ReadParam.nData[18] = data[15];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[16]);	ReadParam.nData[19] = data[16];
	// ���� ���Ժ� parameter.
	fgets(line,512,in);
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[17]);	ReadParam.nData[21] = data[17];
	// Straw ���� üũ Offset. 3point. 
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[18]);	ReadParam.nData[20] = data[18];
	
	// Ư�� Threshold
	fgets(line,512,in);
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[19]);	ReadParam.nData[1] = data[19];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[20]);	ReadParam.nData[2] = data[20];
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[21]);	ReadParam.nData[3] = data[21];

	// Camera Gain
	fgets(line,512,in);
	fgets(line,512,in); sscanf(line,"%f",&fdata[0]);	ReadParam.fData[1] = fdata[0];

	// ��� �¿� �� ����ġ
	fgets(line,512,in);
	fgets(line,512,in); sscanf(line,"%f",&fdata[1]);	ReadParam.fData[2] = fdata[1];

	// ������ ���� ���� Threshold value.
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%d",&data[22]);
	if(data[22]<0 || data[22] > 255) data[22] = data[17];	ReadParam.nData[5] = data[22];

	// ī�޶� ���� ���ǵ�.
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%f",&fdata[2]);
	if(fdata[2]<0 || fdata[2] > 100) fdata[2] = 1.31f;	ReadParam.fData[3] = fdata[2];

	// ��� ���̴� �κ� ���� ���.
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%f",&fdata[3]);
	if(fdata[3]<0 || fdata[3] > 5) fdata[3] = 2;			ReadParam.fData[4] = fdata[3];

	// ��� ���̴� �κ� ���� �Ϻ�.
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%f",&fdata[4]);
	if(fdata[4]<0 || fdata[4] > 5) fdata[4] = 2;			ReadParam.fData[5] = fdata[4];

	// ī�޶� ��� �ڵ� ��ȯ ��� ��� ����. 
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%d",&data[23]);
	if(data[23]<0 || data[23] > 1) data[23] = 0;			ReadParam.nData[22] = data[23];
	fgets(line,512,in); fgets(line,512,in);		// ��� 14 �Ǵ� 15 ���.
	sscanf(line,"%d",&data[25]);					
	if(data[25]<0 || data[25] > 20) data[25] = 0;			ReadParam.nData[24] = data[25];

	// ��� �� �����ϴ� ��ġ. 
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%d",&data[24]);
	if(data[24]<-100 || data[24] > 1000) data[24] = 10;			ReadParam.nData[23] = data[24];

	fclose(in);}


	// Ŭ���� �ʱ�ȭ
	m_process->m_highlimit = threshold;
	m_process->m_chuck_limit = data[0];
	m_process->GV_limit = data[1];
	m_process->GV_line_limit = data[2];
	m_process->GV_maxline_limit = data[3];
	m_process->dev_limit = data[4];
	m_process->m_EtqOff[0] = data[5];
	m_process->m_EtqOff[1] = data[6];
	m_process->m_EtqOff[2] = data[7];
	m_process->m_EtqOff[3] = data[8];
	m_process->m_OffX[0] = data[9];
	m_process->m_OffX[1] = data[10];
	m_process->m_OffX[2] = data[11];
	m_process->m_OffX[3] = data[12];
	m_process->m_OffX[4] = data[13];
	m_process->m_OffY[0] = data[14];
	m_process->m_OffY[1] = data[15];
	m_process->m_OffY[2] = data[16];
	m_process->m_nThresTransInsert = data[17];
	m_process->m_nStrawExistOffset = data[18];
	m_process->m_nSPThres[0] = data[19];
	m_process->m_nSPThres[1] = data[20];
	m_process->m_nSPThres[2] = data[21];
	m_process->m_fCamGain = fdata[0];
	m_process->m_fPokDiff = fdata[1];
	m_process->m_transMaguri = data[22];
	m_process->m_fCamShutter = fdata[2];
	m_process->m_fChuckSlideTop = fdata[3];
	m_process->m_fChuckSlideBottom = fdata[3];
	m_process->m_UseCam14mode = data[23];
	m_process->m_SelectCamMode = data[25];
	m_process->m_OffX[5] = data[24];
}


void CInspectImage::InitInspectSettingValues()
{
	m_inspect_setting.m_Slide = 0.0; 
	m_inspect_setting.m_SlideLength = 0;

	m_inspect_setting.m_Magunus_Slide = 0.0;
	m_inspect_setting.m_Magunus_Slide2 = 0.0;
	m_inspect_setting.m_Magunus_SlideLength = 0;
	m_inspect_setting.m_maguri_max_length = 0;

	m_inspect_setting.m_CannyLow = 0;
	m_inspect_setting.m_CannyHigh = 0;
	m_inspect_setting.m_CannyApp = 0;

	m_inspect_setting.m_minChuck = 0;
	m_inspect_setting.m_maxChuck = 0;

	m_inspect_setting.resol = 0;
	m_inspect_setting.camWidth = 0;
	m_inspect_setting.camHeight = 0;
	m_inspect_setting.triggerPerPitch = 0;
	m_inspect_setting.inspPitch = 0;

	m_inspect_setting.colorlimit = 0;
	m_inspect_setting.brightlimit = 0;
	m_inspect_setting.masksize = 0;

	m_inspect_setting.pixsize = 0;
	m_inspect_setting.mid_pixsize = 0;
	m_inspect_setting.jabara_pixsize = 0;
	m_inspect_setting.chuck_pixsize = 0;
	m_inspect_setting.insert_pixsize = 0;
	m_inspect_setting.imul_etq_left_pix = 0;
	m_inspect_setting.imul_etq_right_pix = 0;

	m_inspect_setting.imul_top_threshold = 0;
	m_inspect_setting.imul_mid_threshold = 0;
	m_inspect_setting.imul_jabara_threshold = 0;
	m_inspect_setting.imul_chuck_threshold = 0;
	m_inspect_setting.imul_insert_threshold = 0;
	m_inspect_setting.imul_etq_left_threshold = 0;
	m_inspect_setting.imul_etq_right_threshold = 0;

	m_inspect_setting.imul_top_mask = 0;
	m_inspect_setting.imul_top_mask_cols = 0;
	m_inspect_setting.imul_mid_mask = 0;
	m_inspect_setting.imul_jabara_mask = 0;
	m_inspect_setting.imul_chuck_mask = 0;
	m_inspect_setting.imul_chuck_mask_cols = 0;
	m_inspect_setting.imul_insert_mask = 0;
	m_inspect_setting.imul_insert_pos = 0;
	m_inspect_setting.imul_etq_left_mask = 0;
	m_inspect_setting.imul_etq_right_mask = 0;

	// �ڹٶ� ����
	m_inspect_setting.jabara_recognize = 0;
	m_inspect_setting.jabara_num = 0;
	m_inspect_setting.jabara_margin_of_error = 0;
	m_inspect_setting.jabara_width_stdev = 0;
	m_inspect_setting.jabara_distance_stdev = 0;
	m_inspect_setting.jabara_gradient_stdev = 0;
	m_inspect_setting.jabara_neighbor = 0;

	// ��Ƽ�� ����
	m_inspect_setting.etiquette_recognize = 0;
	m_inspect_setting.etiquette_position = 0;
	m_inspect_setting.etiquette_size = 0;

	// ���� ����
	m_inspect_setting.m_length_limit = 0;

	// �� ����
	m_inspect_setting.m_pok_limit = 0;
	m_inspect_setting.m_pok = 0;

	// ���� ����
	m_inspect_setting.m_angle_limit = 0;

	// ���� ���˻�
	m_inspect_setting.angle_limit[0] = 0;
	m_inspect_setting.angle_limit[1] = 0;
	// �¿� ����
	m_inspect_setting.angle_diff = 0;


	m_inspect_setting.m_maguri_diff_ratio = 0;
	m_inspect_setting.m_maguri_angle_gap = 0;
	m_inspect_setting.m_maguri_angle_continuous = 0;
	m_inspect_setting.m_maguri_angle_diff = 0;
	m_inspect_setting.m_mid_angle_gap = 0;
	m_inspect_setting.m_mid_angle_continuous = 0;
	m_inspect_setting.m_mid_angle_diff = 0;
	m_inspect_setting.m_mid_angle_height_diff = 0;
	m_inspect_setting.m_top_angle_gap = 0;
	m_inspect_setting.m_top_angle_continuous = 0;
	m_inspect_setting.m_top_angle_diff = 0;
	m_inspect_setting.m_top_angle_height_diff = 0;
	m_inspect_setting.m_chuck_angle_gap = 0;
	m_inspect_setting.m_chuck_angle_continuous = 0;
	m_inspect_setting.m_chuck_angle_diff = 0;
	m_inspect_setting.m_chuck_angle_height_diff = 0;
	m_inspect_setting.m_insert_angle_gap = 0;
	m_inspect_setting.m_insert_angle_continuous = 0;
	m_inspect_setting.m_insert_angle_diff = 0;

	m_inspect_setting.imul_top_persent = 0;
	m_inspect_setting.imul_mid_persent = 0;

	m_inspect_setting.imul_top_size = 0;
	m_inspect_setting.imul_mid_size = 0;

	m_inspect_setting.imul_mid_notinspect = 0;

	m_inspect_setting.stabbed_insert_notinspect = 0;
	m_inspect_setting.stabbed_insert_pix = 0;

}