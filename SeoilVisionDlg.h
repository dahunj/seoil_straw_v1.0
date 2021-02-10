// SeoilVisionDlg.h : header file
//

#if !defined(AFX_SEOILVISIONDLG_H__0CD82E11_6F7B_4617_8F8F_C2209705ED2A__INCLUDED_)
#define AFX_SEOILVISIONDLG_H__0CD82E11_6F7B_4617_8F8F_C2209705ED2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "classes/GetPicture.h"
#include "classes/inspectimage.h"

#include "SimpleDraw.h"

// 모니터 통신용
#include "SeoilMonitoring.h"
#include "NoFlickerStatic.h"

//#include "AMCControl.h" // legacy
#include "IOControl.h"

#include "mmcdll.h"

#define _MAX_BUTTONS 6
#define _EZ_BTN_ID_FIRST 50000
#define _EZ_BTN_ID_END 51000

#define _AMC_BOARD_NUM	1

#define REPORT_PATH	_T("c:\\ATI\\Report\\%d_%d_%d.txt")

/////////////////////////////////////////////////////////////////////////////
// CSeoilVisionDlg dialog

static	CStatic m_status;
static	CStatic m_status1;
static	CStatic m_status2;
static	CStatic m_status3;
static	CStatic m_time;

// cwlee add
/*
static CNoFlickerStatic m_status;
static CNoFlickerStatic m_status1;
static CNoFlickerStatic m_status2;
static CNoFlickerStatic m_status3;
static CNoFlickerStatic m_time;
*/

#define SHOW_MAIN_WIDTH 640
#define SHOW_MAIN_HEIGHT 512

class CSeoilVisionDlg : public CDialog
{
// Construction
public:
	void BFlyCameraTestFunc();
	bool m_isLoaded;
	//CAMCControl m_ctrlIO;
	CIOControl *m_pCtrlIO;
	int m_ioType;

	virtual ~CSeoilVisionDlg();
	CSeoilVisionDlg(CWnd* pParent = NULL);	// standard constructor
	
	CSimpleDraw m_mainView;
	CSimpleDraw *m_mainInspView;
	int m_numOfInspView;

	enum { IDD = IDD_SEOILVISION_DIALOG };
	CRichEditCtrl	m_ctrREditResult;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

// Implementation
protected:
	HICON m_hIcon;

	//{{AFX_MSG(CSeoilVisionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMainBtnClick(UINT nID);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInspSetting();
	afx_msg void OnInspStart();
	afx_msg void OnCamSetting();
	afx_msg void OnCameraSet2();
	afx_msg void OnInspResult();
	afx_msg void OnInspSave();
	afx_msg void OnUpdateInspSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInspStart(CCmdUI* pCmdUI);
	afx_msg void OnInspView();
	afx_msg void OnUpdateInspView(CCmdUI* pCmdUI);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTestSaveimage();
	

	afx_msg void OnTestSavegrabImage();
	afx_msg void OnFileParameterNewer();
	
	afx_msg void OnInspSetcolor();
	afx_msg void OnDestroy();
	afx_msg void OnTestSaverejectimage();
	afx_msg void OnTestTwRed();
	afx_msg void OnTestTwYellow();
	afx_msg void OnTestTwGreen();
	
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);

	afx_msg LRESULT OnRestart(WPARAM wParam, LPARAM lParam);

	void EraseBkgnd();

	void RecordRejectInfo();
	void WriteToReportFile(FILE *fp);
	void ReadFromReportFile(FILE *fp);

	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnFileOpen();
	void InitGetPicture(INSPECT_SETTING *pInsp);
	void SetGlobalVariable(INSPECT_SETTING *pInsp);
	void ReadSettingFile();

	void ParameterRefresh();
	void OnNoStrawSetting(int nSet=0);
	BOOL m_bUseInspEtq;
	int m_tepe;
	
	bool m_bShowMain;
	int m_nMainImage;
	IplImage *m_show;
	CString m_sFileText;
	CString m_sMainText;
	CString m_strIO;
	char m_lineIO[8];
	bool m_bTestSaveImage;
	bool m_bTestSavePartImage;
	void RejectX( bool type );
	int m_camnum;
	bool m_view;
	void MakeReport();
	void UpdateTime(UINT hour, UINT min, UINT sec);
	void UpdateStatus(ULONG ok, ULONG ng, ULONG angle, ULONG pok, ULONG maguri, ULONG jabara, ULONG chuck, ULONG insert, ULONG imul,
		UINT imul1=0, UINT imul2=0, UINT imul3=0, UINT imul4=0, UINT imul5=0, ULONG etiquette=0);
	void UpdateRejectInfo(int nType1, int nNumber, RESULT_INSPVALUE *pResultV);
	void StartInspection();
	
	// 검사용 클래스
	CGetPicture *m_picture;
	
	// 버튼
	CButton *m_btn[6];
	CFont *m_pFont;
	CFont *m_pFont1;

	bool m_save;
	bool m_inspect;

	// RS-232 Communication
	LRESULT OnReceive(WPARAM length, LPARAM lpara);
	LRESULT OnThreadClosed(WPARAM length, LPARAM lpara);

// 색빨대
	int m_colorstraw, m_trans;
	int m_colorthres[3];
// RPM
	int m_nSetRPM;		// RPM setting.
// Sol IO
	int m_SolIO;
	int m_SolTime;
	int m_EmptySolGap;
	bool m_bSolOnOff;

	// 파일명 최종 저장
	CString m_filename;
	CString m_file_name;
	CString m_pathname;

	CSeoilMonitoring m_monitor;

	bool m_restarted;

	//afx_msg void OnCamtestCamtest();
	afx_msg void OnTestTest();
	afx_msg void OnRpm();
	afx_msg void OnSolOn();
	afx_msg void OnSolOff();

	void SaveSetting2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void DisableMenu();
	void EnableMenu();
	void TraceMenuItem_Disable(CMenu *pMenu);
	void TraceMenuItem_Enable(CMenu *pMenu);
};

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

#define STRAW_IMUL_TOP		111
#define STRAW_IMUL_MID		112
#define STRAW_IMUL_JABARA	113
#define STRAW_IMUL_CHUCK	114
#define STRAW_IMUL_INSERT	115

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEOILVISIONDLG_H__0CD82E11_6F7B_4617_8F8F_C2209705ED2A__INCLUDED_)
