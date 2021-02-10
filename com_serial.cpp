// com_serial.cpp: implementation of the com_serial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "com_serial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

com_serial::com_serial()
{

}

com_serial::~com_serial()
{
	if (m_bIsOpened)
		Close();
	if (m_pEvent!=NULL)
	{
		delete m_pEvent;
	}
}

com_serial::com_serial(CString port = _T("\\\\.\\Com1"), CString baudrate = _T("19200"), CString parity = _T("None"), CString databit = _T("8 Bit"), CString stopbit = _T("1 Bit"))
{
	m_sComPort=port;
	m_sBaudrate=baudrate;
	m_sParity=parity;
	m_sDatabit=databit;
	m_sStopbit=stopbit;
	m_bFlowchk=1;
	m_bIsOpened=FALSE;
	m_nLength=0;
	memset(m_sInBuf, 0, MAXBUF*2);
	m_pEvent = new CEvent (FALSE, TRUE);
}

void com_serial::InitValues(CString port = _T("\\\\.\\Com1"), CString baudrate = _T("19200"), CString parity = _T("None"), CString databit = _T("8 Bit"), CString stopbit = _T("1 Bit"))
{
	m_sComPort=port;
	m_sBaudrate=baudrate;
	m_sParity=parity;
	m_sDatabit=databit;
	m_sStopbit=stopbit;
	m_bFlowchk=1;
	m_bIsOpened=FALSE;
	m_nLength=0;
	memset(m_sInBuf, 0, MAXBUF*2);
	m_pEvent = new CEvent (FALSE, TRUE);
}

//-------------------------------------------------------------------------

bool com_serial::Create(HWND hWnd)
{
	m_hwnd=hWnd;

	InitValues();

	m_hComDev = CreateFile(m_sComPort, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, NULL);
	if(m_hComDev!=INVALID_HANDLE_VALUE)
		m_bIsOpened= TRUE;
	else 
		return FALSE;
	
	ResetSerial();

	m_OLW.Offset=0;
	m_OLW.OffsetHigh=0;
	m_OLR.Offset=0;
	m_OLR.OffsetHigh=0;

	m_OLR.hEvent =CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_OLR.hEvent == NULL)
	{
		CloseHandle(m_OLR.hEvent);
		return FALSE;
	}
	
	m_OLW.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_OLW.hEvent == NULL)
	{
		CloseHandle(m_OLW.hEvent);
		return FALSE;
	}
	
//	AfxBeginThread(CommThread, (LPVOID)this);
	EscapeCommFunction(m_hComDev, SETDTR);
	return TRUE;
	
}

