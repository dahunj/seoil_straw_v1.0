#pragma once


// CRPMSettingDlg dialog

class CRPMSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CRPMSettingDlg)

public:
	CRPMSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRPMSettingDlg();

// Dialog Data
	enum { IDD = IDD_DLG_RPM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	int m_rpm;

public:
	int GetRPM();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
