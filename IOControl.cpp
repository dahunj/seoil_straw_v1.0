/*
#include "stdafx.h"
#include "IOControl.h"

#include "AXL.h"
#include "AXHS.h"

#include "AXD.h"

#define m_nodeNum 16
#define m_modulePos 0

CIOControl::CIOControl(void)
{
}

CIOControl::~CIOControl(void)
{
	if(!AxlClose())
	{
		AfxMessageBox(_T("EIP ´Ý±â ½ÇÆÐ!\n"));
	}
}

UINT CIOControl::BeginDummyConnectionThread(LPVOID lParam)
{
	return AxlOpen();
}

bool CIOControl::InitBoard()
{
	return true;

	BOOL isOpened = AxlIsOpened();

	AfxBeginThread(BeginDummyConnectionThread, NULL);

	Sleep(2000);

	// Open Check
	if (!isOpened)
	{
		DWORD isOk = AxlOpen();
		
		if (isOk != AXT_RT_SUCCESS && isOk != AXT_RT_OPEN_ALREADY)
		{
			return false;
		}
	}

	// Module Check
	DWORD dwStatus;
	AxdInfoIsDIOModule(&dwStatus);

	if (dwStatus == STATUS_EXIST)
	{
		long numberOfModules;
		AxdInfoGetModuleCount(&numberOfModules);

		if (numberOfModules == 0)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// Get Module Info
	DWORD moduleId;
	long nodeNum, modulePos;
	AxdInfoGetModule(0, &nodeNum, &modulePos, &moduleId);

	return true;
}

bool CIOControl::DetectStrawSensor()
{
	return true;

	DWORD value;

	AxdiNReadInportBit(m_nodeNum, m_modulePos, INPUT_STRAW_SENSOR, &value);

	if(value == TRUE)
		return true;
	else
		return false;
}

void CIOControl::TurnOnRedLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, OUTPUT_LAMP_RED, TRUE);
}

void CIOControl::TurnOnGreenLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, OUTPUT_LAMP_GREEN, TRUE);
}

void CIOControl::TurnOnYellowLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, OUTPUT_LAMP_YELLOW, TRUE);
}

void CIOControl::TurnOffRedLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, OUTPUT_LAMP_RED, FALSE);
}

void CIOControl::TurnOffGreenLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, OUTPUT_LAMP_GREEN, FALSE);
}

void CIOControl::TurnOffYellowLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, OUTPUT_LAMP_YELLOW, FALSE);
}

void CIOControl::TurnOnLamp()
{
	TurnOnRedLamp();
	TurnOnGreenLamp();
	TurnOnYellowLamp();
}

void CIOControl::TurnOffLamp()
{
	TurnOffRedLamp();
	TurnOffGreenLamp();
	TurnOffYellowLamp();
}

void CIOControl::StartBlow()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, OUTPUT_STRAW_BLOW, TRUE);
}

void CIOControl::StopBlow()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, OUTPUT_STRAW_BLOW, FALSE);
}
*/