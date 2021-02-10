// GetPicture.h: interface for the CGetPicture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETPICTURE_H__EE6223D5_F65B_4BFF_939A_EE2CA53BAD97__INCLUDED_)
#define AFX_GETPICTURE_H__EE6223D5_F65B_4BFF_939A_EE2CA53BAD97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 
// #include "../SeoilVision.h"
//#include "../SeoilVisionDlg.h"


#include "FlyCapture2.h"
#include "FlyCapture2GUI.h"

#include "StrawProcess.h"

// STL�� ����ϱ� ���� �������
#include <afxmt.h>
#include <VECTOR>
#include <ITERATOR>
using namespace std;



// ��������
#include "PreInspection.h"

#include <sys/timeb.h>

class CSeoilVisionDlg;

class CGetPicture;


static CEvent	m_gEvent[_MAX_CAMERAS][_IMAGE_BUFFER_SIZE];					// �˻� �̺�Ʈ

//static CEvent	g_ThreadEvent[_MAX_CAMERAS][_IMAGE_BUFFER_SIZE];

// �˻�� Ÿ��
typedef struct _INSPECT_THREAD
{
	CGetPicture	*pInspector;
	int				nCamNum;
	int				nImageBufferNum;
	UINT			nImageCount;
} InspectThread;

// �˻� ����� ������ Ÿ��
struct INSPECT_RESULT_COUNT
{
	long ok;
	long ng;
	long ng_pok;
	long ng_angle;
	long ng_maguri;
	long ng_jabara;
	long ng_etiquette;
	long ng_chuck;
	long ng_insert;
	long ng_imul;
	long ng_imul_top;
	long ng_imul_mid;
	long ng_imul_jabara;
	long ng_imul_chuck;
	long ng_imul_insert;

	struct INSPECT_RESULT_COUNT& operator+=(const struct INSPECT_RESULT_COUNT &rhs)
	{
		ok += rhs.ok;
		ng += rhs.ng;
		ng_pok += rhs.ng_pok;
		ng_angle += rhs.ng_angle;
		ng_maguri += rhs.ng_maguri;
		ng_jabara += rhs.ng_jabara;
		ng_etiquette += rhs.ng_etiquette;
		ng_chuck += rhs.ng_chuck;
		ng_insert += rhs.ng_insert;
		ng_imul += rhs.ng_imul;
		ng_imul_top += rhs.ng_imul_top;
		ng_imul_mid += rhs.ng_imul_mid;
		ng_imul_jabara += rhs.ng_imul_jabara;
		ng_imul_chuck += rhs.ng_imul_chuck;
		ng_imul_insert += rhs.ng_imul_insert;

		return *this;
	}
};
static INSPECT_RESULT_COUNT m_inspect_result;

// �˻� �ϳ�?
static bool cam_start2[_MAX_CAMERAS];
static int ImageDatas[3];
// �ð� ����
static struct _timeb   m_timeStart;
static struct _timeb   m_timeFinish;
static ULONG m_totalTime;
static int m_nSolgap;		// Sol �Ÿ� �о��.
static bool m_bYFlip;		// Y Flip ���� �о��.
static bool m_bXAxisFlip;   // X Flip ������ �о��.
static int m_nCamNum;		// Camera ID ���ο� ����� �� �о��.

struct REJECT_INFO
{
	UINT m_strawNum;
	bool m_status;
};

class CGetPicture  
{
public:
	CGetPicture();
	virtual ~CGetPicture();

public:
	FlyCapture2::Camera m_cam;
	int m_nInterval;
	//FlyCapture2::CameraControlDlg m_camCtlDlg;

	void InitMainInspView();
	void InitAfterDBLoad();
	bool InitAll();
	bool InitCamera();
	void SetMainDlg(CSeoilVisionDlg *pMain);
	static UINT BlackFlyGrabImage(LPVOID pParam); 
	static UINT BlackFlyGrabImage_PLC(LPVOID pParam);
	static UINT BlowStrawThread(LPVOID pParam);

	UINT StartBlackFlyCamera();
	
	void SetInterval();



	void SaveErrorImage(CString errType, CvRect areaTmp, int imgCnt, int bufNum, int nowCnt);
	bool SaveRefImage(IplImage *pImage,int Image_ID);
	void SetInspectionDlg(LPVOID *diag);
	void StrawInspectionStop();
	bool StrawInspectionStart();
	void StopInspectionProcess();
	void StartInspectionProcess();
	void ResetResultData();
	void SetPreInspection(LPVOID *pre);
	UINT StrawExistanceCheck(int Inspect_ID, int Buffer_Num, int Image_Count, int now_count);
	UINT StrawInspect(int Inspect_ID, int Buffer_Num, int Image_Count, int now_count, bool bfindstraw);
	USHORT GetStrawStatus(int nTabletNum);
	bool AddStrawStatus(int nStrawNum, USHORT nStatus);
	void ResetStrawStatus();
	bool DoInspect(int Inspect_ID, int Buffer_Num, int Image_Count, InspectThread *datas);
	static UINT InspectThreadFunction(LPVOID pParam);
	bool StopCamera();
	bool SaveRefImageStop();
	UINT StartCamera();
	bool SaveRefImageStart();
	UINT SaveRefImage(int Image_ID);
	UINT UnlockCriticalSectionImage(UINT uiCamNum, CGetPicture *pPicture);
	UINT LockCriticalSectionImage(UINT uiCamNum, CGetPicture *pPicture);
	static UINT GrabImage(LPVOID pParam);
	bool Init();
//	CControlCamera *pCamera[_MAX_CAMERAS];

