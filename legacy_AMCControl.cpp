#include "stdafx.h"
#include "legacy_AMCControl.h"
#include "mmcdll.h"

CAMCControl::CAMCControl(void)
{
}

CAMCControl::~CAMCControl(void)
{
}

bool CAMCControl::InitBoard()
{
	if ( amc_usb_init( _BOARD_NUM ) )
		return false;

	if ( amc_init_setting( _BOARD_NUM ) )
		return false;

	return true;
}

bool CAMCControl::DetectStrawSensor()
{
	long out2 = amc_get_out_io( _STRAW_SENSOR );
	long out = amc_get_io( _STRAW_SENSOR );

	out &= 0x00000001;

	return ( out == 1 );
}

void CAMCControl::TurnOnRedLamp()
{
	amc_set_bit( _LAMP_RED );
}

void CAMCControl::TurnOnYellowLamp()
{
	amc_set_bit( _LAMP_YELLOW );
}

void CAMCControl::TurnOnGreenLamp()
{
	amc_set_bit( _LAMP_GREEN );
}

void CAMCControl::TurnOffRedLamp()
{
	amc_reset_bit( _LAMP_RED );
}

void CAMCControl::TurnOffYellowLamp()
{
	amc_reset_bit( _LAMP_YELLOW );
}

void CAMCControl::TurnOffGreenLamp()
{
	amc_reset_bit( _LAMP_GREEN );
}

void CAMCControl::TurnOnLamp()
{
	TurnOnRedLamp();
	TurnOnYellowLamp();
	TurnOnGreenLamp();
}

void CAMCControl::TurnOffLamp()
{
	TurnOffRedLamp();
	TurnOffYellowLamp();
	TurnOffGreenLamp();
}

void CAMCControl::StartBlow()
{
	amc_set_bit( _STRAW_BLOW );
}

void CAMCControl::StopBlow()
{
	amc_reset_bit( _STRAW_BLOW );
}