void com_serial::ResetSerial()
{
	DCB dcb;
	DWORD DErr;
	COMMTIMEOUTS CommTimeOuts;

	if (!m_bIsOpened) return;
	ClearCommError(m_hComDev, &DErr, NULL);
	// 통신 포트의 사이즈를 결정합니다.
	SetupComm(m_hComDev, InBufSize, OutBufSize);
	// 모든 tx/rx 동작을 제한하고 또한 버퍼의 내용을 버린다.
	PurgeComm(m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	
	// set up for overlapped I/O
	// 통신 선로 상에서 한 바이트가 전송되고  또한 바이트가 전송되기까지의 시간.
	CommTimeOuts.ReadIntervalTimeout =MAXDWORD;
	// Read operation 에서 타임아웃을 사용하지 않는다.
	CommTimeOuts.ReadTotalTimeoutMultiplier=0;
	CommTimeOuts.ReadTotalTimeoutConstant=0;
	
	CommTimeOuts.WriteTotalTimeoutConstant=1000;
	CommTimeOuts.WriteTotalTimeoutMultiplier=0;
	//통신포트 타임아웃을 설정한다.
	SetCommTimeouts(m_hComDev, &CommTimeOuts);
	
	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength =sizeof(DCB);
	//통신포트의 DCB를 얻어온다.
	GetCommState(m_hComDev, &dcb);
	//DCB를 설정한다.
	dcb.fBinary=TRUE;
	dcb.fParity=TRUE;
	
	if (m_sBaudrate=="1200") dcb.BaudRate=CBR_1200;
	else if(m_sBaudrate=="2400") dcb.BaudRate=CBR_2400;
	else if (m_sBaudrate=="4800") dcb.BaudRate=CBR_4800;
	else if (m_sBaudrate=="9600") dcb.BaudRate=CBR_9600;
	else if (m_sBaudrate=="19200") dcb.BaudRate=CBR_19200;
	else if (m_sBaudrate=="38400") dcb.BaudRate=CBR_38400;
	else if (m_sBaudrate=="57600") dcb.BaudRate=CBR_57600;
	else if (m_sBaudrate=="115200") dcb.BaudRate=CBR_115200;

	if (m_sParity =="None") dcb.Parity= NOPARITY;
	else if (m_sParity =="Even") dcb.Parity= EVENPARITY;
	else if (m_sParity =="Odd") dcb.Parity=ODDPARITY;

	if (m_sDatabit =="7 Bit") dcb.ByteSize=7;
	else if(m_sDatabit=="8 Bit") dcb.ByteSize=8;
	
	if (m_sStopbit =="1 Bit") dcb.StopBits=ONESTOPBIT;
	else if (m_sStopbit =="1.5 Bit") dcb.StopBits=ONE5STOPBITS;
	else if (m_sStopbit =="2 Bit") dcb. StopBits=TWOSTOPBITS;
	
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;
	dcb.fOutxDsrFlow = FALSE;
	
	if (m_bFlowchk)
	{
		dcb.fOutX=FALSE;
		dcb.fInX=FALSE;
		dcb.XonLim=2048;
		dcb.XoffLim=1024;
	}
	else{
		dcb.fOutxDsrFlow = TRUE;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
	}
	//설정된 DCB로 통신 포트의 제어 파라미터를 설정.
	SetCommState(m_hComDev, &dcb);
	//Input Buffer 에 데이터가 들어왔을 때 이벤트가 발생하도록  설정한다.
	SetCommMask(m_hComDev, EV_RXCHAR);	
}

bool com_serial::Send(LPCTSTR outbuf, int len)
{
	bool bRet = TRUE;
	DWORD ErrorFlags;
	COMSTAT ComStat;
	DWORD BytesWritten;
	DWORD BytesSend=0;
	// 통신 포트의 모든 에러를 리셋 합니다.
	ClearCommError(m_hComDev, &ErrorFlags, &ComStat);
	// overlapped I/O를 통해 Outbuf의 내용을 len 길이 만큼 전송합니다.

	char buf[100];

	memset(buf, 0x00, sizeof(buf));

 	buf[0]=(BYTE)0x02;
	BytesWritten=len;
	for (int i=0; i<4; i++)
	{
		buf[i+1]=outbuf[i];
	}
	for (int j=0; j<len-6; j++)
	{
		buf[5+j]=outbuf[4+j];
	}
	
	buf[len-1]=(BYTE)0x0D;

	if(!WriteFile(m_hComDev,buf,len, &BytesWritten, &m_OLW)){
		int temp = GetLastError();
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if(WaitForSingleObject(m_OLW.hEvent, 1000)!=WAIT_OBJECT_0)
				bRet =FALSE;
			else GetOverlappedResult(m_hComDev, &m_OLW, &BytesWritten, FALSE);
		}
		else bRet = FALSE;
	}
	// 다시 한번 통신 포트의 모든 에러를 Reset한다.
	ClearCommError(m_hComDev, &ErrorFlags, &ComStat);
	return bRet;

}

