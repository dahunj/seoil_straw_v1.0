#include "stdafx.h"
#include "PLCControl.h"
#include <math.h>

CPLCControl::CPLCControl(void) :
CMD_PLC_INIT(_T("%01#WDD0005000053")),
CMD_LAMP_GREEN_ON(_T("%01#WCSY00041**\r")),
CMD_LAMP_RED_ON(_T("%01#WCSY00021**\r")),
CMD_LAMP_YELLOW_ON(_T("%01#WCSY00031**\r")),
CMD_LAMP_GREEN_OFF(_T("%01#WCSY00040**\r")),
CMD_LAMP_RED_OFF(_T("%01#WCSY00020**\r")),
CMD_LAMP_YELLOW_OFF(_T("%01#WCSY00030**\r")),
CMD_SOL5_ON(_T("%01#WCSY00051**\r")),
CMD_SOL6_ON(_T("%01#WCSY00061**\r")),
CMD_SOL5_OFF(_T("%01#WCSY00050**\r")),
CMD_SOL6_OFF(_T("%01#WCSY00060**\r"))
{
	m_rpm = -1;
	m_solNum = -1;
}

CPLCControl::~CPLCControl(void)
{
	m_comSerial.Clear();
}

bool CPLCControl::InitBoard()
{
	bool isConnected = m_comSerial.Create(AfxGetMainWnd()->GetSafeHwnd());
	
	if(isConnected == false)
		return false;

	InitPLC();

	return isConnected;
}

void CPLCControl::InitPLC()
{
	CString cmd = CMD_PLC_INIT;
	CString strHex;
	CString strEnd = _T("**\r");

	double oneCycleTime = (60.0 / m_rpm) * 1000.0;
	unsigned int hex[4];
	unsigned int decimal;
	for(int i = 1; i <= 3; i++)
	{
		decimal = (unsigned int)floor(oneCycleTime * i / 5 + 0.5);
		GetHex(decimal, hex);
		
		strHex.Format(_T("%X%X%X%X"), hex[1], hex[0], hex[3], hex[2]);
		cmd += strHex;
	}
	cmd += strEnd;

	m_comSerial.Send(LPCTSTR(cmd), cmd.GetLength() + 1);

	Sleep(1000);
}

bool CPLCControl::DetectStrawSensor()
{
	return true; // Trigger 방식이므로 straw sensor는 사용하지 않음
}

void CPLCControl::TurnOnRedLamp()
{
	m_comSerial.Send(LPCTSTR(CMD_LAMP_RED_ON), CMD_LAMP_RED_ON.GetLength() + 1);
	Sleep(20);
}

void CPLCControl::TurnOnYellowLamp()
{
	m_comSerial.Send(LPCTSTR(CMD_LAMP_YELLOW_ON), CMD_LAMP_YELLOW_ON.GetLength() + 1);
	Sleep(20);
}

void CPLCControl::TurnOnGreenLamp()
{
	m_comSerial.Send(LPCTSTR(CMD_LAMP_GREEN_ON), CMD_LAMP_GREEN_ON.GetLength() + 1);
	Sleep(20);
}

void CPLCControl::TurnOffRedLamp()
{
	m_comSerial.Send(LPCTSTR(CMD_LAMP_RED_OFF), CMD_LAMP_RED_OFF.GetLength() + 1);
	Sleep(20);
}

void CPLCControl::TurnOffYellowLamp()
{
	m_comSerial.Send(LPCTSTR(CMD_LAMP_YELLOW_OFF), CMD_LAMP_YELLOW_OFF.GetLength() + 1);
	Sleep(20);
}

void CPLCControl::TurnOffGreenLamp()
{
	m_comSerial.Send(LPCTSTR(CMD_LAMP_GREEN_OFF), CMD_LAMP_GREEN_OFF.GetLength() + 1);
	Sleep(20);
}

void CPLCControl::TurnOnLamp()
{
	TurnOnGreenLamp();
	TurnOnRedLamp();
	TurnOnYellowLamp();
}

void CPLCControl::TurnOffLamp()
{
	TurnOffGreenLamp();
	TurnOffRedLamp();
	TurnOffYellowLamp();
}

void CPLCControl::StartBlow()
{
	CString cmd;

	if(m_solNum == 5) cmd = CMD_SOL5_ON;
	else if(m_solNum == 6) cmd = CMD_SOL6_ON;
	else return;

	m_comSerial.Send(LPCTSTR(cmd), cmd.GetLength() + 1);
}

void CPLCControl::StopBlow()
{
	CString cmd;
	if(m_solNum == 5) cmd = CMD_SOL5_OFF;
	else if(m_solNum == 6) cmd = CMD_SOL6_OFF;
	else return;

	m_comSerial.Send(LPCTSTR(cmd), cmd.GetLength() + 1);
}

void CPLCControl::SetRPM(int rpm)
{
	m_rpm = rpm;
}

void CPLCControl::SetSol(int solNum)
{
	m_solNum = solNum;
}

void CPLCControl::GetHex(unsigned int decimal, unsigned int hex[])
{
	unsigned int mask = 0x0000000f;
	for(int idx = 0; idx < 4; idx++)
	{
		hex[idx] = decimal & mask;
		decimal >>= 4;
	}
}