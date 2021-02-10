// ErrorListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SeoilVision.h"
#include "ErrorListDlg.h"
#include "SeoilVisionDlg.h"


// CErrorListDlg dialog

IMPLEMENT_DYNAMIC(CErrorListDlg, CDialog)

CErrorListDlg::CErrorListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CErrorListDlg::IDD, pParent)
	, m_dateStart(COleDateTime::GetCurrentTime())
	, m_dateEnd(COleDateTime::GetCurrentTime())
	, m_dateCurrent(COleDateTime::GetCurrentTime())
{
}

CErrorListDlg::~CErrorListDlg()
{
}

void CErrorListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_START, m_dateStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIME_END, m_dateEnd);
}


BEGIN_MESSAGE_MAP(CErrorListDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_DAY, &CErrorListDlg::OnBnClickedBtnDay)
	ON_BN_CLICKED(IDC_BTN_WEEK, &CErrorListDlg::OnBnClickedBtnWeek)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CErrorListDlg::OnBnClickedBtnSearch)
END_MESSAGE_MAP()


// CErrorListDlg message handlers

void CErrorListDlg::OnBnClickedBtnDay()
{
	DateDiff(0);
}

void CErrorListDlg::OnBnClickedBtnWeek()
{
	DateDiff(6);
}

void CErrorListDlg::UpdateRecord(INSPECT_RESULT_COUNT *pRecord)
{
	double total = (double)(pRecord->ok + pRecord->ng);
	
	SetDlgItemInt(IDC_EDIT_TOTAL, pRecord->ok + pRecord->ng);
	SetDlgItemInt(IDC_EDIT_OK, pRecord->ok);
	SetDlgItemInt(IDC_EDIT_NG, pRecord->ng);

	SetDlgItemInt(IDC_EDIT_NG_ANGLE, pRecord->ng_angle);
	SetDlgItemInt(IDC_EDIT_NG_POK, pRecord->ng_pok);
	SetDlgItemInt(IDC_EDIT_NG_MAGURI, pRecord->ng_maguri);
	SetDlgItemInt(IDC_EDIT_NG_CHUCK, pRecord->ng_chuck);
	SetDlgItemInt(IDC_EDIT_NG_INSERT, pRecord->ng_insert);
	SetDlgItemInt(IDC_EDIT_NG_ETIQUETTE, pRecord->ng_etiquette);
	SetDlgItemInt(IDC_EDIT_NG_JABARA, pRecord->ng_jabara);
	SetDlgItemInt(IDC_EDIT_NG_IMUL, pRecord->ng_imul);

	SetDlgItemInt(IDC_EDIT_NG_IMUL_TOP, pRecord->ng_imul_top);
	SetDlgItemInt(IDC_EDIT_NG_IMUL_MID, pRecord->ng_imul_mid);
	SetDlgItemInt(IDC_EDIT_NG_IMUL_JABARA, pRecord->ng_imul_jabara);
	SetDlgItemInt(IDC_EDIT_NG_IMUL_CHUCK, pRecord->ng_imul_chuck);
	SetDlgItemInt(IDC_EDIT_NG_IMUL_INSERT, pRecord->ng_imul_insert);

	WrappedSetDlgItemText(IDC_EDIT_OK_PER, pRecord->ok, total);
	WrappedSetDlgItemText(IDC_EDIT_NG_PER, pRecord->ng, total);
	WrappedSetDlgItemText(IDC_EDIT_NG_ANGLE_PER, pRecord->ng_angle, pRecord->ng);
	WrappedSetDlgItemText(IDC_EDIT_NG_POK_PER, pRecord->ng_pok, pRecord->ng);
	WrappedSetDlgItemText(IDC_EDIT_NG_MAGURI_PER, pRecord->ng_maguri, pRecord->ng);
	WrappedSetDlgItemText(IDC_EDIT_NG_CHUCK_PER, pRecord->ng_chuck, pRecord->ng);
	WrappedSetDlgItemText(IDC_EDIT_NG_INSERT_PER, pRecord->ng_insert, pRecord->ng);
	WrappedSetDlgItemText(IDC_EDIT_NG_ETIQUETTE_PER, pRecord->ng_etiquette, pRecord->ng);
	WrappedSetDlgItemText(IDC_EDIT_NG_JABARA_PER, pRecord->ng_jabara, pRecord->ng);
	WrappedSetDlgItemText(IDC_EDIT_NG_IMUL_PER, pRecord->ng_imul, pRecord->ng);
}

void CErrorListDlg::WrappedSetDlgItemText(unsigned int nID, unsigned int val, double total)
{
	double percentage;
	
	if(total == 0.0)
		percentage = 0.0;
	else
		percentage = val / total * 100.0;

	char output[16];
	
	sprintf(output, "%3.5f %%", percentage);
	
	SetDlgItemText(nID, output);
}

void CErrorListDlg::OnBnClickedBtnSearch()
{
	UpdateData(TRUE);

	if(m_dateStart > m_dateEnd)
	{
		AfxMessageBox(_T("날짜를 다시 설정하십시오!"));
		return;
	}

	COleDateTimeSpan span(1, 0, 0, 0);
	COleDateTime date = m_dateStart;

	INSPECT_RESULT_COUNT record;
	INSPECT_RESULT_COUNT result;

	memset(&record, 0, sizeof(INSPECT_RESULT_COUNT));
	memset(&result, 0, sizeof(INSPECT_RESULT_COUNT));

	CString filePath;
	FILE *fp;
	
	for(; date <= m_dateEnd; date +=span)
	{
		filePath.Format(REPORT_PATH, date.GetYear(), date.GetMonth(), date.GetDay());

		fp = fopen(filePath.GetBuffer(), "rb");

		if(fp != NULL)
		{
			fread(&record, sizeof(INSPECT_RESULT_COUNT), 1, fp);
			
			result += record;

			fclose(fp);
		}
	}
	
	UpdateRecord(&result);
}

BOOL CErrorListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitDate();

	return TRUE;
}

void CErrorListDlg::InitDate()
{
	DateDiff(0);
}

void CErrorListDlg::DateDiff(unsigned int date)
{
	COleDateTimeSpan diff(date, 0, 0, 0);

	m_dateEnd = m_dateCurrent;
	m_dateStart = m_dateCurrent - diff;

	UpdateData(false);
}
