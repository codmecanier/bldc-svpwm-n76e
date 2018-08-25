#include "3PhaseInverter.h"

void TurnOFFALLSwitches()
{	
	TurnOFF_HU; 
	TurnOFF_HV; 
	TurnOFF_HW; 
	TurnOFF_LU; 
	TurnOFF_LV; 
	TurnOFF_LW; 
}

void Inverter_ControlGPIO_Init()
{
	TurnOFFALLSwitches();
	P1M1 &= 0x9d;
	P1M0 |= 0x62;
	P3M0 |= 0x20;
	P3M1 &= 0xdf;
	P5M0 |= 0x10;
	P5M1 &= 0xef;
}
