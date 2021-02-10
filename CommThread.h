#pragma once
#include "define.h"


#define END_CHARACTER       13
#define BUFF_SIZE 2048
class	CCommThread
{
public:
	CCommThread();
	~CCommThread();
private:
	HANDLE		m_hComm;				
	CString		m_sPortName;			
	BOOL		m_bConnected;			
	OVERLAPPED	m_osRead, m_osWrite;	
	WORD		m_wPortID;	
	HWND        m_hCommWnd;
	HANDLE		m_hThreadWatchComm;	
	bool        m_bIsThreadEnd;
	void       (*m_fnCallBack)(BYTE* _buff, int _dwRead);	

private:
    DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);
	CString byIndexComPort(int _nPort);
	static DWORD	ThreadWatchComm(CCommThread* pComm);
	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);

public:
	DWORD   WriteComm(const CString strBuff, bool bIsAscii = true);
	BOOL	OpenPort(void (*callback)(BYTE* _buff, int _dwRead), CommInfo _CommInfo );
	void	ClosePort();
	BYTE    byCode2AsciiValue(char cData);
};

