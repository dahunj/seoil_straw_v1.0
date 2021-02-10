// INSParam.cpp : implementation file
//

#include "stdafx.h"
#include "SeoilVision.h"
#include "INSParam.h"
#include "classes/PreInspection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CINSParam dialog


CINSParam::CINSParam(CWnd* pParent /*=NULL*/)
	: CDialog(CINSParam::IDD, pParent)
{
	//{{AFX_DATA_INIT(CINSParam)
	m_nMainThres = 150;
	m_nSatuthres = 240;
	m_nMaguriThres = 100;
	m_nChuckThres = 100;
	m_nAngleThres = 100;
	m_midNotInspArea = 0;
	m_chuckLeft = 0.00f;
	m_nEtqtoStopper = 25;
	m_nEtqInspArea = 80;
	m_maguriSideDepth = 2;
	m_nEtqLength = 310;
	m_chuckNotInspArea = 2;
	m_nImulMidToStopper = 10;
	m_nImulChuckToStopper = 10;
	m_nImulInsertHeight = 20;
	m_nImulInsertNoInsp = 50;
	m_nImulInsertUpDown = 10;
	m_nImulJabaraNoInsp = 50;
	m_nImulMaguriNoInsp = 15;
	m_transRatio = 3;
	m_fCameraGain = 0.0f;
	m_fCameraShutter = 3.0f;
	m_fChuckPokAverage = 1.3f;
	m_fChuckDownScale = 0.08f;
	m_fChuckUpScale = 0.08f;
	m_nImageCenter = -100;
	m_nTransInsertThres = 20;
	m_nTransMaguriThres = 15;
	m_bUse14Trigger = FALSE;
	m_nImulReduce = 10;
	m_nColorThres01 = 0;
	m_nColorThres02 = 0;
	m_nColorThres03 = 0;
	m_nTriChangeMode = 0;
	//}}AFX_DATA_INIT
}


