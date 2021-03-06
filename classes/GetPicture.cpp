// GetPicture.cpp: implementation of the CGetPicture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "CViewerTest.h"
#include "GetPicture.h"

#include "highgui.h"

#include "../SeoilVision.h"
#include "../SeoilVisionDlg.h"

#include "../mmcdll.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 서일

#include "../Extern.h"

using namespace FlyCapture2;

void CGetPicture::InitAfterDBLoad()
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	for(int i = 0; i < _IMAGE_BUFFER_SIZE; i++)
		m_pGrabImageBuffer[0][i] = cvCreateImage(cvSize(_CamWidth, _CamHeight), 8, 1);

	m_pre = new CPreInspection;
	m_pre->Create(NULL);
	m_pre->ShowWindow(SW_HIDE);

	if ( resizeView != NULL )
		cvReleaseImage( &resizeView );

	resizeView = cvCreateImage( cvSize( 640, (int) ( ( 640.0 / ( float ) _CamWidth ) * ( float ) _CamHeight ) ), 8, 1 );

	if ( rgbResizeView != NULL )
		cvReleaseImage( &rgbResizeView );

	rgbResizeView = cvCreateImage( cvSize( 640, (int) ( ( 640.0 / ( float ) _CamWidth ) * ( float ) _CamHeight ) ), 8, 3 );
}

CGetPicture::CGetPicture()
{
	TRACE(_T("\n\nCGetPicture()\n\n"));

	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	// 검사용 클래스 초기화
	cam_1 = NULL;
	cam_2 = NULL;

	memset(ImageDatas, 1000, sizeof(int)*3);
	// 카메라 시작 않음
	for (int i=0;i<_MAX_CAMERAS;i++)
	{
		cam_start[i]= FALSE;
		m_saved_ref_image[i] = 0;
		m_iImageCount[i] = 0;
		
		// 클래스 초기화
		for (int j = 0; j < _IMAGE_BUFFER_SIZE; j++)
		{
			m_pInspectData[i][j] = new InspectThread;
			m_pGrabImageBuffer[i][j] = cvCreateImage( cvSize( _CamWidth, _CamHeight ), 8, 1);
			m_process[i][j] = new CStrawProcess;
		}
	}
	
	// 이미지 저장 초기화
	m_save_image = FALSE;
	m_inspect_image = FALSE;
	m_view= FALSE;
	m_bUseSolNoStraw = FALSE;
	m_viewCnt = 0;
	m_camtoview = 0;
	
	// 초기화 다이얼로그
	m_pre = new CPreInspection;
	m_pre->Create(NULL);
	m_pre->ShowWindow(SW_HIDE);
	
	FILE *fn = fopen(_T("C:\\ATI\\Setting.txt"), _T("r"));
	int nread=0, nCam=0;
	int bread = 0;
	if (fn != NULL){
		if(_CAM_NUM==0){
			fscanf(fn, _T("SolGap1		%d\n"), &m_nSolgap);
			fscanf(fn, _T("SolGap2		%d\n"), &nread);
			fscanf(fn, "YFlip1		%d\n", &m_bYFlip);
			fscanf(fn, "YFlip2		%d\n", &bread);
			fscanf(fn, "CamNumber1	%d\n", &m_nCamNum);
			fscanf(fn, "CamNumber2	%d\n", &nCam);
			fscanf(fn, "XAxisFlip	%d\n", &m_bXAxisFlip);
		}else if(_CAM_NUM==1){
			fscanf(fn, "SolGap1		%d\n", &nread);
			fscanf(fn, "SolGap2		%d\n", &m_nSolgap);
			fscanf(fn, "YFlip1		%d\n", &bread);
			fscanf(fn, "YFlip2		%d\n", &m_bYFlip);
			fscanf(fn, "CamNumber1	%d\n", &nCam);
			fscanf(fn, "CamNumber2	%d\n", &m_nCamNum);
			fscanf(fn, "XAxisFlip	%d\n", &m_bXAxisFlip);
		}
		fclose(fn);
	}else{
		AfxMessageBox(_T("Setting 파일이 없습니다.(沒有Setting文檔)"));
	}

	resizeView    = cvCreateImage(cvSize( 640, ( int )( ( 640.0/( float )_CamWidth ) * ( float ) _CamHeight ) ), 8, 1 );
	rgbResizeView = cvCreateImage(cvSize( 640, ( int )( ( 640.0/( float )_CamWidth ) * ( float ) _CamHeight ) ), 8, 3 );
	
	bStrawEnd = FALSE;
	bStrawStart = FALSE;
	bEtiqueFind = FALSE;
	inspect_temp = TRUE;

	m_nInterval = 0;
}

CGetPicture::~CGetPicture()
{
	// 검사용 클래스 지우기
	int i, j;
	for (i=0;i<_MAX_CAMERAS;i++)
	{
		for (j=0;j<_IMAGE_BUFFER_SIZE;j++)
			delete m_process[i][j];
	}

	delete cam_1;
	delete cam_2;

	// 이미지 지우기
	for ( i=0;i<_MAX_CAMERAS;i++)
	{
		cvReleaseImage(&cam_image[i]);
	}
	
	delete m_pre->m_settingdlg;
	
	if(m_pre != NULL) // ADD cwlee
	{
		m_pre->DestroyWindow(); // MOD cwlee
		delete m_pre;
	}

	for ( i = 0; i< _MAX_CAMERAS; i++ )
	{
		for ( j = 0; j< _IMAGE_BUFFER_SIZE; j++ )
		{
			delete m_pInspectData[i][j];
			cvReleaseImage( &m_pGrabImageBuffer[i][j] );
		}
	}

	for ( int i = 0; i < _TEST_NUM; i++ )
	{
		cvReleaseImage( &imgOk[i] );
	}

	if ( m_cam.IsConnected() == true )
	{
		m_cam.Disconnect();
	}

	if ( resizeView != NULL)
	{
		cvReleaseImage( &resizeView );
	}

	if ( rgbResizeView != NULL )
	{
		cvReleaseImage( &rgbResizeView );
	}
}

bool CGetPicture::InitCamera()
{
	if ( _CamWidth > 1280 || _CamHeight > 1024 ||
		_CamWidth < 0 || _CamHeight < 0 )
	{
		return false;
	}

	Error error;

	// 카메라 연결
	if ( m_cam.IsConnected() == false )
	{
		error = m_cam.Connect();
		if ( error != PGRERROR_OK )
		{
			AfxMessageBox( _T("CGetPicture::InitCamera() - 카메라 연결 실패!(相機連接失敗!)") );
			return false;
		}
	}
	
	// 기본 정보 획득
	Format7Info fmt7Info;
	bool supported;

	error = m_cam.GetFormat7Info( &fmt7Info, &supported );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("CGetPicture::InitCamera() - 기본정보 획득 실패!(獲得基本信息失敗)") );
		return false;
	}
	
	// 이미지 포멧 획득
	Format7ImageSettings fmt7ImageSettings;
	unsigned int packSize;
	float percentage;
	
	error = m_cam.GetFormat7Configuration(&fmt7ImageSettings, &packSize, &percentage);
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("CGetPicture::InitCamera() - format 7 획득 실패!(獲得Format7失敗)") );
		return false;
	}

	if ( fmt7ImageSettings.offsetX + _CamWidth > fmt7Info.maxWidth )
	{
		AfxMessageBox( _T("CGetPicture::InitCamera() - 카메라 가로 크기를 다시 설정해주세요!(請再設定影像寬度)") );
		return false;
	}

	if ( fmt7ImageSettings.offsetY + _CamHeight > fmt7Info.maxHeight )
	{
		AfxMessageBox( _T("CGetPicture::InitCamera() - 카메라 세로 크기를 다시 설정해주세요!(請再設定影像高度)") );
		return false;
	}

	Format7PacketInfo packetInfo;
	bool valid;

	fmt7ImageSettings.mode			= ( Mode ) MODE_0;
	fmt7ImageSettings.width			= _CamWidth;
	fmt7ImageSettings.height		= _CamHeight;
	fmt7ImageSettings.pixelFormat	= PIXEL_FORMAT_MONO8;
	
	// 이미지 포멧 검증
	error = m_cam.ValidateFormat7Settings( 
		&fmt7ImageSettings, 
		&valid, 
		&packetInfo );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("CGetPicture::InitCamera() - format 7 검증 실패!(Format7驗收失敗)") );
		return false;
	}

	if ( !valid )
	{
		AfxMessageBox( _T("CGetPicture::InitCamera() - format 7 값을 다시 설정해주세요!(請再設定Format7參數)") );
		return false;
	}

	if ( 9000 < packetInfo.recommendedBytesPerPacket )
	{
		packetInfo.recommendedBytesPerPacket = 9000;
	}

	// 카메라 ROI 설정
	error = m_cam.SetFormat7Configuration(
		&fmt7ImageSettings, 
		packetInfo.recommendedBytesPerPacket );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("CGetPicture::InitCamera() - format 7 세팅 실패!(Format7設定失敗)") );
		return false;
	}

	//CSeoilVisionDlg *pMain = (CSeoilVisionDlg*) AfxGetMainWnd();
	//int rpm = pMain->m_nSetRPM;
	//float frame = 60.0 / rpm * 1000.0; // 한 바퀴당 소요시간
	//frame /= _MAXTRI; // 한 바퀴 소요시간 _MAXTRI 등분
	//m_nInterval = ( int )frame;

	SetInterval();

	/*
	frame = 1000.0 / frame;
	frame = floor(frame) + 1;

	Property frmRate;
	frmRate.type		= FRAME_RATE;
	frmRate.absControl	= true;
	frmRate.absValue	= frame;
	frmRate.onOff		= true;

	TRACE( _T("\n----------------------\n") );
	TRACE( _T("RPM: %d\n"), rpm);
	TRACE( _T("frame: %f\n"), frame );
	TRACE( _T("Interval: %d"), m_nInterval );
	TRACE( _T("\n----------------------\n") );

	error = m_cam.SetProperty( &frmRate );
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("CGetPicture::InitCamera() - Frame Rate 세팅 실패!") );
		return false;
	}
	*/

	TRACE(_T("\n---------------------\n"));
	TRACE(_T("카메라 초기화 성공(相机初始化成功)"));
	TRACE(_T("\n---------------------\n"));

	AfxMessageBox( _T("카메라가 정상적으로 연결되었습니다!(连接相机正常)") );

	return true;
}

void CGetPicture::SetInterval()
{
	CSeoilVisionDlg *pMain = (CSeoilVisionDlg*) AfxGetMainWnd();
	int rpm = pMain->m_nSetRPM;
	double frame = 60.0 / rpm * 1000000.0; // 한 바퀴당 소요시간
	frame /= _MAXTRI; // 한 바퀴 소요시간 _MAXTRI 등분

	m_nInterval = ( int )frame;

	TRACE( _T("\n----------------------\n") );
	TRACE( _T("RPM: %d\n"), rpm);
	TRACE( _T("Interval: %d"), m_nInterval );
	TRACE( _T("\n----------------------\n") );
}

void CGetPicture::SetMainDlg(CSeoilVisionDlg *pMain)
{
	m_maindlg = pMain;
}

bool CGetPicture::InitAll()
{
	// 카메라 해상도 확인
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
	{
		return false;
	}

	bool result = true;
	
	// Critical Section 초기화
	cam_1 = new CCriticalSection;
	cam_2 = new CCriticalSection;
	
	// 이미지 초기화
	for (int i = 0; i < _MAX_CAMERAS; i++)
	{
		cam_image[i] = cvCreateImage(
			cvSize(_CamWidth,_CamHeight), 8, 1 );
	}
	// 검사 안한다.
	m_inspect_image = false;
	
	// 처음에 결과 초기화
	ResetResultData();
	
	// 불량 저장 안함
	m_save_error = false;
	m_save_all = false;
	m_save_part_error = false;
	
	// 재시작 X
	m_restart = 0;
	
	// cwlee test
	if (_TEST_NUM != 1 && _TEST_NUM > 0)
	{
		CString fileBase = FILE_BASE_PATH;
		CString fileEnd;

		CString filePath;

		for(int i = 0; i < _TEST_NUM; i++)
		{
			fileEnd.Format(_T("%d.bmp"), i);

			filePath = fileBase + fileEnd;

			imgOk[i] = cvLoadImage(filePath.GetBuffer(), CV_LOAD_IMAGE_GRAYSCALE);
		}
	}
	
	result = InitCamera();

	return true;
}

