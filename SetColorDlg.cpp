// SetColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SeoilVision.h"
#include "SetColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetColorDlg dialog


CSetColorDlg::CSetColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetColorDlg)
	m_bInspEtq = FALSE;
	//}}AFX_DATA_INIT

	Init();
}


void CSetColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetColorDlg)
	DDX_Check(pDX, IDC_CHK_INSP_ETQ, m_bInspEtq);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetColorDlg, CDialog)
	//{{AFX_MSG_MAP(CSetColorDlg)
	ON_BN_CLICKED(IDC_TRAN_NORM, OnTranNorm)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_TRANS_TRANS, OnTransTrans)
	ON_BN_CLICKED(IDC_COLOR_NORM, OnColorNorm)
	ON_BN_CLICKED(IDC_COLOR_RED, OnColorRed)
	ON_BN_CLICKED(IDC_COLOR_GREEN, OnColorGreen)
	ON_BN_CLICKED(IDC_COLOR_BLUE, OnColorBlue)
	ON_BN_CLICKED(IDC_TEPE_TE, OnTepeTe)
	ON_BN_CLICKED(IDC_TEPE_PE, OnTepePe)
	ON_BN_CLICKED(IDC_CHK_INSP_ETQ, OnChkInspEtq)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetColorDlg message handlers

void CSetColorDlg::OnTranNorm() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_TRAN_NORM,TRUE);
	CheckDlgButton(IDC_TRANS_TRANS,FALSE);
	m_trans = 0;
}

//DEL int CSetColorDlg::m_trans()
//DEL {
//DEL 
//DEL }

void CSetColorDlg::Init()
{
	// 색 및 칼라 빨대 설정
	m_color    = 0;
	m_trans = 0;

}

void CSetColorDlg::SetTransColor(int trans, int color, int UseInspEtq)
{
	// 컬러와 색 여부 설정
	m_trans = trans;
	m_color    = color;
	m_bInspEtq = UseInspEtq;
//	UpdateData(false);
	return;

}

INT_PTR CSetColorDlg::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	// 셋팅 맞추주기
	if (m_trans == 0)
	{
// 		CheckRadioButton(IDC_TRAN_NORM,IDC_TRANS_TRANS,IDC_TRAN_NORM);
// 		CheckDlgButton(IDC_TRAN_NORM,TRUE);
	}
	else
	{

	}

	return CDialog::DoModal();
}

void CSetColorDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	// 셋팅 맞추주기
	if (m_trans == 0)
	{
// 		CheckRadioButton(IDC_TRAN_NORM,IDC_TRANS_TRANS,IDC_TRAN_NORM);
		CheckDlgButton(IDC_TRAN_NORM,TRUE);
		CheckDlgButton(IDC_TRANS_TRANS,FALSE);
	}
	else
	{
		CheckDlgButton(IDC_TRAN_NORM,FALSE);
		CheckDlgButton(IDC_TRANS_TRANS,TRUE);
	}
	
	switch(m_color)
	{
	case 0:
		CheckDlgButton(IDC_COLOR_NORM,TRUE);
		CheckDlgButton(IDC_COLOR_RED,FALSE);
		CheckDlgButton(IDC_COLOR_GREEN,FALSE);
		CheckDlgButton(IDC_COLOR_BLUE,FALSE);
		break;
	case 1:
		CheckDlgButton(IDC_COLOR_NORM,FALSE);
		CheckDlgButton(IDC_COLOR_RED,TRUE);
		CheckDlgButton(IDC_COLOR_GREEN,FALSE);
		CheckDlgButton(IDC_COLOR_BLUE,FALSE);
		break;
	case 2:
		CheckDlgButton(IDC_COLOR_NORM,FALSE);
		CheckDlgButton(IDC_COLOR_RED,FALSE);
		CheckDlgButton(IDC_COLOR_GREEN,TRUE);
		CheckDlgButton(IDC_COLOR_BLUE,FALSE);

		break;
	case 3:
		CheckDlgButton(IDC_COLOR_NORM,FALSE);
		CheckDlgButton(IDC_COLOR_RED,FALSE);
		CheckDlgButton(IDC_COLOR_GREEN,FALSE);
		CheckDlgButton(IDC_COLOR_BLUE,true);

		break;
	}

	if (m_tepe == 0)

	{
		CheckDlgButton(IDC_TEPE_TE,TRUE);
		CheckDlgButton(IDC_TEPE_PE,FALSE);

	}
	else
	{
		CheckDlgButton(IDC_TEPE_TE,FALSE);
		CheckDlgButton(IDC_TEPE_PE,TRUE);
	}
}

void CSetColorDlg::OnTransTrans() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_TRAN_NORM,FALSE);
	CheckDlgButton(IDC_TRANS_TRANS,TRUE);
	m_trans = 1;
}

void CSetColorDlg::OnColorNorm() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_COLOR_NORM,TRUE);
	CheckDlgButton(IDC_COLOR_RED,FALSE);
	CheckDlgButton(IDC_COLOR_GREEN,FALSE);
	CheckDlgButton(IDC_COLOR_BLUE,FALSE);
	m_color = 0;	
}

void CSetColorDlg::OnColorRed() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_COLOR_NORM,false);
	CheckDlgButton(IDC_COLOR_RED,true);
	CheckDlgButton(IDC_COLOR_GREEN,FALSE);
	CheckDlgButton(IDC_COLOR_BLUE,FALSE);
	m_color = 1;	
	
}

void CSetColorDlg::OnColorGreen() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_COLOR_NORM,false);
	CheckDlgButton(IDC_COLOR_RED,FALSE);
	CheckDlgButton(IDC_COLOR_GREEN,true);
	CheckDlgButton(IDC_COLOR_BLUE,FALSE);
	m_color = 2;	
	
}

void CSetColorDlg::OnColorBlue() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_COLOR_NORM,false);
	CheckDlgButton(IDC_COLOR_RED,FALSE);
	CheckDlgButton(IDC_COLOR_GREEN,true);
	CheckDlgButton(IDC_COLOR_BLUE,FALSE);
	m_color = 3;	
	
}

void CSetColorDlg::OnTepeTe() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_TEPE_TE,TRUE);
	CheckDlgButton(IDC_TEPE_PE,FALSE);
	m_tepe = 0;
}

void CSetColorDlg::OnTepePe() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_TEPE_TE,FALSE);
	CheckDlgButton(IDC_TEPE_PE,TRUE);
	m_tepe = 1;
}

void CSetColorDlg::OnChkInspEtq() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
}