void CINSParam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CINSParam)
	DDX_Text(pDX, IDC_ED_THRES_MAIN, m_nMainThres);
	DDV_MinMaxInt(pDX, m_nMainThres, 0, 255);
	DDX_Text(pDX, IDC_ED_THRES_SATURATION, m_nSatuthres);
	DDV_MinMaxInt(pDX, m_nSatuthres, 0, 255);
	DDX_Text(pDX, IDC_ED_THRES_MAGURI, m_nMaguriThres);
	DDV_MinMaxInt(pDX, m_nMaguriThres, -50, 300);
	DDX_Text(pDX, IDC_ED_THRES_CHUCK, m_nChuckThres);
	DDV_MinMaxInt(pDX, m_nChuckThres, 0, 300);
	DDX_Text(pDX, IDC_ED_THRES_ANGLE, m_nAngleThres);
	DDV_MinMaxInt(pDX, m_nAngleThres, 0, 300);
	DDX_Text(pDX, IDC_ED_ETIQ_CHABRIGHT, m_midNotInspArea);
	DDV_MinMaxInt(pDX, m_midNotInspArea, 0, 100);
	DDX_Text(pDX, IDC_ED_ETIQ_DEVIATION, m_chuckLeft);
	DDV_MinMaxFloat(pDX, m_chuckLeft, -500.f, 500.f);
	DDX_Text(pDX, IDC_ED_ETIQ_ETIQSTOP, m_nEtqtoStopper);
	DDV_MinMaxInt(pDX, m_nEtqtoStopper, -50, 300);
	DDX_Text(pDX, IDC_ED_ETIQ_INSPECTAREA, m_nEtqInspArea);
	DDV_MinMaxInt(pDX, m_nEtqInspArea, -50, 300);
	DDX_Text(pDX, IDC_ED_ETIQ_JABARAETIQ, m_maguriSideDepth);
	DDV_MinMaxInt(pDX, m_maguriSideDepth, -50, 50);
	DDX_Text(pDX, IDC_ED_ETIQ_LENGTH, m_nEtqLength);
	DDV_MinMaxInt(pDX, m_nEtqLength, -50, 1000);
	DDX_Text(pDX, IDC_ED_ETIQ_LINECNT, m_chuckNotInspArea);
	DDV_MinMaxInt(pDX, m_chuckNotInspArea, 0, 100);
	DDX_Text(pDX, IDC_ED_IMUL_CHUCKLR, m_nImulMidToStopper);
	DDV_MinMaxInt(pDX, m_nImulMidToStopper, 0, 100);
	DDX_Text(pDX, IDC_ED_IMUL_CHUCKSTOP, m_nImulChuckToStopper);
	DDV_MinMaxInt(pDX, m_nImulChuckToStopper, 0, 100);
	DDX_Text(pDX, IDC_ED_IMUL_INSERT_HEIGHT, m_nImulInsertHeight);
	DDV_MinMaxInt(pDX, m_nImulInsertHeight, -50, 300);
	DDX_Text(pDX, IDC_ED_IMUL_INSERTNOINSPECT, m_nImulInsertNoInsp);
	DDV_MinMaxInt(pDX, m_nImulInsertNoInsp, -50, 300);
	DDX_Text(pDX, IDC_ED_IMUL_INSERTUPDOWN, m_nImulInsertUpDown);
	DDV_MinMaxInt(pDX, m_nImulInsertUpDown, -50, 300);
	DDX_Text(pDX, IDC_ED_IMUL_JABARA, m_nImulJabaraNoInsp);
	DDV_MinMaxInt(pDX, m_nImulJabaraNoInsp, -50, 300);
	DDX_Text(pDX, IDC_ED_IMUL_MAGURI, m_nImulMaguriNoInsp);
	DDV_MinMaxInt(pDX, m_nImulMaguriNoInsp, -50, 300);
	DDX_Text(pDX, IDC_ED_IMUL_UPDOWN, m_transRatio);
	//DDV_MinMaxInt(pDX, m_nImulUpDownNoInsp, -50, 300);
	DDV_MinMaxInt(pDX, m_transRatio, 0, 100);
	DDX_Text(pDX, IDC_ED_ETC_CAMERAGAIN, m_fCameraGain);
	DDV_MinMaxFloat(pDX, m_fCameraGain, -6.25f, 100.f);
	DDX_Text(pDX, IDC_ED_ETC_CAMSHUTTER, m_fCameraShutter);
	DDV_MinMaxFloat(pDX, m_fCameraShutter, 0.f, 100.f);
	DDX_Text(pDX, IDC_ED_ETC_CHUCKAVG, m_fChuckPokAverage);
	DDV_MinMaxFloat(pDX, m_fChuckPokAverage, 0.f, 100.f);
	DDX_Text(pDX, IDC_ED_ETC_CHUCKDOWNSCALE, m_fChuckDownScale);
	DDV_MinMaxFloat(pDX, m_fChuckDownScale, 0.f, 100.f);
	DDX_Text(pDX, IDC_ED_ETC_CHUCKUPSCALE, m_fChuckUpScale);
	DDV_MinMaxFloat(pDX, m_fChuckUpScale, 0.f, 100.f);
	DDX_Text(pDX, IDC_ED_ETC_IMGCENTER, m_nImageCenter);
	DDV_MinMaxInt(pDX, m_nImageCenter, -500, 500);
	DDX_Text(pDX, IDC_ED_THRES_TRANSINSERT, m_nTransInsertThres);
	DDV_MinMaxInt(pDX, m_nTransInsertThres, 0, 255);
	DDX_Text(pDX, IDC_ED_THRES_TRANSMAGURI, m_nTransMaguriThres);
	DDV_MinMaxInt(pDX, m_nTransMaguriThres, 0, 255);
	DDX_Check(pDX, IDC_CHK_USETRIGGER14, m_bUse14Trigger);
	DDX_Text(pDX, IDC_ED_IMUL_CHUCKWIDTH_LIMIT, m_nImulReduce);
	DDV_MinMaxInt(pDX, m_nImulReduce, 0, 100);
	DDX_Text(pDX, IDC_ED_COLORSTRAW_THRES01, m_nColorThres01);
	DDV_MinMaxInt(pDX, m_nColorThres01, 0, 255);
	DDX_Text(pDX, IDC_ED_COLORSTRAW_THRES02, m_nColorThres02);
	DDV_MinMaxInt(pDX, m_nColorThres02, 0, 255);
	DDX_Text(pDX, IDC_ED_COLORSTRAW_THRES03, m_nColorThres03);
	DDV_MinMaxInt(pDX, m_nColorThres03, 0, 255);
	DDX_Radio(pDX, IDC_RADIO_TRI14, m_nTriChangeMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CINSParam, CDialog)
	//{{AFX_MSG_MAP(CINSParam)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBtnApply)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CINSParam message handlers

