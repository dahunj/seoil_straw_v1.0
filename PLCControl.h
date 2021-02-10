#pragma once

#include "IOControl.h"
#include "com_serial.h"

typedef struct _tmpType
{
	int top, right, bottom, left;
} tmpType;

class CPLCControl : public CIOControl
{
protected:
	const CString CMD_PLC_INIT;
	const CString CMD_LAMP_RED_ON;
	const CString CMD_LAMP_GREEN_ON;
	const CString CMD_LAMP_YELLOW_ON;
	const CString CMD_LAMP_RED_OFF;
	const CString CMD_LAMP_GREEN_OFF;
	const CString CMD_LAMP_YELLOW_OFF;
	const CString CMD_SOL5_ON;
	const CString CMD_SOL6_ON;
	const CString CMD_SOL5_OFF;
	const CString CMD_SOL6_OFF;

	com_serial m_comSerial;
	int m_rpm;
	int m_solNum;

	void InitPLC();
	void GetHex(unsigned int decimal, unsigned int hex[]);

public:
	CPLCControl(void);
	~CPLCControl(void);

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

	void SetRPM(int rpm);
	void SetSol(int solNum);
};
