// SeoilMonitoring.cpp: implementation of the CSeoilMonitoring class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SeoilMonitoring.h"
#include "Log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSeoilMonitoring::CSeoilMonitoring()
{
	m_makemem = false;
	Init();
}

CSeoilMonitoring::~CSeoilMonitoring()
{
	Destroy();

}
CSeoilMonitoring::CSeoilMonitoring(bool makemem)
{

	if (makemem)
	{
		m_makemem = true;

	}
	else
		m_makemem = false;
	Init();
}


void CSeoilMonitoring::ReadFullFilename()
{/*
	MakeMem();
	LPVOID lpData = :: MapViewOfFile(h_fullname,FILE_MAP_ALL_ACCESS,0,0,MAX_STRING);
	CheckNullPoint(lpData);
	strcpy(m_fullname,(char*)lpData);

	ReturnMem();*/
	char temp[MAX_STRING];
	FILE *file = fopen("c:\\ATI\\fullname.txt","r");
	
	if (file == NULL)
	{
		m_fullname.Format("");
	}
	else{
	fgets(temp,MAX_STRING,file);
	m_fullname.Format("%s",temp);
	fclose(file);}

}

void CSeoilMonitoring::Init()
{
//h_filename = ::CreateFileMapping(INVALID_HANDLE_VALUE,
///		NULL,PAGE_READWRITE,0,MAX_STRING,"FileName");
	//h_fullname = ::CreateFileMapping(INVALID_HANDLE_VALUE,
//		NULL,PAGE_READWRITE,0,MAX_STRING,"FullName");
//	h_pathname = ::CreateFileMapping(INVALID_HANDLE_VALUE,
//		NULL,PAGE_READWRITE,0,MAX_STRING,"PathName");
//	h_errdata = ::CreateFileMapping(INVALID_HANDLE_VALUE,
//		NULL,PAGE_READWRITE,0,sizeof(struct INSPECTION_RESTORE_DATA),"ErrData");
//	if (m_makemem)
//		MakeMem();

	m_status  = STATUS_NONE;
	m_errorcount = 0;
	memset(m_errorcaption,0x00,MAX_STRING*100);
	m_detectee.Format("NULL");
	m_detector.Format("NULL");

	// �� ����� ����
		// �޸� ���� ������̼�
//	if (!m_makemem){h_filename = ::CreateFileMapping(INVALID_HANDLE_VALUE,
//		NULL,PAGE_READWRITE,0,MAX_STRING,"FileName");
///	h_fullname = ::CreateFileMapping(INVALID_HANDLE_VALUE,
//		NULL,PAGE_READWRITE,0,MAX_STRING,"FullName");
//	h_pathname = ::CreateFileMapping(INVALID_HANDLE_VALUE,
//		NULL,PAGE_READWRITE,0,MAX_STRING,"PathName");
//	h_errdata = ::CreateFileMapping(INVALID_HANDLE_VALUE,
//		NULL,PAGE_READWRITE,0,sizeof(struct INSPECTION_RESTORE_DATA),"ErrData");}
	

}

void CSeoilMonitoring::ReadPathName()
{
//	MakeMem();
//	LPVOID lpData = :: MapViewOfFile(h_pathname,FILE_MAP_ALL_ACCESS,0,0,MAX_STRING);
//	CheckNullPoint(lpData);
//	strcpy(m_pathname,(char*)lpData);
//ReturnMem();
	//FILE *file = fopen("c:\\ATI\\pathname.txt","r");
	//fscanf(file,"%s",&m_pathname);
	//fclose(file);
	char temp[MAX_STRING];
	FILE *file = fopen("c:\\ATI\\pathname.txt","r");
	if (file != NULL)
	{fgets(temp,MAX_STRING,file);
	m_pathname.Format("%s",temp);
	fclose(file);}
	else
	{
		m_pathname.Format("");
	}
}

void CSeoilMonitoring::ReadFileName()
{/*
	MakeMem();
	LPVOID lpData = :: MapViewOfFile(h_filename,FILE_MAP_ALL_ACCESS,0,0,MAX_STRING);
	CheckNullPoint(lpData);
	strcpy(m_filename,(char*)lpData);

ReturnMem();*/
//	FILE *file = fopen("c:\\ATI\\filename.txt","r");/
//	fscanf(file,"%s",&m_filename);
//	fclose(file);
	char temp[MAX_STRING];
	FILE *file = fopen("c:\\ATI\\filename.txt","r");
	
	if (file != NULL)

	{fgets(temp,MAX_STRING,file);
	m_filename.Format("%s",temp);
	fclose(file);}
	else
	{
		m_filename.Format("");
	}
}

