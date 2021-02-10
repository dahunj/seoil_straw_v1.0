#if !defined(AFX_INSPRESULT_H__BEEF0B90_0BB6_40C9_BB7A_FC8B5FA0ECCF__INCLUDED_)
#define AFX_INSPRESULT_H__BEEF0B90_0BB6_40C9_BB7A_FC8B5FA0ECCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InspResult.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInspResult dialog

class CInspResult : public CDialog
{
public:
	CInspResult(CWnd* pParent = NULL);   // standard constructor
	CString result;

	enum { IDD = IDD_RESULT };
	CRichEditCtrl	m_richedit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspResult)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInspResult)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSPRESULT_H__BEEF0B90_0BB6_40C9_BB7A_FC8B5FA0ECCF__INCLUDED_)
