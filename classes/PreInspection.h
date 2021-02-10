#if !defined(AFX_PREINSPECTION_H__DCF212BF_A9D4_4FBA_A477_FDD77B6D89B0__INCLUDED_)
#define AFX_PREINSPECTION_H__DCF212BF_A9D4_4FBA_A477_FDD77B6D89B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreInspection.h : header file
//

#include "cxcore.h"
#include "cv.h"
#include "highgui.h"

#include "InspectImage.h"


#include "../resource.h"
#include "InspectSettingDlg.h"

#include "../INSParam.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "../CopyBtnDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CPreInspection dialog
class CSeoilVisionDlg;

UINT TestDoInspection(LPVOID lParam);

class CPreInspection : public CDialog
{
// Construction
public:
	CSeoilVisionDlg * m_MainPt;
	void SetPointer(CSeoilVisionDlg* CSpoint);
	void ReadParam(INSP_PARAM ParamData);
	void WriteParam(INSP_PARAM ParamData);
	CString m_sAutoOpenPath;
	bool m_bAutoOpen;
	CPreInspection(CWnd* pParent = NULL);   // standard constructor
	~CPreInspection();

	// 검사용 그림
	CInspectImage m_imageView;
	// 검사용 다이얼로그
	CInspectSettingDlg *m_settingdlg;

	CINSParam	* m_ptParam;
	
	// 그림 준비?
	bool m_prepared;

	// DB 완료?
	bool m_DBLoaded;
	CString m_sLoadImage;

// Dialog Data
	//{{AFX_DATA(CPreInspection)
	enum { IDD = IDD_PRE_INSPECTION };
	CButton	m_angle_right;
	CButton	m_angle_left;
	CButton	m_chuck;
	CButton	m_pok;
	CButton	m_maguri;
	CButton	m_jabara;
	CButton	m_insert;
	CButton m_etiquette;

	BOOL	m_bViewROI01;
	BOOL	m_bViewROI02;
	BOOL	m_bViewROI03;
	BOOL	m_bViewROI04;
	BOOL	m_bViewROI05;

	bool	*m_bViewROIS;
	//}}AFX_DATA

	afx_msg void OnDbload();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreInspection)
	public:
	virtual BOOL Create(CWnd *pParentWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
public:

	// Generated message map functions
	//{{AFX_MSG(CPreInspection)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLoadimage();
	afx_msg void OnSetValue();
	afx_msg void OnSetParam();
	afx_msg void OnRoi1();
	afx_msg void OnRoi2();
	afx_msg void OnRoi3();
	afx_msg void OnRoi4();
	afx_msg void OnRoi5();
	void OnRoi6();
	void OnRoi7();
	void OnRoi8();
	void OnRoi9();
	afx_msg void OnROI1_1();
	afx_msg void OnTest();
	afx_msg void OnSavedb();
	afx_msg void OnMaguri();
	afx_msg void OnChuck();
	afx_msg void OnJabara();
	afx_msg void OnInsert();
	afx_msg void OnPok();
	afx_msg void OnAngleLeft();
	afx_msg void OnAngleRight();
	afx_msg void OnEtiquette();

	afx_msg void OnBtnTtest();
	afx_msg void OnChkRoi01();
	afx_msg void OnChkRoi02();
	afx_msg void OnChkRoi03();
	afx_msg void OnChkRoi04();
	afx_msg void OnChkRoi05();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CCopyBtnDialog m_dlgCopyBtn;
	CButton m_checkROI1;
	void ShowROIS( int roi, BOOL isChecked );
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedButtonInspResult();
	CButton m_btnSaveDB;
	CButton m_btnLoadDB;
	CButton m_btn_imul_etq_left;
	CButton m_btn_imul_etq_right;
	afx_msg void OnBnClickedEtqLeft();
	afx_msg void OnBnClickedEtqRight();
};

// int bool -> int
// int BoolToInt(bool x)
// {
// 	if (x)
// 		return 1;
// 	else
// 		return 0;
//} 
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_PREINSPECTION_H__DCF212BF_A9D4_4FBA_A477_FDD77B6D89B0__INCLUDED_)