bool CGetPicture::Init()
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return false;

	// 뷰어 받기
	// 현재는 테스트용이므로 테스트 뷰어 받기
	
	bool result = true;
	
	// 카메라 초기화
	for (int i=0;i<_MAX_CAMERAS;i++)
	{
		// 		 CControlCamera pCamera = new CControlCamera[2];
		// 		 result = result && pCamera[i]->Init(i);
	//	pCamera[i] = new CControlCamera;
		//		result = result && pCamera[i]->Init(i);
		
		// 2008 11 04 한혜민 시리얼 넘버로 깨우기
		int SerialNum;
		if (m_nCamNum==10)		SerialNum = _SERIAL_NUM_10;		// 1 호기
		else if(m_nCamNum==15)	SerialNum = _SERIAL_NUM_15;
		else if(m_nCamNum==20)	SerialNum = _SERIAL_NUM_20;		// 2 호기
		else if(m_nCamNum==25)	SerialNum = _SERIAL_NUM_25;
		else if(m_nCamNum==30)	SerialNum = _SERIAL_NUM_30;		// 3 호기
		else if(m_nCamNum==35)	SerialNum = _SERIAL_NUM_35;
		else if(m_nCamNum==40)	SerialNum = _SERIAL_NUM_40;		// 4 호기
		else if(m_nCamNum==45)	SerialNum = _SERIAL_NUM_45;
		else if(m_nCamNum==50)	SerialNum = _SERIAL_NUM_50;		// 5 호기
		else if(m_nCamNum==55)	SerialNum = _SERIAL_NUM_55;
		else if(m_nCamNum==60)	SerialNum = _SERIAL_NUM_60;		// 6 호기
		else if(m_nCamNum==65)	SerialNum = _SERIAL_NUM_65;
		else if(m_nCamNum==70)	SerialNum = _SERIAL_NUM_70;		// 7 호기
		else if(m_nCamNum==75)	SerialNum = _SERIAL_NUM_75;
		else if(m_nCamNum==80)	SerialNum = _SERIAL_NUM_80;		// 8 호기
		else if(m_nCamNum==85)	SerialNum = _SERIAL_NUM_85;
		else if(m_nCamNum==90)	SerialNum = _SERIAL_NUM_90;		// 9 호기
		else if(m_nCamNum==95)	SerialNum = _SERIAL_NUM_95;
		else if(m_nCamNum==100)	SerialNum = _SERIAL_NUM_100;		// 10 호기
		else if(m_nCamNum==105)	SerialNum = _SERIAL_NUM_105;
		else if(m_nCamNum==110)	SerialNum = _SERIAL_NUM_110;		// 11 호기
		else if(m_nCamNum==115)	SerialNum = _SERIAL_NUM_115;
		else if(m_nCamNum==120)	SerialNum = _SERIAL_NUM_120;		// 12 호기
		else if(m_nCamNum==125)	SerialNum = _SERIAL_NUM_125;
		else if(m_nCamNum==130)	SerialNum = _SERIAL_NUM_130;		// 13 호기
		else if(m_nCamNum==135)	SerialNum = _SERIAL_NUM_135;
		else if(m_nCamNum==140)	SerialNum = _SERIAL_NUM_140;		// 14 호기
		else if(m_nCamNum==145)	SerialNum = _SERIAL_NUM_145;
		else if(m_nCamNum==150)	SerialNum = _SERIAL_NUM_150;		// 15 호기
		else if(m_nCamNum==155)	SerialNum = _SERIAL_NUM_155;
		else if(m_nCamNum==160)	SerialNum = _SERIAL_NUM_160;		// 16 호기
		else if(m_nCamNum==165)	SerialNum = _SERIAL_NUM_165;
		else if(m_nCamNum==170)	SerialNum = _SERIAL_NUM_170;		// 17 호기
		else if(m_nCamNum==175)	SerialNum = _SERIAL_NUM_175;
		else if(m_nCamNum==180)	SerialNum = _SERIAL_NUM_180;		// 18 호기
		else if(m_nCamNum==185)	SerialNum = _SERIAL_NUM_185;
		else if(m_nCamNum==190)	SerialNum = _SERIAL_NUM_190;		// 19 호기
		else if(m_nCamNum==195)	SerialNum = _SERIAL_NUM_195;
		else if(m_nCamNum==200)	SerialNum = _SERIAL_NUM_200;		// 20 호기
		else if(m_nCamNum==205)	SerialNum = _SERIAL_NUM_205;
		else if(m_nCamNum==210)	SerialNum = _SERIAL_NUM_210;		// 21 호기
		else if(m_nCamNum==215)	SerialNum = _SERIAL_NUM_215;
		else if(m_nCamNum==220)	SerialNum = _SERIAL_NUM_220;		// 22 호기
		else if(m_nCamNum==225)	SerialNum = _SERIAL_NUM_225;
		else if(m_nCamNum==230)	SerialNum = _SERIAL_NUM_230;		// 23 호기
		else if(m_nCamNum==235)	SerialNum = _SERIAL_NUM_235;
		else if(m_nCamNum==240)	SerialNum = _SERIAL_NUM_240;		// 24 호기
		else if(m_nCamNum==245)	SerialNum = _SERIAL_NUM_245;
		else if(m_nCamNum==250)	SerialNum = _SERIAL_NUM_250;		// 25 호기
		else if(m_nCamNum==255)	SerialNum = _SERIAL_NUM_255;
		else if(m_nCamNum==260)	SerialNum = _SERIAL_NUM_260;		// 26 호기
		else if(m_nCamNum==265)	SerialNum = _SERIAL_NUM_265;
		
		// 3차 출고분
		else if(m_nCamNum==300)	SerialNum = _SERIAL_NUM_300;		// 3차분-1
		else if(m_nCamNum==305)	SerialNum = _SERIAL_NUM_305;		// 3차분-2
		else if(m_nCamNum==310)	SerialNum = _SERIAL_NUM_310;		// 3차분-3
		else if(m_nCamNum==315)	SerialNum = _SERIAL_NUM_315;		// 3차분-4
		else if(m_nCamNum==320)	SerialNum = _SERIAL_NUM_320;		// 3차분-5
		else if(m_nCamNum==325)	SerialNum = _SERIAL_NUM_325;		// 3차분-6


		// cwlee add
		else if ( m_nCamNum == 400) SerialNum = _SERIAL_NUM_400;
		
	}
	
	// Critical Section 초기화
	cam_1 = new CCriticalSection;
	cam_2 = new CCriticalSection;
	
	// 이미지 초기화
	for (int i=0;i<_MAX_CAMERAS;i++)
		cam_image[i] = cvCreateImage(cvSize(_CamWidth,_CamHeight),8,3);
	
	// 검사 안한다.
	m_inspect_image = false;
	
	// 처음에 결과 초기화
	ResetResultData();
	
	// 불량 저장 안함
	m_save_error = false;
	m_save_all = false;
	m_save_part_error = false;
	
	// 재시작 X
	m_restart = 0;
	
	// cwlee test
	imgOk[0] = cvLoadImage(_T("c:\\ATI\\OK_24_0.bmp"));
	imgOk[1] = cvLoadImage(_T("c:\\ATI\\OK_24_1.bmp"));
	imgOk[2] = cvLoadImage(_T("c:\\ATI\\OK_24_2.bmp"));
	imgOk[3] = cvLoadImage(_T("c:\\ATI\\OK_24_3.bmp"));
	imgOk[4] = cvLoadImage(_T("c:\\ATI\\OK_24_4.bmp"));

	return result;
}


UINT CGetPicture::BlowStrawThread(LPVOID pParam)
{
	CGetPicture *pMyself = ( CGetPicture* )pParam;

	while( pMyself->cam_start[0] )
	{
		for ( int i = 0; i < _REJECT_BUFFER_SIZE; i++ )
		{
			int imgCnt = pMyself->m_iImageCount[0];
			int rejNum = m_rejectnum[i];
			int rejType = m_rejects[i];

			// 2008 11 04 한혜민 -> 소팅 구간 나중에 고칠것
			if ( rejNum == imgCnt - m_nSolgap 
				&& imgCnt - m_nSolgap > 0 
				&& ( rejType == 1 || rejType == 3) )
			{
				if ( imgCnt > _IMAGE_BUFFER_SIZE )
				{
					m_maindlg->RejectX(true);
				}
				m_rejectnum[i] = -1;
			}
			else if ( rejType == 0 
				&& rejNum == imgCnt- m_nSolgap
				&& imgCnt - m_nSolgap > 0 )
			{
				if ( imgCnt > _IMAGE_BUFFER_SIZE )
				{
					m_maindlg->RejectX(false);
				}
				m_rejectnum[i] = -1;
			}
		}
	}

	::AfxEndThread( 0xff );

	return 0;
}




UINT CGetPicture::BlackFlyGrabImage(LPVOID pParam)
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
	{
		AfxMessageBox(_T("카메라 가로, 세로 크기가 잘못되었습니다!(影像宽度和高度有错)"));
		return 0;
	}

	TRACE(_T("m_inspect_image : %d\n"), m_inspect_image );

	LARGE_INTEGER startingTime, endingTime, elapsedMilliseconds;
	
	elapsedMilliseconds.QuadPart = 0;

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency( &frequency );


	CGetPicture *pMyself = (CGetPicture*)pParam;
	
	bool err = true;

	bool saveChk;
	
	int cam_num = 0;
	int buffer_num = 0;
	int imgCnt = _MAXTRI;

	int trash_image = 0;

	BOOL flag;
	
	int imgOkIdx = 0;

	//TriggerMode m_trigger;
	//m_trigger.onOff = true;
	//Error error_temp = pMyself->m_cam.SetTriggerMode(&m_trigger);

	//if(error_temp != PGRERROR_OK)
	//{
	//	AfxMessageBox( _T("SetTriggerMode() - 트리거 모드 On 실패") );
	//}

	// 카메라 캡쳐 시작
	Error error = pMyself->m_cam.StartCapture();
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("BlackFlyGrabImage() - 카메라 캡쳐 시작 실패(獲得影像失敗)") );
		//return 0;
	}

	TRACE(_T("\n--------------------\n"));
	TRACE(_T("캡쳐 시작(获得影像开始)"));
	TRACE(_T("\n--------------------\n"));

	IplImage *pImage = cvCreateImage( cvSize(_CamWidth, _CamHeight), 8, 1 );
	TRACE( _T("Cam Width: %d, Cam Height: %d"), _CamWidth, _CamHeight );

	FlyCapture2::Image capturedImage;
	
	capturedImage.ReleaseBuffer();
	
	CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());

	while(pMyself->cam_start[0])
	{
		// 시간측정 해보기
		// 신호 대기
		if ( imgCnt >= _MAXTRI)
		{
			while( !pMainDlg->m_pCtrlIO->DetectStrawSensor() && pMyself->cam_start[0] );

			imgCnt = 0;
		}

		if ( imgCnt > 0 )
		{
			QueryPerformanceCounter( &endingTime );
			elapsedMilliseconds.QuadPart = endingTime.QuadPart - startingTime.QuadPart;

			elapsedMilliseconds.QuadPart *= 1000000;
			elapsedMilliseconds.QuadPart /= frequency.QuadPart;

			if ( elapsedMilliseconds.QuadPart < pMyself->m_nInterval )
				continue;
		}

		
		pMyself->m_cam.FireSoftwareTrigger();
		QueryPerformanceCounter( &startingTime );

		// 한 장 그랩
		pMyself->LockCriticalSectionImage(0, NULL);

		error = pMyself->m_cam.RetrieveBuffer( &capturedImage );
		if ( error != PGRERROR_OK )
		{
			TRACE( _T("\n-----------------------------------\n") );
			TRACE( _T("CGetPicture::BlackFlyGrabImage() - retrieve buffer Error!\n") );
			TRACE( _T("Error: %s\n"), error.GetDescription() );
			TRACE( _T("-----------------------------------\n") );
		}

		memcpy( pImage->imageData, capturedImage.GetData(), pImage->imageSize );
		capturedImage.ReleaseBuffer();

		if ( _TEST_NUM != 1 && _TEST_NUM != 0)
		{
			imgOkIdx %= _TEST_NUM;
			cvCopyImage( imgOk[imgOkIdx++], pImage );
		}

		if(m_bXAxisFlip == true)
			cvFlip( pImage, pImage );
		if ( m_bYFlip == TRUE )	// Y 방향 Flip인 경우. 
			cvFlip( pImage, pImage, 1 );
		
		pMyself->UnlockCriticalSectionImage( 0, NULL );
	
		// 에러 없는 경우에 한해
		if (err == TRUE)
		{
			// 만약 이미지 저장이면
			if (m_save_image)
			{
				saveChk = pMyself->SaveRefImage( pImage, 0);

				if ( saveChk == false )
				{
					pMyself->cam_start[0] = false;
					break;
				}
			}
			// 검사라면,
			if (m_inspect_image)
			{
				// 2008 09 12 한혜민
				// 안정성 위해 버퍼 1ㅐ만큼은 보림
				if (m_imgCount[0] < 5 * _IMAGE_BUFFER_SIZE)
				{
					pMyself->m_iImageCount[0]++;
					m_imgCount[0]++;
					continue;
				}

				// 메인 이미지 출력
				if (pMyself->m_view)
				{
					if (pMyself->m_camtoview == 0)
					{
						// 30장마다 하나씩 보여줌 ( 메인 화면 이미지 )
						if (pMyself->m_iImageCount[0] % 30 == 0)
						{
							cvResize( pImage, resizeView );
							cvCvtColor( resizeView, rgbResizeView, CV_GRAY2RGB );

							for ( int i = 0; i < pMainDlg->m_numOfInspView; i++ )
							{
								pMainDlg->m_mainInspView[i].DrawImage(
									rgbResizeView, 
									SHOW_MAIN_WIDTH, 
									(int)((640.0 / _CamWidth) * _CamHeight));
							}
						}
					}
				}
				
				// 카운트 계산
				cam_num = 0;
				buffer_num = pMyself->m_iImageCount[0] % _IMAGE_BUFFER_SIZE;
				
				// 검사 수행
				if (pMyself->m_iImageCount[cam_num] >=0)
				{
					pMyself->m_pInspectData[cam_num][buffer_num]->nCamNum = cam_num;
					pMyself->m_pInspectData[cam_num][buffer_num]->nImageBufferNum = buffer_num;
					pMyself->m_pInspectData[cam_num][buffer_num]->nImageCount = pMyself->m_iImageCount[cam_num];
					pMyself->m_pInspectData[cam_num][buffer_num]->pInspector = pMyself;
					
					// 버퍼로 이미지 복사
					cvCopyImage( pImage, pMyself->m_pGrabImageBuffer[0][buffer_num] );
				
					// 이벤트 발생
					flag = m_gEvent[cam_num][buffer_num].SetEvent();
				}
				// 이미지 카운트 증가
				pMyself->m_iImageCount[cam_num]++;
				m_imgCount[cam_num]++;
			}
		}

		imgCnt++;
	}
	
	// 종료
	if ( pMyself->m_cam.IsConnected() == true )
	{
		Error error;
		
		error = pMyself->m_cam.StopCapture();
		if ( error != PGRERROR_OK )
		{
			TRACE( _T("Stop capture 실패!") );
		}
	}

	m_inspect_image = false;

	cvReleaseImage( &pImage );

	TRACE ( _T("%d 카메라 종료(相機關閉)\n"), 0 );
	
	::AfxEndThread(0xff);	
	
	// 리턴
	return TRUE;
}














