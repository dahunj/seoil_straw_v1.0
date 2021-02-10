#pragma once
#include "afxwin.h"

class AFX_EXT_CLASS CDlgParam : public CDialog
{
	DECLARE_DYNAMIC(CDlgParam)

public:
	CDlgParam(CWnd* pParent = NULL);
	virtual ~CDlgParam(void);

	enum { IDD = IDD_DLG_NULL_PAGE };

	virtual	void UpdateParam(bool bSave);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	BOOL m_bInit;
	CATIProperty m_wndPropList;

	virtual	void InitProp();
};
