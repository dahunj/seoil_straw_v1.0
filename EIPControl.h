#pragma once

#include "IOControl.h"

class CEIPControl : public CIOControl
{
protected:
	long m_nodeNum;
	long m_modulePos;
	const int EIP_IN_STRAW_SENSOR;
	const int EIP_OUT_STRAW_BLOW;
	const int EIP_OUT_LAMP_RED;
	const int EIP_OUT_LAMP_YELLOW;
	const int EIP_OUT_LAMP_GREEN;

	static UINT BeginDummyConnectionThread(LPVOID lParam);

public:
	CEIPControl(void);
	~CEIPControl(void);

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