UINT CGetPicture::LockCriticalSectionImage(UINT uiCamNum, CGetPicture *pPicture)
{
	switch(uiCamNum)
	{
	case 0:
		// 1번 카메라 이미지 잠그기
		cam_1->Lock();
		break;
	case 1:
		// 이번엔 2번
		cam_2->Lock();
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

UINT CGetPicture::UnlockCriticalSectionImage(UINT uiCamNum, CGetPicture *pPicture)
{
	switch(uiCamNum)
	{
	case 0:
		// 1번 카메라 이미지 잠그기
		cam_1->Unlock();
		break;
	case 1:
		// 이번엔 2번
		cam_2->Unlock();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

UINT CGetPicture::SaveRefImage(int Image_ID)
{
	
	// 아직 끝 안남 -> Default 100장
	if (m_saved_ref_image[Image_ID] < m_max_ref_image)
	{
		// 저장하기
		CString filename;
		CString filename_tail;
		
		m_saved_ref_image[Image_ID] ++;
		// 2008 11 05 한혜민 -> 카메라 번호 고정
		filename_tail.Format("_%02d_%03d.bmp", _CAM_NUM + 1, m_saved_ref_image[Image_ID]);
		filename = m_ref_filename + filename_tail;
		
		// 현 이미지 저장
		cvSaveImage(filename,cam_image[Image_ID]);
	}
	else
	{
		// 종료 로직
		bool stop = TRUE;
		
		for (int i=0;i<_MAX_CAMERAS;i++)
		{
			if (m_saved_ref_image[Image_ID] < m_max_ref_image)
			{
				stop = FALSE;
				break;
			}
		}
		
		if (stop)
		{
			
		}// 진짜종료 -> 로직 넣기
	}
	
	return TRUE;
}

bool CGetPicture::SaveRefImageStart()
{
	// 영상 저장
	if (!m_save_image)
	{
		LPCTSTR lpszFilter;
		int Inspect_ID = 0;
		
		lpszFilter = "지원파일(*.bmp)|*.bmp|";
		CFileDialog dlg(FALSE, "bmp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, lpszFilter);
		dlg.m_ofn.lpstrTitle = "참조 영상 저장(保存參考用影像)";
		
		if(dlg.DoModal() == IDOK)
		{
			int i;
			int n;
			CString filename;
			filename = dlg.GetPathName();
			n = filename.ReverseFind('.');

			m_ref_filename = filename.Left(n);
			m_max_ref_image = 100;
			
			for(i = 0; i < _MAX_CAMERAS; i++)
				m_saved_ref_image[i] = 0;
			
			// 카메라 시작
			m_save_image = TRUE;
			
			//if(StartCamera() == TRUE)
			if ( StartBlackFlyCamera() )
			{
				CString strTemp;
				strTemp.Format("참조 영상 저장 시작(參考影像保存開始)");
				//PrintMsg(strTemp, ez_NO_ERROR, Inspect_ID);
				
				return true;
			}
			else
			{
				CString strTemp;
				strTemp.Format("참조 영상을 저장할 수 없습니다.(不能保存參考影像)");
				//PrintMsg(strTemp, ez_ERROR_INSPECT_TABLET_SAVE_REFERENCE, Inspect_ID);
				
				m_save_image = FALSE;
				return FALSE;
			}
		}		
	}
	
	return false;
}


UINT CGetPicture::StartBlackFlyCamera()
{
	ZeroMemory(cam_start, sizeof(BOOL) * _MAX_CAMERAS);
	
	CSeoilVisionDlg *pMainDlg = (CSeoilVisionDlg*)(AfxGetApp()->GetMainWnd());

	cam_start[0] = TRUE;
	if(pMainDlg->m_ioType == 2)
	{
		m_pGrabThread[0] = ::AfxBeginThread(BlackFlyGrabImage_PLC, (LPVOID*)this);
	}
	else
	{
		m_pGrabThread[0] = ::AfxBeginThread(BlackFlyGrabImage, (LPVOID*)this);
	}
	//m_pGrabThread[0] = ::AfxBeginThread(BlackFlyGrabImage, (LPVOID*)this);
	m_hGrabThreadHandle[0] = m_pGrabThread[0]->m_hThread;
	
	// 카메라에 연결된 Grab thread 3개가 모두 실행되는 것을 확인한 후 다음 step을 계속 진행 
	while(!(::WaitForMultipleObjects(_MAX_CAMERAS, m_hGrabThreadHandle, TRUE, 0) == WAIT_TIMEOUT))
	{
		TRACE( _T("스레드 생성중입니다.(正在生成Thread)\n") );
	}
	
	return true;
}


UINT CGetPicture::StartCamera()
{
	// 카메라 돌리기
	
//	THREAD_DATATYPE	* pDataOfCamera;
//	CControlCamera	* pCamera;
	
	ZeroMemory(cam_start, sizeof(BOOL) * _MAX_CAMERAS);
	
	int nCount;
	
	for(nCount = 0; nCount < _MAX_CAMERAS; ++nCount) {
		
//		pDataOfCamera = m_vCameraDoc[nCount];
		
		//if(pDataOfCamera) {
			
		//	pCamera = pDataOfCamera->pCamera;
		//	pDataOfCamera->pPicture = this;
		//	pDataOfCamera->nImageNum = 0;
			//			pDataOfCamera->pImage = cvCreateImage(cvSize(1024,768),8,3);
		//	cam_start[nCount] = TRUE;
		//	m_pGrabThread[nCount] = ::AfxBeginThread(GrabImage, (LPVOID*)pDataOfCamera);
		//	m_hGrabThreadHandle[nCount] = m_pGrabThread[nCount]->m_hThread;
			
		//}
	}
	
	// 카메라에 연결된 Grab thread 3개가 모두 실행되는 것을 확인한 후 다음 step을 계속 진행 
	//while(!(::WaitForMultipleObjects(_MAX_CAMERAS, m_hGrabThreadHandle, TRUE, 0) == WAIT_TIMEOUT)) TRACE("스레드 생성중 기다리셈\n"); 
	
	//	if ( IsWindow( m_pSettingSheet->GetSafeHwnd() ) ) {
	//		((CEzCameraSettingPropSheet*)m_pSettingSheet)->RemoveROIPage();		
	//	}		
	
	return true;
}

bool CGetPicture::SaveRefImageStop()
{
	// 끝내기
	if(m_save_image)
	{
		m_save_image = FALSE;
		
		if(StopCamera() == true)
		{
			CString strTemp;
			strTemp.Format("참조 영상 저장 중지(參考影像保存停止)");
			//PrintMsg(strTemp, ez_NO_ERROR);
			
			return true;
		}
		else
		{
			CString strTemp;
			strTemp.Format("참조 영상 저장 중지 실패(參考影像停止失敗)");
			//PrintMsg(strTemp, ez_ERROR_INSPECT_TABLET_SAVE_REFERENCE_END);
			
			return false;
		}
	}
	else
	{
		return true;
	}
}

bool CGetPicture::StopCamera()
{
	// 종료 대기 후 일괄 종료
	// 작업중이던 thread는 한 단위의 처리를 마무리 하고 종료되도록 
	ZeroMemory(cam_start, sizeof(BOOL) * _MAX_CAMERAS);
	
	// 카메라 쓰레드 종료 대기
	::WaitForMultipleObjects(_MAX_CAMERAS, m_hGrabThreadHandle, TRUE, INFINITE);
	
	//if ( IsWindow( m_pSettingSheet->GetSafeHwnd() ) ) {
	//	((CEzCameraSettingPropSheet*)m_pSettingSheet)->AddROIPage();
	//}
	
	return true;	
}

UINT CGetPicture::InspectThreadFunction(LPVOID pParam)
{
	InspectThread *pData = (InspectThread *)pParam;
	
//	g_ThreadEvent[pData->nCamNum][pData->nImageBufferNum].SetEvent();

	// 카메라 도는 동안,
	while (cam_start2[pData->nCamNum])
	{
		// 먼저 멈춘다.
		m_gEvent[pData->nCamNum][pData->nImageBufferNum].Lock();
		
		// 풀리면 검사 돈다
		if (cam_start2[pData->nCamNum])
		{
			pData->pInspector->DoInspect(pData->nCamNum, pData->nImageBufferNum, pData->nImageCount, pData);
		}
	}
	
	// 쓰레드 끝내기
	TRACE("Inspect Thread [ %d ][ %d ] : 종료\n", pData->nCamNum, pData->nImageBufferNum);
	::AfxEndThread(0xf0);
	return 0;
	
}


bool CGetPicture::DoInspect(int Inspect_ID, int Buffer_Num, int Image_Count, InspectThread *datas)
{
	// cwlee add
	USHORT errStat = NOTHING;

	// 현 사진에서의 빨대 번호 얻기
	//long straw_num[_MAXROI];

	if(_MAXROI <= 0 || _MAXROI > 9)
		return false;

	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return false;

	long straw_num[10];
	USHORT status = NOTHING;
	
	// 검사 끝났는지 여부 보기
	bool finalized = false;
	bool findStraw = false;
	int Rejectposition[2]={0,0};
	
	if (Inspect_ID == 0)
	{
		// 현 thread에서 관찰할 빨대 개수
		for ( int i = 0; i < _MAXROI; i++ )
		{
			straw_num[i] = Image_Count - i;
		}

		// 사진 버리기
		if (Image_Count == 1-1)
		{
			straw_num[2] = -1;
			straw_num[1] = -1;
		}
		else if (Image_Count == 2-1)
		{
			straw_num[2] = -1;
		}
	}
	else
	{
		for ( int i = 0; i < _MAXROI; i++ )
		{
			straw_num[i] = Image_Count - i -_MAXROI;
		}
	}
	
	// 현재 몇 회 검사째인지도 보아야 함
	
	// 사진 걸기
	bool colorthresholding = false;
	inspect_temp = true;
	
	m_process[Inspect_ID][Buffer_Num]->SetImageSize( _CamWidth, _CamHeight );

	if(m_pGrabImageBuffer[Inspect_ID][Buffer_Num] != NULL)
	{
		if (m_process[Inspect_ID][Buffer_Num]->m_colorstraw == 0)
		{
			m_process[Inspect_ID][Buffer_Num]->SetMonoImage(m_pGrabImageBuffer[Inspect_ID][Buffer_Num]);
		}	
		else if (m_process[Inspect_ID][Buffer_Num]->m_colorstraw > 0 && m_process[Inspect_ID][Buffer_Num]->m_colorthres[m_process[Inspect_ID][Buffer_Num]->m_colorstraw-1] != NULL)// color인 경우.
		{
		
		}
	}
	else
	{
		m_process[Inspect_ID][Buffer_Num]->s_inspect_temp = false;
	}
	
	for (int i = 0; i < _MAXROI; i++ )
	{
		bool bDoInspect = TRUE;
		
		if(_MAXTRI != 1)
		{
			if ( i % _MAXTRI == 0)
			{
				findStraw = false;
			}
		}
		
		if ( ( i == 0 && straw_num[i] > -1 ) )
		{
			bDoInspect = TRUE;
		}	
		else if ( straw_num[i] > -1 )
		{
			// 현재 상태를 얻는다.
			if ( Image_Count < 0 )
			{
				// 사진 없음
				return true;
			}
			
			status = GetStrawStatus( straw_num[i] );

			// 없는 경우
			if ( status == NO_STRAW )
			{
				bDoInspect = TRUE;
			}
			
			// 이미 불량 (에티켓 불량인 경우 나타날때까지 계속 검사)
			else if ( status >= PATH_INSPECT && status != STRAW_ETIQUETTE )
			{
				// 넘기기
				bDoInspect = FALSE;
				errStat = status; // cwlee add
				//i = _MAXROI - 1;  // cwlee add
			}
			else if ( status != NO_STRAW ) // 정상 or 에티켓 불량
			{
				bDoInspect = TRUE;
			}
		}
		
		if( !m_process[Inspect_ID][Buffer_Num]->s_inspect_temp )
		{
			bDoInspect = false;
		}
		
		// 검사하기 (불량이 아닌 빨대에 대해 진행 - 에티켓 불량 제외)
		UINT result;
		if ( bDoInspect && straw_num[i] > -1 )
		{
			// 검사루틴
			UINT result = StrawInspect(Inspect_ID,Buffer_Num,Image_Count,i, findStraw);
			
			// thread 실행 순서가 꼬일 때 순서 맞추기
			// 응급처치때문에 첫 검사값이 이상해짐.
			// 아래 주석을 풀지 말고 생각해보기.
			//status = GetStrawStatus(straw_num[i]); 


			// 2009 04 22 한혜민
			// 단 한 번이라도 에러 걸린건 다음에 노스트로 되어도 일단 무조건 불어내도록 함
			// 대형 이물로 인해 노 스트로 되는 경우에 대비하기 위함
			// 서일 요청사항임
			if ( result == NO_STRAW )
			{
				if (status == NO_STRAW || status == NOTHING)
				{
					AddStrawStatus(straw_num[i], NO_STRAW);
				}
				else
				{
					// cwlee del: 다시 추가할 필요 없음
					//  - status는 원래 저장되어 있는 값이므로 필요 없음
					//AddStrawStatus(straw_num[i],status);
					findStraw = true;
				}
			}
			else // 정상, 불량
			{ 
				if(i == 0) {
					bStrawStart = TRUE;
					bEtiqueFind = FALSE;
					bStrawEnd = FALSE;
				} else if(i == _MAXROI - 1) {
					bStrawEnd = TRUE;
					bStrawStart = FALSE;
				}
				
				// case:
				//  - 에티켓 불량 --> 정상 = 정상
				//  - 에티켓 불량 --> 불량 = 불량
				//  - 정상 --> 에티켓 불량 = 정상
				//  - 불량 --> 에티켓 불량 = 불량

				// 에티켓 불량 --> 정상: 정상
				if(status == STRAW_ETIQUETTE && result == STRAW_NO_ERROR) 
				{
					AddStrawStatus(straw_num[i], STRAW_NO_ERROR);
				}
				// 정상 --> 에티켓 불량: 정상
				else if(status == STRAW_NO_ERROR && result == STRAW_ETIQUETTE) 
				{
					AddStrawStatus(straw_num[i], STRAW_NO_ERROR);
				}
				// 기존 결과가 불량: 유지
				else if (status != STRAW_NO_ERROR && status != NOTHING && status != NO_STRAW && status != STRAW_ETIQUETTE) 
				{
					AddStrawStatus(straw_num[i], status); // cwlee del: 그냥 유지
				}
				// 그 외: 결과 반영
				else 
				{
					AddStrawStatus(straw_num[i], result);
				}
				

				// 불량
				if(result > PATH_INSPECT && result < NO_STRAW && findStraw == false && result != STRAW_NO_ERROR)
				{
					if(result == STRAW_ETIQUETTE)
					{
						if(i == _MAXROI - 1)
						{
							m_Rejectposition = Buffer_Num;
						}
					}
					else
					{
						m_Rejectposition = Buffer_Num;
					}
				}


				findStraw = TRUE;
			}
		}
		else if (straw_num[i] > -1)	// bDoInspect 가 false인 경우에만 사용.
		{
			/*
			// 빨대 유무만 판별
			UINT result;
			result = StrawExistanceCheck(Inspect_ID, Buffer_Num, Image_Count, i);
			// 결과 갱신
			if ( result == NO_STRAW )
				AddStrawStatus(straw_num[i], result);
			*/
		}
		
		if (straw_num[i] > -1 )
		{
			result = GetStrawStatus(straw_num[i]);
		}

		// 동작 확인용
//#define _MY_TEST
#ifdef _MY_TEST
		TRACE(_T("Thread[%2d,%d]\ts[%4d]:%d\ts[%4d]:%d\ts[%4d]:%d\ts[%4d]:%d\ts[%4d]:%d\ts[%4d]:%d\ts[%4d]:%d\ts[%4d]:%d\ts[%4d]:%d\n"),
			Buffer_Num,
			i,
			straw_num[0], GetStrawStatus(straw_num[0]),
			straw_num[1], GetStrawStatus(straw_num[1]),
			straw_num[2], GetStrawStatus(straw_num[2]),
			straw_num[3], GetStrawStatus(straw_num[3]),
			straw_num[4], GetStrawStatus(straw_num[4]),
			straw_num[5], GetStrawStatus(straw_num[5]),
			straw_num[6], GetStrawStatus(straw_num[6]),
			straw_num[7], GetStrawStatus(straw_num[7]),
			straw_num[8], GetStrawStatus(straw_num[8])
			);
#endif

		if ( Inspect_ID == 0 && i == _MAXROI-1 && straw_num[i] > -1 )
		{
			// 2009 05 21 한혜민
			// 노스트로 아니면 solcount 비워서 no_straw 위치 정렬함
			if ( GetStrawStatus( straw_num[i] ) != NO_STRAW )
			{
				solblowNoStrawCnt = 0;
				solblowcnt = 0;
				m_maindlg->OnNoStrawSetting(1);
			}
			
			// 시간 계산
			_ftime(&m_timeFinish);
			m_totalTime = (ULONG)(m_timeFinish.time-m_timeStart.time);
			
			long solcnt=0;
			
			for( int tmpIdx = 0; tmpIdx < _MAXROI; tmpIdx++ )
			{
				status = GetStrawStatus( straw_num[i - tmpIdx] );

				if( status == NO_STRAW )
				{
					solcnt++;
					solblowNoStrawCnt++;
				}
			}

			if ( solblowNoStrawCnt > m_maindlg->m_EmptySolGap )
			{
				m_maindlg->OnNoStrawSetting(-1);
				solblowNoStrawCnt=0;
			}
			
			if ( bStrawEnd==TRUE && bEtiqueFind==FALSE
				&& m_process[Inspect_ID][Buffer_Num]->m_bUseInspectEtq==TRUE )		// 에티켓 검사 여부.
			{
				result = 150;									// 에러시 폭 에러.
			}	
			if(m_process[Inspect_ID][Buffer_Num]->m_bUseInspectEtq==TRUE)		// Log 남기기.
			{
				
			}
			
			/////////////////////////////////cwlee add
			if (bDoInspect == FALSE)
			{
				//result = errStat;
			}/////////////////////////////////////////

			if (result == STRAW_NO_ERROR && Image_Count > 5 * _IMAGE_BUFFER_SIZE)
			{
				m_inspect_result.ok++;
			}
			else if (result != NO_STRAW && Image_Count > 5 * _IMAGE_BUFFER_SIZE)
			{
				m_inspect_result.ng++;	

				switch(result)
				{
				case STRAW_POK:
					m_inspect_result.ng_pok++;
					break;
				case STRAW_ANGLE:
					m_inspect_result.ng_angle++;
					break;
				case STRAW_MAGURI:
					m_inspect_result.ng_maguri++;
					break;
				case STRAW_JABARA:
					m_inspect_result.ng_jabara++;
					break;
				case STRAW_CHUCK:
					m_inspect_result.ng_chuck++;
					break;
				case STRAW_INSERT:
					m_inspect_result.ng_insert++;
					break;
				case STRAW_IMUL:
					m_inspect_result.ng_imul++;
					break;
				case STRAW_IMUL_TOP:
					m_inspect_result.ng_imul++;
					m_inspect_result.ng_imul_top++;
					break;
				case STRAW_IMUL_MID:
					m_inspect_result.ng_imul++;
					m_inspect_result.ng_imul_mid++;
					break;
				case STRAW_IMUL_JABARA:
					m_inspect_result.ng_imul++;
					m_inspect_result.ng_imul_jabara++;
					break;
				case STRAW_IMUL_CHUCK:
					m_inspect_result.ng_imul++;
					m_inspect_result.ng_imul_chuck++;
					break;
				case STRAW_IMUL_INSERT:
					m_inspect_result.ng_imul++;
					m_inspect_result.ng_imul_insert++;
					break;
				case 150:
					result = STRAW_POK;
					m_inspect_result.ng_pok++;
					bStrawEnd =FALSE;
					break;
				case STRAW_ETIQUETTE:
					m_inspect_result.ng_etiquette++;
					break;
				case STRAW_BLACKPOINT:
					m_inspect_result.ng_imul++;
					break;
				}
			}
			
			// 빨대 불기 설정
			if (result >=PATH_INSPECT && (result < NO_STRAW || (m_bUseSolNoStraw==TRUE && solcnt >= _MAXTRI && solblowcnt >= _MAXTRI)) && Image_Count > 5*_IMAGE_BUFFER_SIZE)
			{
				REJECT_INFO temp;	
				temp.m_status = false;
				temp.m_strawNum = Image_Count;
				
				// 빈빨대 불기
				if (m_bUseSolNoStraw == TRUE && solcnt >= _MAXTRI && solblowcnt >= _MAXTRI && result == NO_STRAW)
				{
					m_rejects[(Image_Count) % _REJECT_BUFFER_SIZE] = 3;
					m_rejectnum[(Image_Count) % _REJECT_BUFFER_SIZE] = Image_Count;
				}
				// 불량빨대 불기
				else
				{
					m_rejects[Image_Count % _REJECT_BUFFER_SIZE] = 1;
					m_rejectnum[Image_Count % _REJECT_BUFFER_SIZE] = Image_Count;
				}

				solblowcnt=0;
			}
			else if (Image_Count > 5*_IMAGE_BUFFER_SIZE)
			{
				REJECT_INFO temp;
				temp.m_status = true;
				temp.m_strawNum = Image_Count;
				if ( m_rejects[(Image_Count) % _REJECT_BUFFER_SIZE]!=3 )
				{
					m_rejects[Image_Count % _REJECT_BUFFER_SIZE] = 0;
					m_rejectnum[Image_Count % _REJECT_BUFFER_SIZE] = Image_Count;
				}
			}
			// 업데이트
			m_maindlg->UpdateStatus(
				m_inspect_result.ok, m_inspect_result.ng, m_inspect_result.ng_angle,
				m_inspect_result.ng_pok, m_inspect_result.ng_maguri, m_inspect_result.ng_jabara,
				m_inspect_result.ng_chuck, m_inspect_result.ng_insert, m_inspect_result.ng_imul,
				m_inspect_result.ng_imul_top, m_inspect_result.ng_imul_mid, m_inspect_result.ng_imul_jabara, 
				m_inspect_result.ng_imul_chuck, m_inspect_result.ng_imul_insert,
				m_inspect_result.ng_etiquette
				);
			m_maindlg->UpdateTime(m_totalTime/3600,(m_totalTime%3600)/60,m_totalTime%60);

			// 불량 정보 업데이트 ( 우측 리스트 )
			if(result >= 100 && result <900 && m_Rejectposition >= 0 && m_Rejectposition<500)
			{
				m_maindlg->UpdateRejectInfo(result,m_inspect_result.ng, &( m_process[Inspect_ID][m_Rejectposition]->m_ResultValue ) );
			}
		}
		else if (Inspect_ID == 0 && i == _MAXROI-1)
		{
			// 소팅 알고리즘
			
			if (result >=PATH_INSPECT && result < NO_STRAW )
			{
				REJECT_INFO temp;
				temp.m_status = false;
				temp.m_strawNum = Image_Count;
				
			}
			else
			{
				REJECT_INFO temp;
				temp.m_status = true;
				temp.m_strawNum = Image_Count;
			}
		}
	}	

	solblowcnt++; 
	return true;
}

void CGetPicture::ResetStrawStatus()
{
	// 상태 초기화
	memset(m_strawStatus, NOTHING, sizeof(m_strawStatus));

	m_reject.clear();
	
	for (int Count = 0; Count < _MAX_CAMERAS ; Count++)
	{
		m_iImageCount[0] = 0;
	}
}

bool CGetPicture::AddStrawStatus(int nStrawNum, USHORT nStatus)
{
	m_strawStatus[nStrawNum % _STATUS_BUFFER_SIZE] = nStatus;
	
	return true;
}

USHORT CGetPicture::GetStrawStatus(int nTabletNum)
{
	return m_strawStatus[nTabletNum % _STATUS_BUFFER_SIZE];
}

void CGetPicture::SaveErrorImage(CString errType, CvRect areaTmp, int imgCnt, int bufNum, int nowCnt)
{
	CvRect area = areaTmp;

	if(m_save_part_error == TRUE)
	{
		CString filename;
		filename.Format(_T("c:\\Image\\%ld_%ld_%s_%d.bmp"), imgCnt, bufNum, errType.GetBuffer(), nowCnt);

		cvSetImageROI(m_pGrabImageBuffer[0][bufNum], area);
		cvSaveImage(filename, m_pGrabImageBuffer[0][bufNum]);
		cvResetImageROI(m_pGrabImageBuffer[0][bufNum]);
	}
	else if(m_save_error == TRUE)
	{
		CString filename;
		filename.Format(_T("c:\\Image\\%ld_%ld_%s_%d.bmp"), imgCnt, bufNum, errType.GetBuffer(), nowCnt);

		cvSaveImage(filename, m_pGrabImageBuffer[0][bufNum]);
	}
}

UINT CGetPicture::StrawInspect(int Inspect_ID, int Buffer_Num, int Image_Count, int now_count, bool findStraw)
{
	// 유무판별
	int isExist = StrawExistanceCheck(Inspect_ID,Buffer_Num,Image_Count,now_count);
	
	if(isExist == -1)
	{
		return STRAW_INSERT;
	}
	if (isExist == NO_STRAW || findStraw==TRUE) // findStraw == FALSE;
	{
		return NO_STRAW;
	}
	
	// 저장
	if (m_save_all == TRUE)
	{
		CString filename;	
		filename.Format("c:\\Image\\Image_%ld_%ld.bmp",Image_Count,_CAM_NUM);
		cvSaveImage(filename,m_pGrabImageBuffer[Inspect_ID][Buffer_Num]);
	}
	
	

	// 검사 옵션에 따라 검사 수행
	// 폭->폭검사->1각->2각->비뚤림->마구리->자바라->축관->역삽->이물 순 검사
	
	// 검사 실시 전 설정값 넘기기
	m_process[Inspect_ID][Buffer_Num]->m_CannyLow			 = m_pre->m_imageView.m_inspect_setting.m_CannyLow;
	m_process[Inspect_ID][Buffer_Num]->m_CannyHigh			 = m_pre->m_imageView.m_inspect_setting.m_CannyHigh;
	m_process[Inspect_ID][Buffer_Num]->m_CannyApp			 = m_pre->m_imageView.m_inspect_setting.m_CannyApp;
	m_process[Inspect_ID][Buffer_Num]->m_Magunus_Slide		 = m_pre->m_imageView.m_inspect_setting.m_Magunus_Slide;
	m_process[Inspect_ID][Buffer_Num]->m_Magunus_Slide2		 = m_pre->m_imageView.m_inspect_setting.m_Magunus_Slide2;
	m_process[Inspect_ID][Buffer_Num]->m_Magunus_SlideLength = m_pre->m_imageView.m_inspect_setting.m_Magunus_SlideLength;
	m_process[Inspect_ID][Buffer_Num]->m_minChuck			 = cvRound((m_pre->m_imageView.m_inspect_setting.m_minChuck) / (m_pre->m_imageView.m_inspect_setting.resol)); // ADD cwlee (원래 cvRound 없었음)
	m_process[Inspect_ID][Buffer_Num]->m_maxChuck			 = cvRound((m_pre->m_imageView.m_inspect_setting.m_maxChuck) / (m_pre->m_imageView.m_inspect_setting.resol)); 
	m_process[Inspect_ID][Buffer_Num]->m_Slide				 = m_pre->m_imageView.m_inspect_setting.m_Slide;
	m_process[Inspect_ID][Buffer_Num]->m_SlideLength		 = m_pre->m_imageView.m_inspect_setting.m_SlideLength;

	//축관 검사에서 쓰는 값임...
	m_process[Inspect_ID][Buffer_Num]->m_jabara.slide_up = 0.01;
	m_process[Inspect_ID][Buffer_Num]->m_jabara.up_slide = 0.01;
	m_process[Inspect_ID][Buffer_Num]->m_jabara.slide_down = 0.01;
	m_process[Inspect_ID][Buffer_Num]->m_jabara.down_slide = 0.01;

	if(m_pGrabImageBuffer[Inspect_ID][Buffer_Num]->imageData != 0)
	{

		// 1. 마구리 검사 유무 ------------------------------ // m_ThredImageMaguri
		if(m_pre->m_imageView.m_inspect_ox.maguri)
		{
			m_process[Inspect_ID][Buffer_Num]->SetROI(
				m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].x,
				m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y,
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x),
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].y));

			bool maguriResult = m_process[Inspect_ID][Buffer_Num]->InspectMaguri(false);

			if(!maguriResult)
			{
				SaveErrorImage(_T("maguri"), m_process[Inspect_ID][Buffer_Num]->GetROI(), Image_Count, Buffer_Num, now_count);
				return STRAW_MAGURI;
			}

			// 2. 마구리 옆면 검사 유무 ------------------------------	m_ThredImageMaguri
			if(m_pre->m_imageView.m_inspect_ox.maguri_side)
			{
				bool maguriSideResult = m_process[Inspect_ID][Buffer_Num]->InspectMaguriSide(false);
				if(!maguriSideResult)
				{
					SaveErrorImage(_T("maguri_side"), m_process[Inspect_ID][Buffer_Num]->GetROI(), Image_Count, Buffer_Num, now_count);
					return STRAW_MAGURI;
				}
			}

			// 14. 마구리 찍힘 검사 유무 ------------------------------	m_ThredImageMaguri
			if(m_pre->m_imageView.m_inspect_ox.maguri_angle)
			{
				bool maguriAngleResult = m_process[Inspect_ID][Buffer_Num]->InspectMaguriAngle(false);
				if(!maguriAngleResult)
				{
					SaveErrorImage(_T("maguri_angle"), m_process[Inspect_ID][Buffer_Num]->GetROI(), Image_Count, Buffer_Num, now_count);
					return STRAW_MAGURI;
				}
			}
		}

		// 15. 상단 찍힘 검사 유무 ----------------------------------  m_ThredImageMaguri
		if(m_pre->m_imageView.m_inspect_ox.top_angle && m_pre->m_imageView.m_inspect_ox.maguri)
		{
			if(m_pre->m_imageView.m_inspect_ox.jabara) // 자바라 검사 유무
			{
				m_process[Inspect_ID][Buffer_Num]->SetROI(
					m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x,
					m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x - m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].x),
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].y));
				bool topAngleResult = m_process[Inspect_ID][Buffer_Num]->InspectTopAngle(false);
				if(!topAngleResult)
				{
					SaveErrorImage(_T("top_angle"), m_process[Inspect_ID][Buffer_Num]->GetROI(), Image_Count, Buffer_Num, now_count);
					return STRAW_ANGLE;
				}
			}
			else if(m_pre->m_imageView.m_inspect_ox.chuck) // 축관 검사 유무
			{
				m_process[Inspect_ID][Buffer_Num]->SetROI(
					m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x,
					m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x) / 2,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].y));
				bool topAngleResult = m_process[Inspect_ID][Buffer_Num]->InspectTopAngle(false);
				if(!topAngleResult)
				{
					SaveErrorImage(_T("top_angle"), m_process[Inspect_ID][Buffer_Num]->GetROI(), Image_Count, Buffer_Num, now_count);
					return STRAW_ANGLE;
				}
			}
		}

		// 16. 중단 찍힘 검사 유무 ----------------------------------  m_ThredImageMaguri
		if(m_pre->m_imageView.m_inspect_ox.mid_angle && m_pre->m_imageView.m_inspect_ox.chuck)
		{
			if(m_pre->m_imageView.m_inspect_ox.jabara) // 자바라 검사 유무
			{
				m_process[Inspect_ID][Buffer_Num]->SetROI(
					m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].x,
					m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].y,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].x - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x) - g_midNotInspArea,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].y));
				bool midAngleResult = m_process[Inspect_ID][Buffer_Num]->InspectMidAngle(false);
				if(!midAngleResult)
				{
					SaveErrorImage(_T("mid_angle"), m_process[Inspect_ID][Buffer_Num]->GetROI(), Image_Count, Buffer_Num, now_count);
					return STRAW_POK;
				}
			}
			else if(m_pre->m_imageView.m_inspect_ox.maguri) // 마구리 검사 유무
			{
				m_process[Inspect_ID][Buffer_Num]->SetROI(
					(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x + m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x) / 2,
					m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x) / 2 - g_midNotInspArea,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].y));
				bool midAngleResult = m_process[Inspect_ID][Buffer_Num]->InspectMidAngle(false);
				if(!midAngleResult)
				{
					SaveErrorImage(_T("mid_angle"), m_process[Inspect_ID][Buffer_Num]->GetROI(), Image_Count, Buffer_Num, now_count);
					return STRAW_POK;
				}
			}
		}
		
		// 5. 축관 검사 유무 -----------------------------  m_ThredImageChuck
		if (m_pre->m_imageView.m_inspect_ox.chuck)
		{
			CvRect	area = cvRect(
				m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x,
				m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].y,
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[1].x),
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[1].y)
				);
			
			// 검사
			bool bInspResult = m_process[Inspect_ID][Buffer_Num]->CheckChuck(
				area, 
				m_pre->m_imageView.m_inspect_setting.resol,
				false );
			
			// 에러
			if(!bInspResult)
			{
				SaveErrorImage(_T("chuck"), area, Image_Count, Buffer_Num, now_count);
				
				return STRAW_CHUCK;
			}

			// 17. 축관 찍힘 검사 유무 ------------------------------   m_ThredImageChuck
			if(m_pre->m_imageView.m_inspect_ox.chuck_angle)
			{
				m_process[Inspect_ID][Buffer_Num]->SetROI(
					area.x + g_chuckLeft,
					area.y, 
					area.width - g_chuckLeft, 
					area.height);
				bool insertAngleResult = m_process[Inspect_ID][Buffer_Num]->InspectChuckAngle(false);
				if(!insertAngleResult)
				{
					SaveErrorImage(_T("chuck_angle"), area, Image_Count, Buffer_Num, now_count);
					return STRAW_CHUCK;
				}
			}
		}

		bool insertResult = true;
		
		// 6. 역삽 검사 유무 ------------------------------  m_ThredImageInsert
		if (m_pre->m_imageView.m_inspect_ox.insert)
		{
			CvRect area = cvRect(
				m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x,
				m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y,
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].x),
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].y)
				);
			
			// 검사
			insertResult = m_process[Inspect_ID][Buffer_Num]->CheckInsert(area, 1, false, m_pre->m_imageView.m_inspect_setting.m_Slide, m_pre->m_imageView.m_inspect_setting.m_SlideLength, m_pre->m_imageView.m_inspect_setting.stabbed_insert_notinspect, m_pre->m_imageView.m_inspect_setting.stabbed_insert_pix);
			
			// 에러
			if(!insertResult)
			{
				SaveErrorImage(_T("insert"), area, Image_Count, Buffer_Num, now_count);

				return STRAW_INSERT;
			}

			
			
			// 내측 각도------------------------------
			/*
			if(m_pre->m_imageView.m_inspect_ox.insert_angle)
			{
				m_process[Inspect_ID][Buffer_Num]->SetROI(
					m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x,
					m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].x),
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].y));
				bool insertAngleResult = m_process[Inspect_ID][Buffer_Num]->InspectInsertAngle(false);
				if(!insertAngleResult)
				{
					SaveErrorImage(_T("insert_angle"), area, Image_Count, Buffer_Num, now_count);
					return STRAW_INSERT;
				}
			}
			*/
			
			
		}

		// 18. 내측 찍힘 검사 유무 ------------------------------  m_ThredImageInsert
		
		if(insertResult == true)
		{
			if(m_pre->m_imageView.m_inspect_ox.insert_angle)
			{
				CvRect area = cvRect(
					m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x,
					m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y,
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].x),
					abs(m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].y)
					);

				// 검사
				bool insertAngleResult = m_process[Inspect_ID][Buffer_Num]->CheckInsertStabbed(area, 1, false, m_pre->m_imageView.m_inspect_setting.m_Slide, m_pre->m_imageView.m_inspect_setting.m_SlideLength, m_pre->m_imageView.m_inspect_setting.stabbed_insert_notinspect, m_pre->m_imageView.m_inspect_setting.stabbed_insert_pix);
				
				if(!insertAngleResult)
				{
					SaveErrorImage(_T("insert_angle"), area, Image_Count, Buffer_Num, now_count);
					return STRAW_INSERT;
				}
			}

		}

		
		

		// 3. 자바라 검사 유무 ------------------------------	  m_binaryImage
		if ( m_pre->m_imageView.m_inspect_ox.jabara )
		{
			CvRect area = cvRect(
				m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].x,
				m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].y,
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].x),
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].y)
				);
			
			// 내부 검사
			bool jabaraResult = m_process[Inspect_ID][Buffer_Num]->InspectJabara(
				area,
				m_pre->m_imageView.m_inspect_setting.jabara_num,
				m_pre->m_imageView.m_inspect_setting.jabara_margin_of_error,
				10,
				m_pre->m_imageView.m_inspect_setting.jabara_recognize,
				m_pre->m_imageView.m_inspect_setting.jabara_distance_stdev,
				m_pre->m_imageView.m_inspect_setting.jabara_width_stdev,
				false
				);


			// 4. 자바라 경계 검사 유무
			if( jabaraResult == true && m_pre->m_imageView.m_inspect_ox.jabara_edge )
			{
				// 표면 검사
				jabaraResult = m_process[Inspect_ID][Buffer_Num]->InspectJabaraEdge(
					area,
					m_pre->m_imageView.m_inspect_setting.jabara_num,
					m_pre->m_imageView.m_inspect_setting.jabara_margin_of_error,
					m_pre->m_imageView.m_inspect_setting.jabara_gradient_stdev,
					m_pre->m_imageView.m_inspect_setting.jabara_neighbor,
					false
					);
			}

			// 에러
			if ( !jabaraResult )
			{
				SaveErrorImage(_T("jabara"), area, Image_Count, Buffer_Num, now_count);
				
				return STRAW_JABARA;
			}
		}


		// 이물 검사 영역이 하나라도 존재할 경우 빨대 전체에 대해 Edge 구하기
		if ( m_pre->m_imageView.m_inspect_ox.imul		 || 
			 m_pre->m_imageView.m_inspect_ox.imul_chuck  || 
			 m_pre->m_imageView.m_inspect_ox.imul_mid	 ||
			 m_pre->m_imageView.m_inspect_ox.imul_insert || 
			 m_pre->m_imageView.m_inspect_ox.imul_jabara )
		{
			// 빨대 하나에 대한 ROI
			CvRect area = cvRect( m_pre->m_imageView.m_ROI[now_count][0].x,
						   m_pre->m_imageView.m_ROI[now_count][0].y,
						   abs( m_pre->m_imageView.m_ROI[now_count][0].x - m_pre->m_imageView.m_ROI[now_count][1].x ),
						   abs( m_pre->m_imageView.m_ROI[now_count][0].y - m_pre->m_imageView.m_ROI[now_count][1].y ) );

			// Otsu
			m_process[Inspect_ID][Buffer_Num]->OtsuThreshold(
				area, 
				m_process[Inspect_ID][Buffer_Num]->m_OriginalImage,
				false );
		}

		// 10. 이물 축관 검사 유무   m_binaryImage
		if ( m_pre->m_imageView.m_inspect_ox.imul_chuck && m_pre->m_imageView.m_inspect_ox.chuck )
		{
			CvRect area = cvRect(
				m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x,
				m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].y,
				abs( m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[1].x),
				abs( m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[1].y) );

			// 12. 이물 좌측 에티켓 검사 유무, 13. 이물 우측 에티켓 검사 유무
			bool blackResult = m_process[Inspect_ID][Buffer_Num]->InspectBlackPoint( 
				now_count,
				area,														// ROI ( 축관영역 )
				m_pre->m_imageView.m_inspect_setting.imul_chuck_mask_cols,	// 축관 마스크 크기(Cols)
				m_pre->m_imageView.m_inspect_setting.imul_chuck_mask,		// 축관 마스크 크기(Rows)
				m_pre->m_imageView.m_inspect_setting.imul_chuck_threshold,  // 축관 threshold
				m_pre->m_imageView.m_inspect_setting.chuck_pixsize,			// 허용 이물 크기
				false,														// 출력 여부
				_T("chuck"), STRAW_IMUL_CHUCK,
				area,
				m_pre->m_imageView.m_inspect_setting.imul_chuck_threshold,
				1,
				1
				);

			if ( !blackResult )
			{
				SaveErrorImage( _T("imulChuck"), area, Image_Count, Buffer_Num, now_count );

				return STRAW_IMUL_CHUCK;
			}
		}

		// 11. 이물 내측 검사 유무   m_binaryImage
		if ( m_pre->m_imageView.m_inspect_ox.imul_insert && m_pre->m_imageView.m_inspect_ox.insert )
		{
			CvRect area = cvRect( 
				m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x,
				m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y,
				(int)(abs( m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].x ) * ( m_pre->m_imageView.m_inspect_setting.imul_insert_pos / 100.0 )),
				abs( m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].y ) );

			bool blackResult = m_process[Inspect_ID][Buffer_Num]->InspectBlackPoint(
				now_count,
				area,														// ROI ( 내측영역 )
				1,															// 내측 마스크 크기(Cols)
				m_pre->m_imageView.m_inspect_setting.imul_insert_mask,		// 내측 마스크 크기(Rows)
				m_pre->m_imageView.m_inspect_setting.imul_insert_threshold, // 이물 threshold
				m_pre->m_imageView.m_inspect_setting.insert_pixsize,		// 허용 이물 크기
				false,														// 출력 여부
				_T("insert"), STRAW_IMUL_INSERT,
				area,
				m_pre->m_imageView.m_inspect_setting.imul_insert_threshold,
				1,
				1
				);

			if ( !blackResult )
			{
				SaveErrorImage( _T("imulInsert"), area, Image_Count, Buffer_Num, now_count );

				return STRAW_IMUL_INSERT;
			}
		}

		CvRect topArea = cvRect( 0, 0, 0, 0 );
		CvRect midArea = cvRect( 0, 0, 0, 0 );

		// 9. 이물 자바라 검사  m_binaryImage
		if ( m_pre->m_imageView.m_inspect_ox.jabara )
		{
			if ( m_pre->m_imageView.m_inspect_ox.imul_jabara )
			{
				CvRect area = cvRect( 
					m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].x,
					m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].y,
					abs( m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].x ),
					abs( m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].y ) );

				bool blackResult = m_process[Inspect_ID][Buffer_Num]->InspectBlackPoint(
					now_count,
					area,													    // ROI ( 자바라 영역 )
					1,															// 자바라 마스크 크기(Cols)
					m_pre->m_imageView.m_inspect_setting.imul_jabara_mask,		// 자바라 마스크 크기(Rows)
					m_pre->m_imageView.m_inspect_setting.imul_jabara_threshold, // 자바라 threshold
					m_pre->m_imageView.m_inspect_setting.jabara_pixsize,		// 허용 이물 크기
					false,														// 출력 여부
					_T("jabara"), STRAW_IMUL_JABARA,
					area,
					m_pre->m_imageView.m_inspect_setting.imul_jabara_threshold,
					1,
					1
					);

				if ( !blackResult )
				{
					SaveErrorImage( _T("imulJabara"), area, Image_Count, Buffer_Num, now_count );

					return STRAW_IMUL_JABARA;
				}
			}

			// 7. 이물 상단 검사 유무 ROI ( 왼쪽 끝 ~ 자바라 왼쪽 )
			if ( m_pre->m_imageView.m_inspect_ox.imul )
			{
				topArea = cvRect( m_pre->m_imageView.m_ROI[now_count][0].x,
								  m_pre->m_imageView.m_ROI[now_count][0].y,
								  abs( m_pre->m_imageView.m_ROI[now_count][0].x - m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].x ),
								  abs( m_pre->m_imageView.m_ROI[now_count][0].y - m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].y ) );
			}

			// 8. 이물 중단 검사 유무 ROI ( 자바라 오른쪽 ~ 축관 왼쪽 )
			if ( m_pre->m_imageView.m_inspect_ox.imul_mid )
			{
				if ( m_pre->m_imageView.m_inspect_ox.chuck )
				{
					midArea = cvRect( m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].x,
									  m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].y,
									  abs( m_pre->m_imageView.m_inspect_ROI[now_count].jabara[1].x - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x ),
									  abs( m_pre->m_imageView.m_inspect_ROI[now_count].jabara[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[1].y ) );
				}
			}
		}
		// 자바라 검사 안 할 경우
		else
		{
			if ( m_pre->m_imageView.m_inspect_ox.chuck )
			{
				// (왼쪽 끝 ~ 축관 왼쪽) / 2
				int halfWidth = abs( m_pre->m_imageView.m_ROI[now_count][0].x - m_pre->m_imageView.m_inspect_ROI[now_count].chuck[0].x ) / 2;

				// 왼쪽 끝 ~ 중간
				if ( m_pre->m_imageView.m_inspect_ox.imul )
				{
					topArea = cvRect( m_pre->m_imageView.m_ROI[now_count][0].x,
									  m_pre->m_imageView.m_ROI[now_count][0].y,
									  halfWidth,
									  abs( m_pre->m_imageView.m_ROI[now_count][0].y - m_pre->m_imageView.m_ROI[now_count][1].y ) );
				}

				// 중간 ~ 축관 왼쪽
				if ( m_pre->m_imageView.m_inspect_ox.imul_mid )
				{
					midArea = cvRect( m_pre->m_imageView.m_ROI[now_count][0].x + halfWidth,
									  m_pre->m_imageView.m_ROI[now_count][0].y,
									  halfWidth,
									  abs( m_pre->m_imageView.m_ROI[now_count][0].y - m_pre->m_imageView.m_ROI[now_count][1].y ) );
				}
			}
		}

		// 7. 이물 상단 검사
		if ( m_pre->m_imageView.m_inspect_ox.imul )
		{
			CvRect maguri_rect = cvRect(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].x,
						  m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y,
						  abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x),
						  abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].y));

			CvRect Ettiquet_Right_rect = cvRect(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[0].x,
							 m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[0].y,
							  abs(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[1].x),
							  abs(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[1].y));

			CvRect Ettiquet_Left_rect = cvRect(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[0].x,
							 m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[0].y,
							  abs(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[1].x),
							  abs(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[1].y));


			bool blackResult = m_process[Inspect_ID][Buffer_Num]->InspectBlackPoint(
				now_count,
				topArea,												 // ROI
				m_pre->m_imageView.m_inspect_setting.imul_top_mask_cols, // 상단 마스크 크기(Cols)
				m_pre->m_imageView.m_inspect_setting.imul_top_mask,		 // 상단 마스크 크기(Rows)
				m_pre->m_imageView.m_inspect_setting.imul_top_threshold, // 상단 threshold
				m_pre->m_imageView.m_inspect_setting.pixsize,			 // 허용 이물 크기
				false,													 // 출력 여부
				_T("top"), STRAW_IMUL_TOP,
				maguri_rect,
				m_pre->m_imageView.m_inspect_setting.imul_top_persent,
				m_pre->m_imageView.m_inspect_setting.imul_top_size,
				m_pre->m_imageView.m_inspect_setting.imul_mid_notinspect
				);

			if ( !blackResult )
			{
				SaveErrorImage( _T("imulTop"), topArea, Image_Count, Buffer_Num, now_count );

				return STRAW_IMUL_TOP;
			}
		}
		
		// 8. 이물 중단 검사
		if ( m_pre->m_imageView.m_inspect_ox.imul_mid )
		{
			CvRect maguri_rect = cvRect(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].x,
						  m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y,
						  abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].x),
						  abs(m_pre->m_imageView.m_inspect_ROI[now_count].maguri[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].maguri[1].y));

			CvRect Ettiquet_Right_rect = cvRect(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[0].x,
							 m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[0].y,
							  abs(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[1].x),
							  abs(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_right[1].y));

			CvRect Ettiquet_Left_rect = cvRect(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[0].x,
							 m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[0].y,
							  abs(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[1].x),
							  abs(m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].imul_etq_left[1].y));

			bool blackResult = m_process[Inspect_ID][Buffer_Num]->InspectBlackPoint(
				now_count,
				midArea,												 // ROI
				1,														 // 중단 마스크 크기(Cols)
				m_pre->m_imageView.m_inspect_setting.imul_mid_mask,		 // 중단 마스크 크기(Rows)
				m_pre->m_imageView.m_inspect_setting.imul_mid_threshold, // 중단 threshold
				m_pre->m_imageView.m_inspect_setting.mid_pixsize,		 // 허용 이물 크기
				false,													 // 출력 여부
				_T("mid"), STRAW_IMUL_MID,
				Ettiquet_Right_rect,
				m_pre->m_imageView.m_inspect_setting.imul_mid_persent,
				m_pre->m_imageView.m_inspect_setting.imul_mid_size,
				m_pre->m_imageView.m_inspect_setting.imul_mid_notinspect
				);
			if ( !blackResult )
			{
				SaveErrorImage( _T("imulMid"), midArea, Image_Count, Buffer_Num, now_count );

				return STRAW_IMUL_MID;
			}
		}


		// 에티켓------------------------------ 
		if(m_pre->m_imageView.m_inspect_ox.etiquette) // 검사 안함.
		{
			CvRect area = cvRect(
				m_pre->m_imageView.m_inspect_ROI[now_count].etiquette[0].x,
				m_pre->m_imageView.m_inspect_ROI[now_count].etiquette[0].y,
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].etiquette[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].etiquette[1].x),
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].etiquette[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].etiquette[1].y)
				);

			// 검사
			bool etiqResult = m_process[Inspect_ID][Buffer_Num]->InspectEtiquette(
				area,
				m_pre->m_imageView.m_inspect_setting.etiquette_size,
				m_pre->m_imageView.m_inspect_setting.etiquette_recognize,
				m_pre->m_imageView.m_inspect_setting.etiquette_position,
				false
				);

			if(!etiqResult)
			{
				return STRAW_ETIQUETTE;
			}
		}
		
		// 검사 끝
		return STRAW_NO_ERROR;
	}
	else
	{
		AfxMessageBox(_T("이미지 없음"));
		return NO_STRAW;
	}
}

