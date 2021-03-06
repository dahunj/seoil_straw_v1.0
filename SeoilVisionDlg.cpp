// SeoilVisionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SeoilVision.h"

#include "FlyCapture2.h"
#include "FlyCapture2GUI.h"

#include "SeoilVisionDlg.h"

#include "cxcore.h"
#include "cv.h"

#include "InspResult.h"
#include "SetColorDlg.h"
#include "RPMSettingDlg.h"
#include "ErrorListDlg.h"

#include "AMCControl.h"
#include "EIPControl.h"
#include "PLCControl.h"
#include "RS232Control.h"

#include "mmcdll.h"

#include <afxdlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

#include "Global.h"

#define IMAGE_MAIN_STRAW	_T("c:\\ATI\\Seoil_MainImage.bmp")
#define IMAGE_MAIN_T_P		_T("c:\\ATI\\T_P_Straw.bmp")

//#include "FlyCapture2.h"

using namespace FlyCapture2;
using namespace std;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers


	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeoilVisionDlg dialog

CSeoilVisionDlg::CSeoilVisionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSeoilVisionDlg::IDD, pParent)
{
	if (_CAM_NUM==0)
	{
		m_hIcon = AfxGetApp()->LoadIcon(IDI_ICO_MAIN1);
	}
	else if(_CAM_NUM==1)
		m_hIcon = AfxGetApp()->LoadIcon(IDI_ICO_MAIN2);
	
	m_picture = NULL;
	m_bTestSaveImage = FALSE;
	m_bTestSavePartImage = FALSE;
	m_save = FALSE;
	m_inspect = FALSE;
	m_view = FALSE;
	m_bShowMain = TRUE;
	m_camnum = 0;
	m_nMainImage=0;
	m_strIO="";
	memset(m_lineIO,0,sizeof(char)*8);
	m_ioType = -1;

	m_isLoaded = false;

	for (int i=0; i<_MAX_BUTTONS; i++)	{
		m_btn[i] = NULL;
	}
	m_pFont = NULL;
	m_pFont1 = NULL;
}

void CSeoilVisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSeoilVisionDlg)
	DDX_Control(pDX, IDC_RICHED_RESULT, m_ctrREditResult);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSeoilVisionDlg, CDialog)
	//{{AFX_MSG_MAP(CSeoilVisionDlg)
	ON_CONTROL_RANGE(BN_CLICKED,_EZ_BTN_ID_FIRST, _EZ_BTN_ID_END, OnMainBtnClick)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDM_INSP_SETTING, OnInspSetting)
	ON_COMMAND(IDM_INSP_START, OnInspStart)
	ON_COMMAND(IDM_CAM_SETTING, OnCamSetting)
	ON_COMMAND(IDM_CAMERA_SET2, OnCameraSet2)
	ON_COMMAND(IDM_INSP_RESULT, OnInspResult)
	ON_COMMAND(IDM_INSP_SAVE, OnInspSave)
	ON_UPDATE_COMMAND_UI(IDM_INSP_SAVE, OnUpdateInspSave)
	ON_UPDATE_COMMAND_UI(IDM_INSP_START, OnUpdateInspStart)
	ON_COMMAND(IDM_INSP_VIEW, OnInspView)
	ON_UPDATE_COMMAND_UI(IDM_INSP_VIEW, OnUpdateInspView)
	ON_WM_TIMER()
	ON_COMMAND(IDM_TEST_SAVEIMAGE, OnTestSaveimage)
	ON_COMMAND(IDM_FILE_OPEN, OnFileOpen)
	ON_COMMAND(IDM_TEST_SAVEGRAB_IMAGE, OnTestSavegrabImage)
	ON_COMMAND(IDM_FILE_PARAMETER_NEWER, OnFileParameterNewer)
	ON_COMMAND(IDM_INSP_SETCOLOR, OnInspSetcolor)
	ON_WM_DESTROY()
	ON_COMMAND(IDM_TEST_SAVEREJECTIMAGE, OnTestSaverejectimage)
	ON_COMMAND(IDM_TEST_TW_RED, OnTestTwRed)
	ON_COMMAND(IDM_TEST_TW_YELLOW, OnTestTwYellow)
	ON_COMMAND(IDM_TEST_TW_GREEN, OnTestTwGreen)
	ON_MESSAGE(UM_RESTART,OnRestart)
	
	ON_WM_ERASEBKGND()
	
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TEST_TEST, &CSeoilVisionDlg::OnTestTest)
	ON_COMMAND(IDM_RPM, &CSeoilVisionDlg::OnRpm)
	ON_COMMAND(IDM_SOL_ON, &CSeoilVisionDlg::OnSolOn)
	ON_COMMAND(ID_SOL_OFF, &CSeoilVisionDlg::OnSolOff)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeoilVisionDlg message handlers

