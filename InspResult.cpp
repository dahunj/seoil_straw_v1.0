// InspResult.cpp : implementation file
//

#include "stdafx.h"
#include "SeoilVision.h"
#include "InspResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspResult dialog


CInspResult::CInspResult(CWnd* pParent /*=NULL*/)
	: CDialog(CInspResult::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInspResult)
	//}}AFX_DATA_INIT
}


void CInspResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInspResult)
//	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_RESULT, m_richedit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInspResult, CDialog)
	//{{AFX_MSG_MAP(CInspResult)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspResult message handlers

INT_PTR CInspResult::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	

	// Å×½ºÆ®
//	SetDlgItemText(IDC_RESULT,"!!!");
//	SetDlgItemText(IDC_EDIT1,"!!!");

	return CDialog::DoModal();
}

void CInspResult::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	SetDlgItemText(IDC_RESULT,result);
	// TODO: Add your message handler code here
//	SetDlgItemText(IDC_RESULT,"!!!\r\n!!!");
//	SetDlgItemText(IDC_EDIT1,"!!!\n\r5!!!");
//	m_richedit.ReplaceSel("!!!");
}