UINT CGetPicture::StrawExistanceCheck(int Inspect_ID, int Buffer_Num, int Image_Count, int now_count)
{
	
	UINT result;
	
	result = STRAW_ERROR;
	
	int trans = m_process[Inspect_ID][Buffer_Num]->m_trans;

	
	if(trans == 0) // 일반 빨대
	{
		CvRect area = cvRect(m_pre->m_imageView.m_ROI[now_count][0].x,m_pre->m_imageView.m_ROI[now_count][0].y,abs(m_pre->m_imageView.m_ROI[now_count][0].x-m_pre->m_imageView.m_ROI[now_count][1].x),abs(m_pre->m_imageView.m_ROI[now_count][0].y-m_pre->m_imageView.m_ROI[now_count][1].y));
		
		if(!m_process[Inspect_ID][Buffer_Num]->StrawExistanceCheck(area, 0))
			result = NO_STRAW;
	}
	else // 투명 빨대
	{
		CvRect totalArea = cvRect(m_pre->m_imageView.m_ROI[now_count][0].x,m_pre->m_imageView.m_ROI[now_count][0].y,abs(m_pre->m_imageView.m_ROI[now_count][0].x-m_pre->m_imageView.m_ROI[now_count][1].x),abs(m_pre->m_imageView.m_ROI[now_count][0].y-m_pre->m_imageView.m_ROI[now_count][1].y));
		CvRect partialArea = cvRect(
				m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x,
				m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y,
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].x - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].x),
				abs(m_pre->m_imageView.m_inspect_ROI[now_count].insert[0].y - m_pre->m_imageView.m_inspect_ROI[now_count].insert[1].y)
				);

		int isExist = m_process[Inspect_ID][Buffer_Num]->StrawExistanceCheckForTrans(partialArea, totalArea, false);
		if(isExist == FALSE)
			result = NO_STRAW;
		else if(isExist == -1)
			result = -1;
		//if(!m_process[Inspect_ID][Buffer_Num]->StrawExistanceCheckForTrans(partialArea, totalArea, false))
		//	result = NO_STRAW;
	}

	return result;
	
}