	// �ð� ���� ����
//	_timeb m_timeStart;
//	_timeb m_timeFinish;


	// ī�޶� ������
	//cam_data_type cams[2];
	bool cam_start[_MAX_CAMERAS];

	// �̹��� ������ ���ΰ�.
	// �ƴϸ� �˻��� ���ΰ�.

	// �˻�� Ŭ����


	// ī�޶��� �̹��� ������

	// ����� �̹���
	int m_saved_ref_image[_MAX_CAMERAS];
	int m_max_ref_image;
	// ���� ���ϸ�
	CString m_ref_filename;

	// Blow Thread
	CWinThread *m_pBlowThread;
	HANDLE		m_hBlowThread;

	// ī�޶� ���� ������
	CWinThread *	m_pGrabThread[_MAX_CAMERAS];		// grab ������
	HANDLE			m_hGrabThreadHandle[_MAX_CAMERAS];	// grab �������� �ڵ�

	// �˻� ���� ������
	InspectThread *	m_pInspectData[_MAX_CAMERAS][_IMAGE_BUFFER_SIZE];
	CWinThread *	m_pInspectThread[_MAX_CAMERAS][_IMAGE_BUFFER_SIZE];			// �˻� ������
	HANDLE			m_hInspectThreadHandle[_MAX_CAMERAS][_IMAGE_BUFFER_SIZE];	// �˻� ������ �ڵ�

	// �̹��� ī��Ʈ
	// �̹��� ����
//	IplImage *m_pGrabImageBuffer[_MAX_CAMERAS][_IMAGE_BUFFER_SIZE];

	// ���� ����
	USHORT m_strawStatus[_STATUS_BUFFER_SIZE];

	// ������
	CPreInspection *m_pre;

	// ��Ƽ�� ���� ����.

	bool m_view;
	int m_viewCnt;
	// �� ī�޶�
	int m_camtoview;
	// No Straw �� ��쿡�� �Ҿ�� �ɼ�.
	bool m_bUseSolNoStraw;

	long m_iImageCount[_MAX_CAMERAS];

public:
	void StatusReset();
	int m_tepe;
	void SetColorInfo(int trans, int color, int thres1, int thres2, int thres3, BOOL EtqInsp);
	void SetNewParameter(CString strpath);
static 	REJECT_INFO GetRejectStatus(int nTabletNum);
static 	bool AddRejectStatus(int nStrawNum, REJECT_INFO nStatus);
	IplImage *m_pGrabImageBuffer[_MAX_CAMERAS][_IMAGE_BUFFER_SIZE];
//	CPreInspection *m_pre;
 //   CPreInspection1 *m_pre2;

	INSPECT_RESULT_COUNT m_final_result;

	// �ҷ�����
	bool m_save_error;
	bool m_save_all;
	bool m_save_part_error;

	COleDateTime inTime;
	CString m_sday;
	CString m_shour;
	CString m_sminute;
	CString m_ssecond;
	//CString m_sfname;

	// ���� �����?
	int m_restart;
};

static float m_fCamGain;
static float m_fCamShutter;
static int m_UseCam14mode;
static int m_SelectCamMode;
static long m_imgCount[_MAX_CAMERAS];
static long	m_rejectnum[_REJECT_BUFFER_SIZE];
static CCriticalSection *cam_1, *cam_2;
static	int m_rejects[_REJECT_BUFFER_SIZE];
static	IplImage *cam_image[_MAX_CAMERAS];
static  IplImage *resizeView;
static	IplImage *rgbResizeView;
static	bool m_save_image;
static	bool m_inspect_image;
static 	CStrawProcess *m_process[_MAX_CAMERAS][_IMAGE_BUFFER_SIZE];
static CSeoilVisionDlg *m_maindlg;
static vector<REJECT_INFO*>	m_reject;	// �ҷ� ����
static long solblowcnt;
static int solblowNoStrawCnt;
static bool bStrawStart;
static bool bStrawEnd;
static bool bEtiqueFind;
static int m_Rejectposition;
static bool inspect_temp;

// ���� Define
#define NOTHING				0
#define STRAW_NO_ERROR		1
#define PATH_INSPECT		100
#define STRAW_MAGURI		101
#define STRAW_CHUCK			102
#define STRAW_ANGLE			103
#define STRAW_POK			104
#define STRAW_INSERT		105
#define STRAW_ETIQUETTE		106
#define STRAW_JABARA		107
#define STRAW_IMUL			110
#define STRAW_IMUL1			111
#define STRAW_IMUL2			112
#define STRAW_IMUL3			113
#define STRAW_IMUL4			114
#define STRAW_IMUL5			115
#define STRAW_BLACKPOINT	120

#define STRAW_ERROR			199

#define NO_STRAW			900





// cwlee test
#define _TEST_NUM	1

#define FILE_BASE_PATH	_T("C:\\ATI\\OK_24_")

static IplImage* imgOk[_TEST_NUM];
static IplImage* imgErr[_TEST_NUM];



#endif // !defined(AFX_GETPICTURE_H__EE6223D5_F65B_4BFF_939A_EE2CA53BAD97__INCLUDED_)
