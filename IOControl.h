#pragma once

/*
enum IO_OUTPUT
{ 
	OUTPUT_STRAW_BLOW  = 8,
	OUTPUT_LAMP_RED	 = 9,
	OUTPUT_LAMP_YELLOW = 10,
	OUTPUT_LAMP_GREEN  = 11
};

enum IO_INPUT
{
	INPUT_STRAW_SENSOR = 0
};

enum EIP_NODE_NUMBERS
{
	EIP_NODE_NUM
};
*/

class CIOControl
{
//protected:
//	static UINT BeginDummyConnectionThread(LPVOID lParam);

public:
	//CIOControl(void);
	//~CIOControl(void);

	virtual bool InitBoard() = 0;
	
	virtual bool DetectStrawSensor() = 0;

	virtual void TurnOnRedLamp() = 0;
	virtual void TurnOnYellowLamp() = 0;
	virtual void TurnOnGreenLamp() = 0;
	
	virtual void TurnOffRedLamp() = 0;
	virtual void TurnOffYellowLamp() = 0;
	virtual void TurnOffGreenLamp() = 0;
	
	virtual void TurnOnLamp() = 0;
	virtual void TurnOffLamp() = 0;

	virtual void StartBlow() = 0;
	virtual void StopBlow() = 0;
};