void CGetPicture::SetPreInspection(LPVOID *pre)
{
	// 검사값 설정용 포인터
	m_pre = (CPreInspection *)pre;
	return;
}

void CGetPicture::ResetResultData()
{
	// 결과값 리셋
	if ( !m_restart )
	{
		m_inspect_result.ng = 0;
		m_inspect_result.ng_angle = 0;
		m_inspect_result.ng_chuck = 0;
		m_inspect_result.ng_imul = 0;
		m_inspect_result.ng_imul_top = 0;
		m_inspect_result.ng_imul_mid = 0;
		m_inspect_result.ng_imul_jabara = 0;
		m_inspect_result.ng_imul_chuck = 0;
		m_inspect_result.ng_imul_insert = 0;
		m_inspect_result.ng_insert = 0;
		m_inspect_result.ng_jabara = 0;
		m_inspect_result.ng_etiquette = 0;
		m_inspect_result.ng_maguri = 0;
		m_inspect_result.ng_pok = 0;
		m_inspect_result.ok = 0;
	}
	else
	{
		struct INSPECTION_RESTORE_DATA data;
		FILE *file = fopen( _T("c:\\ATI\\errdata.txt"), _T("rb") );
		
		if ( file != NULL )
		{
			fread( &data, sizeof( struct INSPECTION_RESTORE_DATA ), 1, file );
			
			fclose( file );
			
			m_inspect_result.ng = data.ng;
			
			m_inspect_result.ng_etiquette = data.ng_etiquette;
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
			m_inspect_result.ok = data.ok;
			
		}
		else
		{
			m_inspect_result.ng = 0;
			m_inspect_result.ng_angle = 0;
			m_inspect_result.ng_chuck = 0;
			m_inspect_result.ng_imul = 0;
			m_inspect_result.ng_imul_top = 0;
			m_inspect_result.ng_imul_mid = 0;
			m_inspect_result.ng_imul_jabara = 0;
			m_inspect_result.ng_imul_chuck = 0;
			m_inspect_result.ng_imul_insert = 0;
			m_inspect_result.ng_insert = 0;
			m_inspect_result.ng_jabara = 0;
			m_inspect_result.ng_etiquette = 0;
			m_inspect_result.ng_maguri = 0;
			m_inspect_result.ng_pok = 0;
			m_inspect_result.ok = 0;
		}
		
	}
		
	for (int i=0;i<_REJECT_BUFFER_SIZE;i++)
	{
		m_rejects[i] = false;
		m_rejectnum[i] = -1;
	}
	
	return;
}

