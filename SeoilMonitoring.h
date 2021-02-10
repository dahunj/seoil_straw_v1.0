// SeoilMonitoring.h: interface for the CSeoilMonitoring class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEOILMONITORING_H__DEBE9C40_F750_4702_8706_4BBAF8845C8F__INCLUDED_)
#define AFX_SEOILMONITORING_H__DEBE9C40_F750_4702_8706_4BBAF8845C8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_STRING 2048

// 상태 정의
#define STATUS_NONE 0
#define STATUS_STARTED 1
#define STATUS_STOP 0
#define STATUS_ERROR 2

// 유저 메시지 + 101 -> 시작했다.

#define	UM_STARTED	WM_USER+101
#define	UM_UPDATE_RESULT	WM_USER+102
#define	UM_END	WM_USER+103

#define	UM_RESTART WM_USER+201
#define	UM_READY	WM_USER+202

struct INSPECTION_RESTORE_DATA
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
ULONG m_totalTime;
};

class CSeoilMonitoring  
{
public:
	void CheckNullPoint(LPVOID ptr);
	void ReturnMem();
	void ReadErrData();
	struct INSPECTION_RESTORE_DATA GetErrData();
	void SayStart();
	void SayEnd();
	CString m_detector;
	void SetDetector(LPCTSTR name);
	void SendEnumMessageToWindow(LPCTSTR window, UINT message, WPARAM wparam, LPARAM lparam);
	void Destroy();
	void SetDestDetectee(LPCTSTR window);
	void GetAck();
	void SetProgramEXE(LPCTSTR name);
	bool DetectError();
	void AddErrorMsg(LPCTSTR msg);
	void StopInsp();
	void StartedInsp();
	void SetStatus(int status);
	LPCSTR GetFullName();
	LPCSTR GetPathName();
	LPCSTR GetFileName();
	void MakeMem();
	void SendMessageToWindow(LPCTSTR window, UINT message, WPARAM wparam, LPARAM lparam);
	void WritePathName(CString name);
	void WriteFileName(CString name);
	void WriteFullFilename(CString name);
	void WriteErrData(struct INSPECTION_RESTORE_DATA data);
	void ReadFileName();
	void ReadPathName();
	void Init();
	void ReadFullFilename();
	CSeoilMonitoring();
	CSeoilMonitoring(bool makemem);
	virtual ~CSeoilMonitoring();

	CString m_fullname;
	CString m_pathname;
	CString m_filename;

	struct INSPECTION_RESTORE_DATA m_errdata;

	HANDLE h_fullname;
	HANDLE h_pathname;
	HANDLE h_filename,h_errdata;

	bool m_makemem;

	int m_status;

	// 에러개수
	int m_errorcount;
	// 에커캡션
	char m_errorcaption[100][MAX_STRING];
	// 100개에 대해서 저장

	CString m_program_path;
	CString m_detectee;
};

#endif // !defined(AFX_SEOILMONITORING_H__DEBE9C40_F750_4702_8706_4BBAF8845C8F__INCLUDED_)
