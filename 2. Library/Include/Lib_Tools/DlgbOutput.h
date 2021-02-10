#pragma once


#define MAX_OUTPUT_NUM	500
// CDlgbOutput dialog
class COutputList : public CListBox
{
	// Construction
public:
	COutputList();

	// Implementation
	void AddOutput(CString strMsg);
public:
	virtual ~COutputList();

protected:
	DECLARE_MESSAGE_MAP()
};

class AFX_EXT_CLASS CDlgbOutput : public CPaneDialog
{
	DECLARE_DYNAMIC(CDlgbOutput)

public:
	CDlgbOutput(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgbOutput();

	// Dialog Data
	enum { IDD = IDD_DLGB_OUTPUT };

	void Init(int nNum, CString *pstrTitle, CString strLogPath, int nInterval);
	void AddOutput(int nIndex, CString strMsg, bool bTime=true);
	CString GetFilePath(int nIndex);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	afx_msg LRESULT OnInitDialog( WPARAM wParam, LPARAM lParam);	

	DECLARE_MESSAGE_MAP()

	CMFCTabCtrl	m_wndTab;
	COutputList *m_pOutputPage;
	int m_nListNum;
	CFont m_Font;
	CString m_sTemp;
	CString m_sTemp2;
	CTime m_time;

	//////////////////////////////////////////////////////////////////////////
	// Log
	// [ 2012-4-18 ] : YS-Park
	CString *m_pstrLogMsg;
	CString *m_pstrTitle;
	CString m_logpath;
	CString m_strMainLogPath;
	CString m_strLastLogPath;
	CString m_strWriteMessage;
	void SaveLog2File();
	void SaveLog2File(int nIndex);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};