void CGetPicture::StartInspectionProcess()
{
	CString strDir;
	SYSTEMTIME time;
	GetLocalTime( &time );

	ZeroMemory( cam_start, sizeof(BOOL) * _MAX_CAMERAS );
	this->cam_start[0] = TRUE;

	m_pBlowThread = AfxBeginThread( BlowStrawThread, ( LPVOID* )this );
	m_hBlowThread = m_pBlowThread->m_hThread;
	while(!(WaitForSingleObject( m_hBlowThread, 1000 )));

	if(m_maindlg->m_ioType == 2)
	{
		m_pGrabThread[0] = ::AfxBeginThread(BlackFlyGrabImage_PLC, (LPVOID*)this);
	}
	else
	{
		m_pGrabThread[0] = ::AfxBeginThread(BlackFlyGrabImage, (LPVOID*)this);
	}
	//m_pGrabThread[0] = ::AfxBeginThread(BlackFlyGrabImage, (LPVOID*)this);
	m_hGrabThreadHandle[0] = m_pGrabThread[0]->m_hThread;
	
	// 다 시작까지 기다리기
	while(!(::WaitForMultipleObjects(_MAX_CAMERAS, m_hGrabThreadHandle, TRUE, 0) == WAIT_TIMEOUT)) TRACE("스레드 생성중 기다리셈\n"); 
	
	// 시작하기로 했나?
	if (m_inspect_image)
	{
		for(int i = 0; i < _MAX_CAMERAS; ++i) {
			
			for(int j = 0; j < _IMAGE_BUFFER_SIZE; ++j)  {		
				
				m_pInspectData[i][j]->nCamNum = i;
				m_pInspectData[i][j]->nImageBufferNum = j;
				m_pInspectData[i][j]->pInspector = this;
				cam_start2[i] = TRUE; //GetPicture -> m_original 한 개
				m_pInspectThread[i][j] = ::AfxBeginThread(InspectThreadFunction, (LPVOID *)m_pInspectData[i][j]);		// inspect thread _MAX_CAMERAS * _IMAGE_BUFFER_SIZE 개 생성.
				m_hInspectThreadHandle[i][j] = m_pInspectThread[i][j]->m_hThread;
				
				Sleep(100);
				
				
				// 개시!
			}
			cam_start[i] = TRUE;
		}
	}
	
	// 기다림
	for(int nCount = 0; nCount < _MAX_CAMERAS; ++nCount) {
		while(!(::WaitForMultipleObjects(_IMAGE_BUFFER_SIZE, m_hInspectThreadHandle[nCount], TRUE, 0)))
			TRACE("wait for thread create\n");
		TRACE("%d camera inspect thread created\n", nCount);
	}
	
	// 타이머 켜기
	m_maindlg->SetTimer(1001,10,NULL);
	
	return;
	
}	

