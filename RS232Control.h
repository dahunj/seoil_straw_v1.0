#pragma once
#include "CommThread.h"
#include "define.h"
#include "IOControl.h"

class RS232Control : public CIOControl
{
public:
	CCommThread m_commSignal;
	CommInfo m_CommInfo;

	static BOOL m_bIsConnected;
	static CString m_s_strMsg;
	static bool m_bTrigger;

	RS232Control(void);
	~RS232Control(void);

	static void OnReadMessage(BYTE* _buff, int _dwRead);

	bool InitBoard();

	bool DetectStrawSensor();

	void TurnOnRedLamp();
	void TurnOnYellowLamp();
	void TurnOnGreenLamp();

	void TurnOffRedLamp();
	void TurnOffYellowLamp();
	void TurnOffGreenLamp();

	void TurnOnLamp();
	void TurnOffLamp();

	void StartBlow();
	void StopBlow();
};
