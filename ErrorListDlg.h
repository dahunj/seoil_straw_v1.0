#pragma once

// CErrorListDlg dialog

struct INSPECT_RESULT_COUNT;
#define REPORT_PATH	_T("c:\\ATI\\Report\\%d_%d_%d.txt")

class CErrorListDlg : public CDialog
{
	DECLARE_DYNAMIC(CErrorListDlg)

public:
	CErrorListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CErrorListDlg();

	enum { IDD = IDD_DLG_ERROR_LIST };

protected:
	void DateDiff(unsigned int date);
	void InitDate();
	void UpdateRecord(INSPECT_RESULT_COUNT *pRecord);
	void WrappedSetDlgItemText(unsigned int nID, unsigned int val, double total);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	COleDateTime m_dateStart;
	COleDateTime m_dateEnd;
	COleDateTime m_dateCurrent;

	afx_msg void OnBnClickedBtnDay();
	afx_msg void OnBnClickedBtnWeek();
	afx_msg void OnBnClickedBtnSearch();
	virtual BOOL OnInitDialog();
};
