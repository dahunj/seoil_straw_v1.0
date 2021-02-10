#pragma once


// CToolDlgROIMove dialog
#define D2DROI_MOVE			0
#define D2DROI_MOVE_ALL		1
#define D2DROI_COPY			2
#define D2DROI_COPY_ALL		3
#define D2DROI_CUTOFF		4

class AFX_EXT_CLASS CToolDlgROIMove : public CDialog
{
	DECLARE_DYNAMIC(CToolDlgROIMove)

public:
	CToolDlgROIMove(int nMode, int nMaxROINum, CWnd* pParent = NULL);   // standard constructor
	virtual ~CToolDlgROIMove();

// Dialog Data
	enum { IDD = IDD_TOOL_DLG_MOVE_ROI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	CComboBox m_ctrlSrc;
	CComboBox m_ctrlDest;

	int m_nMaxROINum;
	int m_nMode;
	int m_nSrc;
	int m_nDest;
	virtual BOOL OnInitDialog();
};
