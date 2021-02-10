#if !defined(AFX_INSPECTIMAGE_H__8F30D305_790E_494E_A8AF_0142164D76E8__INCLUDED_)
#define AFX_INSPECTIMAGE_H__8F30D305_790E_494E_A8AF_0142164D76E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InspectImage.h : header file
//
#include "ViewingImage.h"
#include "StrawProcess.h"
#include "cxcore.h"
#include "highgui.h"
#include "cv.h"

/////////////////////////////////////////////////////////////////////////////
// CInspectImage window

class CInspectImage : public CStatic
{
// Construction
public:
	clock_t m_clktime;
	CInspectImage();

	// 검사용 클래스
	CStrawProcess *m_process;

	// 검사 결과 구조체
	//RESULT_INSPECT m_result_inspect[_MAXROI];
	//RESULT_INSPVALUE m_result_Value[_MAXROI];

	RESULT_INSPECT   *m_result_inspect;
	RESULT_INSPVALUE *m_result_Value;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspectImage)
	//}}AFX_VIRTUAL

// Implementation
public:
	int DoInspection();
	void SetROIS(int num);
	int m_ROIS;
	void DrawImage();
	bool SetROI(int num);
	bool LoadImage(LPCSTR filename);
	void DrawCopyBoxes(int num);
	bool SetInspectionROI(int roi, int type);
	void InitROIs();
	void DrawLine(CDC* pDC, int x1, int y1, int x2, int y2, BYTE R, BYTE G, BYTE B, int nPenStyle);
	void DrawX(CDC *pDC, int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b, int nPenStyle, int gap);
	void DrawSquare(CDC* pDC, int x1, int y1, int x2, int y2, BYTE r, BYTE g, BYTE b, int nPenStyle);
	void DrawCircle(CDC *pDC, int x, int y, int radius, BYTE R, BYTE G, BYTE B, int nPenStyle);
	void DrawImage(CViewingImage *pImage);
	CBitmap *m_pbitmap;

	// 검사 결과 그리나?
	bool m_drawinsepct;


	// 이미지 크기 등
	int m_iWinWidth;
	int m_iWinHeight;

	int m_iImageWidth;
	int m_iImageHeight;

	// 현재 클릭 상황
	int status_down;
	int status_move;

	// 현재 이미지 로드 상황
	bool m_Loaded;
	
	// 시작 위치
	CPoint m_start;
	CPoint m_now;

	// ROI들 1번~3번 + 예비 4번 빨대
	//CPoint m_ROI[_MAXROI][2];
	CPoint **m_ROI;

	// 2번 복사용
	CPoint m_ROI2[2];

	// 구조체
	struct INSPECT_ROI
	{
		CPoint maguri[2];	 // 마구리
		CPoint insert[2];	 // 역삽
		CPoint pok[2];		 // 폭
		CPoint angle[2][2];	 // 각도 2개 (좌, 우)
		CPoint jabara[2];	 // 자바라
		CPoint chuck[2];	 // 축관
		CPoint imul[2];	     // 이물
		CPoint etiquette[2]; // 에티켓
		CPoint imul_etq_left[2];  // 에티켓 좌측 이물
		CPoint imul_etq_right[2]; // 에티켓 우측 이물
	};

	


	INSPECT_SETTING m_inspect_setting;

	void InitInspectSettingValues();

	// ROI별로 검사 구조체도 예비 4개
	//INSPECT_ROI m_inspect_ROI[_MAXROI];
	INSPECT_ROI *m_inspect_ROI;

	// 검사 설정용 변수
	struct INSPECT_OX{
		BOOL maguri;		// 마구리 검사 유무
		BOOL insert;		// 역삽 검사 유무
		BOOL jabara;		// 자바라 검사 유무
		BOOL jabara_edge;	// 자바라 경계 검사 유무
		BOOL chuck;			// 축관 검사 유무
		BOOL imul;			// 이물 상단 검사 유무
		BOOL imul_mid;		// 이물 중단 검사 유무
		BOOL imul_jabara;	// 이물 자바라 검사 유무
		BOOL imul_chuck;	// 이물 축관 검사 유무
		BOOL imul_insert;	// 이물 내측 검사 유무
		BOOL imul_etq_left;	// 이물 좌측 에티켓 검사 유무
		BOOL imul_etq_right;// 이물 우측 에티켓 검사 유무
		BOOL maguri_angle;	// 마구리 찍힘 검사 유무
		BOOL mid_angle;		// 중단 찍힘 검사 유무
		BOOL top_angle;		// 상단 찍힘 검사 유무
		BOOL chuck_angle;	// 축관 찍힘 검사 유무
		BOOL insert_angle;	// 내측 찍힘 거사 유무
		BOOL maguri_side;	// 마구리 옆면 검사 유무

		///////////////// 사용 안함.
		BOOL etiquette; 
		BOOL pok;
		BOOL angle[2];
	};
	
	void InitOX();
	INSPECT_OX m_inspect_ox;
	void CopyROI(int num);

	// 현재 복사하는 ROI 번호

	int m_now_copying_ROI;

	// 검사 영역의 Offset (ROI 영역 시점을 기준으로)
	// 1번 영역에서만 해당되는 것임
	struct INSPECT_OFFSET{
		CPoint maguri[2];	// 마구리
		CPoint insert[2];	// 역삽
		CPoint pok[2];		// 폭
		CPoint angle[2][2];	// 각도 2개 (좌, 우)
		CPoint jabara[2];	// 자바라
		CPoint chuck[2];	// 축관
		CPoint imul[2];	    // 이물질은 예비로 10개까지
		CPoint etiquette[2]; // 에티켓
		CPoint imul_etq_left[2];  // 에티켓 좌측 이물
		CPoint imul_etq_right[2]; // 에티켓 우측 이물
		
	};
	INSPECT_OFFSET m_inspect_offset;

	// 임시 자바라용 높, 낮 평균
	double m_jabara_up_avg[3];
	double m_jabara_down_avg[3];

	virtual ~CInspectImage();

	// 자체 그림
	IplImage *m_nowView;
	IplImage *m_resizeView;
	IplImage *m_rgbResizeView;

	// Generated message map functions
public:
	void SetNewParameter(CString strpath);
	
	//bool m_bExistObj[_MAXROI];
	BOOL *m_bExistObj;

	void CopyROI2(int num);
	void DrawImage(IplImage *pImage);
	void DrawImage(IplImage *pImage, int width, int height);
	//{{AFX_MSG(CInspectImage)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CShowingImage window

class CShowingImage : public CStatic
{
public:
	CShowingImage();

public:
	virtual ~CShowingImage();

protected:
	DECLARE_MESSAGE_MAP()
};

class CInspImage : public CInspectImage  
{
public:
	CInspImage();
	virtual ~CInspImage();
};

#endif // !defined(AFX_INSPECTIMAGE_H__8F30D305_790E_494E_A8AF_0142164D76E8__INCLUDED_)