void CGetPicture::StopInspectionProcess()
{
	// 끝내기
	int i,j;
	
	// 작업중이던 thread는 한 단위의 처리를 마무리 하고 종료되도록 
	ZeroMemory(cam_start, sizeof(BOOL) * _MAX_CAMERAS);
	
	if ( m_inspect_image == true ) 
	{	
		// 검사 쓰레드 종료 이벤트 발생
		ZeroMemory(cam_start2, sizeof(BOOL) * _MAX_CAMERAS);
		
		for(i = 0; i < _MAX_CAMERAS; ++i) {
			
			for(j = 0; j < _IMAGE_BUFFER_SIZE; ++j) {
				
				m_gEvent[i][j].SetEvent();	
			}
		}
	}
	else
	{
		return;
	}
	
	m_inspect_image = false;
	
	// 카메라 쓰레드 종료 대기
	::WaitForMultipleObjects(_MAX_CAMERAS, m_hGrabThreadHandle, TRUE, INFINITE);
	
	// 타이머 죽이기
	m_maindlg->KillTimer(1001);
	
	return;
}

void CGetPicture::InitMainInspView()
{
	// 검사 중지에서 초기화 하는 것 잊지 말기

	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
		return;

	CRect size;
	size.left = 10;
	size.right = SHOW_MAIN_WIDTH + size.left; // 640 + 10

	double basePos = (640.0 / _CamWidth) * _CamHeight;

	if(basePos > SHOW_MAIN_HEIGHT)
		basePos = SHOW_MAIN_HEIGHT;

	int numOfInspView = (int)(SHOW_MAIN_HEIGHT / basePos);

	if(numOfInspView < 1)
		numOfInspView = 1;

	m_maindlg->m_numOfInspView = numOfInspView;
	
	if(m_maindlg->m_mainInspView != NULL)
		delete [] (m_maindlg->m_mainInspView);

	m_maindlg->m_mainInspView = new CSimpleDraw[numOfInspView];

	for(int i=0; i < numOfInspView; i++)
	{
		size.top = (int)(basePos * i + 10);
		size.bottom = (int)(size.top + basePos);
	
		m_maindlg->m_mainInspView[i].Create(NULL, SW_HIDE | WS_BORDER, size, m_maindlg, m_maindlg->m_nFlags);
	}
}

bool CGetPicture::StrawInspectionStart()
{
	if (!m_pre->m_DBLoaded)
	{
		// Load 안 됨
		AfxMessageBox("DB를 만들거나 불러와주세요!(請打開或者開具DB)");
		return FALSE;
	}

	InitMainInspView();

	// 이미지 초기화
	ResetStrawStatus();
	
	// 카운트 초기화	
	ResetResultData();

	// 검사 시작 시각 체크
	_ftime(&m_timeStart);
	
	// 검사 시작
	m_inspect_image = true;
	StartInspectionProcess();
	
	return TRUE;
	
}

void CGetPicture::StrawInspectionStop()
{
	// 정지시키기
	// 정지시킬 때 이것을 부르도록 한다.
	
	StopInspectionProcess();
	
}

void CGetPicture::SetInspectionDlg(LPVOID *diag)
{
	m_maindlg = (CSeoilVisionDlg *)diag;
}

bool CGetPicture::SaveRefImage(IplImage *pImage,int Image_ID)
{
	// 아직 끝 안남 -> Default 100장

	if(m_saved_ref_image[Image_ID] < 100)
	{
		m_saved_ref_image[Image_ID]++;

		return true;
	}
	else if (m_saved_ref_image[Image_ID] < m_max_ref_image + 100)
	{
		// 저장하기
		CString filename;
		CString filename_tail;
		
		m_saved_ref_image[Image_ID] ++;
		filename_tail.Format("_%02d_%03d.bmp", _CAM_NUM + 1, m_saved_ref_image[Image_ID] - 100);
		filename = m_ref_filename + filename_tail;
		
		// 현 이미지 저장
		cvSaveImage(filename, pImage);

		return true;
	}

	m_save_image = false;

	return false;
}

bool CGetPicture::AddRejectStatus(int nStrawNum, REJECT_INFO nStatus)
{
	// 상태 추가
	if (nStrawNum >= m_reject.size())
	{
		// 새거 들어왔다.
		REJECT_INFO *pStatus = new REJECT_INFO;
		
		*pStatus = nStatus;
		m_reject.push_back(pStatus);
	}
	else
	{
		// 기존 변경
		*m_reject[nStrawNum] = nStatus;
	}
	
	return true;
}

REJECT_INFO CGetPicture::GetRejectStatus(int nTabletNum)
{
	REJECT_INFO temp;
	temp.m_status = 1;
	temp.m_strawNum = 0;
	
	if(nTabletNum < m_reject.size())
		return *m_reject[nTabletNum];
	else
		// 실종!
		return temp;
}