BOOL CSeoilVisionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	m_mainInspView = NULL;
	m_numOfInspView = 0;

	// 크기 조절
	RECT windowRect;
	::GetWindowRect(::GetDesktopWindow(), &windowRect);

	int nReduceSize = 210; // default: 210
	
	int clientWidth = 920;
	int clientHeight = 790;

	int sx = windowRect.right / 2 - clientWidth / 2;
	int sy = windowRect.bottom / 2 - clientHeight / 2;

	SetWindowPos(&wndTop, sx, sy, clientWidth, clientHeight, NULL);
	// 920 x 790
	CRect size;

	size.left = 10;
	size.right = SHOW_MAIN_WIDTH + size.left; // 640 + 10
	size.top = 10;
	size.bottom = SHOW_MAIN_HEIGHT + size.top; // 512 + 10
	
	m_mainView.Create(NULL, WS_VISIBLE, size, this, this->m_nFlags);
	
	GetDlgItem(IDC_RICHED_RESULT)->MoveWindow(670, 50, 220, 500);
	
	// 버튼 생성
	int i=0;

	for (i=0;i<_MAX_BUTTONS;i++)
	{
		m_btn[i] = new CButton;
	}
	
	i=0;
	m_btn[0]->Create(_T("검사 설정(設定參數)"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  CRect(0,0,0,0),this, _EZ_BTN_ID_FIRST + i++);
	m_btn[1]->Create(_T("검사 시작(檢測開始)"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  CRect(0,0,0,0),this, _EZ_BTN_ID_FIRST + i++);
	m_btn[2]->Create(_T("불량 내역(缺陷統計)"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  CRect(0,0,0,0),this, _EZ_BTN_ID_FIRST + i++);
	m_btn[3]->Create(_T("2번 검사 설정"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  CRect(0,0,0,0),this, _EZ_BTN_ID_FIRST + i++);
	m_btn[4]->Create(_T("빈 빨대 불기"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  CRect(0,0,0,0),this, _EZ_BTN_ID_FIRST + i++);
	m_btn[5]->Create(_T("정보 초기화(信息格式化)"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,  CRect(0,0,0,0),this, _EZ_BTN_ID_FIRST + i++);

	m_pFont = new CFont;
	m_pFont->CreatePointFont(100, "Garamond");

	m_pFont1 = new CFont;
	m_pFont1->CreatePointFont(150,"Arial");

	m_btn[0]->SetFont(m_pFont);
	m_btn[1]->SetFont(m_pFont);
	m_btn[2]->SetFont(m_pFont);
	m_btn[3]->SetFont(m_pFont);
	m_btn[4]->SetFont(m_pFont);
	m_btn[5]->SetFont(m_pFont);

	
	m_btn[0]->MoveWindow(10,  800 - nReduceSize - 55, 140, 50);	// 검사 설정.
	m_btn[5]->MoveWindow(170, 800 - nReduceSize - 55, 140, 50);	// 정보 초기화.
	m_btn[1]->MoveWindow(330, 800 - nReduceSize - 55, 140, 50);	// 검사시작.
	m_btn[2]->MoveWindow(490, 800 - nReduceSize - 55, 140, 50);	// 불량 내역.
	m_btn[3]->MoveWindow(130, 800 - nReduceSize - 55, 100, 50);	//////////////////
	m_btn[4]->MoveWindow(490, 800 - nReduceSize - 55, 100, 50);	// 빈빨대 불기(없앰)

	m_btn[3]->ShowWindow(SW_HIDE); m_btn[3]->EnableWindow(FALSE);
	m_btn[4]->ShowWindow(SW_HIDE); m_btn[4]->EnableWindow(FALSE);

	// 빈 그림 그려주기
	IplImage *temp = cvCreateImage(
		cvSize( 640, 522 ),
		8,
		3);
	m_show = cvCreateImage(
		cvSize( 640, 522 ),
		8,
		3);

	CString str;
	char line[128];
	str = IMAGE_MAIN_STRAW;
	sprintf(line,str);
	
	temp = cvLoadImage(line, CV_LOAD_IMAGE_COLOR);
	m_show = cvCloneImage(temp);
	
	cvFlip(m_show, m_show, 0);
	m_mainView.DrawImage(
		m_show, 
		SHOW_MAIN_WIDTH, 
		SHOW_MAIN_HEIGHT + 10);

	cvReleaseImage(&temp);
	SetTimer(10,5000,NULL);		// BHJ 20081211 MainImage Showing.

	nReduceSize+=40;
	m_status.Create("검사 준비중(在準備檢測)...",WS_CHILD | WS_VISIBLE | WS_EX_LEFT   | SS_CENTERIMAGE | SS_SUNKEN
		, CRect(0,0,0,0), this,_EZ_BTN_ID_FIRST+100);
	m_status.SetFont(m_pFont1);
	m_status.MoveWindow(10,800-nReduceSize + 40,650-nReduceSize/10,30);//
	
	m_status1.Create("",WS_CHILD | WS_VISIBLE | WS_EX_LEFT   | SS_CENTERIMAGE | SS_SUNKEN
		, CRect(0,0,0,0), this,_EZ_BTN_ID_FIRST+101);
	m_status1.SetFont(m_pFont1);
	m_status1.MoveWindow(10,840-nReduceSize + 40,650-nReduceSize/10,30);//

	m_status2.Create("",WS_CHILD | WS_VISIBLE | WS_EX_LEFT   | SS_CENTERIMAGE | SS_SUNKEN
		, CRect(0,0,0,0), this,_EZ_BTN_ID_FIRST+104);
	m_status2.SetFont(m_pFont1);
	m_status2.MoveWindow(10,880-nReduceSize + 40,650-nReduceSize/10,30);//

	m_status3.Create("",WS_CHILD | WS_VISIBLE | WS_EX_LEFT   | SS_CENTERIMAGE | SS_SUNKEN
		, CRect(0,0,0,0), this,_EZ_BTN_ID_FIRST+101);
	m_status3.SetFont(m_pFont1);
	m_status3.MoveWindow(10,920-nReduceSize + 40, 815 ,30);//

//	m_time.Create("경과 시간(經過時間) 00:00:00",WS_CHILD | WS_VISIBLE |SS_CENTER   | SS_CENTERIMAGE | SS_SUNKEN
//		, CRect(0,0,0,0), this,_EZ_BTN_ID_FIRST+102);
	m_time.Create("경과 시간 00:00:00",WS_CHILD | WS_VISIBLE |SS_CENTER   | SS_CENTERIMAGE | SS_SUNKEN
		, CRect(0,0,0,0), this,_EZ_BTN_ID_FIRST+102);
	m_time.SetFont(m_pFont1);
	m_time.MoveWindow(660-nReduceSize/10,800-nReduceSize+ 40,184,70);

	// Main Text 저장.
	CString wtstr = _T("Seoil EzVision");

	SetWindowText(wtstr);
	m_sMainText = wtstr;
	m_sFileText ="";

	m_trans = m_colorstraw = m_tepe = m_nSetRPM = m_SolIO = m_SolTime = m_EmptySolGap = 0;

	// 색칼라빨대 여부 읽어오기
	FILE *setfile2 = fopen("c:\\ATI\\Setting2.txt","r");
	fscanf(setfile2, "Transparency		%d\n", &m_trans);
	fscanf(setfile2, "IsColorStraw		%d\n", &m_colorstraw);
	fscanf(setfile2, "IsTeOrPe			%d\n", &m_tepe);
	fscanf(setfile2, "RPM				%d\n", &m_nSetRPM);
	fscanf(setfile2, "Sol_IO			%d\n", &m_SolIO);
	fscanf(setfile2, "Sol_Time(ms)		%d\n", &m_SolTime);
	fscanf(setfile2, "Empty_Sol_Gap		%d\n", &m_EmptySolGap);
	fscanf(setfile2, "IO_Type			%d\n", &m_ioType);

	if(m_ioType != -1)
	{
		if(m_ioType == 0) // AMC
		{
			m_pCtrlIO = new CAMCControl;
		}
		else if(m_ioType == 1) // EIP
		{
			m_pCtrlIO = new CEIPControl;
		}
		else if(m_ioType == 2) // PLC
		{
			m_pCtrlIO = new CPLCControl;
			((CPLCControl*)m_pCtrlIO)->SetRPM(m_nSetRPM);
			((CPLCControl*)m_pCtrlIO)->SetSol(m_SolIO);
		}
		else if(m_ioType == 3) // RS232
		{
			m_pCtrlIO = new RS232Control;
		}
		else
		{
			AfxMessageBox(_T("올바른 I/O로 설정해주세요!\nC:/ATI/Setting2.txt\nAMC : 0\nEIP: 1\nPLC: 2"));
			return FALSE;
		}
	}
	else
	{
		m_pCtrlIO = new CEIPControl;
		AfxMessageBox(_T("Setting2.txt에 I/O 코드를 설정해주세요!(請在Setting2.txt文檔裏設定I/O編號)\nC:/ATI/Setting2.txt\nAMC : 0\nEIP: 1\nPLC: 2"));
		return FALSE;
	}

	m_bUseInspEtq = FALSE;

//	if(m_nUseInspEtq<0 || m_nUseInspEtq>1) m_nUseInspEtq=0;

	fclose(setfile2);
	if (m_SolTime <= 0 || m_SolTime > 1000) 
		AfxMessageBox(_T("C:\\ATI\\Setting2.txt Sol_Time설정 부분이 잘못 되었습니다.  0~1000 사이로 설정하세요!. "));
	
	FILE *color = fopen("c:\\ATI\\colorthres.txt","r");
	for (i=0;i<3;i++)
		fscanf(color,"%d",&m_colorthres[i]);
	fclose(color);

	
	m_monitor.SetDetector("SeoilErrorDetector");
	m_restarted = false;

	// 완료 다 되면 나중에 읽기
	SetTimer(1234,1000,NULL);

	if ( !(m_pCtrlIO->InitBoard()) )
	{
		AfxMessageBox( _T("I/O 초기화 실패! 상태를 확인하십시오.") );
	}
	else
	{
		m_pCtrlIO->TurnOnLamp();
	}
	
	//if (m_ctrlIO.InitBoard())
	//{
	//	AfxMessageBox(_T("I/O 초기화 성공!"));
	//}
	//else
	//{
	//	AfxMessageBox(_T("I/O 초기화 실패!"));
	//}
	

	m_bSolOnOff = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSeoilVisionDlg::InitGetPicture(INSPECT_SETTING *pInsp)
{
	if(m_picture != NULL)
	{
		delete m_picture;
	}

	// MAXROI는 여기서 대입
	this->SetGlobalVariable(pInsp);

	m_picture = new CGetPicture;

	if ( m_picture->InitAll() != true )
	{
		delete m_picture;

		return;
	}
	
	m_picture->m_tepe = m_tepe;
	
	m_picture->SetColorInfo(
		m_trans, 
		m_colorstraw, 
		m_colorthres[0], 
		m_colorthres[1], 
		m_colorthres[2], 
		m_bUseInspEtq );
	
	// 파라미터 갱신.(빨대 타입별로 파라미터 갱신함.)
	OnFileParameterNewer();

	m_picture->m_pre->SetPointer(this);
}

BOOL CSeoilVisionDlg::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

void CSeoilVisionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ( nID == SC_CLOSE )
	{
		if ( m_picture != NULL )
		{
			if ( m_inspect_image == true )
			{
				m_picture->StrawInspectionStop();
			}
		}

		CDialog::OnSysCommand( nID, lParam );
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSeoilVisionDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	if(!m_inspect_image)
	{
		CDC *pDC;
		pDC=GetDC();
		long x,y;

		COLORREF rgb;
		
		CString str;
		UINT nFormat  = DT_LEFT|DT_SINGLELINE|DT_VCENTER;
		CRect rect(710, 15, 910, 45);

		if ( m_bSolOnOff == true )
		{
			str = _T("솔벨브 켜짐(Sol Valve On)");
			rgb = RGB( 50, 255, 50 );
		}
		else
		{
			str = _T("솔벨브 꺼짐(Sol Valve Off)");
			rgb = RGB( 255, 50, 50 );
		}

		for ( y = 25; y < 35; y++ )
		{
			for ( x = 690; x < 700; x++ )
			{
				pDC->SetPixel( x, y, rgb );
			}
		}

		pDC->DrawText( str, rect, nFormat );
		
		ReleaseDC(pDC);
	}
	CDialog::OnPaint();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSeoilVisionDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSeoilVisionDlg::OnMainBtnClick(UINT nID)
{
	//if ( m_isLoaded == false )
	//{
	//	return;
	//}

	CString str="1234";
	// ID 따라 판단
	switch(nID)
	{
	// 검사 설정
	case _EZ_BTN_ID_FIRST:
		if ( m_isLoaded == false )
		{
			return;
		}
		OnInspSetting();		
		break;

	// 검사 시작
	case _EZ_BTN_ID_FIRST+1:
		if ( m_isLoaded == false )
		{
			return;
		}

		m_picture->inTime = COleDateTime::GetCurrentTime();
		m_picture->m_sday = m_picture->inTime.Format(_T("%d"));
		m_picture->m_shour = m_picture->inTime.Format(_T("%H"));
		m_picture->m_sminute = m_picture->inTime.Format(_T("%M"));
		m_picture->m_ssecond = m_picture->inTime.Format(_T("%S"));
	
		StartInspection();
		break;

	// 검사 결과
	case _EZ_BTN_ID_FIRST+2:		
		OnInspResult();
		break;

	case _EZ_BTN_ID_FIRST+3:
		if ( m_isLoaded == false )
		{
			return;
		}
		break;

	case _EZ_BTN_ID_FIRST+4:
		if ( m_isLoaded == false )
		{
			return;
		}
		OnNoStrawSetting();
		break;

	case _EZ_BTN_ID_FIRST+5:
		if ( m_isLoaded == false )
		{
			return;
		}
		m_picture->StatusReset();
		m_ctrREditResult.SetSel( 0, -1 );
		m_ctrREditResult.ReplaceSel( _T(" ") );
		m_ctrREditResult.Clear();
	}

}

void CSeoilVisionDlg::ReadFromReportFile(FILE *fp)
{
	INSPECT_RESULT_COUNT record;
	fread(&record, sizeof(INSPECT_RESULT_COUNT), 1, fp);
	m_inspect_result += record;

	fseek(fp, 0, SEEK_SET);
}

void CSeoilVisionDlg::WriteToReportFile(FILE *fp)
{
	fwrite(&m_inspect_result, sizeof(INSPECT_RESULT_COUNT), 1, fp);
}

void CSeoilVisionDlg::RecordRejectInfo()
{
	CString filePath;
	COleDateTime currentTime = COleDateTime::GetCurrentTime();
	filePath.Format(REPORT_PATH, currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay());
	FILE *fp = fopen(filePath.GetBuffer(), "r+b");

	if(fp == NULL)
	{
		fp = fopen(filePath.GetBuffer(), "wb");
		WriteToReportFile(fp);
	}
	else
	{
		ReadFromReportFile(fp);
		WriteToReportFile(fp);
	}

	fclose(fp);

	// reset 부분 잘 봐야 함
	memset(&m_inspect_result, 0, sizeof(INSPECT_RESULT_COUNT));
}

void CSeoilVisionDlg::StartInspection()
{
	if(!m_isLoaded)
		return;

	m_picture->SetInspectionDlg((LPVOID*)(this));

	if (!m_inspect_image)
	{
		// 글자 바꾸기

		if (m_restarted)
			m_picture->m_restart = 1;
		else
			m_picture->m_restart = 0;
		
		if(m_picture->StrawInspectionStart())
		{	
			DisableMenu();
			m_pCtrlIO->TurnOffLamp();
			m_pCtrlIO->TurnOnGreenLamp();

			m_btn[1]->SetWindowText("검사 중지");
			m_btn[2]->EnableWindow(FALSE);
			m_inspect_image = !m_inspect_image;

			m_mainView.ShowWindow(SW_HIDE);

			for(int tmpI = 0; tmpI < m_numOfInspView; tmpI++)
				m_mainInspView[tmpI].ShowWindow(SW_SHOW);

			m_view = TRUE;
			m_picture->m_view = TRUE;
		
			KillTimer(10);		// BHJ 20081211 MainImage Showing.
			m_bShowMain = false;
			

			if(m_restarted == false){
				m_monitor.WriteFullFilename(m_filename);
				m_monitor.WritePathName(m_pathname);
				m_monitor.WriteFileName(m_file_name);
			}
			
			m_monitor.SayStart();

			// 저장용 활성화
			// 0.5초에 1회
			//SetTimer(1236,500,NULL);
		}
	}
	else
	{
		EnableMenu();
		m_pCtrlIO->TurnOnLamp();

		///////////////////////////////////////////////
		RecordRejectInfo();

		m_picture->m_restart = 0;
		m_btn[1]->SetWindowText("검사 시작");
		m_picture->StrawInspectionStop();
		m_btn[2]->EnableWindow(TRUE);
		m_inspect_image = !m_inspect_image;

		m_mainView.ShowWindow(SW_SHOW);

		for(int tIdx = 0; tIdx < m_numOfInspView; tIdx++)
			m_mainInspView[tIdx].ShowWindow(SW_HIDE);

		// 초기화
		delete [] m_mainInspView;
		m_mainInspView = NULL;
		m_numOfInspView = 0;

		CFile file;
		CString strfile, strTemp, strTemp2;
		CFileException e;
		SYSTEMTIME time;

		// 경과시간 재기
		GetLocalTime(&time);

		ULONG  uTotalTime = m_totalTime ;
		UINT hour, min, sec;
		hour = uTotalTime / 3600;
		min = (uTotalTime % 3600) / 60;
		sec = uTotalTime % 60;
		/*
		strfile.Format("c:\\ATI\\Report\\%4d%2d%2d%0d%0d%0d_Straw_Inspection.txt", time.wYear , time.wMonth , time.wDay ,time.wHour , time.wMinute , time.wSecond);
		if ( !file.Open((LPCTSTR)strfile, CFile::modeCreate | CFile::modeReadWrite, &e)) {
			e.ReportError();
			return;
		}
	
		CArchive ar(&file, CArchive::store);

		// 검사일시 출력
		strTemp.Format("검사일시 : %4d년 %2d월 %2d일 %2d:%2d:%2d\r\n", time.wYear , time.wMonth , time.wDay , time.wHour , time.wMinute , time.wSecond);
		ar.Write(strTemp,strTemp.GetLength());

		// 소요시간 출력

		strTemp.Format ( 
			"\r\n"
			" 불량률 : %3.2f %% \r\n"
			" 검사 시간 : %02d 시 %02d 분 %02d 초\r\n",
			(float)(m_inspect_result.ng) /	(float)(m_inspect_result.ng+m_inspect_result.ok) * 100  ,
			hour, min, sec );
		
		ar.Write(strTemp , strTemp.GetLength() );


		// 총 갯수 저장
		strTemp.Format("\r\n\r\n총 검사 갯수 : %10ld개\r\n양품 갯수 : %10ld개\r\n불량 갯수 : %10ld개",m_inspect_result.ok+m_inspect_result.ng,m_inspect_result.ok,m_inspect_result.ng);

		//불량률 저장

		ar.Write(strTemp,strTemp.GetLength());

		// 각 유형별 저장
		
		strTemp2.Format("\r\n\r\n유형 별 불량 갯수\r\n폭 불량 : %10ld\r\n각 불량 : %10ld\r\n마구리 불량 : %10ld\r\n자바라 불량 : %10ld\r\n축관 불량 : %10ld\r\n역삽입 불량 : %10ld\r\n이물질 불량 : %10ld",
			m_inspect_result.ng_pok,m_inspect_result.ng_angle,m_inspect_result.ng_maguri,
			m_inspect_result.ng_jabara,m_inspect_result.ng_chuck,m_inspect_result.ng_insert,m_inspect_result.ng_imul);

		ar.Write(strTemp2,strTemp2.GetLength());


		ar.Close();							 	


		file.Close();
		*/
		SetTimer(10,5000, NULL);
		m_nMainImage=0;
		
		m_monitor.SayEnd();

		// 끝 쓰기

		// 저장용 죽이기
		//KillTimer(1236);
		
	}
}	

void CSeoilVisionDlg::EraseBkgnd()
{
	CWnd::OnEraseBkgnd(this->GetDC());
}

void CSeoilVisionDlg::OnInspSetting() 
{
	if(_MAXROI <= 0)
		return;

	if(m_inspect_image)
	{
		// Disable
		m_picture->m_pre->m_btnLoadDB.EnableWindow(FALSE);
		m_picture->m_pre->m_btnSaveDB.EnableWindow(FALSE);
	}
	else
	{
		// Enable
		m_picture->m_pre->m_btnLoadDB.EnableWindow(TRUE);
		m_picture->m_pre->m_btnSaveDB.EnableWindow(TRUE);
	}

	// 검사 설정
	for(int i=0; i<_MAXROI; i++) 
		m_picture->m_pre->m_imageView.m_bExistObj[i] = TRUE;

	m_picture->m_pre->m_bViewROI01=TRUE;
	m_picture->m_pre->m_bViewROI02=TRUE;
	m_picture->m_pre->m_bViewROI03=TRUE;
	m_picture->m_pre->m_bViewROI04=TRUE;
	m_picture->m_pre->m_bViewROI05=TRUE;
	m_picture->m_pre->ShowWindow(SW_SHOW);
}

void CSeoilVisionDlg::OnInspStart() 
{
	// TODO: Add your command handler code here
	StartInspection();
	m_inspect = !m_inspect;
	if(m_inspect) {
		KillTimer(10);		// BHJ 20081211 MainImage Showing.
		m_bShowMain=false;
		if(MODEL_16==0){
			if (m_restarted)
				SetTimer(1235,500,NULL);
		}
	}
	else {
		SetTimer(10,5000, NULL);
		m_nMainImage=0;
	}
	
}

void CSeoilVisionDlg::OnCamSetting() 
{
	// 기존
	//m_picture->pCamera[0]->ViewCamSetting();

	if ( m_isLoaded != true )
	{
		AfxMessageBox( _T("설정 파일을 먼저 불러와주세요!(請先打開設定文件)") );
		return;
	}

	LARGE_INTEGER startingTime, endingTime, elapsedMilliseconds;
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency( &frequency );

	QueryPerformanceCounter( &startingTime );

	FlyCapture2::CameraControlDlg camCtrlDlg;

	QueryPerformanceCounter( &endingTime );

	elapsedMilliseconds.QuadPart = endingTime.QuadPart - startingTime.QuadPart;
	elapsedMilliseconds.QuadPart *= 1000;
	elapsedMilliseconds.QuadPart /= frequency.QuadPart;

	TRACE( _T("Create camCtrlDlg: %d ms\n"), elapsedMilliseconds.QuadPart );

	if ( m_picture->m_cam.IsConnected() == false )
	{
		AfxMessageBox( _T("카메라가 연결되지 않았습니다.(Camera is Not Conect)") );
		return;
	}
}

void CSeoilVisionDlg::OnCameraSet2() 
{
	//m_picture->pCamera[0]->ViewCamSetting();	
	if (m_bShowMain)
	{
		KillTimer(10);
		m_bShowMain = false;
	}else
	{
		SetTimer(10, 5000, NULL);
		m_bShowMain = true;
	}
}

void CSeoilVisionDlg::OnInspResult() 
{
	MakeReport();
}

void CSeoilVisionDlg::UpdateStatus(ULONG ok, ULONG ng, ULONG angle, ULONG pok, ULONG maguri, ULONG jabara, ULONG chuck, ULONG insert, ULONG imul, 
											  UINT imul_top, UINT imul_mid, UINT imul_jabara, UINT imul_chuck, UINT imul_insert, ULONG etiquette)
{

	CString m_str, m_str1, m_str2, m_str3;
	m_str.Format("정품 : %6ld     총 불량 : %6ld     상단각 불량 : %6ld",ok,ng,angle);
	m_str1.Format("중단각 불량 : %6ld     마구리 불량 : %6ld     자바라 불량 : %6ld",pok, maguri, jabara);
	m_str2.Format("축관 불량 : %6ld     삽입 불량 : %6ld     에티켓 불량 : %6ld",chuck, insert, etiquette);
	m_str3.Format(_T("이물 불량: %6ld     상단 : %4ld     자바라: %4ld     중단 : %4ld     축관 : %4ld     내측 : %4ld"), imul, imul_top, imul_jabara, imul_mid, imul_chuck, imul_insert);
	
	m_status.SetWindowText(m_str);
	m_status1.SetWindowText(m_str1);
	m_status2.SetWindowText(m_str2);
	m_status3.SetWindowText(m_str3);

	m_inspect_result.ok = ok;
	m_inspect_result.ng = ng;
	m_inspect_result.ng_angle = angle;
	m_inspect_result.ng_chuck = chuck;
	m_inspect_result.ng_imul = imul;
	m_inspect_result.ng_imul_top = imul_top;
	m_inspect_result.ng_imul_mid = imul_mid;
	m_inspect_result.ng_imul_jabara = imul_jabara;
	m_inspect_result.ng_imul_chuck = imul_chuck;
	m_inspect_result.ng_imul_insert = imul_insert;
	m_inspect_result.ng_insert = insert;
	m_inspect_result.ng_jabara = jabara;
	m_inspect_result.ng_maguri = maguri;
	m_inspect_result.ng_pok = pok;
	m_inspect_result.ng_etiquette = etiquette;
}

void CSeoilVisionDlg::UpdateTime(UINT hour, UINT min, UINT sec)
{
	CString m_str;
	if (m_restarted == false)
//		m_str.Format("경과 시간(經過時間) %2d:%2d:%2d",hour,min,sec);
		m_str.Format("경과 시간 %2d:%2d:%2d",hour,min,sec);
	else
		m_str.Format("재시작됨!! %2d:%2d:%2d",hour,min,sec);
	
	m_time.SetWindowText(m_str);
}

void CSeoilVisionDlg::OnInspSave() 
{
	if ( m_isLoaded == false )
	{
		AfxMessageBox( _T("설정 파일을 먼저 불러오십시오!") );
		return;
	}

	if(!m_save)
	{
		//m_save = TRUE;
		if(!m_picture->SaveRefImageStart())
		{
			AfxMessageBox("참조 영상 저장 실패!(保存影像失敗)");
		}
		else
		{
			m_save = !m_save;			
		}
		//m_save = TRUE;
	}
	else
	{
		if(!m_picture->SaveRefImageStop())
		{
			AfxMessageBox("참조 영상 저장 중지 실패!");
		}
		else
		{
			m_save = !m_save;
		}
	}
}

void CSeoilVisionDlg::OnUpdateInspSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (m_save)
		pCmdUI->SetText("참조 이미지 취득 중지");
	else
		pCmdUI->SetText("참조용 이미지 취득(100장)");
}

void CSeoilVisionDlg::MakeReport()
{
	CErrorListDlg dlgErrorList;

	dlgErrorList.DoModal();
	/*
	CInspResult m_dlg;

	CString result;
	result.Format("빨대 검사 결과\r\n\r\n"
		"양품 : %6ld\r\n"
		"총 불량품 : %6ld\r\n"
		"각도 불량 : %6ld\r\n"
		"폭 불량 : %6ld\r\n"
		"마구리 불량 : %6ld\r\n"
		"자바라 불량 : %6ld\r\n"
		"축관 불량 : %6ld\r\n"
		"삽입 불량 : %6ld\r\n"
		"이물질 불량 : %6ld\r\n",m_inspect_result.ok,m_inspect_result.ng,m_inspect_result.ng_angle,m_inspect_result.ng_pok
		,m_inspect_result.ng_maguri,m_inspect_result.ng_jabara,m_inspect_result.ng_chuck,m_inspect_result.ng_insert,m_inspect_result.ng_imul);

	m_dlg.result = result;

	m_dlg.DoModal();
	*/
}

void CSeoilVisionDlg::OnUpdateInspStart(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	// 토글 시작 <-> 종료
	if (m_inspect)
	{
		pCmdUI->SetText("검사 중지(檢査停止)");
	}
	else
	{
		pCmdUI->SetText("검사 시작");
	}
}

void CSeoilVisionDlg::OnInspView() 
{
	// TODO: Add your command handler code here
//	m_view  = !m_view;
}

void CSeoilVisionDlg::OnUpdateInspView(CCmdUI* pCmdUI) 
{
	return;

	// TODO: Add your command update UI handler code here
	if (!m_view)
	{
		pCmdUI->SetText("실시간 영상 보지 않음");
		if(m_bTestSaveImage==TRUE) OnTestSaveimage(); 
	}
	else
	{
		pCmdUI->SetText("실시간 영상 보기");
	}
	m_view  = !m_view;
	m_picture->m_view = m_view;
	
}

void CSeoilVisionDlg::OnTimer(UINT_PTR nIDEvent) 
{	
	if ( nIDEvent == 9876 )
	{
		//TRACE( _T("%d\n"), m_ctrlIO.DetectStrawSensor() );
	}

	if(nIDEvent == 1235)
	{
	}
	// 솔 끄기
	if ( nIDEvent == 100 )
	{
		m_pCtrlIO->StopBlow();

		KillTimer( 100 );
	}
	else if (nIDEvent==10 && !m_inspect_image)
	{
		cvReleaseImage(&m_show);
		CString str;
		char line[128];
	
		if(m_nMainImage==0){
			str = IMAGE_MAIN_STRAW;
			m_nMainImage++;
		}
		else if(m_nMainImage==1){
			str = IMAGE_MAIN_T_P;
			m_nMainImage=0;
		}
		
		sprintf(line,str);
		
		m_show = cvLoadImage(line,CV_LOAD_IMAGE_COLOR);
		cvFlip(m_show);
		m_mainView.DrawImage(
			m_show, 
			SHOW_MAIN_WIDTH, 
			SHOW_MAIN_HEIGHT + 10);
		
		Invalidate(false);
	}
	else if (nIDEvent == 1001)
	{

	}


	// 재시작
	if (nIDEvent == 1234)
	{
		FILE *file= fopen("c:\\ATI\\restart.txt","r");
		if (file != NULL)
		{
			int test;
			fscanf(file,"%d",&test);
			fclose(file);
			if (test == 1)
			{
				file = fopen("c:\\ATI\\restart.txt","w");
				fprintf(file,"0");
				fclose(file);
				// 재시작 루틴
				struct INSPECTION_RESTORE_DATA data;

				m_monitor.ReadFileName();
				m_monitor.ReadFullFilename();
				m_monitor.ReadErrData();

				data = m_monitor.GetErrData();

				// 주어진 파일로 읽기 
				FILE *file;
				{
					file = fopen(m_monitor.GetFullName(),"r");
					if (file == NULL)
					{
						// 에러 발생
						AfxMessageBox("파일을 여는 중 문제가 발생했습니다!");
						fclose(file);
			
						return;
					}
					CString tempath, sInsp1, sInsp2;

					m_sFileText.Format (" [현재 사용 중인 파일] - %s",m_monitor.GetFileName());
					SetWindowText("Seoil EzVision 카메라" + m_sFileText);

					tempath=m_monitor.GetFullName();
					
					sInsp1 = tempath;
					tempath.Delete(tempath.GetLength()-1,1); 
					sInsp2 = tempath +"2";

					m_picture->m_pre->m_bAutoOpen = true;
					m_picture->m_pre->m_sAutoOpenPath = sInsp1;
					m_picture->m_pre->ShowWindow(SW_SHOW);
					m_picture->m_pre->OnDbload();
					m_picture->m_pre->ShowWindow(SW_HIDE);					
				}

				// 데이터 넣기	
				m_inspect_result.ng = data.ng;
				m_inspect_result.ng_angle = data.ng_angle;
				m_inspect_result.ng_chuck = data.ng_chuck;
				m_inspect_result.ng_imul = data.ng_imul;
				m_inspect_result.ng_imul_top = data.ng_imul_top;
				m_inspect_result.ng_imul_mid = data.ng_imul_mid;
				m_inspect_result.ng_imul_jabara = data.ng_imul_jabara;
				m_inspect_result.ng_imul_chuck = data.ng_imul_chuck;
				m_inspect_result.ng_imul_insert = data.ng_imul_insert;
				m_inspect_result.ng_insert = data.ng_insert;
				m_inspect_result.ng_jabara = data.ng_jabara;
				m_inspect_result.ng_maguri = data.ng_maguri;
				m_inspect_result.ng_pok = data.ng_pok;

				m_totalTime = data.m_totalTime;
		
				// 자동 시작
				m_restarted = true;
				OnMainBtnClick(_EZ_BTN_ID_FIRST+1);
			}
			else
			{

			}
		}

		KillTimer(1234);
	}
	CDialog::OnTimer(nIDEvent);
}

void CSeoilVisionDlg::RejectX( bool blow )
{
	if ( m_bSolOnOff == false )
		return;

	// 불량 제거
	if ( blow == true )
	{
		m_pCtrlIO->StartBlow();

		SetTimer( 100, m_SolTime, NULL );
	}
	else
	{
		m_pCtrlIO->StopBlow();
	}
}

CSeoilVisionDlg::~CSeoilVisionDlg()
{
	RecordRejectInfo();

	m_pCtrlIO->TurnOffLamp();

	if(m_pCtrlIO != NULL)
		delete m_pCtrlIO;

	cvReleaseImage(&m_show);
	
	for (int i=0; i<_MAX_BUTTONS; i++)
	{
		if(m_btn[i] != NULL)
			delete m_btn[i];
	}
	
	if(m_pFont != NULL)
		delete m_pFont;
	
	if(m_pFont1 != NULL)
		delete m_pFont1;
	
	if(m_picture != NULL)
	{
		if ( m_inspect_image == true)
		{
			m_picture->StrawInspectionStop();
		}
		delete m_picture;
	}

	if(m_mainInspView != NULL)
		delete [] m_mainInspView;
}
// 2008 BHJ 자동 검사시 불량 이미지만 저장 기능. 
void CSeoilVisionDlg::OnTestSaveimage() 
{
	if(!m_isLoaded)
		return;
	// TODO: Add your command handler code here
	if (m_bTestSaveImage==FALSE)
	{
		SetWindowText(m_sMainText+m_sFileText+"  [테스트모드] : 불량이미지 저장중입니다....在保存缺陷影像");
		m_bTestSaveImage = TRUE;
		m_picture->m_save_error = true;
		m_picture->m_save_all = false;
		m_picture->m_save_part_error = false;
		m_view  = FALSE;
		m_picture->m_view = m_view;
	}else {
		SetWindowText(m_sMainText+m_sFileText);
		m_bTestSaveImage = FALSE;
		m_picture->m_save_error = false;
		m_picture->m_save_all = false;
		m_picture->m_save_part_error = false;
	}
}

void CSeoilVisionDlg::SetGlobalVariable(INSPECT_SETTING *pInsp)
{
	if(pInsp->camWidth > 1280 || pInsp->camWidth < 0)
	{
		AfxMessageBox( _T("카메라 가로 길이가 허용 범위를 벗어나 초기 값으로 변경합니다!") );
		pInsp->camWidth = 1280;
	}

	if(pInsp->camHeight > 1024 || pInsp->camHeight < 0)
	{
		AfxMessageBox( _T("카메라 높이가 허용 범위를 벗어나 초기 값으로 변경합니다!") );
		pInsp->camHeight = 256;
	}
	
	if(pInsp->triggerPerPitch > 5 || pInsp->triggerPerPitch < 0)
	{
		AfxMessageBox( _T("트리거 값이 허용 범위를 벗어나 초기 값으로 변경합니다!") );
		pInsp->triggerPerPitch = 5;
	}
	
	if(pInsp->inspPitch > 3 || pInsp->inspPitch < 0)
	{
		pInsp->inspPitch = 3;
		AfxMessageBox( _T("피치 값이 허용 범위를 벗어나 초기 값으로 변경합니다!") );
	}
	
	if((pInsp->inspPitch * pInsp->triggerPerPitch) > 9)
	{
		pInsp->triggerPerPitch = 5;
		pInsp->inspPitch = 1;
		AfxMessageBox( _T("트리거, 피치 최대값을 초과하여 기본 값으로 변경합니다!") );
	}

	_MAXTRI = pInsp->triggerPerPitch;
	_MAXPIT = pInsp->inspPitch;

	_MAXROI = _MAXTRI * _MAXPIT;
	
	_CamWidth = pInsp->camWidth;
	_CamHeight = pInsp->camHeight;

	double ratio = 1024.0 / _CamWidth;
	
	if(ratio * _CamHeight > 570)
	{
		ratio = 570.0 / _CamHeight;
	}

	_ShowWidth  = (int)(ratio * _CamWidth);
	_ShowHeight = (int)(ratio * _CamHeight);
}

void CSeoilVisionDlg::OnFileOpen() 
{
	// TODO: Add your command handler code here
	LPCSTR 	lpszFilter = "검사 데이터 파일(*.csv)|*.csv";
	
	CFileDialog diag(TRUE, _T("csv"), NULL, OFN_EXPLORER | OFN_HIDEREADONLY, lpszFilter);

	FILE *file;

	if (diag.DoModal() == IDOK)
	{
		CString ksrFileName = diag.GetPathName();

		file = fopen(ksrFileName,"r");

		if (file == NULL)
		{
			// 에러 발생
			AfxMessageBox("파일을 여는 중 문제가 발생했습니다!");
			fclose(file);
			return;
		}

		CString tempath, sInsp;
	
		m_sFileText = " [현재 사용 중인 파일] - " + diag.GetFileName();
		SetWindowText("Seoil EzVision 카메라" + m_sFileText);

		tempath = diag.GetPathName();
		
		sInsp = tempath;
		tempath.Delete(tempath.GetLength()-1,1); 
		
		INSPECT_SETTING inspSetting;
		//fread(&inspSetting, sizeof(inspSetting), 1, file);
		
		fscanf(file, "%d", &inspSetting.pixsize);
		fscanf(file, "%d", &inspSetting.mid_pixsize);
		fscanf(file, "%d", &inspSetting.jabara_pixsize);
		fscanf(file, "%d", &inspSetting.chuck_pixsize);
		fscanf(file, "%d", &inspSetting.insert_pixsize);
		fscanf(file, "%d", &inspSetting.imul_etq_left_pix);
		fscanf(file, "%d", &inspSetting.imul_etq_right_pix);

		fscanf(file, "%d", &inspSetting.imul_top_threshold);
		fscanf(file, "%d", &inspSetting.imul_mid_threshold);
		fscanf(file, "%d", &inspSetting.imul_jabara_threshold);
		fscanf(file, "%d", &inspSetting.imul_chuck_threshold);
		fscanf(file, "%d", &inspSetting.imul_insert_threshold);
		fscanf(file, "%d", &inspSetting.imul_etq_left_threshold);
		fscanf(file, "%d", &inspSetting.imul_etq_right_threshold);

		fscanf(file, "%d", &inspSetting.imul_top_mask);
		fscanf(file, "%d", &inspSetting.imul_top_mask_cols);
		fscanf(file, "%d", &inspSetting.imul_mid_mask);
		fscanf(file, "%d", &inspSetting.imul_jabara_mask);
		fscanf(file, "%d", &inspSetting.imul_chuck_mask);
		fscanf(file, "%d", &inspSetting.imul_chuck_mask_cols);
		fscanf(file, "%d", &inspSetting.imul_insert_mask);
		fscanf(file, "%d", &inspSetting.imul_insert_pos);
		fscanf(file, "%d", &inspSetting.imul_etq_left_mask);
		fscanf(file, "%d", &inspSetting.imul_etq_right_mask);

		fscanf(file, "%lf", &inspSetting.jabara_recognize);
		fscanf(file, "%d", &inspSetting.jabara_num);
		fscanf(file, "%d", &inspSetting.jabara_margin_of_error);
		fscanf(file, "%lf", &inspSetting.jabara_width_stdev);
		fscanf(file, "%lf", &inspSetting.jabara_distance_stdev);
		fscanf(file, "%lf", &inspSetting.jabara_gradient_stdev);
		fscanf(file, "%d", &inspSetting.jabara_neighbor);

		fscanf(file, "%lf", &inspSetting.etiquette_recognize);
		fscanf(file, "%lf", &inspSetting.etiquette_position);
		fscanf(file, "%d", &inspSetting.etiquette_size);

		fscanf(file, "%d", &inspSetting.camWidth);
		fscanf(file, "%d", &inspSetting.camHeight);
		fscanf(file, "%d", &inspSetting.triggerPerPitch);
		fscanf(file, "%d", &inspSetting.inspPitch);

		fscanf(file, "%lf", &inspSetting.m_Magunus_Slide);
		fscanf(file, "%lf", &inspSetting.m_Magunus_Slide2);
		fscanf(file, "%d", &inspSetting.m_Magunus_SlideLength);
		fscanf(file, "%d", &inspSetting.m_maguri_max_length);

		fscanf(file, "%d", &inspSetting.m_minChuck);
		fscanf(file, "%d", &inspSetting.m_maxChuck);
		fscanf(file, "%lf", &inspSetting.m_pok_limit);
		fscanf(file, "%lf", &inspSetting.m_Slide);
		fscanf(file, "%d", &inspSetting.m_SlideLength);

		fscanf(file, "%d", &inspSetting.angle_diff);
		fscanf(file, "%d", &inspSetting.m_pok);
		fscanf(file, "%lf", &inspSetting.resol);

		fscanf(file, "%lf", &inspSetting.angle_limit[0]);
		fscanf(file, "%lf", &inspSetting.angle_limit[1]);

		fscanf(file, "%lf", &inspSetting.m_maguri_diff_ratio);
		fscanf(file, "%d", &inspSetting.m_maguri_angle_gap);
		fscanf(file, "%d", &inspSetting.m_maguri_angle_continuous);
		fscanf(file, "%d", &inspSetting.m_maguri_angle_diff);
		fscanf(file, "%d", &inspSetting.m_mid_angle_gap);
		fscanf(file, "%d", &inspSetting.m_mid_angle_continuous);
		fscanf(file, "%d", &inspSetting.m_mid_angle_diff);
		fscanf(file, "%d", &inspSetting.m_mid_angle_height_diff);
		fscanf(file, "%d", &inspSetting.m_top_angle_gap);
		fscanf(file, "%d", &inspSetting.m_top_angle_continuous);
		fscanf(file, "%d", &inspSetting.m_top_angle_diff);
		fscanf(file, "%d", &inspSetting.m_top_angle_height_diff);
		fscanf(file, "%d", &inspSetting.m_chuck_angle_gap);
		fscanf(file, "%d", &inspSetting.m_chuck_angle_continuous);
		fscanf(file, "%d", &inspSetting.m_chuck_angle_diff);
		fscanf(file, "%d", &inspSetting.m_chuck_angle_height_diff);
		fscanf(file, "%d", &inspSetting.m_insert_angle_gap);
		fscanf(file, "%d", &inspSetting.m_insert_angle_continuous);
		fscanf(file, "%d", &inspSetting.m_insert_angle_diff);

		fscanf(file, "%lf", &inspSetting.imul_top_persent);
		fscanf(file, "%lf", &inspSetting.imul_mid_persent);

		fscanf(file, "%d", &inspSetting.imul_top_size);
		fscanf(file, "%d", &inspSetting.imul_mid_size);

		fscanf(file, "%d", &inspSetting.imul_mid_notinspect);

		fscanf(file, "%d", &inspSetting.stabbed_insert_notinspect);
		fscanf(file, "%d", &inspSetting.stabbed_insert_pix);

		fclose(file);
		
		//SetGlobalVariable(&inspSetting);

		// m_picture 생성
		InitGetPicture(&inspSetting);
		m_picture->m_pre->m_bAutoOpen = true;
		m_picture->m_pre->m_sAutoOpenPath = sInsp;
		m_picture->m_pre->OnDbload();

		// 파일명 업데이트
		m_filename = diag.GetPathName();
		m_file_name = diag.GetFileName();

		m_isLoaded = true;
	}
}

void CSeoilVisionDlg::OnTestSavegrabImage() 
{
	if(!m_isLoaded)
		return;
	// TODO: Add your command handler code here
	if (m_bTestSaveImage==FALSE)
	{
		SetWindowText(m_sMainText+m_sFileText+"  [테스트모드] : 모든 영상 저장중입니다....在保存所有影像");
		m_bTestSaveImage = TRUE;
		m_picture->m_save_all = true;
		m_picture->m_save_error = false;
		m_picture->m_save_part_error = false;
		m_view  = FALSE;
		m_picture->m_view = m_view;
	}else {
		SetWindowText(m_sMainText+m_sFileText);
		m_bTestSaveImage = FALSE;
		m_picture->m_save_all = false;
		m_picture->m_save_error = false;
		m_picture->m_save_part_error = false;
	}
}

void CSeoilVisionDlg::OnFileParameterNewer() 
{
	if ( m_picture == NULL )
	{
		AfxMessageBox( _T("설정파일 로드 후 사용하세요!") );
		return;
	}

	if(m_colorstraw == 0 && m_tepe == 0){	// 흰색 빨대에 자바라 없음.
		m_picture->SetNewParameter("c:\\ATI\\parameter_NC_NJB.txt");
		m_picture->m_pre->m_imageView.SetNewParameter("c:\\ATI\\parameter_NC_NJB.txt");
	}
	else if (m_colorstraw == 0 && m_tepe == 1)		// 흰색 빨대에 자바라 있음.
	{
		m_picture->SetNewParameter("c:\\ATI\\parameter_NC_YJB.txt");
		m_picture->m_pre->m_imageView.SetNewParameter("c:\\ATI\\parameter_NC_YJB.txt");
	}
	else if (m_colorstraw > 0 && m_tepe == 0)		// 색 빨대에 자바라 없음.
	{
		m_picture->SetNewParameter("c:\\ATI\\parameter_YC_NJB.txt");
		m_picture->m_pre->m_imageView.SetNewParameter("c:\\ATI\\parameter_YC_NJB.txt");
	}
	else if (m_colorstraw > 0 && m_tepe == 1)		// 색 빨대에 자바라 있음.
	{
		m_picture->SetNewParameter("c:\\ATI\\parameter_YC_YJB.txt");
		m_picture->m_pre->m_imageView.SetNewParameter("c:\\ATI\\parameter_YC_YJB.txt");
	}

	m_picture->m_pre->m_ptParam->m_nColorThres01=m_colorthres[0];
	m_picture->m_pre->m_ptParam->m_nColorThres02=m_colorthres[1];
	m_picture->m_pre->m_ptParam->m_nColorThres03=m_colorthres[2];
	m_picture->m_pre->m_ptParam->UpdateData(false);

	ReadSettingFile();

	TRACE(_T("Empty Sol Gap: %d\n"), m_EmptySolGap);
	TRACE(_T("Sol Gap: %d\n"), m_nSolgap);
}

void CSeoilVisionDlg::ReadSettingFile()
{
	int read;

	FILE *fp = fopen( _T("c:\\ATI\\Setting2.txt"), _T("r") );
	fscanf( fp, _T("Transparency		%d\n"), &m_trans);
	fscanf( fp, _T("IsColorStraw		%d\n"), &m_colorstraw);
	fscanf( fp, _T("IsTeOrPe			%d\n"), &m_tepe);
	fscanf( fp, _T("RPM				%d\n"), &m_nSetRPM);
	fscanf( fp, _T("Sol_IO			%d\n"), &m_SolIO);
	fscanf( fp, _T("Sol_Time(ms)		%d\n"), &m_SolTime);
	fscanf( fp, _T("Empty_Sol_Gap		%d\n"), &m_EmptySolGap);
	fscanf( fp, _T("IO_Type			%d\n"), &m_ioType);
	fclose( fp );

	fp = fopen( _T("c:\\ATI\\Setting.txt"), _T("r") );
	fscanf( fp, _T("SolGap1		%d\n"), &m_nSolgap);
	fscanf( fp, _T("SolGap2		%d\n"), &read);
	fscanf( fp, _T("YFlip1		%d\n"), &m_bYFlip);
	fscanf( fp, "YFlip2		%d\n", &read);
	fscanf( fp, "CamNumber1	%d\n", &m_nCamNum);
	fscanf( fp, "CamNumber2	%d\n", &read);
	fscanf( fp, "XAxisFlip	%d\n", &m_bXAxisFlip);
}


void CSeoilVisionDlg::OnInspSetcolor() 
{
	if(!m_isLoaded)
	{
		MessageBox(_T("설정 파일을 불러온 후 사용하세요!(先打開設定文件以後使用)"));
		return;
	}

	CSetColorDlg dlg;

	dlg.SetTransColor(m_trans,m_colorstraw, m_bUseInspEtq);
	dlg.m_tepe = m_tepe;

	if (dlg.DoModal() == IDOK)
	{
		m_trans = dlg.m_trans;
		m_colorstraw = dlg.m_color;
		m_tepe = dlg.m_tepe;
		m_bUseInspEtq = dlg.m_bInspEtq;
// 		m_picture->m_process->m_trans = m_trans;
// 		m_picture->m_process->m_colorstraw = m_colorstraw;
		m_picture->m_tepe = m_tepe;
		m_picture->SetColorInfo(m_trans,m_colorstraw,m_colorthres[0],m_colorthres[1],m_colorthres[2], m_bUseInspEtq);
		

		// 파일 기록
		FILE *setting = fopen("c:\\ATI\\Setting2.txt","w");
		fprintf( setting, _T("Transparency		%d\n"), m_trans);
		fprintf( setting, _T("IsColorStraw		%d\n"), m_colorstraw);
		fprintf( setting, _T("IsTeOrPe			%d\n"), m_tepe);
		fprintf( setting, _T("RPM				%d\n"), m_nSetRPM);
		fprintf( setting, _T("Sol_IO				%d\n"), m_SolIO);
		fprintf( setting, _T("Sol_Time(ms)		%d\n"), m_SolTime);
		fprintf( setting, _T("Empty_Sol_Gap		%d\n"), m_EmptySolGap );
		fprintf( setting, _T("IO_Type			%d\n"), m_ioType);
		fclose(setting);
	}
	else
	{
	}
}

void CSeoilVisionDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	Invalidate(false);
}

void CSeoilVisionDlg::OnTestSaverejectimage() 
{
	if(!m_isLoaded)
		return;
	// TODO: Add your command handler code here
	if (m_bTestSavePartImage==FALSE)
	{
		SetWindowText(m_sMainText+m_sFileText+"  [테스트모드] : 불량부분 이미지 저장중입니다....在保存缺陷部位影像");
		m_bTestSavePartImage = TRUE;
		m_picture->m_save_error = false;
		m_picture->m_save_all = false;
		m_picture->m_save_part_error = true;
	}else {
		SetWindowText(m_sMainText+m_sFileText);
		m_bTestSavePartImage = FALSE;
		m_picture->m_save_error = false;
		m_picture->m_save_all = false;
		m_picture->m_save_part_error = false;
	}
}

void CSeoilVisionDlg::OnTestTwRed() 
{
	m_pCtrlIO->TurnOnRedLamp();

	m_pCtrlIO->StartBlow();
}

void CSeoilVisionDlg::OnTestTwYellow() 
{
	m_pCtrlIO->TurnOnYellowLamp();

	m_pCtrlIO->StopBlow();
}

void CSeoilVisionDlg::OnTestTwGreen() 
{
	m_pCtrlIO->TurnOnGreenLamp();
}

void CSeoilVisionDlg::OnNoStrawSetting(int nSet)
{
	if (nSet==0)
	{
		if ( m_picture->m_bUseSolNoStraw == FALSE )
		{
			m_picture->m_bUseSolNoStraw = TRUE;
			m_btn[4]->SetWindowText( _T("빈빨대불기중지") );
		}
		else
		{
			m_picture->m_bUseSolNoStraw = FALSE;
			m_btn[4]->SetWindowText( _T("빈 빨대 불기") );
		}
	}
	else if ( nSet == 1 )
	{
		m_picture->m_bUseSolNoStraw = TRUE;
		m_btn[4]->SetWindowText( _T("빈빨대불기중지") );
	}
	else if ( nSet == -1 )
	{
		m_picture->m_bUseSolNoStraw = FALSE;
		m_btn[4]->SetWindowText( _T("빈 빨대 불기") );
	}
}

void CSeoilVisionDlg::UpdateRejectInfo(int nType1, int nNumber, RESULT_INSPVALUE *pResultV)
{
	char line1[128], line2[128];
	sprintf(line1,"%d ", nNumber);
	//restr = line1;
	
	RESULT_INSPVALUE ResultV;

	memcpy( &ResultV, pResultV, sizeof( RESULT_INSPVALUE ) );

	switch(nType1)
	{
	case STRAW_MAGURI:
		sprintf(line2, "마구리불량 - %d (%d), 투명내측 %d\n", ResultV.Rmaguri[0] + ResultV.Rmaguri[1], ResultV.Rmaguri[2], ResultV.Rmaguri[3]);
		break;
	case STRAW_CHUCK:
		sprintf(line2, "축관불량 - %.2lf(%.2lf~%.2lf)\n", ResultV.Rchuck[0], ResultV.Rchuck[1], ResultV.Rchuck[2]);
		break;
	case STRAW_ANGLE:
		sprintf(line2,"상단각불량-각:%d,폭:%d\n", ResultV.top.isAngleOk, ResultV.top.isHeightOk);
		break;
	case STRAW_POK:
		sprintf(line2,"중단각불량-각:%d,폭:%d\n", ResultV.mid.isAngleOk, ResultV.mid.isHeightOk);
		break;
	case STRAW_INSERT:
		sprintf(line2,"삽입불량 - 끝단 %d(3), 길이 %d (%d)\n",  abs((int)(ResultV.Rinsult[0]-ResultV.Rinsult[1])), (int)ResultV.Rinsult[2], (int)ResultV.Rinsult[3]);
		break;
	case STRAW_IMUL:
		sprintf(line2, _T("이물 - 개수 %d\n"), ResultV.black_num);
		break;
	case STRAW_IMUL_TOP:
		sprintf(line2, _T("상단이물 - 개수 %d \n"), ResultV.Rimul[0][0]);
		break;
	case STRAW_IMUL_MID:
		sprintf(line2, _T("중단이물 - 개수 %d \n"), ResultV.Rimul[1][0]);
		break;
	case STRAW_IMUL_JABARA:
		sprintf(line2, _T("자바라이물 - 개수 %d \n"), ResultV.Rimul[2][0]); 
		break;
	case STRAW_IMUL_CHUCK:
		sprintf(line2, _T("축관이물 - 개수 %d \n"), ResultV.Rimul[3][0]);
		break;
	case STRAW_IMUL_INSERT:
		sprintf(line2, _T("내측이물 - 개수 %d \n"), ResultV.Rimul[4][0]);
		break;
	case STRAW_JABARA:
		sprintf(line2, "자바라-개수%d,두께%.3lf,거리%.3lf,기울기%.3lf,높이:%d \n", 
			ResultV.jabara_num, ResultV.jabara_width_stdev, ResultV.jabara_distance_stdev, ResultV.jabara_gradient_stdev, ResultV.jabara_neighbor);
		break;
	case 9999:
		sprintf(line2, "불량테스트 - %2.2f (%2.2f) \n", 1.02324546, 2.345678);
		break;
	case STRAW_ETIQUETTE:
		sprintf(line2, _T("에티켓 - 없음 \n"));
		break;
	}
	CString restr = line1;

	restr += line2;
	
	m_ctrREditResult.ReplaceSel(restr);
}

void CSeoilVisionDlg::ParameterRefresh()
{
	OnFileParameterNewer();
	m_picture->SetColorInfo(m_trans,m_colorstraw,m_colorthres[0],m_colorthres[1],m_colorthres[2], m_bUseInspEtq);
}


LRESULT CSeoilVisionDlg::OnRestart(WPARAM wParam, LPARAM lParam)
{

	// 재시작 하기
	// 로드부터 다시하기

	struct INSPECTION_RESTORE_DATA data;

	m_monitor.ReadFileName();
	m_monitor.ReadFullFilename();
	m_monitor.ReadErrData();

	data = m_monitor.GetErrData();

	// 주어진 파일로 읽기 
	FILE *file;

	{
		file = fopen(m_monitor.GetFullName(),"r");
		if (file == NULL)
		{
			// 에러 발생
			AfxMessageBox("파일을 여는 중 문제가 발생했습니다!");
			fclose(file);
			
			return 0;
		}
		CString tempath, sInsp1, sInsp2;

//		m_sFileText = m_monitor.GetFileName();
//		SetWindowText("Seoil EzVision 카메라" + m_sFileText);
		m_sFileText.Format (" [현재 사용 중인 파일] - %s",m_monitor.GetFileName());
		SetWindowText("Seoil EzVision 카메라" + m_sFileText);

		

		tempath=m_monitor.GetFullName();
		
		sInsp1 = tempath;
		tempath.Delete(tempath.GetLength()-1,1); 
		sInsp2 = tempath +"2";

		m_picture->m_pre->m_bAutoOpen = true;
		m_picture->m_pre->m_sAutoOpenPath = sInsp1;
		m_picture->m_pre->ShowWindow(SW_SHOW);
		m_picture->m_pre->OnDbload();
		m_picture->m_pre->ShowWindow(SW_HIDE);

//		m_picture->m_pre2->m_bAutoOpen = true;
//		m_picture->m_pre2->m_sAutoOpenPath = sInsp2;
//		m_picture->m_pre2->ShowWindow(SW_SHOW);
//		m_picture->m_pre2->OnDbload();
//		m_picture->m_pre2->ShowWindow(SW_HIDE);
//		UpdateStatus(1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4, 5, 6);

		
	}

	// 데이터 넣기	
	m_inspect_result.ng = data.ng;
	m_inspect_result.ng_angle = data.ng_angle;
	m_inspect_result.ng_chuck = data.ng_chuck;
	m_inspect_result.ng_imul = data.ng_imul;
	m_inspect_result.ng_imul_top = data.ng_imul_top;
	m_inspect_result.ng_imul_mid = data.ng_imul_mid;
	m_inspect_result.ng_imul_jabara = data.ng_imul_jabara;
	m_inspect_result.ng_imul_chuck = data.ng_imul_chuck;
	m_inspect_result.ng_imul_insert = data.ng_imul_insert;
	m_inspect_result.ng_insert = data.ng_insert;
	m_inspect_result.ng_jabara = data.ng_jabara;
	m_inspect_result.ng_maguri = data.ng_maguri;
	m_inspect_result.ng_pok = data.ng_pok;

	m_totalTime = data.m_totalTime;
	

	// 자동 시작
//	Sleep(500);
	m_restarted = true;
	OnInspStart();

	LRESULT result = 0;

	return result;
}

void CSeoilVisionDlg::BFlyCameraTestFunc()
{
	FlyCapture2::Camera *pCamera = NULL;
	FlyCapture2::Error error;

	pCamera = new FlyCapture2::Camera;

	// Connect to selected camera
	error = pCamera->Connect();
	if ( error != FlyCapture2::PGRERROR_OK )
	{
		AfxMessageBox(_T("Connect Failed!"));
		return;
	}
	
	
	FlyCapture2::Format7Info fmt7Info;
	bool supported;
	
	fmt7Info.mode = (Mode)MODE_0;
	error = pCamera->GetFormat7Info( &fmt7Info, &supported );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox(_T("Get Format 7 Failed!"));
		return;
	}
	
	if ( supported != true )
	{
		AfxMessageBox(_T("Format 7 is not supported"));
		return;
	}

	Format7ImageSettings fmt7ImageSettings;
	fmt7ImageSettings.mode = ( Mode ) MODE_0;
	fmt7ImageSettings.offsetX = 0;
	fmt7ImageSettings.offsetY = 0;
	fmt7ImageSettings.width = 1280;
	fmt7ImageSettings.height = 512;
	fmt7ImageSettings.pixelFormat = PIXEL_FORMAT_MONO8;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;
	error = pCamera->ValidateFormat7Settings( &fmt7ImageSettings, &valid, &fmt7PacketInfo );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("Validate Failed!") );
		return;
	}
	if ( valid != true )
	{
		AfxMessageBox( _T("Format 7 settings are not valid!") );
		return;
	}

	Format7ImageSettings oldFmt7ImageSettings;

	unsigned int pack;
	float per;

	error = pCamera->GetFormat7Configuration(
		&oldFmt7ImageSettings,
		&pack,
		&per );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("Get format 7 failed!") );
		return;
	}

	error = pCamera->SetFormat7Configuration(
		&fmt7ImageSettings, 
		fmt7PacketInfo.recommendedBytesPerPacket );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("Set format 7 Failed!") );
		return;
	}


	if ( pCamera->IsConnected() != true )
	{
		AfxMessageBox(_T("Disconnected!"));
	}

	
	Property frmRate;
	frmRate.type = FRAME_RATE;
	error = pCamera->GetProperty( &frmRate );
	if( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("Get Property error!") );
		return;
	}
	
	frmRate.type = FRAME_RATE;
	frmRate.absControl = true;
	frmRate.absValue = 30.0;

	error = pCamera->SetProperty( &frmRate );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("set property error!") );
		return;
	}


	error = pCamera->StartCapture();
	if ( error != FlyCapture2::PGRERROR_OK )
	{
		CString csMessage;
		csMessage.Format(
			_T("Connect Failure: %s"), error.GetDescription() );
		AfxMessageBox( csMessage, MB_ICONSTOP );
		return;
	}

	TRACE( _T("format 7 info----------------\n") );
	TRACE( _T("Pixel Format: %X\n"), oldFmt7ImageSettings.pixelFormat );
	TRACE( _T("Width: %d\n"), oldFmt7ImageSettings.width );
	TRACE( _T("Height: %d\n"), oldFmt7ImageSettings.height );
	TRACE( _T("offset X: %d\n"), oldFmt7ImageSettings.offsetX );
	TRACE( _T("offset Y: %d\n"), oldFmt7ImageSettings.offsetY );
	TRACE( _T("Mode: %d\n"), oldFmt7ImageSettings.mode );
	TRACE( _T("frame rate: %f\n"), frmRate.absValue );
	TRACE( _T("------------------------\n") );


	FlyCapture2::Image buffImage;
	error = pCamera->RetrieveBuffer(&buffImage);
	if ( error != FlyCapture2::PGRERROR_OK )
	{
		AfxMessageBox(_T("Retrieve Buffer Failed!"));
		return;
	}

	IplImage *img = cvCreateImage(cvSize(buffImage.GetCols(), buffImage.GetRows()), 8, 1);

	memcpy(img->imageData, buffImage.GetData(), img->imageSize);

	cvSaveImage(_T("c:\\lalalala.bmp"), img);

	cvReleaseImage(&img);

	error = pCamera->Disconnect();
	if ( error != FlyCapture2::PGRERROR_OK )
	{
		AfxMessageBox(_T("Disconnect Failed!"));
		return;
	}

	delete pCamera;	

	AfxMessageBox( _T("끝!") );
	
}

