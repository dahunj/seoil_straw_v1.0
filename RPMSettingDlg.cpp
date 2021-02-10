// RPMSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SeoilVision.h"
#include "SeoilVisionDlg.h"
#include "RPMSettingDlg.h"


// CRPMSettingDlg dialog

IMPLEMENT_DYNAMIC(CRPMSettingDlg, CDialog)

CRPMSettingDlg::CRPMSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRPMSettingDlg::IDD, pParent)
	, m_rpm(0)
{
}

CRPMSettingDlg::~CRPMSettingDlg()
{
}

void CRPMSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RPM, m_rpm);
}

int CRPMSettingDlg::GetRPM()
{
	return m_rpm;
}


BEGIN_MESSAGE_MAP(CRPMSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRPMSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRPMSettingDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRPMSettingDlg message handlers

void CRPMSettingDlg::OnBnClickedOk()
{	
	OnOK();
}

void CRPMSettingDlg::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CRPMSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSeoilVisionDlg *pMain = ( CSeoilVisionDlg* ) AfxGetMainWnd();

	m_rpm = pMain->m_nSetRPM;

	UpdateData( FALSE );

	return TRUE;
}
