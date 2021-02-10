#if !defined(AFX_SETCOLORDLG_H__7EDBD794_B1F5_4A87_BB34_A703818D9526__INCLUDED_)
#define AFX_SETCOLORDLG_H__7EDBD794_B1F5_4A87_BB34_A703818D9526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetColorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetColorDlg dialog

class CSetColorDlg : public CDialog
{
// Construction
public:
	void SetTransColor(int color, int trans, int UseInspEtq);
	void Init();
	int m_color;
	int m_trans;
	int m_tepe;
	CSetColorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetColorDlg)
	enum { IDD = IDD_SET_COLOR };
	BOOL	m_bInspEtq;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetColorDlg)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetColorDlg)
	afx_msg void OnTranNorm();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTransTrans();
	afx_msg void OnColorNorm();
	afx_msg void OnColorRed();
	afx_msg void OnColorGreen();
	afx_msg void OnColorBlue();
	afx_msg void OnTepeTe();
	afx_msg void OnTepePe();
	afx_msg void OnChkInspEtq();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETCOLORDLG_H__7EDBD794_B1F5_4A87_BB34_A703818D9526__INCLUDED_)