void CSeoilVisionDlg::OnTestTest()
{
}

void CSeoilVisionDlg::SaveSetting2()
{
	FILE *fp = fopen( _T("c:\\ATI\\Setting2.txt"), _T("w") );
	fprintf( fp, _T("Transparency		%d\n"), m_trans );
	fprintf( fp, _T("IsColorStraw		%d\n"), m_colorstraw );
	fprintf( fp, _T("IsTeOrPe			%d\n"), m_tepe );
	fprintf( fp, _T("RPM				%d\n"), m_nSetRPM );
	fprintf( fp, _T("Sol_IO				%d\n"), m_SolIO );
	fprintf( fp, _T("Sol_Time(ms)		%d\n"), m_SolTime );
	fprintf( fp, _T("Empty_Sol_Gap		%d\n"), m_EmptySolGap );
	fprintf( fp, _T("IO_Type			%d\n"), m_ioType );
	fclose ( fp );
}

void CSeoilVisionDlg::OnRpm()
{
	if ( m_inspect_image == true )
	{
		AfxMessageBox( _T("검사 중지 후 사용하세요!") );
		return;
	}

	CRPMSettingDlg dlg;

	if ( dlg.DoModal() == IDOK )
	{
		int tmp = dlg.GetRPM();
		
		if ( tmp < 1 || tmp > 1000 )
		{
			AfxMessageBox( _T("RPM을 1 ~ 1000 사이로 설정하십시오!") );
			return;
		}

		m_nSetRPM = tmp;

		SaveSetting2();

		if ( m_isLoaded == true )
		{
			m_picture->SetInterval();
		}
	}
}

