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

	// 상황 초기화
	status_down = 0;
	status_move = 0;
	m_Loaded = false;
	m_ROIS = 0;
	
	// 모든 ROI 초기화
	InitROIs();

	// 모든 검사 초기화


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

	// 자체 이미지 생성
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

	// 이미지 삭제
	if( m_nowView != NULL )
		cvReleaseImage( &m_nowView );

	if( m_resizeView != NULL )
		cvReleaseImage( &m_resizeView );

	if( m_rgbResizeView != NULL )
		cvReleaseImage( &m_rgbResizeView );

	if( m_pbitmap != NULL)
		delete m_pbitmap;

	// 검사 클래스 삭세

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

	// 이미지 그리기
	dcMem.CreateCompatibleDC(pDC);
	dcMem.SelectObject(m_pbitmap);
	dc.BitBlt(1,1,rectViewer.Width() - 1, rectViewer.Height() - 1 , &dcMem,0,0,SRCCOPY);
	
	dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(rectViewer);
	
	// Do not call CStatic::OnPaint() for painting messages
	
	// ROI그리기
	for ( int i = 0; i < _MAXROI; i++ )
	{
		// 만약 ROI 셋팅 안되면 넘어감
		if (m_ROI[i][0].x == -1)
			continue;
		if(m_bExistObj[i]==false) 
			continue;
		
		// ROI는 백색
		DrawSquare(pDC,m_ROI[i][0].x, m_ROI[i][0].y,m_ROI[i][1].x,m_ROI[i][1].y,255,255,255,PS_DASHDOT);

		// 마구리 그리기->적색
		if (m_inspect_ROI[i].maguri[0].x !=-1 && m_inspect_ox.maguri)
			DrawSquare(pDC,m_inspect_ROI[i].maguri[0].x,m_inspect_ROI[i].maguri[0].y,m_inspect_ROI[i].maguri[1].x,m_inspect_ROI[i].maguri[1].y,255,0,0,PS_SOLID);

		// 자바라 그리기 -> 녹색
		if (m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.jabara)
			DrawSquare(pDC,m_inspect_ROI[i].jabara[0].x,m_inspect_ROI[i].jabara[0].y,m_inspect_ROI[i].jabara[1].x,m_inspect_ROI[i].jabara[1].y,0,255,0,PS_SOLID);

		// 폭 그리기 -> 청색
		if (m_inspect_ROI[i].pok[0].x != -1 && m_inspect_ox.pok)
				DrawSquare(pDC,m_inspect_ROI[i].pok[0].x,m_inspect_ROI[i].pok[0].y,m_inspect_ROI[i].pok[1].x,m_inspect_ROI[i].pok[1].y,0,0,255,PS_SOLID);

		// 축관 그리기 ->적+녹
		if (m_inspect_ROI[i].chuck[0].x != -1 && m_inspect_ox.chuck)
			DrawSquare(pDC,m_inspect_ROI[i].chuck[0].x,m_inspect_ROI[i].chuck[0].y,m_inspect_ROI[i].chuck[1].x,m_inspect_ROI[i].chuck[1].y,255,255,0,PS_SOLID);

		// 역삽 그리기 -> 적+청
		if (m_inspect_ROI[i].insert[0].x != -1 && m_inspect_ox.insert)
			DrawSquare(pDC,m_inspect_ROI[i].insert[0].x,m_inspect_ROI[i].insert[0].y,m_inspect_ROI[i].insert[1].x,m_inspect_ROI[i].insert[1].y,255,0,255,PS_SOLID);

		// 각 그리기 (2개)-> 청+녹
		for (int j=0;j<2;j++)
		{
			//if (m_inspect_ROI[i].angle[j][0].x != -1 && m_inspect_ox.angle[j])
			//	DrawSquare(pDC,m_inspect_ROI[i].angle[j][0].x,m_inspect_ROI[i].angle[j][0].y,m_inspect_ROI[i].angle[j][1].x,m_inspect_ROI[i].angle[j][1].y,0,255,255,PS_SOLID);

		}

		// 에티켓 그리기-> 보라
		if (m_inspect_ROI[i].etiquette[0].x !=-1 && m_inspect_ox.etiquette)
			DrawSquare(pDC,m_inspect_ROI[i].etiquette[0].x,m_inspect_ROI[i].etiquette[0].y,m_inspect_ROI[i].etiquette[1].x,m_inspect_ROI[i].etiquette[1].y,128,0,255,PS_SOLID);

		// 좌측 에티켓 이물 그리기 -> 보라
		if(m_inspect_ROI[i].imul_etq_left[0].x != -1 && m_inspect_ox.imul_etq_left)
		{
			DrawSquare(pDC,m_inspect_ROI[i].imul_etq_left[0].x,m_inspect_ROI[i].imul_etq_left[0].y,m_inspect_ROI[i].imul_etq_left[1].x,m_inspect_ROI[i].imul_etq_left[1].y,128,0,255,PS_SOLID);
		}

		// 우측 에티켓 이물 그리기 -> 파랑
		if(m_inspect_ROI[i].imul_etq_right[0].x != -1 && m_inspect_ox.imul_etq_right)
		{
			DrawSquare(pDC,m_inspect_ROI[i].imul_etq_right[0].x,m_inspect_ROI[i].imul_etq_right[0].y,m_inspect_ROI[i].imul_etq_right[1].x,m_inspect_ROI[i].imul_etq_right[1].y,0,0,255,PS_SOLID);
		}

		// 이물 그리기->회색 (생략)
		//if (m_inspect_ROI[i].imul[0].x != -1 && m_inspect_ox.imul)
		//	DrawSquare(pDC,m_inspect_ROI[i].imul[0].x, m_inspect_ROI[i].imul[0].y, m_inspect_ROI[i].imul[1].x, m_inspect_ROI[i].imul[1].y,128,128,128,PS_SOLID);
		

		// 이물 상단
		if ( m_inspect_ox.jabara )
		{
			// 자바라 있는 경우 ( 상단 )
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

			// 자바라 있는 경우 ( 중단 )
			if (   m_inspect_ROI[i].jabara[0].x != -1 && m_inspect_ox.imul_mid // 자바라 확인
				&& m_inspect_ROI[i].chuck[0].x != -1  && m_inspect_ox.chuck )  // 축관 확인
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
			// 자바라 없는 경우 ( 상단 )
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

			// 자바라 없는 경우 ( 중단 )
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

		
		// 이물 자바라 
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

		// 이물 축관
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

		// 이물 내측
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

		// 상단 각도 그리기
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

		// 중단 각도 그리기
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


	// 결과 그리기 -> 있는 경우만
	if (m_drawinsepct)
	{
		// 결과 있는 경우 결과를 OX로 그리기
		for (int i=0;i<m_ROIS;i++)
		{
			// 마구리부터 그린다.
			// Exsist 안하면 continue;
			if(m_bExistObj[i]==false) continue;

			// 마구리
			if ( m_inspect_ox.maguri )
			{
				if (m_result_inspect[i].maguri)
				{
					// O표
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
					// X표
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

			// 추가 시작 -------------------------------------------------

			// 이물질 부분-------------------------------------------------------
			if ( m_inspect_ox.jabara && m_inspect_ROI[i].jabara[0].x != -1 )
			{
				// 자바라 있을 때 이물 상단
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
				
				// 이물 중단
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
				// 축관이 없으면 검사 안 함
				if ( m_inspect_ox.chuck && m_inspect_ROI[i].chuck[0].x != -1 )
				{
					// 자바라 없을 때 이물 상단
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

					// 자바라 없을 때 이물 중단
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

			

			// 이물 자바라 ( 자바라 영역 )
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
					// X표
					DrawX(
						pDC,
						m_inspect_ROI[i].jabara[0].x, m_inspect_ROI[i].jabara[0].y,
						m_inspect_ROI[i].jabara[1].x, m_inspect_ROI[i].jabara[1].y,
						128, 128, 128,
						PS_SOLID,
						_GAP );
				}
			}

			

			// 이물 축관 ( 축관 영역 )
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

			// 이물 역삽 ( 역삽 영역 )
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

			// 추가 끝 ----------------------------------------------

			// 이물 끝 ----------------------------------------------

			// 자바라
			if (m_inspect_ox.jabara)
			{
				if (m_result_inspect[i].jabara)
				{
					// O표
					DrawCircle(pDC,(m_inspect_ROI[i].jabara[0].x+m_inspect_ROI[i].jabara[1].x)/2,(m_inspect_ROI[i].jabara[0].y+m_inspect_ROI[i].jabara[1].y)/2,5,0,255,0,PS_SOLID);
				}
				else
				{
					// X표
					DrawLine(pDC,m_inspect_ROI[i].jabara[0].x,m_inspect_ROI[i].jabara[0].y,m_inspect_ROI[i].jabara[1].x,m_inspect_ROI[i].jabara[1].y,0,255,0,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].jabara[0].x,m_inspect_ROI[i].jabara[1].y,m_inspect_ROI[i].jabara[1].x,m_inspect_ROI[i].jabara[0].y,0,255,0,PS_SOLID);
				}
			}

			// 축관
			if (m_inspect_ox.chuck)
			{
				if (m_result_inspect[i].chuck)
				{
					// O표
					DrawCircle(pDC,(m_inspect_ROI[i].chuck[0].x+m_inspect_ROI[i].chuck[1].x)/2,(m_inspect_ROI[i].chuck[0].y+m_inspect_ROI[i].chuck[1].y)/2,5,255,255,0,PS_SOLID);
					
				}
				else
				{
					// X표
					DrawLine(pDC,m_inspect_ROI[i].chuck[0].x,m_inspect_ROI[i].chuck[0].y,m_inspect_ROI[i].chuck[1].x,m_inspect_ROI[i].chuck[1].y,255,255,0,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].chuck[0].x,m_inspect_ROI[i].chuck[1].y,m_inspect_ROI[i].chuck[1].x,m_inspect_ROI[i].chuck[0].y,255,255,0,PS_SOLID);
				}
			}

			// 역삽
			if (m_inspect_ox.insert)
			{
				if (m_result_inspect[i].insert)
				{
					// O표
					DrawCircle(pDC,(m_inspect_ROI[i].insert[0].x+m_inspect_ROI[i].insert[1].x)/2,(m_inspect_ROI[i].insert[0].y+m_inspect_ROI[i].insert[1].y)/2,5,255,0,255,PS_SOLID);
					
				}
				else
				{
					// X표
					DrawLine(pDC,m_inspect_ROI[i].insert[0].x,m_inspect_ROI[i].insert[0].y,m_inspect_ROI[i].insert[1].x,m_inspect_ROI[i].insert[1].y,255,0,255,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].insert[0].x,m_inspect_ROI[i].insert[1].y,m_inspect_ROI[i].insert[1].x,m_inspect_ROI[i].insert[0].y,255,0,255,PS_SOLID);
				}
			}
			// 폭
			if (m_inspect_ox.pok)
			{
				if (m_result_inspect[i].pok != -1.0)
				{
					// O표
					DrawCircle(pDC,(m_inspect_ROI[i].pok[0].x+m_inspect_ROI[i].pok[1].x)/2,(m_inspect_ROI[i].pok[0].y+m_inspect_ROI[i].pok[1].y)/2,5,0,0,255,PS_SOLID);
					
				}
				else
				{
					// X표
					DrawLine(pDC,m_inspect_ROI[i].pok[0].x,m_inspect_ROI[i].pok[0].y,m_inspect_ROI[i].pok[1].x,m_inspect_ROI[i].pok[1].y,0,0,255,PS_SOLID);
					DrawLine(pDC,m_inspect_ROI[i].pok[0].x,m_inspect_ROI[i].pok[1].y,m_inspect_ROI[i].pok[1].x,m_inspect_ROI[i].pok[0].y,0,0,255,PS_SOLID);
				}
			}
			// 각
			for (int j=0;j<2;j++)
			{
				if (m_inspect_ox.angle[j])
				{
					if(m_result_inspect[i].angle[j] >= -900.0)
					{
						// O표
						DrawCircle(pDC,(m_inspect_ROI[i].angle[j][0].x+m_inspect_ROI[i].angle[j][1].x)/2,(m_inspect_ROI[i].angle[j][0].y+m_inspect_ROI[i].angle[j][1].y)/2,5,0,255,255,PS_SOLID);
						
					}
					else
					{
						// X표
						DrawLine(pDC,m_inspect_ROI[i].angle[j][0].x,m_inspect_ROI[i].angle[j][0].y,m_inspect_ROI[i].angle[j][1].x,m_inspect_ROI[i].angle[j][1].y,0,255,255,PS_SOLID);
						DrawLine(pDC,m_inspect_ROI[i].angle[j][0].x,m_inspect_ROI[i].angle[j][1].y,m_inspect_ROI[i].angle[j][1].x,m_inspect_ROI[i].angle[j][0].y,0,255,255,PS_SOLID);
					}
				}
			}
			
			// 에티켓
			if (m_inspect_ox.etiquette)
			{
				if (m_result_inspect[i].etiquette)
				{
					// O표
					DrawCircle(pDC,(m_inspect_ROI[i].etiquette[0].x+m_inspect_ROI[i].etiquette[1].x)/2,(m_inspect_ROI[i].etiquette[0].y+m_inspect_ROI[i].etiquette[1].y)/2,5,128,0,255,PS_SOLID);
				}
				else
				{
					// X표
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

			// 상단 각도
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

			// 중단 각도
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

	// 이미지의 비트맵 그냥 뿌리기

	// 비트맵 정보 셋팅
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
	
	// 마구리 : 1 자바라 : 2 축관 : 3 역삽 : 4
	// 폭 : 5 각 좌 : 11 각우 : 12 이물질 : 20~29
	// 에티켓: 30

	// 그리고 현 ROI는 100자리수 
	int now = status_down / 100;
	int stat = status_down % 100;

	double dRatioX = (double)_CamWidth/_ShowWidth;
	double dRatioY = (double)_CamHeight/_ShowHeight;
	point.x = (int)(point.x * dRatioX);
	point.y = (int)(point.y * dRatioY);

	// ROI 벗어나는지 검사
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

	// 마구리 검사로 되어있다.
	if (stat == 1 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;

		m_inspect_ROI[now].maguri[0] = point;

		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.maguri[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.maguri[0].y = point.y - m_ROI[0][0].y ;

		}
	}

	// 자바라 검사로 되어있다.
	if (stat == 2 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 단위로

		m_inspect_ROI[now].jabara[0] = point;
		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.jabara[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.jabara[0].y = point.y - m_ROI[0][0].y ;
		}

	}

	// 축관 검사로 되어있다.
	if (stat == 3 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;

		// status 100 단위로

		m_inspect_ROI[now].chuck[0] = point;
		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.chuck[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.chuck[0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// 역삽 검사로 되어있다.
	if (stat == 4 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 단위로

		m_inspect_ROI[now].insert[0] = point;

		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.insert[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.insert[0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// 폭 검사로 되어있다.
	if (stat == 5 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 단위로

		m_inspect_ROI[now].pok[0] = point;

		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.pok[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.pok[0].y = point.y - m_ROI[0][0].y ;

		}

	}

	// 좌측각 검사로 되어있다.
	if (stat == 11 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 단위로

		m_inspect_ROI[now].angle[0][0] = point;

		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.angle[0][0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.angle[0][0].y = point.y - m_ROI[0][0].y ;

		}
	}

	// 우측각 검사로 되어있다.
	if (stat == 12 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 단위로

		m_inspect_ROI[now].angle[1][0] = point;
		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.angle[1][0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.angle[1][0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// 좌측 에티켓 이물 검사로 되어있다.
	if (stat == 40 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 단위로

		m_inspect_ROI[now].imul_etq_left[0] = point;

		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.imul_etq_left[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul_etq_left[0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// 우측 에티켓 이물 검사로 되어있다.
	if (stat == 41 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;
		// status 100 단위로

		m_inspect_ROI[now].imul_etq_right[0] = point;

		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.imul_etq_right[0].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul_etq_right[0].y = point.y - m_ROI[0][0].y ;
		}
	}

	// 이물질 검사
	//if (status_down > 19 && status_down < 30)
	//{
	//	int now_imul = status_down - 20;
	//	status_move = status_down;
		// 1번 영역이면 Offset도 설정
	//	if (now == 0)
	//	{
	//		m_inspect_offset.imul[0].x = point.x - m_ROI[0][0].x ;
	//		m_inspect_offset.imul[0].y = point.y - m_ROI[0][0].y ;
	//	}
	//}

	// 에티켓 검사
	if(stat == 30 && status_down < 900)
	{
		status_move = status_down;
		status_down = 0;

		m_inspect_ROI[now].etiquette[0] = point;

		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.etiquette[0].x = point.x - m_ROI[0][0].x;
			m_inspect_offset.etiquette[0].y = point.y - m_ROI[0][0].y;
		}
	}

	// ROI 셋팅은 900~908
	if (status_down >899 && status_down < 1000)
	{
		int roi = status_down - 900;
		status_move = status_down;
		status_down = 0;
		m_ROI[roi][0] = point;
	}

	// ROI 복사의 경우는 1001~1003 (num번호 따라)
	if (status_down >= 1000 && status_down <1000 + _MAXROI)
	{
		// 만약 영역 벗어나면 안으로 들어가기
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
		// 만약 영역 벗어나면 안으로 들어가기
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

	// 원상 회복
	if (status_move < 900)
	{
  		int now = status_move / 100;

		// 추후 행동

		// 이물질의 경우 이미지 만들기
		if (status_move > 19 && status_move < 30)
		{
			int now_imul = status_move - 20;
		}
		
		// 마구리 : 1 자바라 : 2 축관 : 3 역삽 : 4
		// 폭 : 5 각 좌 : 11 각우 : 12 이물질 : 20~29
		// 에티켓: 30

		// 마구리 검사로 되어있다.
		if (status_move == 1 && status_down < 900)
		{
			//m_start = point;
			// status 100 단위로
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.maguri[0].x = m_inspect_ROI[now].maguri[0].x - m_ROI[0][0].x ;
				m_inspect_offset.maguri[0].y = m_inspect_ROI[now].maguri[0].y - m_ROI[0][0].y ;
				m_inspect_offset.maguri[1].x = m_inspect_ROI[now].maguri[1].x - m_ROI[0][0].x ;
				m_inspect_offset.maguri[1].y = m_inspect_ROI[now].maguri[1].y - m_ROI[0][0].y ;
			}

			// 다른 ROI 도 수정해 준다.
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

		// 자바라 검사로 되어있다.
		if (status_move == 2 && status_down < 900)
		{
			// status 100 단위로

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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.jabara[0].x = m_inspect_ROI[now].jabara[0].x - m_ROI[0][0].x ;
				m_inspect_offset.jabara[0].y = m_inspect_ROI[now].jabara[0].y - m_ROI[0][0].y ;
				m_inspect_offset.jabara[1].x = m_inspect_ROI[now].jabara[1].x - m_ROI[0][0].x ;
				m_inspect_offset.jabara[1].y = m_inspect_ROI[now].jabara[1].y - m_ROI[0][0].y ;

			}
			// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

		// 축관 검사로 되어있다.
		if (status_move == 3 && status_down < 900)
		{
			// status 100 단위로
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.chuck[0].x = m_inspect_ROI[now].chuck[0].x - m_ROI[0][0].x ;
				m_inspect_offset.chuck[0].y = m_inspect_ROI[now].chuck[0].y - m_ROI[0][0].y ;
				m_inspect_offset.chuck[1].x = m_inspect_ROI[now].chuck[1].x - m_ROI[0][0].x ;
				m_inspect_offset.chuck[1].y = m_inspect_ROI[now].chuck[1].y - m_ROI[0][0].y ;

			}
			// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

		// 역삽 검사로 되어있다.
		if (status_move == 4 && status_down < 900)
		{
			// status 100 단위로
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.insert[0].x = m_inspect_ROI[now].insert[0].x - m_ROI[0][0].x ;
				m_inspect_offset.insert[0].y = m_inspect_ROI[now].insert[0].y - m_ROI[0][0].y ;
				m_inspect_offset.insert[1].x = m_inspect_ROI[now].insert[1].x - m_ROI[0][0].x ;
				m_inspect_offset.insert[1].y = m_inspect_ROI[now].insert[1].y - m_ROI[0][0].y ;

			}
			// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

		// 폭 검사로 되어있다.
		if (status_move == 5 && status_down < 900)
		{
			// status 100 단위로
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.pok[0].x = m_inspect_ROI[now].pok[0].x - m_ROI[0][0].x ;
				m_inspect_offset.pok[0].y = m_inspect_ROI[now].pok[0].y - m_ROI[0][0].y ;
				m_inspect_offset.pok[1].x = m_inspect_ROI[now].pok[1].x - m_ROI[0][0].x ;
				m_inspect_offset.pok[1].y = m_inspect_ROI[now].pok[1].y - m_ROI[0][0].y ;

			}
			// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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


		// 좌측 에티켓 이물 검사로 되어있다.
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.imul_etq_left[0].x = m_inspect_ROI[now].imul_etq_left[0].x - m_ROI[0][0].x ;
				m_inspect_offset.imul_etq_left[0].y = m_inspect_ROI[now].imul_etq_left[0].y - m_ROI[0][0].y ;
				m_inspect_offset.imul_etq_left[1].x = m_inspect_ROI[now].imul_etq_left[1].x - m_ROI[0][0].x ;
				m_inspect_offset.imul_etq_left[1].y = m_inspect_ROI[now].imul_etq_left[1].y - m_ROI[0][0].y ;
			}

			// 다른 ROI 도 수정해 준다.
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





		// 우측 에티켓 이물 검사로 되어있다.
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.imul_etq_right[0].x = m_inspect_ROI[now].imul_etq_right[0].x - m_ROI[0][0].x ;
				m_inspect_offset.imul_etq_right[0].y = m_inspect_ROI[now].imul_etq_right[0].y - m_ROI[0][0].y ;
				m_inspect_offset.imul_etq_right[1].x = m_inspect_ROI[now].imul_etq_right[1].x - m_ROI[0][0].x ;
				m_inspect_offset.imul_etq_right[1].y = m_inspect_ROI[now].imul_etq_right[1].y - m_ROI[0][0].y ;
			}

			// 다른 ROI 도 수정해 준다.
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





		// 좌측각 검사로 되어있다.
		if (status_move == 11 && status_down < 900)
		{
			// status 100 단위로
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.angle[0][0].x = m_inspect_ROI[now].angle[0][0].x - m_ROI[0][0].x ;
				m_inspect_offset.angle[0][0].y = m_inspect_ROI[now].angle[0][0].y - m_ROI[0][0].y ;
				m_inspect_offset.angle[0][1].x = m_inspect_ROI[now].angle[0][1].x - m_ROI[0][0].x ;
				m_inspect_offset.angle[0][1].y = m_inspect_ROI[now].angle[0][1].y - m_ROI[0][0].y ;

			}
			// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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



		// 우측각 검사로 되어있다.
		if (status_move == 12 && status_down < 900)
		{
			// status 100 단위로
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.angle[1][0].x = m_inspect_ROI[now].angle[1][0].x - m_ROI[0][0].x ;
				m_inspect_offset.angle[1][0].y = m_inspect_ROI[now].angle[1][0].y - m_ROI[0][0].y ;
				m_inspect_offset.angle[1][1].x = m_inspect_ROI[now].angle[1][1].x - m_ROI[0][0].x ;
				m_inspect_offset.angle[1][1].y = m_inspect_ROI[now].angle[1][1].y - m_ROI[0][0].y ;

			}
			// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

		// 이물질 검사
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.imul[0].x = m_inspect_ROI[now].imul[0].x - m_ROI[0][0].x ;
				m_inspect_offset.imul[0].y = m_inspect_ROI[now].imul[0].y - m_ROI[0][0].y ;
				m_inspect_offset.imul[1].x = m_inspect_ROI[now].imul[1].x - m_ROI[0][0].x ;
				m_inspect_offset.imul[1].y = m_inspect_ROI[now].imul[1].y - m_ROI[0][0].y ;

			}
			// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

		// 에티켓 검사로 되어있다.
		if (status_move == 30 && status_down < 900)
		{
			// status 100 단위로
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
			// 1번 영역이면 Offset도 설정
			if (now == 0)
			{
				m_inspect_offset.etiquette[0].x = m_inspect_ROI[now].etiquette[0].x - m_ROI[0][0].x ;
				m_inspect_offset.etiquette[0].y = m_inspect_ROI[now].etiquette[0].y - m_ROI[0][0].y ;
				m_inspect_offset.etiquette[1].x = m_inspect_ROI[now].etiquette[1].x - m_ROI[0][0].x ;
				m_inspect_offset.etiquette[1].y = m_inspect_ROI[now].etiquette[1].y - m_ROI[0][0].y ;

			}
			// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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
		
		// 추후 행동
		status_move = 0;
	}

	if (status_move >= 1000 && status_move < 1010)
	{
		int now = status_move - 1000;
		
		// 추후 행동
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

	// 그리고 현 ROI는 100자리수 
	double dRatioX = (double)_CamWidth/_ShowWidth;
	double dRatioY = (double)_CamHeight/_ShowHeight;
	point.x = (int)(point.x * dRatioX);
	point.y = (int)(point.y * dRatioY);

 	int now = status_move / 100;
	int stat = status_move % 100;

	// ROI 벗어나는지 검사
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


	// 마구리 이동중
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

		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// ROI 셋팅은 900~903
	if (status_move >899 && status_move < 1000)
	{
		int roi = status_move - 900;
		
		m_ROI[roi][1] = point;



		Invalidate(FALSE);
	}

	// 자바라 검사로 되어있다.
	if (stat == 2 && status_move < 900)
	{
		// status 100 단위로

		m_inspect_ROI[now].jabara[1] = point;

		if (now == 0)
		{
			m_inspect_offset.jabara[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.jabara[1].y = point.y - m_ROI[0][0].y ;

		}
		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// 축관 검사로 되어있다.
	if (stat == 3 && status_move < 900)
	{
		// status 100 단위로

		m_inspect_ROI[now].chuck[1] = point;

		if (now == 0)
		{
			m_inspect_offset.chuck[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.chuck[1].y = point.y - m_ROI[0][0].y ;

		}

		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// 역삽 검사로 되어있다.
	if (stat == 4 && status_move < 900)
	{
		// status 100 단위로

		m_inspect_ROI[now].insert[1] = point;

		if (now == 0)
		{
			m_inspect_offset.insert[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.insert[1].y = point.y - m_ROI[0][0].y ;

		}
		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// 폭 검사로 되어있다.
	if (stat == 5 && status_move < 900)
	{
		// status 100 단위로

		m_inspect_ROI[now].pok[1] = point;
		if (now == 0)
		{
			m_inspect_offset.pok[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.pok[1].y = point.y - m_ROI[0][0].y ;

		}

		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// 좌측각 검사로 되어있다.
	if (stat == 11 && status_move < 900)
	{
		// status 100 단위로

		m_inspect_ROI[now].angle[0][1] = point;
		if (now == 0)
		{
			m_inspect_offset.angle[0][1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.angle[0][1].y = point.y - m_ROI[0][0].y ;

		}

		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// 우측각 검사로 되어있다.
	if (stat == 12 && status_move < 900)
	{
		// status 100 단위로

		m_inspect_ROI[now].angle[1][1] = point;
		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.angle[1][1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.angle[1][1].y = point.y - m_ROI[0][0].y ;

		}
		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// 이물질 검사
	if (status_move > 19 && status_move < 30)
	{
		int now_imul = status_move - 20;

		m_inspect_ROI[now].imul[1] = point;
		
		// 1번 영역이면 Offset도 설정
		if (now == 0)
		{
			m_inspect_offset.imul[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul[1].y = point.y - m_ROI[0][0].y ;
		}
		
		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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


	// 에티켓 이동중
	if (stat == 30 && status_move < 900)
	{
		m_inspect_ROI[now].etiquette[1] = point;

		if (now == 0)
		{
			m_inspect_offset.etiquette[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.etiquette[1].y = point.y - m_ROI[0][0].y ;

		}

		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// 좌측 에티켓 이물 이동중
	if (stat == 40 && status_move < 900)
	{
		m_inspect_ROI[now].imul_etq_left[1] = point;

		if (now == 0)
		{
			m_inspect_offset.imul_etq_left[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul_etq_left[1].y = point.y - m_ROI[0][0].y ;

		}

		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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

	// 우측 에티켓 이물 이동중
	if (stat == 41 && status_move < 900)
	{
		m_inspect_ROI[now].imul_etq_right[1] = point;

		if (now == 0)
		{
			m_inspect_offset.imul_etq_right[1].x = point.x - m_ROI[0][0].x ;
			m_inspect_offset.imul_etq_right[1].y = point.y - m_ROI[0][0].y ;

		}

		// 만약 다른 ROI까지도 있다면 그것도 수정해준다.
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


	// ROI 복사의 경우는 1001~1003 (num번호 따라)
	if (status_move > 1000 && status_move < 1000 + _MAXROI)
	{

		// 만약 영역 벗어나면 안으로 들어가기
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
	else if (status_move==1000)	// 1번 ROI 복사하기. (2번 ROI 정보를 사용함)
	{
		// 만약 영역 벗어나면 안으로 들어가기
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

	// ROI2 복사의 경우는 2000~1003 (num번호 따라)
	if (status_move > 1999 && status_move <2004)
	{

		// 만약 영역 벗어나면 안으로 들어가기
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

	// 사각형 그리기
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

	// 선 그리기
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
	// 모든 ROI 초기화
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

	// ROI 설정해주기
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
	// ROI를 복사한다. 같은 검사 설정으로 다른 빨대에까지 적용하기
	// num의 범위는 1~3까지. 0은 원본이므로 패스

	if (num < 1 || num > _MAXROI - 1)
	{
		AfxMessageBox("ROI 선택이 잘못되었습니다!");
		return;
	}

	// 검사하기로 된 모든 것들에 대해 검사 영역 설정이 0번에서 되어있어야 함.

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
		AfxMessageBox("모든 검사 영역을 설정하세요!");
		return;
	}

	AfxMessageBox("화면에 클릭하여 원하는 영역으로 Drag&Drop하여 옮기세요!\n(拖放到原意移動的領域)");
	status_down = 1000 + num;
	// 1001~3 -> 복사용
	m_now_copying_ROI = num;

	// 갯수 갱신
	if (m_ROIS < num+1)
		m_ROIS = num+1;

	return;

}

void CInspectImage::DrawCopyBoxes(int num)
{
	// ROI 복사할 때, 검사 영역 그려주기

	// 폭
	if(m_inspect_ox.pok)
	{
		m_inspect_ROI[num].pok[0].x = m_ROI[num][0].x+m_inspect_offset.pok[0].x;
		m_inspect_ROI[num].pok[1].x = m_ROI[num][0].x+m_inspect_offset.pok[1].x;
		m_inspect_ROI[num].pok[0].y = m_ROI[num][0].y+m_inspect_offset.pok[0].y;
		m_inspect_ROI[num].pok[1].y = m_ROI[num][0].y+m_inspect_offset.pok[1].y;
	}

	// 마구리
	if (m_inspect_ox.maguri)
	{
		m_inspect_ROI[num].maguri[0].x = m_ROI[num][0].x+m_inspect_offset.maguri[0].x;
		m_inspect_ROI[num].maguri[1].x = m_ROI[num][0].x+m_inspect_offset.maguri[1].x;
		m_inspect_ROI[num].maguri[0].y = m_ROI[num][0].y+m_inspect_offset.maguri[0].y;
		m_inspect_ROI[num].maguri[1].y = m_ROI[num][0].y+m_inspect_offset.maguri[1].y;

	}

	// 자바라
	if (m_inspect_ox.jabara)
	{
		m_inspect_ROI[num].jabara[0].x = m_ROI[num][0].x+m_inspect_offset.jabara[0].x;
		m_inspect_ROI[num].jabara[1].x = m_ROI[num][0].x+m_inspect_offset.jabara[1].x;
		m_inspect_ROI[num].jabara[0].y = m_ROI[num][0].y+m_inspect_offset.jabara[0].y;
		m_inspect_ROI[num].jabara[1].y = m_ROI[num][0].y+m_inspect_offset.jabara[1].y;

	}

	// 축관
	if (m_inspect_ox.chuck)
	{
		m_inspect_ROI[num].chuck[0].x = m_ROI[num][0].x+m_inspect_offset.chuck[0].x;
		m_inspect_ROI[num].chuck[1].x = m_ROI[num][0].x+m_inspect_offset.chuck[1].x;
		m_inspect_ROI[num].chuck[0].y = m_ROI[num][0].y+m_inspect_offset.chuck[0].y;
		m_inspect_ROI[num].chuck[1].y = m_ROI[num][0].y+m_inspect_offset.chuck[1].y;


	}
	
	// 역삽
	if (m_inspect_ox.insert)
	{
		m_inspect_ROI[num].insert[0].x = m_ROI[num][0].x+m_inspect_offset.insert[0].x;
		m_inspect_ROI[num].insert[1].x = m_ROI[num][0].x+m_inspect_offset.insert[1].x;
		m_inspect_ROI[num].insert[0].y = m_ROI[num][0].y+m_inspect_offset.insert[0].y;
		m_inspect_ROI[num].insert[1].y = m_ROI[num][0].y+m_inspect_offset.insert[1].y;


	}

	// 좌각
	if (m_inspect_ox.angle[0])
	{
		m_inspect_ROI[num].angle[0][0].x = m_ROI[num][0].x+m_inspect_offset.angle[0][0].x;
		m_inspect_ROI[num].angle[0][1].x = m_ROI[num][0].x+m_inspect_offset.angle[0][1].x;
		m_inspect_ROI[num].angle[0][0].y = m_ROI[num][0].y+m_inspect_offset.angle[0][0].y;
		m_inspect_ROI[num].angle[0][1].y = m_ROI[num][0].y+m_inspect_offset.angle[0][1].y;


	}

	// 우각
	if (m_inspect_ox.angle[1])
	{
		m_inspect_ROI[num].angle[1][0].x = m_ROI[num][0].x+m_inspect_offset.angle[1][0].x;
		m_inspect_ROI[num].angle[1][1].x = m_ROI[num][0].x+m_inspect_offset.angle[1][1].x;
		m_inspect_ROI[num].angle[1][0].y = m_ROI[num][0].y+m_inspect_offset.angle[1][0].y;
		m_inspect_ROI[num].angle[1][1].y = m_ROI[num][0].y+m_inspect_offset.angle[1][1].y;

	}

	// 이물
	if (m_inspect_ox.imul)
	{
		m_inspect_ROI[num].imul[0].x = m_ROI[num][0].x+m_inspect_offset.imul[0].x;
		m_inspect_ROI[num].imul[1].x = m_ROI[num][0].x+m_inspect_offset.imul[1].x;
		m_inspect_ROI[num].imul[0].y = m_ROI[num][0].y+m_inspect_offset.imul[0].y;
		m_inspect_ROI[num].imul[1].y = m_ROI[num][0].y+m_inspect_offset.imul[1].y;
	}

	// 에티켓
	if(m_inspect_ox.etiquette)
	{
		m_inspect_ROI[num].etiquette[0].x = m_ROI[num][0].x+m_inspect_offset.etiquette[0].x;
		m_inspect_ROI[num].etiquette[1].x = m_ROI[num][0].x+m_inspect_offset.etiquette[1].x;
		m_inspect_ROI[num].etiquette[0].y = m_ROI[num][0].y+m_inspect_offset.etiquette[0].y;
		m_inspect_ROI[num].etiquette[1].y = m_ROI[num][0].y+m_inspect_offset.etiquette[1].y;
	}

	// 좌측 에티켓 이물
	if(m_inspect_ox.imul_etq_left)
	{
		m_inspect_ROI[num].imul_etq_left[0].x = m_ROI[num][0].x+m_inspect_offset.imul_etq_left[0].x;
		m_inspect_ROI[num].imul_etq_left[1].x = m_ROI[num][0].x+m_inspect_offset.imul_etq_left[1].x;
		m_inspect_ROI[num].imul_etq_left[0].y = m_ROI[num][0].y+m_inspect_offset.imul_etq_left[0].y;
		m_inspect_ROI[num].imul_etq_left[1].y = m_ROI[num][0].y+m_inspect_offset.imul_etq_left[1].y;
	}

	// 우측 에티켓 이물
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
	// 파일 불러오기
	// 1024x768의 컬러이여만 된다.
	if( _CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024 )
		return false;

	IplImage *temp;
	temp = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
	
	bool ok = true;
	// 크기, 색상 등 테스트
	if (temp->width != _CamWidth)
		ok = false;
	if (temp->height != _CamHeight)
		ok = false;
	if (temp->nChannels != 1)
		ok = false;

	// 다 괜찮으면 이미지 복사
	if ( ok )
	{
		cvCopy( temp, m_nowView );

		cvResize( m_nowView, m_resizeView );

		cvCvtColor( m_resizeView, m_rgbResizeView, CV_GRAY2RGB );

		cvFlip( m_rgbResizeView, m_rgbResizeView );

		DrawImage( m_rgbResizeView, m_rgbResizeView->width, m_rgbResizeView->height );
	}

	// 릴리즈
	cvReleaseImage(&temp);

	return ok;
}

bool CInspectImage::SetROI(int num)
{
	if (!m_Loaded)
	{
		AfxMessageBox("일단 이미지부터 불러오시오!\n(先打開影像)");
		return false;
	}

	// ROI 셋팅 시작
	status_down = 900+num;
	return true;
}

void CInspectImage::SetROIS(int num)
{
	// ROI 개수 셋팅
	
	m_ROIS = num;

	return;

}

int CInspectImage::DoInspection()
{
	// 검사 실행
	int i, ReturnResult=0;
	int arrIdx = 0;
	


	m_process->ClearInspResult();


	for (i = 0; i < m_ROIS; i++)
	{
		// ROi 셋팅 여부 확인
		if (m_ROI[i][0].x == -1)
		{
			AfxMessageBox("먼저 ROI부터 다 셋팅하세요!");
			return -1;
		}

		// 검사 셋팅 여부 호가인
		// 검사하기로 된 모든 것들에 대해 검사 영역 설정이 0번에서 되어있어야 함.

		bool ok = true;

		// * 이물
		//   + 주변값으로 정해짐
		// * 찍힘
		//   + 주변값으로 정해짐
		// * 폭
		//   + 없앰
		// * 좌, 우 각도
		//   + 없앰
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
			AfxMessageBox("검사하기로 한 검사 영역을 모두 설정하시오!");
			return -1;
		}
	}

	// 검사 실시 전 설정갑 넘기기
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

	// 설정 값 초기화.
	memset(m_process->m_nResultData,0,sizeof(int) *20);
	memset(m_process->m_dResultData,0,sizeof(double) *20);


	// 이미지 상하 뒤집기
	// 이미지 셋팅
	//bool result = m_process->SetColorImage(m_nowView);
	bool result = m_process->SetMonoImage(m_nowView);
	//bool bfindstraw=false;

	if (!result)
	{
		AfxMessageBox("이미지 셋팅 실패!");
		return -1;
	}

	// 축관 검사에서 쓰는 값들....
	m_process->m_jabara.slide_up = 0.01;
	m_process->m_jabara.up_slide = 0.01;
	m_process->m_jabara.slide_down = 0.01;
	m_process->m_jabara.down_slide = 0.01;
 
	// 검사 시작 m_ROIS
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
		if(m_process->m_trans) // 투명
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
		else // 일반
		{
			area = cvRect(m_ROI[i][0].x,
					  m_ROI[i][0].y,
					  abs(m_ROI[i][0].x - m_ROI[i][1].x),
					  abs(m_ROI[i][0].y - m_ROI[i][1].y));
			 ox = m_process->StrawExistanceCheck(area, 1);
		}
		
		// 빨대가 없거나 기존에 검사한 경우 continue
		if (!ox)
		{
			m_bExistObj[i]= false;
			continue;
		}
		else 
		{
			m_bExistObj[i] = true;
		}

		// 마구리 검사
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

		// 상단 각도 검사
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

		// 중단 각도 검사
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

		// 축관 검사
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

			if(result==false) ReturnResult++; // Error 인 경우. 
			m_result_inspect[i].chuck = result;

			// 축관 각도 검사
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

		// 내측 검사
		if (m_inspect_ox.insert)
		{
			area = cvRect(m_inspect_ROI[i].insert[0].x,
						  m_inspect_ROI[i].insert[0].y,
						  abs(m_inspect_ROI[i].insert[0].x - m_inspect_ROI[i].insert[1].x),
						  abs(m_inspect_ROI[i].insert[0].y - m_inspect_ROI[i].insert[1].y));

			result = m_process->CheckInsert(area,1, true, m_inspect_setting.m_Slide, m_inspect_setting.m_SlideLength ,m_inspect_setting.stabbed_insert_notinspect, m_inspect_setting.stabbed_insert_pix);
			if(result==false) ReturnResult++; // Error 인 경우. 
			m_result_inspect[i].insert = result;

			
			// 내측 각도
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
			// 내측 각도
			if(m_inspect_ox.insert_angle)
			{
				area = cvRect(m_inspect_ROI[i].insert[0].x,
					m_inspect_ROI[i].insert[0].y,
					abs(m_inspect_ROI[i].insert[0].x - m_inspect_ROI[i].insert[1].x),
					abs(m_inspect_ROI[i].insert[0].y - m_inspect_ROI[i].insert[1].y));

				result = m_process->CheckInsertStabbed(area,1, true, m_inspect_setting.m_Slide, m_inspect_setting.m_SlideLength ,m_inspect_setting.stabbed_insert_notinspect, m_inspect_setting.stabbed_insert_pix);
				if(result==false) ReturnResult++; // Error 인 경우. 
				m_result_inspect[i].insert = result;

			}
		}

		
		
		// 자바라 검사
		if ( m_inspect_ox.jabara )
		{
			area = cvRect(m_inspect_ROI[i].jabara[0].x,
						  m_inspect_ROI[i].jabara[0].y,
						  abs(m_inspect_ROI[i].jabara[0].x - m_inspect_ROI[i].jabara[1].x),
						  abs(m_inspect_ROI[i].jabara[0].y - m_inspect_ROI[i].jabara[1].y));


			result = m_process->InspectJabara(
				area,									  // ROI
				m_inspect_setting.jabara_num,			  // 자바라 개수
				m_inspect_setting.jabara_margin_of_error, // 자바라 개수 허용 오차
				10,										  // 제거할 오브젝트 크기
				m_inspect_setting.jabara_recognize,		  // 히스토그램에서 threshold로 정할 상위 n% 픽셀
				m_inspect_setting.jabara_distance_stdev,  // 자바라 사이 거리 표준편차
				m_inspect_setting.jabara_width_stdev,	  // 자바라 두께 표준편차
				true									  // 출력 여부
				);

			if( result == true && m_inspect_ox.jabara_edge )
			{
				result = m_process->InspectJabaraEdge(
					area,									  // ROI
					m_inspect_setting.jabara_num,			  // 자바라 개수
					m_inspect_setting.jabara_margin_of_error, // 자바라 개수 허용 오차
					m_inspect_setting.jabara_gradient_stdev,  // 기울기 표준편차
					m_inspect_setting.jabara_neighbor,	      // 한 면의 min, max 차이
					true								      // 출력 여부
					);
			}

			if(result==false) ReturnResult++; // Error 인 경우.
			m_result_inspect[i].jabara = result;
		}

		// 이물 검사 ---------------------------------
		
		// 이물 검사 영역이 하나라도 존재할 경우 빨대 전체에 대해 Edge 구하기
		if ( m_inspect_ox.imul || m_inspect_ox.imul_chuck || m_inspect_ox.imul_mid ||
			m_inspect_ox.imul_insert || m_inspect_ox.imul_jabara )
		{
			// 빨대 하나에 대한 ROI
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

		// 이물 축관 검사
		if ( m_inspect_ox.imul_chuck && m_inspect_ox.chuck )
		{
			area = cvRect(m_inspect_ROI[i].chuck[0].x,
						  m_inspect_ROI[i].chuck[0].y,
						  abs(m_inspect_ROI[i].chuck[0].x - m_inspect_ROI[i].chuck[1].x),
						  abs(m_inspect_ROI[i].chuck[0].y - m_inspect_ROI[i].chuck[1].y));

			result = m_process->InspectBlackPoint(
				i,
				area,									// ROI ( 축관영역 )
				m_inspect_setting.imul_chuck_mask_cols,										// 축관 마스크 크기(Cols)
				m_inspect_setting.imul_chuck_mask,		// 축관 마스크 크기(Rows)
				m_inspect_setting.imul_chuck_threshold, // 축관 threshold
				m_inspect_setting.chuck_pixsize,		// 허용 이물 크기
				true,									// 출력 여부
				_T("축관"), STRAW_IMUL_CHUCK,
				area,
				m_inspect_setting.imul_chuck_mask_cols,
				1,
				1
				);

			if ( result == false ) ReturnResult++;
			m_result_inspect[i].imul_chuck = result;
		}

		// 이물 역삽 검사
		if ( m_inspect_ox.imul_insert && m_inspect_ox.insert )
		{
			area = cvRect(m_inspect_ROI[i].insert[0].x,
						  m_inspect_ROI[i].insert[0].y,
						  (int)(abs(m_inspect_ROI[i].insert[0].x - m_inspect_ROI[i].insert[1].x) * ( m_inspect_setting.imul_insert_pos / 100.0 )),
						  (int)abs(m_inspect_ROI[i].insert[0].y - m_inspect_ROI[i].insert[1].y));

			result = m_process->InspectBlackPoint(
				i,
				area,									 // ROI ( 내측 영역 )
				1,										 // 내측 마스크 크기(Cols)
				m_inspect_setting.imul_insert_mask,		 // 내측 마스크 크기(Rows)
				m_inspect_setting.imul_insert_threshold, // 내측 threshold
				m_inspect_setting.insert_pixsize,		 // 허용 이물 크기
				true,									 // 출력 여부
				_T("내측"), STRAW_IMUL_INSERT,
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

		// 이물 자바라 검사
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
					area,							         // ROI ( 자바라 영역 )
					1,										 // 자바라 마스크 크기(Cols)
					m_inspect_setting.imul_jabara_mask,		 // 자바라 마스크 크기(Rows)
					m_inspect_setting.imul_jabara_threshold, // 자바라 threshold
					m_inspect_setting.jabara_pixsize,		 // 허용 이물 크기
					true,									 // 출력 여부
					_T("자바라"), STRAW_IMUL_JABARA,
					area,
					m_inspect_setting.imul_jabara_threshold,
					1,
					1
					);

				if ( result == false ) ReturnResult++;
				m_result_inspect[i].imul_jabara = result;
			}

			// 이물 상단 ROI ( 왼쪽 끝 ~ 자바라 왼쪽 )
			if ( m_inspect_ox.imul )
			{
				topArea = cvRect( m_ROI[i][0].x,
								  m_ROI[i][0].y,
								  abs( m_ROI[i][0].x - m_inspect_ROI[i].jabara[0].x ),
								  abs( m_ROI[i][0].y - m_inspect_ROI[i].jabara[1].y ) );
				
			}

			// 이물 중단 ROI ( 자바라 오른쪽 ~ 축관 왼쪽 )
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
		// 자바라 검사 안 할 경우
		else
		{
			if ( m_inspect_ox.chuck )
			{
				// (왼쪽 끝 ~ 축관 왼쪽) / 2
				int halfWidth = abs( m_ROI[i][0].x - m_inspect_ROI[i].chuck[0].x ) / 2;

				// 왼쪽 끝 ~ 중간
				if ( m_inspect_ox.imul )
				{
					topArea = cvRect( m_ROI[i][0].x,
									  m_ROI[i][0].y,
									  halfWidth,
									  abs( m_ROI[i][0].y - m_ROI[i][1].y ) );
				}

				// 중간 ~ 축관 왼쪽
				if ( m_inspect_ox.imul_mid )
				{
					midArea = cvRect( m_ROI[i][0].x + halfWidth,
									  m_ROI[i][0].y,
									  halfWidth,
									  abs( m_ROI[i][0].y - m_ROI[i][1].y ) );
				}
			}
		}

		// 이물 상단 검사
		if ( m_inspect_ox.imul )
		{
			result = m_process->InspectBlackPoint(
				i,
				topArea,							  // ROI
				m_inspect_setting.imul_top_mask_cols, // 상단 마스크 크기(Cols)
				m_inspect_setting.imul_top_mask,	  // 상단 마스크 크기(Rows)
				m_inspect_setting.imul_top_threshold, // 상단 threshold
				m_inspect_setting.pixsize,			  // 허용 이물 크기
				true,								  // 출력 여부
				_T("상단"), STRAW_IMUL_TOP,
				maguri_rect,
				m_inspect_setting.imul_top_persent,
				m_inspect_setting.imul_top_size,
				m_inspect_setting.imul_mid_notinspect
				);

			if ( result == false ) ReturnResult++;
			m_result_inspect[i].imul = result;
		}
		
		// 이물 중단 검사
		if ( m_inspect_ox.imul_mid )
		{
			result = m_process->InspectBlackPoint(
				i,
				midArea,						      // ROI
				1,									  // 중단 마스크 크기(Cols)
				m_inspect_setting.imul_mid_mask,	  // 중단 마스크 크기(Rows)
				m_inspect_setting.imul_mid_threshold, // 중단 threshold
				m_inspect_setting.mid_pixsize,		  // 허용 이물 크기
				true,								  // 출력 여부
				_T("중단"), STRAW_IMUL_MID,
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


		// 에티켓 검사
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
		
		// 피치에 따라 다른 검사 결과
		memcpy(
			&m_result_Value[arrIdx++],
			&( m_process->m_ResultValue ),
			sizeof(m_process->m_ResultValue) );
	}

	m_drawinsepct = true;
	Invalidate(FALSE);

	m_process->ErrorFileClose();

	return ReturnResult; //Error 상태 Return.
}

void CInspectImage::DrawCircle(CDC *pDC, int x, int y, int radius, BYTE R, BYTE G, BYTE B, int nPenStyle)
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	// 선 그리기
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

	// 이미지의 비트맵 그냥 뿌리기

	// 비트맵 정보 셋팅
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
	// 자체 그림 그리기
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

	// 이미지의 비트맵 그냥 뿌리기

	// 비트맵 정보 셋팅
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

	// 이미지의 비트맵 그냥 뿌리기

	// 비트맵 정보 셋팅
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
	// ROI를 복사한다. 같은 검사 설정으로 다른 빨대에까지 적용하기
	// num의 범위는 1~3까지. 0은 원본이므로 패스

	if(num==10)
	{
		// 검사하기로 된 모든 것들에 대해 검사 영역 설정이 0번에서 되어있어야 함.
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
			AfxMessageBox("모든 검사 영역을 설정하세요!");
			return;
		}

		// 이제 Drag&Drop으로...
		AfxMessageBox("화면에 클릭하여 원하는 영역으로 Drag&Drop하여 옮기세요!");
		status_down = 1000 + num;
		// 1001~3 -> 복사용
		m_now_copying_ROI = num;

		// 갯수 갱신
		if (m_ROIS < num+1)
			m_ROIS = num+1;
		return;
	}
	else if (num < 0 || num > _MAXROI - 1 )
	{
		AfxMessageBox("ROI 선택이 잘못되었습니다!");
		return;
	}
	// 이제 Drag&Drop으로...
	AfxMessageBox("화면에 클릭하여 원하는 영역으로 Drag&Drop하여 옮기세요!");
	status_down = 2000 + num;
	// 1001~3 -> 복사용
	m_now_copying_ROI = num;

	// 갯수 갱신
	if (m_ROIS < num+1)
		m_ROIS = num+1;

	// 2천번대

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
		AfxMessageBox("파일이 없습니다.");
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
	// 투명 삽입부 parameter.
	fgets(line,512,in);
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[17]);	ReadParam.nData[21] = data[17];
	// Straw 유무 체크 Offset. 3point. 
	fgets(line,512,in);
	fgets(line,512,in);	sscanf(line,"%d",&data[18]);	ReadParam.nData[20] = data[18];
	
	// 특정 Threshold
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

	// 축관 좌우 폭 제한치
	fgets(line,512,in);
	fgets(line,512,in); sscanf(line,"%f",&fdata[1]);	ReadParam.fData[2] = fdata[1];

	// 마구리 투명 내측 Threshold value.
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%d",&data[22]);
	if(data[22]<0 || data[22] > 255) data[22] = data[17];	ReadParam.nData[5] = data[22];

	// 카메라 셔터 스피드.
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%f",&fdata[2]);
	if(fdata[2]<0 || fdata[2] > 100) fdata[2] = 1.31f;	ReadParam.fData[3] = fdata[2];

	// 축관 꺽이는 부분 기울기 상부.
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%f",&fdata[3]);
	if(fdata[3]<0 || fdata[3] > 5) fdata[3] = 2;			ReadParam.fData[4] = fdata[3];

	// 축관 꺽이는 부분 기울기 하부.
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%f",&fdata[4]);
	if(fdata[4]<0 || fdata[4] > 5) fdata[4] = 2;			ReadParam.fData[5] = fdata[4];

	// 카메라 모드 자동 변환 기능 사용 여부. 
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%d",&data[23]);
	if(data[23]<0 || data[23] > 1) data[23] = 0;			ReadParam.nData[22] = data[23];
	fgets(line,512,in); fgets(line,512,in);		// 모드 14 또는 15 사용.
	sscanf(line,"%d",&data[25]);					
	if(data[25]<0 || data[25] > 20) data[25] = 0;			ReadParam.nData[24] = data[25];

	// 축관 폭 제한하는 수치. 
	fgets(line,512,in); fgets(line,512,in);
	sscanf(line,"%d",&data[24]);
	if(data[24]<-100 || data[24] > 1000) data[24] = 10;			ReadParam.nData[23] = data[24];

	fclose(in);}


	// 클래스 초기화
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

	// 자바라 관련
	m_inspect_setting.jabara_recognize = 0;
	m_inspect_setting.jabara_num = 0;
	m_inspect_setting.jabara_margin_of_error = 0;
	m_inspect_setting.jabara_width_stdev = 0;
	m_inspect_setting.jabara_distance_stdev = 0;
	m_inspect_setting.jabara_gradient_stdev = 0;
	m_inspect_setting.jabara_neighbor = 0;

	// 에티켓 관련
	m_inspect_setting.etiquette_recognize = 0;
	m_inspect_setting.etiquette_position = 0;
	m_inspect_setting.etiquette_size = 0;

	// 길이 관련
	m_inspect_setting.m_length_limit = 0;

	// 폭 관련
	m_inspect_setting.m_pok_limit = 0;
	m_inspect_setting.m_pok = 0;

	// 각도 관련
	m_inspect_setting.m_angle_limit = 0;

	// 다중 각검사
	m_inspect_setting.angle_limit[0] = 0;
	m_inspect_setting.angle_limit[1] = 0;
	// 좌우 각차
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