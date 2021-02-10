#include "stdafx.h"
#include "AMCControl.h"
#include "mmcdll.h"

CAMCControl::CAMCControl(void) :
AMC_BOARD_NUM(1),
AMC_IN_STRAW_SENSOR(0),
AMC_OUT_STRAW_BLOW(0),
AMC_OUT_LAMP_RED(1),
AMC_OUT_LAMP_YELLOW(2),
AMC_OUT_LAMP_GREEN(3)
{
}

CAMCControl::~CAMCControl(void)
{
}

bool CAMCControl::InitBoard()
{
	if ( amc_usb_init( AMC_BOARD_NUM ) )
		return false;

	if ( amc_init_setting( AMC_BOARD_NUM ) )
		return false;

	return true;
}

bool CAMCControl::DetectStrawSensor()
{
	long out2 = amc_get_out_io( AMC_IN_STRAW_SENSOR );
	long out = amc_get_io( AMC_IN_STRAW_SENSOR );

	out &= 0x00000001;

	return ( out == 1 );
}

void CAMCControl::TurnOnRedLamp()
{
	amc_set_bit( AMC_OUT_LAMP_RED );
}

void CAMCControl::TurnOnYellowLamp()
{
	amc_set_bit( AMC_OUT_LAMP_YELLOW );
}

void CAMCControl::TurnOnGreenLamp()
{
	amc_set_bit( AMC_OUT_LAMP_GREEN );
}

void CAMCControl::TurnOffRedLamp()
{
	amc_reset_bit( AMC_OUT_LAMP_RED );
}

void CAMCControl::TurnOffYellowLamp()
{
	amc_reset_bit( AMC_OUT_LAMP_YELLOW );
}

void CAMCControl::TurnOffGreenLamp()
{
	amc_reset_bit( AMC_OUT_LAMP_GREEN );
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
	amc_set_bit( AMC_OUT_STRAW_BLOW );
}

void CAMCControl::StopBlow()
{
	amc_reset_bit( AMC_OUT_STRAW_BLOW );
}