void CSeoilMonitoring::WriteFullFilename(CString name)
{/*
	MakeMem();
	LPVOID lpData = ::MapViewOfFile(h_fullname,FILE_MAP_ALL_ACCESS,0,0,MAX_STRING);
	CheckNullPoint(lpData);
	strcpy((char*)lpData,name);
	::UnmapViewOfFile(lpData);
	ReturnMem();*/
FILE *file = fopen("c:\\ATI\\fullname.txt","w");
	fprintf(file,"%s",name);
	fclose(file);
}

void CSeoilMonitoring::WriteFileName(CString name)
{/*
	MakeMem();
	LPVOID lpData = ::MapViewOfFile(h_filename,FILE_MAP_ALL_ACCESS,0,0,MAX_STRING);
	CheckNullPoint(lpData);
	strcpy((char*)lpData,name);
	::UnmapViewOfFile(lpData);
ReturnMem();*/
FILE *file = fopen("c:\\ATI\\filename.txt","w");
	fprintf(file,"%s",name);
	fclose(file);
}

void CSeoilMonitoring::WritePathName(CString name)
{/*
	MakeMem();
	LPVOID lpData = ::MapViewOfFile(h_pathname,FILE_MAP_ALL_ACCESS,0,0,MAX_STRING);
	CheckNullPoint(lpData);
	strcpy((char*)lpData,name);
	::UnmapViewOfFile(lpData);
ReturnMem();*/
FILE *file = fopen("c:\\ATI\\pathname.txt","w");
	fprintf(file,"%s",name);
	fclose(file);
}

void CSeoilMonitoring::SendMessageToWindow(LPCTSTR window, UINT message, WPARAM wparam, LPARAM lparam)
{
	// �޽��� ��� �����ֱ�
	// 2010 04 28 ������
	HWND hwnd;
	hwnd = ::FindWindow(NULL,window);

	if (hwnd != NULL)
	{

		::SendMessage(hwnd,message,wparam,lparam);
	}

}

void CSeoilMonitoring::MakeMem()
{
	return;
	// �޸� ���� ������̼�
	h_filename = ::CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,PAGE_READWRITE,0,MAX_STRING,"FileName");
	h_fullname = ::CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,PAGE_READWRITE,0,MAX_STRING,"FullName");
	h_pathname = ::CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,PAGE_READWRITE,0,MAX_STRING,"PathName");
	h_errdata = ::CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL,PAGE_READWRITE,0,sizeof(struct INSPECTION_RESTORE_DATA),"ErrData");
}

LPCSTR CSeoilMonitoring::GetFileName()
{
	return m_filename;

}

LPCSTR CSeoilMonitoring::GetPathName()
{
	return m_pathname;

}

LPCSTR CSeoilMonitoring::GetFullName()
{
	return m_fullname;

}

void CSeoilMonitoring::SetStatus(int status)
{

}

void CSeoilMonitoring::StartedInsp()
{
	// �˻� ���۵�
	m_status = STATUS_STARTED;

}

void CSeoilMonitoring::StopInsp()
{
	// �˻� ����
	m_status = STATUS_STOP;

}

void CSeoilMonitoring::AddErrorMsg(LPCTSTR msg)
{
	// �޽��� �߰�
	strcpy(m_errorcaption[m_errorcount],msg);
	m_errorcount++;



}

