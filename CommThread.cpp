#include "stdafx.h"
#include "CommThread.h"




CCommThread::CCommThread()
{

	m_bConnected = FALSE;

}

CCommThread::~CCommThread()
{

}
BOOL CCommThread::OpenPort(void (*callback)(BYTE* _buff, int _dwRead), CommInfo _CommInfo )
{
	COMMTIMEOUTS	timeouts;
	DCB				dcb;
	DWORD			dwThreadID;
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	if ( !(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) ) 	
	{
		return FALSE;
	}
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	if (! (m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
	{
		return FALSE;
	}
	m_sPortName = byIndexComPort(_CommInfo.nPortNumber);
	m_hComm = CreateFile( m_sPortName, 
		GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
		NULL);
	if (m_hComm == (HANDLE) -1)
	{
		//AfxMessageBox(_T("Can't open port"));
		return FALSE;
	}
	SetCommMask( m_hComm, EV_RXCHAR);	
	SetupComm( m_hComm, BUFF_SIZE, BUFF_SIZE);	
	PurgeComm( m_hComm,					
		PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600 / _CommInfo.nBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts( m_hComm, &timeouts);
	dcb.DCBlength = sizeof(DCB);
	GetCommState( m_hComm, &dcb);
	dcb.BaudRate = _CommInfo.nBaud;
	//--> Data 8 Bit
	dcb.ByteSize = _CommInfo.nData;
	//--> Noparity
	dcb.Parity = _CommInfo.nParity;
	//--> 1 Stop Bit
	dcb.StopBits = _CommInfo.nStopBit;
	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}
	m_bConnected = TRUE;
	m_hThreadWatchComm = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadWatchComm, 
		this, 0, &dwThreadID);
	if (! m_hThreadWatchComm)
	{
		m_bConnected = false;
		ClosePort();
		return FALSE;
	}
	m_fnCallBack = callback;
	return TRUE;
}

void CCommThread::ClosePort()
{
	if (m_bConnected == FALSE) return;
	m_bConnected = FALSE;
	SetCommMask( m_hComm, 0);
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	CloseHandle(m_hComm);
}

DWORD CCommThread::WriteComm(const CString strBuff, bool bIsAscii)
{
	int nSize = strBuff.GetLength();
	int bufPos = 0;
	BYTE *Send_buff;
	int nBufferSize;
	int byHigh, byLow;


	if (!bIsAscii)
	{
		if(nSize %2 == 0)
		{
			nBufferSize = nSize;
		}
		else
		{
			nBufferSize = nSize -1; 
		}
		Send_buff = new BYTE[nBufferSize];
		memset(Send_buff,NULL,sizeof(BYTE)*nSize);

		for( int i = 0; i < nBufferSize ; i+=2)
		{
			byHigh = byCode2AsciiValue(strBuff[i]);
			byLow  = byCode2AsciiValue(strBuff[i+1]);
			Send_buff[bufPos++] = (byHigh <<4) | byLow;		
		}
		if(nSize %2 !=0)
		{	
			Send_buff[bufPos++] = byCode2AsciiValue(strBuff[nSize-1]);
		}

	}
	else
	{
		Send_buff = new BYTE[nSize + 1];
		memset(Send_buff,NULL,sizeof(BYTE)*nSize);
		for(int i = 0; i < nSize; i++)
		{
			Send_buff[bufPos++] = strBuff[i];
		}
		Send_buff[bufPos++] = 13;//
	}

	int etc = bufPos % 8;
	for(int j =0; j < bufPos-etc; j+= 8)
	{
		WriteComm(&Send_buff[j], 8) ;
	}
	if(etc != 0)
	{
		WriteComm(&Send_buff[bufPos -etc], etc) ;
	}
	delete [] Send_buff;
	return nSize;

}



DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;
	if( !m_bConnected )		
	{
		return 0;
	}
	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			while (! GetOverlappedResult( m_hComm, &m_osWrite, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm, &dwErrorFlags, &comstat);
		}
	}
	return dwWritten;
}


DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead,dwError, dwErrorFlags;
	COMSTAT comstat;
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	if(dwRead > 0)
	{
		if( !ReadFile( m_hComm, pBuff, nToRead, &dwRead, &m_osRead) )
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				while (! GetOverlappedResult( m_hComm, &m_osRead, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hComm, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hComm, &dwErrorFlags, &comstat);
			}
		}
	}
	return dwRead;
}

BYTE CCommThread::byCode2AsciiValue(char cData)
{
	//이 함수는 char문자를 hex값으로 변경해 주는 함수 입니다.
	BYTE byAsciiValue;
	if( ( '0' <= cData ) && ( cData <='9' ) )
	{
		byAsciiValue = cData - '0';
	}
	else if( ( 'A' <= cData ) && ( cData <= 'F' ) )
	{
		byAsciiValue = (cData - 'A') + 10;
	}
	else if( ( 'a' <= cData ) && ( cData <= 'f' ) )
	{
		byAsciiValue = (cData - 'a') + 10;
	}
	else
	{
		byAsciiValue = 0;
	}
	return byAsciiValue;
}


DWORD	CCommThread::ThreadWatchComm(CCommThread* pComm)
{
	DWORD           dwEvent;
	OVERLAPPED      os;
	BOOL            bOk = TRUE;
	CString         strBuffer; 
	DWORD           dwRead; 
	BYTE            *buff = new BYTE[BUFF_SIZE]; 
	memset( &os, 0, sizeof(OVERLAPPED));
	if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
	{
		bOk = FALSE;
	}
	if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
	{
		bOk = FALSE;
	}
	if( !bOk )
	{
		AfxMessageBox(_T("Error while creating ThreadWatchComm, ") + pComm->m_sPortName);
		return FALSE;
	}
	while (pComm->m_bConnected)
	{
		dwEvent = 0;      
		WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
		memset(buff,0,sizeof(BYTE)*BUFF_SIZE);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			do
			{
				dwRead = pComm->ReadComm( buff, BUFF_SIZE);
				if(dwRead>0)
				{
					pComm->m_fnCallBack(buff, dwRead);
				}
			}while(dwRead);			
		}
		Sleep(0);	
	}
	CloseHandle( os.hEvent);
	pComm->m_hThreadWatchComm = NULL;
	delete[] buff;
	return TRUE;
}
CString CCommThread::byIndexComPort(int _nPort)
{
	CString PortName;
	if (_nPort<6){PortName.Format(_T("COM%d"),_nPort);}
	else{PortName.Format(_T("\\\\.\\COM%d"),_nPort);}
	return PortName;	
}