void CSeoilVisionDlg::OnSolOn()
{
	if ( m_inspect_image == true )
	{
		AfxMessageBox( _T("검사 중지 후 사용하세요!") );
		return;
	}

	m_bSolOnOff = true;

	Invalidate( TRUE );
}

void CSeoilVisionDlg::OnSolOff()
{
	if ( m_inspect_image == true )
	{
		AfxMessageBox( _T("검사 중지 후 사용하세요!") );
		return;
	}

	m_bSolOnOff = false;

	Invalidate( TRUE );
}

BOOL CSeoilVisionDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->wParam == VK_ESCAPE)
	{
		// 검사중이면 escape로 종료하지 않음
		if(m_inspect_image)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSeoilVisionDlg::TraceMenuItem_Disable(CMenu *pMenu)
{
	if(pMenu != NULL)
	{
		int subItemCount = pMenu->GetMenuItemCount();

		for(int pos = 0; pos < subItemCount; pos++)
		{
			CMenu *pSubMenu = pMenu->GetSubMenu(pos);
			if(pSubMenu == NULL)
			{
				int itemId = pMenu->GetMenuItemID(pos);
				pMenu->EnableMenuItem(itemId, MF_BYCOMMAND | MF_DISABLED);
			}
			else
				TraceMenuItem_Disable(pSubMenu);
		}
	}
}

void CSeoilVisionDlg::DisableMenu()
{
	CMenu *pMenu = GetMenu();

	if(pMenu != NULL)
	{
		TraceMenuItem_Disable(pMenu);
	}
}

void CSeoilVisionDlg::TraceMenuItem_Enable(CMenu *pMenu)
{
	if(pMenu != NULL)
	{
		int subItemCount = pMenu->GetMenuItemCount();

		for(int pos = 0; pos < subItemCount; pos++)
		{
			CMenu *pSubMenu = pMenu->GetSubMenu(pos);
			if(pSubMenu == NULL)
			{
				int itemId = pMenu->GetMenuItemID(pos);
				pMenu->EnableMenuItem(itemId, MF_BYCOMMAND | MF_ENABLED);
			}
			else
				TraceMenuItem_Enable(pSubMenu);
		}
	}
}

void CSeoilVisionDlg::EnableMenu()
{
	CMenu *pMenu = GetMenu();

	if(pMenu != NULL)
	{
		TraceMenuItem_Enable(pMenu);
	}
}
