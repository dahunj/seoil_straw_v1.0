#include "StdAfx.h"
#include "Amc.h"
#include "mmcdll.h"

CAmc::CAmc(void)
{
}

CAmc::~CAmc(void)
{

}

int CAmc::BoardInit()
{
	amc_usb_init(1);
	Sleep(1000);
	amc_init_setting(1);
	Sleep(100);

	return FALSE;
}

void CAmc::DetectIO()
{
	long out;
	out = ~amc_get_out_io(0);

	(out & 0x00000001); // 0
	(out & 0x00000002); // 1
}

void CAmc::On(short nNum)
{
	if( nNum == 0 )	amc_set_bit(0);
	if( nNum == 1 ) amc_set_bit(1);

}

void CAmc::Off(short nNum)
{
	if( nNum == 0 )	amc_reset_bit(0);
	if( nNum == 1 ) amc_reset_bit(1);
}