bool CSeoilMonitoring::DetectError()
{
	// �ҷ����鼭 ���� �˻�
	// false�� �̻� ��

	// ���� ���� ���϶����� �������

	// ����� ��ƾ 
	Log log; 
	if (m_status != STATUS_STARTED && m_status != STATUS_STOP)
		return false;

	if(m_status == STATUS_STARTED)
	{
		for (int i=0;i<m_errorcount;i++)
		{
				HWND result;
					result = ::FindWindow(NULL,m_errorcaption[i]);

					if (result != NULL)

					{

						// �� ���� ����
						m_status = STATUS_ERROR;
						log.SaveLog(); 


						DWORD nProcessId;
				 GetWindowThreadProcessId(result, &nProcessId);

							HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, nProcessId);
				 if (hProcess)
				 {
					 /*

					 TerminateProcess(hProcess, -1);

					  CloseHandle(hProcess);

					  // ����� ó��
					  // 500 ms
					  Sleep(500);
					  // ������ ����� �� ���ΰ�.
					  WinExec(m_program_path,SW_SHOW);

					  Sleep(500);
					  SendMessageToWindow("Seoil EzVision ī�޶�1",UM_RESTART,0,0);
*/
					  return true;
					  // �� �̻� �� ��
					  break;
				 }
					}
			
		}
	}
	else if (m_status == STATUS_ERROR)
	{

		// ����� ���� �׽�Ʈ
		// �� ������ �޽��� ������.

		// Restart �϶�!
		
	}


		

	return false;

}

void CSeoilMonitoring::SetProgramEXE(LPCTSTR name)
{
	// ���α׷��� �н��� ���
	m_program_path = name;
}

void CSeoilMonitoring::GetAck()
{
	
	if (m_status != STATUS_ERROR)
		m_status = STATUS_NONE;
	else if (m_status == STATUS_ERROR && strcmp("NULL",m_detectee) != 0)
	{
		SendMessageToWindow(m_detectee,UM_RESTART,0,0);
	}

}

void CSeoilMonitoring::SetDestDetectee(LPCTSTR window)
{
	// �ǰ���� ����
	m_detectee = window;
}

void CSeoilMonitoring::Destroy()
{
	// �ڵ� �ٳ���
//	::CloseHandle(h_errdata);
//	::CloseHandle(h_fullname);
//	::CloseHandle(h_filename);
//	::CloseHandle(h_pathname);

}

void CSeoilMonitoring::SendEnumMessageToWindow(LPCTSTR window, UINT message, WPARAM wparam, LPARAM lparam)
{


}

void CSeoilMonitoring::SetDetector(LPCTSTR name)
{
	m_detector = name;
}

void CSeoilMonitoring::SayEnd()
{

	if (strcmp("NULL",m_detector) != 0)
	{
		SendMessageToWindow(m_detector,UM_END,0,0);
	}

}

void CSeoilMonitoring::SayStart()
{

	if (strcmp("NULL",m_detector) != 0)
	{
		SendMessageToWindow(m_detector,UM_STARTED,0,0);
	}
}

struct INSPECTION_RESTORE_DATA CSeoilMonitoring::GetErrData()
{
	return m_errdata;

}

void CSeoilMonitoring::WriteErrData(struct INSPECTION_RESTORE_DATA data)
{
	// ����
	FILE *file = fopen("c:\\ATI\\errdata.txt","wb");

	fwrite(&data,sizeof(struct INSPECTION_RESTORE_DATA),1,file);

	fclose(file);

	return;
	MakeMem();
	LPVOID lpData = ::MapViewOfFile(h_errdata,FILE_MAP_ALL_ACCESS,0,0,sizeof(struct INSPECTION_RESTORE_DATA));
	CheckNullPoint(lpData);
	memcpy((struct INSPECTION_RESTORE_DATA*)lpData,&data,sizeof(struct INSPECTION_RESTORE_DATA));
	::UnmapViewOfFile(lpData);
	ReturnMem();
}

void CSeoilMonitoring::ReadErrData()
{
	// �б�
	FILE *file = fopen("c:\\ATI\\errdata.txt","rb");
	
	fread(&m_errdata,sizeof(struct INSPECTION_RESTORE_DATA),1,file);

	fclose(file);

	return;
	MakeMem();
	LPVOID lpData = :: MapViewOfFile(h_errdata,FILE_MAP_ALL_ACCESS,0,0,sizeof(struct INSPECTION_RESTORE_DATA));
	CheckNullPoint(lpData);
	memcpy(&m_errdata,(struct INSPECTION_RESTORE_DATA*)lpData,sizeof(struct INSPECTION_RESTORE_DATA));
ReturnMem();
}

void CSeoilMonitoring::ReturnMem()
{

	return;
	CloseHandle(h_errdata);
	CloseHandle(h_filename);
	CloseHandle(h_fullname);
	CloseHandle(h_pathname);
}

void CSeoilMonitoring::CheckNullPoint(LPVOID ptr)
{

		if (ptr == NULL)
		AfxMessageBox("NULL ERROR!");

}
