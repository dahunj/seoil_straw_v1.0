#include "stdafx.h"
#include "EIPControl.h"


#include "AXL.h"
#include "AXHS.h"

#include "AXD.h"

CEIPControl::CEIPControl(void) :
m_nodeNum(0),
m_modulePos(0),
EIP_IN_STRAW_SENSOR(0),
EIP_OUT_STRAW_BLOW(8),
EIP_OUT_LAMP_GREEN(11),
EIP_OUT_LAMP_YELLOW(10),
EIP_OUT_LAMP_RED(9)
{
}

CEIPControl::~CEIPControl(void)
{
	if(!AxlClose())
	{
		AfxMessageBox(_T("EIP ´Ý±â ½ÇÆÐ!\n"));
	}
}

UINT CEIPControl::BeginDummyConnectionThread(LPVOID lParam)
{
	return AxlOpen();
}

bool CEIPControl::InitBoard()
{
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
	AxdInfoGetModule(0, &m_nodeNum, &m_modulePos, &moduleId);

	return true;
}

bool CEIPControl::DetectStrawSensor()
{
	DWORD value;

	AxdiNReadInportBit(m_nodeNum, m_modulePos, EIP_IN_STRAW_SENSOR, &value);

	if(value == TRUE)
		return true;
	else
		return false;
}

void CEIPControl::TurnOnRedLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, EIP_OUT_LAMP_RED, TRUE);
}

void CEIPControl::TurnOnGreenLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, EIP_OUT_LAMP_GREEN, TRUE);
}

void CEIPControl::TurnOnYellowLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, EIP_OUT_LAMP_YELLOW, TRUE);
}

void CEIPControl::TurnOffRedLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, EIP_OUT_LAMP_RED, FALSE);
}

void CEIPControl::TurnOffGreenLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, EIP_OUT_LAMP_GREEN, FALSE);
}

void CEIPControl::TurnOffYellowLamp()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, EIP_OUT_LAMP_YELLOW, FALSE);
}

void CEIPControl::TurnOnLamp()
{
	TurnOnRedLamp();
	TurnOnGreenLamp();
	TurnOnYellowLamp();
}

void CEIPControl::TurnOffLamp()
{
	TurnOffRedLamp();
	TurnOffGreenLamp();
	TurnOffYellowLamp();
}

void CEIPControl::StartBlow()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, EIP_OUT_STRAW_BLOW, TRUE);
}

void CEIPControl::StopBlow()
{
	AxdoNWriteOutportBit(m_nodeNum, m_modulePos, EIP_OUT_STRAW_BLOW, FALSE);
}