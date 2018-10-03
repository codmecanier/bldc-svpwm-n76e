#include <N76E003.h>
#include "BLDC with Hall.h"
#include "3PhaseInverter.h"

bit BLDCReverse = 1;

unsigned char BLDCSpeed;

void SetBLDCSpeed(unsigned char speed)
{
	BLDCSpeed = speed;
}

void HallGpioInit()
{
	P0M1 &= 0XE7;
	P0M2 &= 0XE7;
	P1M1 &= 0XEB;
	P1M2 &= 0XEB;	
	TA = 0X0AA;
	TA = 0X55;
	SFRS = 1;
	P1S |= 0X18;
	P0S |= 0X08;
	TA = 0X0AA;
	TA = 0X55;
	SFRS = 0;
	HA = 1;
	HB = 1;
	HC = 1;
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
}
*/

unsigned char DetermineHallState(bit reverse)
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
unsigned char DetermineCurrentElecCycle(bit reverse)
{
	unsigned char i,j,k;
	j = 3;
	while(j)
	{
		i = DetermineHallState(reverse);
		if(k == i)
		{
			j--;
		}
		k = i;
	}
	return k;
}

void UpdateBLDCInverter(unsigned char eleccycle)
{
	EA = 0;
	switch(eleccycle)
	{
		case 1: {			
			PMEN = 0X30;
			PWM0L = BLDCSpeed;
			PWM2L = 0;
			PWMCON0 |= 0X40;
			break;
		}
		case 2: {
			PMEN = 0X0C;
			PWM0L = BLDCSpeed;
			TA = 0X0AA;
			TA = 0X55;
			SFRS = 1;
			PWM4L = 0;
			TA = 0X0AA;
			TA = 0X55;
			SFRS = 0;
			PWMCON0 |= 0X40;
			break;
		}
		case 3: {
			PMEN = 0X03;
			PWM2L = BLDCSpeed;
			TA = 0X0AA;
			TA = 0X55;
			SFRS = 1;
			PWM4L = 0;
			TA = 0X0AA;
			TA = 0X55;
			SFRS = 0;
			PWMCON0 |= 0X40;
			break;
		}
		case 4: {			
			PMEN = 0X30;
			PWM2L = BLDCSpeed;
			PWM0L = 0;
			PWMCON0 |= 0X40;
			break;
		}
		case 5: {
			PMEN = 0X0C;
			TA = 0X0AA;
			TA = 0X55;
			SFRS = 1;
			PWM4L = BLDCSpeed;
			TA = 0X0AA;
			TA = 0X55;
			SFRS = 0;
			PWM0L = 0;
			PWMCON0 |= 0X40;
			break;
		}
		case 6: {
			PMEN = 0X03;
			TA = 0X0AA;
			TA = 0X55;
			SFRS = 1;
			PWM4L = BLDCSpeed;
			TA = 0X0AA;
			TA = 0X55;
			SFRS = 0;
			PWM2L = 0;
			PWMCON0 |= 0X40;
			break;
		}
		case 0:
			break;
	}
	EA = 1;
}	

void BLDCTimerEventHandler()
{	
	UpdateBLDCInverter(DetermineCurrentElecCycle(BLDCReverse));
}