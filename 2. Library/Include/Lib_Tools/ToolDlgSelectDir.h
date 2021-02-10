#pragma once


// CToolDlgSelectDir dialog

class AFX_EXT_CLASS CToolDlgSelectDir : public CDialog
{
	DECLARE_DYNAMIC(CToolDlgSelectDir)

public:
	CToolDlgSelectDir(CWnd* pParent = NULL);   // standard constructor
	virtual ~CToolDlgSelectDir();

// Dialog Data
	enum { IDD = IDD_TOOL_DLG_SELECT_DIR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int m_nSelectDir;
	virtual BOOL OnInitDialog();
};