int com_serial::Receive(LPSTR inbuf, int len)
{
	CSingleLock	lockObj((CSyncObject*) m_pEvent, FALSE);
	// argument value is not valid
	if(len==0) return -1;
	else if (len > MAXBUF) return -1;
	if(m_nLength==0){
		inbuf[0]='\0';
		return 0;
	}
	// 정상적이라면 이 루틴으로 들어와 실제 들어온 데이터의 길이만큼 input buffer 에서 데이터를 읽어옴.
	else if (m_nLength<=len)
	{
		lockObj.Lock();
		memcpy(inbuf, m_sInBuf, m_nLength);
		memset(m_sInBuf, 0, MAXBUF*2);
		int tmp = m_nLength;
		m_nLength = 0;
		lockObj.Unlock();
		return tmp;
	}
	else{
		lockObj.Lock();
		memcpy(inbuf, m_sInBuf, len);
		memmove(m_sInBuf, m_sInBuf+len, MAXBUF*2-len);
		m_nLength -= len;
		lockObj.Unlock();
		return len;
	}
	
}


UINT CommThread(LPVOID lpData)
{
	extern short g_nRemoteStatus;
	DWORD       ErrorFlags;
	COMSTAT     ComStat;
	DWORD	EvtMask ;
	char	buf[MAXBUF];
	DWORD	Length;
	int	size;
	int	insize = 0;

	com_serial* Comm = (com_serial*)lpData;

	while (Comm->m_bIsOpened) {
		EvtMask = 0;
		Length = 0;
		insize = 0;
		memset(buf,'\0',MAXBUF);
		WaitCommEvent(Comm->m_hComDev,&EvtMask, NULL);
		ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat);
		if ((EvtMask & EV_RXCHAR) && ComStat.cbInQue) {
			if (ComStat.cbInQue > MAXBUF)
				size = MAXBUF;
			else
				size = ComStat.cbInQue;
			do {
				ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat);
				if (!ReadFile(Comm->m_hComDev,buf+insize,size,&Length,&(Comm->m_OLR))) {
				// 에러
					TRACE("Error in ReadFile\n");
					if (GetLastError() == ERROR_IO_PENDING)	{
						if (WaitForSingleObject(Comm->m_OLR.hEvent, 1000) != WAIT_OBJECT_0)
							Length = 0;
						else
							GetOverlappedResult(Comm->m_hComDev,&(Comm->m_OLR),&Length,FALSE);
					}
					else
						Length = 0;
				}
				insize += Length;
			} while ((Length!=0) && (insize<size));
			ClearCommError(Comm->m_hComDev, &ErrorFlags, &ComStat);

			if (Comm->m_nLength + insize > MAXBUF*2)
				insize = (Comm->m_nLength + insize) - MAXBUF*2;


			if ( Comm->m_pEvent->m_hObject != INVALID_HANDLE_VALUE )	Comm->m_pEvent->ResetEvent();
			memcpy(Comm->m_sInBuf+Comm->m_nLength,buf,insize);
			Comm->m_nLength += insize;
			if ( Comm->m_pEvent->m_hObject != INVALID_HANDLE_VALUE ) Comm->m_pEvent->SetEvent();
			LPARAM temp=(LPARAM)Comm;
			//SendMessage(Comm->m_hwnd,WM_MYRECEIVE,Comm->m_nLength,temp);

		}
	}
	PurgeComm(Comm->m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	LPARAM temp=(LPARAM)Comm;
	//SendMessage(Comm->m_hwnd,WM_MYCLOSE,0,temp);
	Comm->HandleClose();
	AfxEndThread(0xff);
	return 0;
}

void com_serial::Close()
{
	if(!m_bIsOpened) return;
	m_bIsOpened =FALSE;
	SetCommMask(m_hComDev, 0);
	EscapeCommFunction(m_hComDev, CLRDTR);
	PurgeComm(m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	Sleep(500);
}



void com_serial::HandleClose()
{
	if ( m_hComDev != INVALID_HANDLE_VALUE ) 			CloseHandle(m_hComDev);
	if ( m_OLR.hEvent != INVALID_HANDLE_VALUE ) 		CloseHandle(m_OLR.hEvent);
	if ( m_OLW.hEvent != INVALID_HANDLE_VALUE )		CloseHandle(m_OLW.hEvent);
}


void com_serial::Clear()
{
	PurgeComm(m_hComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	memset(m_sInBuf,0, MAXBUF*2);
	m_nLength=0;
}
