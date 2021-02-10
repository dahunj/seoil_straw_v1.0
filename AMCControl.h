#pragma once

#include "IOControl.h"

// Board Number
//#define AMC_BOARD_NUM	1

// Input
//#define AMC_STRAW_SENSOR 0

// Output
//#define AMC_STRAW_BLOW	  0
//#define AMC_LAMP_RED	  1
//#define AMC_LAMP_YELLOW   2
//#define AMC_LAMP_GREEN    3

class CAMCControl : public CIOControl
{
protected:
	const int AMC_BOARD_NUM;
	const int AMC_IN_STRAW_SENSOR;
	const int AMC_OUT_STRAW_BLOW;
	const int AMC_OUT_LAMP_RED;
	const int AMC_OUT_LAMP_YELLOW;
	const int AMC_OUT_LAMP_GREEN;
public:
	CAMCControl(void);
	~CAMCControl(void);
	
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
