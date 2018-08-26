#include <STC8FXX.h>
#include "BLDC with Hall.h"
#include "3PhaseInverter.h"
#define DEADTIME 80

unsigned int ONtime, OFFtime, TSwitchBLDC;

static unsigned char PreviousBLDCElecCycle = 0;

bit SwitchON;

bit BLDCReverse =1;

void SetBLDCSpeed(unsigned char speed)
{
	ONtime = (((unsigned long)TSwitchBLDC * speed) >> 8) & 0xffff;
	OFFtime = TSwitchBLDC - ONtime;
	//ONtime = 100;
	//OFFtime = 400;
}

void BLDCDeadtimeDelay(unsigned int t20418)
{
	while(--t20418);
}

void HallGpioInit()
{
	P3M0 &= 0xe3;
	P3M1 &= 0xe3;
	HA = 1;
	HB = 1;
	HC = 1;
	PreviousBLDCElecCycle = 0;
	TSwitchBLDC = 512;
}
/*
unsigned char DetermineCurrentElecCycle(bit reverse)
{
	if(HA && !HB && HC)
		if(reverse)
			return 4;
		else
			return 1;
		
	if(HA && !HB && !HC)
		if(reverse)
			return 5;
		else
			return 2;
		
	if(HA && HB && !HC)
		if(reverse)
			return 6;
		else
			return 3;
		
	if(!HA && HB && !HC)
		if(reverse)
			return 1;
		else
			return 4;
		
	if(!HA && HB && HC)
		if(reverse)
			return 2;
		else
			return 5;
		
	if(!HA && !HB && HC)
		if(reverse)
			return 3;
		else
			return 6;
	return 0;
}*/
unsigned char DetermineCurrentElecCycle(bit reverse)
{
	if(!HA && !HB && !HC)
		if(reverse)
			return 2;
		else
			return 6;
		
	if(HA && !HB && !HC)
		if(reverse)
			return 3;
		else
			return 1;
		
	if(HA && HB && !HC)
		if(reverse)
			return 4;
		else
			return 2;
		
	if(HA && HB && HC)
		if(reverse)
			return 5;
		else
			return 3;
		
	if(!HA && HB && HC)
		if(reverse)
			return 6;
		else
			return 4;
		
	if(!HA && !HB && HC)
		if(reverse)
			return 1;
		else
			return 5;
	return 0;
}

void UpdateBLDCInverter(unsigned char eleccycle, bit L_Enable)
{
	if(PreviousBLDCElecCycle != eleccycle)
	{	
		TurnOFFALLSwitches();
		BLDCDeadtimeDelay(80);
	}
	switch(eleccycle)
	{
		case 1: {
			TurnON_HU;
			if(L_Enable)
				TurnON_LV;
			else 
				TurnOFF_LV;
			break;
		}
		case 2: {
			TurnON_HU;
			if(L_Enable)
				TurnON_LW;
			else 
				TurnOFF_LW;
			break;
		}
		case 3: {
			TurnON_HV;
			if(L_Enable)
				TurnON_LW;
			else 
				TurnOFF_LW;
			break;
		}
		case 4: {
			TurnON_HV;
			if(L_Enable)
				TurnON_LU;
			else 
				TurnOFF_LU;
			break;
		}
		case 5: {
			TurnON_HW;
			if(L_Enable)
				TurnON_LU;
			else 
				TurnOFF_LU;
			break;
		}
		case 6: {
			TurnON_HW;
			if(L_Enable)
				TurnON_LV;
			else 
				TurnOFF_LV;
			break;
		}
	}
	PreviousBLDCElecCycle = eleccycle;
}	

unsigned int BLDCTimerEventHandler()
{	
	SwitchON = !SwitchON;
	UpdateBLDCInverter(DetermineCurrentElecCycle(BLDCReverse),SwitchON);
//	return 418;
	if(SwitchON)
	{
		return ONtime;
	}
	return OFFtime;
}