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

	// �˻�� Ŭ����
	CStrawProcess *m_process;

	// �˻� ��� ����ü
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

	// �˻� ��� �׸���?
	bool m_drawinsepct;


	// �̹��� ũ�� ��
	int m_iWinWidth;
	int m_iWinHeight;

	int m_iImageWidth;
	int m_iImageHeight;

	// ���� Ŭ�� ��Ȳ
	int status_down;
	int status_move;

	// ���� �̹��� �ε� ��Ȳ
	bool m_Loaded;
	
	// ���� ��ġ
	CPoint m_start;
	CPoint m_now;

	// ROI�� 1��~3�� + ���� 4�� ����
	//CPoint m_ROI[_MAXROI][2];
	CPoint **m_ROI;

	// 2�� �����
	CPoint m_ROI2[2];

	// ����ü
	struct INSPECT_ROI
	{
		CPoint maguri[2];	 // ������
		CPoint insert[2];	 // ����
		CPoint pok[2];		 // ��
		CPoint angle[2][2];	 // ���� 2�� (��, ��)
		CPoint jabara[2];	 // �ڹٶ�
		CPoint chuck[2];	 // ���
		CPoint imul[2];	     // �̹�
		CPoint etiquette[2]; // ��Ƽ��
		CPoint imul_etq_left[2];  // ��Ƽ�� ���� �̹�
		CPoint imul_etq_right[2]; // ��Ƽ�� ���� �̹�
	};

	


	INSPECT_SETTING m_inspect_setting;

	void InitInspectSettingValues();

	// ROI���� �˻� ����ü�� ���� 4��
	//INSPECT_ROI m_inspect_ROI[_MAXROI];
	INSPECT_ROI *m_inspect_ROI;

	// �˻� ������ ����
	struct INSPECT_OX{
		BOOL maguri;		// ������ �˻� ����
		BOOL insert;		// ���� �˻� ����
		BOOL jabara;		// �ڹٶ� �˻� ����
		BOOL jabara_edge;	// �ڹٶ� ��� �˻� ����
		BOOL chuck;			// ��� �˻� ����
		BOOL imul;			// �̹� ��� �˻� ����
		BOOL imul_mid;		// �̹� �ߴ� �˻� ����
		BOOL imul_jabara;	// �̹� �ڹٶ� �˻� ����
		BOOL imul_chuck;	// �̹� ��� �˻� ����
		BOOL imul_insert;	// �̹� ���� �˻� ����
		BOOL imul_etq_left;	// �̹� ���� ��Ƽ�� �˻� ����
		BOOL imul_etq_right;// �̹� ���� ��Ƽ�� �˻� ����
		BOOL maguri_angle;	// ������ ���� �˻� ����
		BOOL mid_angle;		// �ߴ� ���� �˻� ����
		BOOL top_angle;		// ��� ���� �˻� ����
		BOOL chuck_angle;	// ��� ���� �˻� ����
		BOOL insert_angle;	// ���� ���� �Ż� ����
		BOOL maguri_side;	// ������ ���� �˻� ����

		///////////////// ��� ����.
		BOOL etiquette; 
		BOOL pok;
		BOOL angle[2];
	};
	
	void InitOX();
	INSPECT_OX m_inspect_ox;
	void CopyROI(int num);

	// ���� �����ϴ� ROI ��ȣ

	int m_now_copying_ROI;

	// �˻� ������ Offset (ROI ���� ������ ��������)
	// 1�� ���������� �ش�Ǵ� ����
	struct INSPECT_OFFSET{
		CPoint maguri[2];	// ������
		CPoint insert[2];	// ����
		CPoint pok[2];		// ��
		CPoint angle[2][2];	// ���� 2�� (��, ��)
		CPoint jabara[2];	// �ڹٶ�
		CPoint chuck[2];	// ���
		CPoint imul[2];	    // �̹����� ����� 10������
		CPoint etiquette[2]; // ��Ƽ��
		CPoint imul_etq_left[2];  // ��Ƽ�� ���� �̹�
		CPoint imul_etq_right[2]; // ��Ƽ�� ���� �̹�
		
	};
	INSPECT_OFFSET m_inspect_offset;

	// �ӽ� �ڹٶ�� ��, �� ���
	double m_jabara_up_avg[3];
	double m_jabara_down_avg[3];

	virtual ~CInspectImage();

	// ��ü �׸�
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
