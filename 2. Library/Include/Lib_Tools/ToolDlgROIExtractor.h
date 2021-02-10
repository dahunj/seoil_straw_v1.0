#pragma once


// CToolDlgROIExtractor dialog

class AFX_EXT_CLASS CToolDlgROIExtractor : public CDialog
{
	DECLARE_DYNAMIC(CToolDlgROIExtractor)

public:
	CToolDlgROIExtractor(CWnd* pParent = NULL, PBYTE pImage=NULL, PBYTE pMASK=NULL, PBYTE pBuff=NULL, int nW=0, int nH=0, CRect rtArea=CRect(0,0,0,0) );   // standard constructor
	virtual ~CToolDlgROIExtractor();

// Dialog Data
	enum { IDD = IDD_TOOL_DLG_EXTRACT_ROI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	PBYTE m_pImage;
	PBYTE m_pMASK;
	PBYTE m_pBuff;
	int m_nW;
	int m_nH;
	CRect m_rtArea;

public:
	int m_nSimilarity;
	afx_msg void OnBnClickedOk();
	CProgressCtrl m_ctrlProgress;
};
