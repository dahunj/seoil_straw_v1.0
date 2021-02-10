#pragma once

// Board Number
#define _BOARD_NUM	1

// Input
#define _STRAW_SENSOR 0

// Output
#define _STRAW_BLOW	  0
#define _LAMP_RED	  1
#define _LAMP_YELLOW  2
#define _LAMP_GREEN   3

/*
#define _STRAW_BLOW	  8
#define _LAMP_RED	  9
#define _LAMP_YELLOW  10
#define _LAMP_GREEN   11
*/

class CAMCControl
{
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
