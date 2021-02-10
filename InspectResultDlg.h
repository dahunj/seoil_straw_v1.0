#pragma once
#include "afxwin.h"
#include "classes/StrawProcess.h"

// CInspectResultDlg dialog

class CInspectResultDlg : public CDialog
{
	DECLARE_DYNAMIC(CInspectResultDlg)

public:
	INT_PTR DoModal(RESULT_INSPVALUE *pInspResult);
	CInspectResultDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInspectResultDlg();

// Dialog Data
	enum { IDD = IDD_DLG_INSP_RESULT };

protected:
	RESULT_INSPVALUE	m_inspResult[3];
	CRichEditCtrl		m_ctrREditInspResult;

	void ClearInspResult();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
