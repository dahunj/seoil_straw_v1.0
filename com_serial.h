// com_serial.h: interface for the com_serial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COM_SERIAL_H__591E372D_230F_4723_A81C_854BF98A7D34__INCLUDED_)
#define AFX_COM_SERIAL_H__591E372D_230F_4723_A81C_854BF98A7D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

#define MAXBUF 50000
#define	InBufSize	50000
#define	OutBufSize	50000
#define  ASCII_XON	0x11
#define  ASCII_XOFF	0x13
#define	WM_MYRECEIVE	(WM_USER+1)
#define	WM_MYCLOSE		(WM_USER+2)

class com_serial : public CCmdTarget
{
public:
	void InitValues(CString port, CString baudrate, CString parity, CString databit, CString stopbit);
	void Clear();
	void HandleClose();
	void Close();
	int Receive(LPSTR inbuf, int len);
	bool Send(LPCTSTR outbuf, int len);
	void ResetSerial();
	bool Create(HWND hWnd);
	com_serial();
	virtual ~com_serial();
	
	com_serial(CString port, CString baudrate, CString parity, CString databit, CString stopbit);
	
	CString m_sComPort;
	CString m_sBaudrate;
	CString m_sParity;
	CString m_sDatabit;
	CString m_sStopbit;
	bool m_bFlowchk;
	bool m_bIsOpened;
	int m_nLength;
	char	m_sInBuf[MAXBUF*2];
	CEvent * m_pEvent;

	HWND	m_hwnd;
	HANDLE  m_hComDev;

	OVERLAPPED m_OLW;
	OVERLAPPED m_OLR;

};
UINT CommThread(LPVOID p);
#endif // !defined(AFX_COM_SERIAL_H__591E372D_230F_4723_A81C_854BF98A7D34__INCLUDED_)