void CGetPicture::SetNewParameter(CString strpath)
{
	FILE *in = fopen(strpath,"r");
	int threshold, data[30];
	float fdata[10];
	char line[512];
	
	INSP_PARAM ReadParam;
	
	if (in == NULL)
	{
		threshold = 120;
		data[0] = 200;
		AfxMessageBox("파일이 없습니다.(沒有文檔)");
		return;
	}
	else
	{
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&threshold);	ReadParam.nData[0] = threshold;
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[0]);	ReadParam.nData[4] = data[0];
		
		//Etiquette Data
		fgets(line,512,in);
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[1]);	ReadParam.nData[6] = data[1];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[2]);	ReadParam.nData[7] = data[2];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[3]);
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[4]);
		ReadParam.fData[0] = data[4] + data[3]*0.01;
		
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[5]);	ReadParam.nData[8] = data[5];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[6]);	ReadParam.nData[9] = data[6];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[7]);	ReadParam.nData[10] = data[7];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[8]);	ReadParam.nData[11] = data[8];
		// Imul Offset
		fgets(line,512,in);
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[9]);	ReadParam.nData[12] = data[9];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[10]);	ReadParam.nData[13] = data[10];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[11]);	ReadParam.nData[14] = data[11];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[12]);	ReadParam.nData[15] = data[12];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[13]);	ReadParam.nData[16] = data[13];
		
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[14]);	ReadParam.nData[17] = data[14];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[15]);	ReadParam.nData[18] = data[15];
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[16]);	ReadParam.nData[19] = data[16];
		// 투명 삽입부 parameter.
		fgets(line,512,in);
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[17]);	ReadParam.nData[21] = data[17];
		// Straw 유무 체크 Offset. 3point. 
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[18]);	ReadParam.nData[20] = data[18];
		// Threshold parameter.
		fgets(line,512,in);
		fgets(line,512,in);									
		fgets(line,512,in);	sscanf(line,"%d",&data[19]);	ReadParam.nData[1] = data[19];// 마구리
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[20]);	ReadParam.nData[2] = data[20];// 축관
		fgets(line,512,in);
		fgets(line,512,in);	sscanf(line,"%d",&data[21]);	ReadParam.nData[3] = data[21];// 삽입부.
		// Camera Gain
		fgets(line,512,in);
		fgets(line,512,in); sscanf(line,"%f",&fdata[0]);	ReadParam.fData[1] = fdata[0];// Camera Gain.
		
		// 축관 좌우폭
		fgets(line,512,in);	fgets(line,512,in);				
		sscanf(line,"%f",&fdata[1]);								ReadParam.fData[2] = fdata[1];
		
		// 마구리 투명 내측 Threshold value.
		fgets(line,512,in); fgets(line,512,in);
		sscanf(line,"%d",&data[22]);
		if(data[22]<0 || data[22] > 255) data[22] = data[17];		ReadParam.nData[5] = data[22];
		
		// 카메라 셔터 스피드.
		fgets(line,512,in); fgets(line,512,in);
		sscanf(line,"%f",&fdata[2]);
		if(fdata[2]<0 || fdata[2] > 100) fdata[2] = 1.31f;		ReadParam.fData[3] = (double)fdata[2];
		
		// 축관 꺽이는 부분 기울기 상부.
		fgets(line,512,in); fgets(line,512,in);
		sscanf(line,"%f",&fdata[3]);
		if(fdata[3]<0 || fdata[3] > 5) fdata[3] = 2;			ReadParam.fData[4] = fdata[3];
		
		// 축관 꺽이는 부분 기울기 하부.
		fgets(line,512,in); fgets(line,512,in);
		sscanf(line,"%f",&fdata[4]);
		if(fdata[4]<0 || fdata[4] > 5) fdata[4] = 2;			ReadParam.fData[5] = fdata[4];
		
		// 카메라 모드 자동 변환 기능 사용 여부. 
		fgets(line,512,in); fgets(line,512,in);
		sscanf(line,"%d",&data[23]);
		if(data[23]<0 || data[23] > 1) data[23] = 0;			ReadParam.nData[22] = data[23];
		
		fgets(line,512,in); fgets(line,512,in);		// 모드 14 또는 15 사용.
		sscanf(line,"%d",&data[25]);	
		if(data[25]<0 || data[25] > 20) data[25] = 0;		ReadParam.nData[24] = data[25];
		
		// 축관 폭 제한하는 수치. 
		fgets(line,512,in); fgets(line,512,in);
		sscanf(line,"%d",&data[24]);
		if(data[24]<-100 || data[24] > 1000) data[24] = 10;			ReadParam.nData[23] = data[24];
		
		fclose(in);}
		
		m_pre->ReadParam(ReadParam);
		
		m_fCamGain = fdata[0];
		m_fCamShutter = fdata[2];
		m_UseCam14mode = data[23];
		for (int i=0;i<_MAX_CAMERAS;i++)
		{
			// 클래스 초기화
			for (int j=0;j<_IMAGE_BUFFER_SIZE;j++)
			{
				m_process[i][j]->m_highlimit = threshold;
				m_process[i][j]->m_chuck_limit = data[0];
				m_process[i][j]->GV_limit = data[1];
				m_process[i][j]->GV_line_limit = data[2];
				m_process[i][j]->GV_maxline_limit = data[3];
				m_process[i][j]->dev_limit = data[4];
				m_process[i][j]->m_EtqOff[0] = data[5];
				m_process[i][j]->m_EtqOff[1] = data[6];
				m_process[i][j]->m_EtqOff[2] = data[7];
				m_process[i][j]->m_EtqOff[3] = data[8];
				m_process[i][j]->m_OffX[0] = data[9];
				m_process[i][j]->m_OffX[1] = data[10];
				m_process[i][j]->m_OffX[2] = data[11];
				m_process[i][j]->m_OffX[3] = data[12];
				m_process[i][j]->m_OffX[4] = data[13];
				m_process[i][j]->m_OffY[0] = data[14];
				m_process[i][j]->m_OffY[1] = data[15];
				m_process[i][j]->m_OffY[2] = data[16];
				m_process[i][j]->m_nThresTransInsert = data[17];
				m_process[i][j]->m_nStrawExistOffset = data[18];
				m_process[i][j]->m_nSPThres[0] = data[19];
				m_process[i][j]->m_nSPThres[1] = data[20];
				m_process[i][j]->m_nSPThres[2] = data[21];
				m_process[i][j]->m_fCamGain = fdata[0];
				m_process[i][j]->m_fPokDiff = fdata[1];
				m_process[i][j]->m_transMaguri = data[22];
				m_process[i][j]->m_fCamShutter = fdata[2];
				m_process[i][j]->m_fChuckSlideTop = fdata[3];
				m_process[i][j]->m_fChuckSlideBottom = fdata[3];
				m_process[i][j]->m_UseCam14mode = data[23];
				m_process[i][j]->m_SelectCamMode = data[25];
				m_process[i][j]->m_OffX[5]	= data[24];
			}
		}
}

void CGetPicture::SetColorInfo(int trans, int color, int thres1, int thres2, int thres3, BOOL EtqInsp)
{
	// 컬러 관련 정보 리셋
	for (int i=0;i<_MAX_CAMERAS;i++)
	{
		// 클래스 초기화
		for (int j=0;j<_IMAGE_BUFFER_SIZE;j++)
		{
			m_process[i][j]->m_colorstraw = color;
			m_process[i][j]->m_trans = trans;
			m_process[i][j]->m_colorthres[0] = thres1;
			m_process[i][j]->m_colorthres[1] = thres2;
			m_process[i][j]->m_colorthres[2] = thres3;
			m_process[i][j]->m_tepe = m_tepe;
			m_process[i][j]->m_bUseInspectEtq = EtqInsp;
			
			m_pre->m_imageView.m_process->m_colorstraw = color;
			m_pre->m_imageView.m_process->m_trans = trans;
			m_pre->m_imageView.m_process->m_colorthres[0] = thres1;
			m_pre->m_imageView.m_process->m_colorthres[1] = thres2;
			m_pre->m_imageView.m_process->m_colorthres[2] = thres3;
			m_pre->m_imageView.m_process->m_tepe = m_tepe;
			m_pre->m_imageView.m_process->m_bUseInspectEtq = EtqInsp;
			
		}
	}
	
}
// 상태 정보 리셋하기.
void CGetPicture::StatusReset()
{
	m_inspect_result.ok = 0;
	m_inspect_result.ng = 0;
	m_inspect_result.ng_angle = 0;
	m_inspect_result.ng_chuck = 0;
	m_inspect_result.ng_etiquette = 0;
	m_inspect_result.ng_imul = 0;
	m_inspect_result.ng_imul_top = 0;
	m_inspect_result.ng_imul_mid = 0;
	m_inspect_result.ng_imul_jabara = 0;
	m_inspect_result.ng_imul_chuck = 0;
	m_inspect_result.ng_imul_insert = 0;
	m_inspect_result.ng_insert = 0;
	m_inspect_result.ng_jabara = 0;
	m_inspect_result.ng_maguri = 0;
	m_inspect_result.ng_pok = 0;

	for (int i=0; i<_MAX_CAMERAS; i++)
	{
		m_iImageCount[i] = 0;
	}
	m_maindlg->UpdateStatus(m_inspect_result.ok,m_inspect_result.ng,m_inspect_result.ng_angle,m_inspect_result.ng_pok,m_inspect_result.ng_maguri,m_inspect_result.ng_jabara,m_inspect_result.ng_chuck,m_inspect_result.ng_insert,m_inspect_result.ng_imul,
		m_inspect_result.ng_imul_top, m_inspect_result.ng_imul_mid, m_inspect_result.ng_imul_jabara, m_inspect_result.ng_imul_chuck, m_inspect_result.ng_imul_insert);
}


UINT CGetPicture::BlackFlyGrabImage_PLC(LPVOID pParam)
{
	if(_CamWidth <= 0 || _CamWidth > 1280 ||
		_CamHeight <= 0 || _CamHeight > 1024)
	{
		AfxMessageBox(_T("카메라 가로, 세로 크기가 잘못되었습니다!"));
		return 0;
	}

	TRACE(_T("m_inspect_image : %d\n"), m_inspect_image );

	CGetPicture *pMyself = (CGetPicture*)pParam;
	
	bool err = true;

	bool saveChk;
	
	int cam_num = 0;
	int buffer_num = 0;

	BOOL flag;
	
	int imgOkIdx = 0;

	

	// 카메라 캡쳐 시작
	Error error = pMyself->m_cam.StartCapture();
	if ( error != PGRERROR_OK )
	{
		AfxMessageBox( _T("BlackFlyGrabImage() - 카메라 캡쳐 시작 실패") );
		return 0;
	}

	TriggerMode m_trigger;
	m_trigger.onOff = true;
	Error error_temp = pMyself->m_cam.SetTriggerMode(&m_trigger);
	
	TRACE(_T("\n--------------------\n"));
	TRACE(_T("캡쳐 시작"));
	TRACE(_T("\n--------------------\n"));

	IplImage *pImage = cvCreateImage( cvSize(_CamWidth, _CamHeight), 8, 1 );
	TRACE( _T("Cam Width: %d, Cam Height: %d"), _CamWidth, _CamHeight );

	FlyCapture2::Image capturedImage;
	
	capturedImage.ReleaseBuffer();
	
	while(pMyself->cam_start[0])
	{
		// 한 장 그랩
		pMyself->LockCriticalSectionImage(0, NULL);

		error = pMyself->m_cam.RetrieveBuffer( &capturedImage );
		if ( error != PGRERROR_OK )
		{
			TRACE( _T("\n-----------------------------------\n") );
			TRACE( _T("CGetPicture::BlackFlyGrabImage() - retrieve buffer Error!\n") );
			TRACE( _T("Error: %s\n"), error.GetDescription() );
			TRACE( _T("-----------------------------------\n") );
		}

		memcpy( pImage->imageData, capturedImage.GetData(), pImage->imageSize );
		if ( _TEST_NUM != 1 && _TEST_NUM != 0)
		{
			imgOkIdx %= _TEST_NUM;
			cvCopyImage( imgOk[imgOkIdx++], pImage );
		}

		capturedImage.ReleaseBuffer();

		//cvFlip( pImage, pImage ); // 아래서 위로 올라오므로 x축 flip
		if(m_bXAxisFlip == true)
			cvFlip(pImage, pImage);
		if ( m_bYFlip == TRUE )	// Y 방향 Flip인 경우. 
			cvFlip( pImage, pImage, 1 );
		
		pMyself->UnlockCriticalSectionImage( 0, NULL );
	
		// 에러 없는 경우에 한해
		if (err == TRUE)
		{
			// 만약 이미지 저장이면
			if (m_save_image)
			{
				saveChk = pMyself->SaveRefImage( pImage, 0 );

				if ( saveChk == false )
				{
					pMyself->cam_start[0] = false;
					break;
				}
			}
			// 검사라면,
			if (m_inspect_image)
			{
				// 2008 09 12 한혜민
				// 안정성 위해 버퍼 1ㅐ만큼은 버림
				if (m_imgCount[0] < 5 * _IMAGE_BUFFER_SIZE)
				{
					pMyself->m_iImageCount[0]++;
					m_imgCount[0]++;
					continue;
				}

				// 메인 이미지 출력
				if (pMyself->m_view)
				{
					if (pMyself->m_camtoview == 0)
					{
						// 30장마다 하나씩 보여줌 ( 메인 화면 이미지 )
						if (pMyself->m_iImageCount[0] % 30 == 0)
						{
							cvResize( pImage, resizeView );
							cvCvtColor( resizeView, rgbResizeView, CV_GRAY2RGB );

							for ( int i = 0; i < m_maindlg->m_numOfInspView; i++ )
							{
								m_maindlg->m_mainInspView[i].DrawImage(
									rgbResizeView, 
									SHOW_MAIN_WIDTH, 
									(int)((640.0 / _CamWidth) * _CamHeight));
							}
						}
					}
				}
				
				// 카운트 계산
				cam_num = 0;
				buffer_num = pMyself->m_iImageCount[0] % _IMAGE_BUFFER_SIZE;
				
				// 검사 수행
				if (pMyself->m_iImageCount[cam_num] >=0)
				{
					pMyself->m_pInspectData[cam_num][buffer_num]->nCamNum = cam_num;
					pMyself->m_pInspectData[cam_num][buffer_num]->nImageBufferNum = buffer_num;
					pMyself->m_pInspectData[cam_num][buffer_num]->nImageCount = pMyself->m_iImageCount[cam_num];
					pMyself->m_pInspectData[cam_num][buffer_num]->pInspector = pMyself;
					
					// 버퍼로 이미지 복사
					cvCopyImage( pImage, pMyself->m_pGrabImageBuffer[0][buffer_num] );
				
					// 이벤트 발생
					flag = m_gEvent[cam_num][buffer_num].SetEvent();
				}
				// 이미지 카운트 증가
				pMyself->m_iImageCount[cam_num]++;
				m_imgCount[cam_num]++;
			}
		}

		//imgCnt++;
	}
	
	// 종료
	if ( pMyself->m_cam.IsConnected() == true )
	{
		Error error;
		
		error = pMyself->m_cam.StopCapture();
		if ( error != PGRERROR_OK )
		{
			TRACE( _T("Stop capture 실패!") );
		}
	}

	m_inspect_image = false;

	cvReleaseImage( &pImage );

	TRACE ( _T("%d 카메라 종료\n"), 0 );
	
	::AfxEndThread(0xff);	
	
	// 리턴
	return TRUE;
}