BOOL CINSParam::Create(CWnd* pParentWnd) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pPreDlg = (CPreInspection*) pParentWnd;
	return CDialog::Create(IDD, pParentWnd);
}

void CINSParam::OnBtnApply() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	INSP_PARAM ParamData;
	ParamData.nData[0]	= m_nMainThres;
	ParamData.nData[1]	= m_nMaguriThres;
	ParamData.nData[2]	= m_nChuckThres;
	ParamData.nData[3]	= m_nAngleThres;
	ParamData.nData[4]	= m_nSatuthres;
	ParamData.nData[5]	= m_nTransMaguriThres;
	ParamData.nData[21]	= m_nTransInsertThres;

	// 에티켓
	ParamData.nData[6]	= m_midNotInspArea;
	ParamData.nData[7]	= m_chuckNotInspArea;
	ParamData.nData[8]	= m_maguriSideDepth;
	ParamData.nData[9]	= m_nEtqtoStopper;
	ParamData.nData[10]	= m_nEtqInspArea;
	ParamData.nData[11]	= m_nEtqLength;
	// 이물
	ParamData.nData[12]	= m_nImulMaguriNoInsp;
	ParamData.nData[13]	= m_nImulJabaraNoInsp;
	ParamData.nData[14]	= m_nImulChuckToStopper;
	ParamData.nData[15]	= m_nImulInsertNoInsp;
	ParamData.nData[16]	= m_nImulMidToStopper;
	ParamData.nData[17]	= m_transRatio;
	ParamData.nData[18]	= m_nImulInsertUpDown;
	ParamData.nData[19]	= m_nImulInsertHeight;
	// 기타 파라미터.
	ParamData.nData[20]	= m_nImageCenter;
	ParamData.nData[22]	= m_bUse14Trigger;
	ParamData.nData[24]	= m_nTriChangeMode;

	// 축관 폭 제한 수치. 추가 20090817
	ParamData.nData[23]	= m_nImulReduce;
	
	ParamData.fData[0]	= m_chuckLeft;				// 에티켓 유무 검사 위한 표준편차.
	ParamData.fData[1]	= m_fCameraGain;
	ParamData.fData[2]	= m_fChuckPokAverage;
	ParamData.fData[3]	= m_fCameraShutter;
	ParamData.fData[4]	= m_fChuckUpScale;
	ParamData.fData[5]	= m_fChuckDownScale;

	// 색 빨대 경우 Threshold 수치 적용.
	ParamData.nData[27] = m_nColorThres01;
	ParamData.nData[28] = m_nColorThres02;
	ParamData.nData[29] = m_nColorThres03;


	m_pPreDlg->WriteParam(ParamData);
}

void CINSParam::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_HIDE);
}

BOOL CINSParam::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( pMsg->message){
		case WM_KEYDOWN:
		{
			int s;
			s = (int)pMsg->wParam;
			if (s==VK_RETURN)
			{
				return true;
			}
			else if (s==VK_ESCAPE)
			{
				return true;
			}
			else if (s==VK_TAB)
			{
				return true;
			}
			else if (s==VK_SPACE)
			{
				//OnTest();
				return true;
			}
		}
    }
	return CDialog::PreTranslateMessage(pMsg);
}
