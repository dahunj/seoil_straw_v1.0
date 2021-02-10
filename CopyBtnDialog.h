#pragma once
#include "afxwin.h"


// CCopyBtnDialog dialog

class CCopyBtnDialog : public CDialog
{
	DECLARE_DYNAMIC(CCopyBtnDialog)

public:
	CCopyBtnDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCopyBtnDialog();

// Dialog Data
	enum { IDD = IDD_DLG_COPYROI };

protected:
	const int k_maxBtns;
	const int k_maxChks;

	CButton *m_pBtns[8];
	CButton *m_pChks[9];

	int m_curPos;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedRoibtn2();
	afx_msg void OnBnClickedRoibtn3();
	afx_msg void OnBnClickedRoibtn4();
	afx_msg void OnBnClickedRoibtn5();
	afx_msg void OnBnClickedRoibtn6();
	afx_msg void OnBnClickedRoibtn7();
	afx_msg void OnBnClickedRoibtn8();
	afx_msg void OnBnClickedRoibtn9();

	void ShowBtns();
	void SetAllCheckBox();

	// ROI 복사 버튼
	CButton m_btnROI2;
	CButton m_btnROI3;
	CButton m_btnROI4;
	CButton m_btnROI5;
	CButton m_btnROI6;
	CButton m_btnROI7;
	CButton m_btnROI8;
	CButton m_btnROI9;
	
	// 체크박스 값
	BOOL m_checkROI1;
	BOOL m_checkROI2;
	BOOL m_checkROI3;
	BOOL m_checkROI4;
	BOOL m_checkROI5;
	BOOL m_checkROI6;
	BOOL m_checkROI7;
	BOOL m_checkROI8;
	BOOL m_checkROI9;

	// 체크박스 컨트롤
	CButton m_btnChkROI1;
	CButton m_btnChkROI2;
	CButton m_btnChkROI3;
	CButton m_btnChkROI4;
	CButton m_btnChkROI5;
	CButton m_btnChkROI6;
	CButton m_btnChkROI7;
	CButton m_btnChkROI8;
	CButton m_btnChkROI9;

	afx_msg void OnBnClickedCheckRoi1();
	afx_msg void OnBnClickedCheckRoi2();
	afx_msg void OnBnClickedCheckRoi3();
	afx_msg void OnBnClickedCheckRoi4();
	afx_msg void OnBnClickedCheckRoi5();
	afx_msg void OnBnClickedCheckRoi6();
	afx_msg void OnBnClickedCheckRoi7();
	afx_msg void OnBnClickedCheckRoi8();
	afx_msg void OnBnClickedCheckRoi9();

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
