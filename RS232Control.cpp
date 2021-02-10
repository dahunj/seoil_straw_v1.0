#include "stdafx.h"
#include "RS232Control.h"

BOOL RS232Control::m_bIsConnected = FALSE;
CString RS232Control::m_s_strMsg = _T("");
bool RS232Control::m_bTrigger = false;

RS232Control::RS232Control(void)
{
}

RS232Control::~RS232Control(void)
{
}

void RS232Control::OnReadMessage(BYTE* _buff, int _dwRead)
{
	for (int index=0; index<_dwRead;index++ )
	{
		m_s_strMsg+= _buff[index];
	}

	int Signal = m_s_strMsg.Find(_T("\r\n"));

	if(Signal != -1)
	{
		int nSignalOn = m_s_strMsg.Find( _T("X01") );
		int nSignalOff = m_s_strMsg.Find( _T("X00") );

		if (nSignalOn == -1 && nSignalOff == -1)
		{

		}else if(nSignalOn != -1 && nSignalOff == -1)
		{
			m_bTrigger = true;
		}else if(nSignalOn == -1 && nSignalOff != -1)
		{
			m_bTrigger = false;
		}

		m_s_strMsg.Empty();
	}


	/*
	if (nSignalOn != -1)
	{
		m_bTrigger = true;
	}
	*/
}

bool RS232Control::InitBoard()
{
	m_CommInfo.nBaud = 38400;
	m_CommInfo.nPortNumber = 3;
	m_CommInfo.nStopBit = 0;
	m_CommInfo.nData = 8;
	m_CommInfo.nParity = 0;	
	m_bIsConnected = m_commSignal.OpenPort(OnReadMessage, m_CommInfo);

	if(m_bIsConnected == FALSE)
	{
		return false;
	}

	return true;
}

bool RS232Control::DetectStrawSensor()
{
	if(m_bTrigger == TRUE)
	{
		m_bTrigger = false;
		return true;
	}else
		return false;
}

void RS232Control::TurnOnRedLamp()
{
	m_commSignal.WriteComm(_T("do 02"));
}

void RS232Control::TurnOnGreenLamp()
{
	m_commSignal.WriteComm(_T("do 08"));
}

void RS232Control::TurnOnYellowLamp()
{
	m_commSignal.WriteComm(_T("do 04"));
}

void RS232Control::TurnOffRedLamp()
{
	m_commSignal.WriteComm(_T("do 0"));
}

void RS232Control::TurnOffGreenLamp()
{
	m_commSignal.WriteComm(_T("do 0"));
}

void RS232Control::TurnOffYellowLamp()
{
	m_commSignal.WriteComm(_T("do 0"));
}

void RS232Control::TurnOnLamp()
{
	m_commSignal.WriteComm(_T("do 14"));
	//TurnOnRedLamp();
	//TurnOnGreenLamp();
	//TurnOnYellowLamp();
}

void RS232Control::TurnOffLamp()
{
	m_commSignal.WriteComm(_T("do 0"));
	//TurnOffRedLamp();
	//TurnOffGreenLamp();
	//TurnOffYellowLamp();
}

void RS232Control::StartBlow()
{
	m_commSignal.WriteComm(_T("do 01"));
}

void RS232Control::StopBlow()
{
	m_commSignal.WriteComm(_T("do 00"));
}