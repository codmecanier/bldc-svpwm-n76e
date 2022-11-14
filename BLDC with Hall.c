#include <N76E003.H>
#include "BLDC with Hall.h"
#include "3PhaseInverter.h"

bit BLDCReverse = 1;
bit HA,HB,HC;
bit Hall_Mode_60deg = 1;

sbit P13 = P1^3;
sbit P14 = P1^4;
sbit P15 = P1^5;

uint8_t BLDCSpeed;
uint8_t eleccycle = 0;

static uint8_t BLDC_Previous_Cycle = 0;

void SetBLDCDirPWM(uint8_t pwm, bit dir)
{
	BLDCReverse = dir;
	BLDCSpeed = pwm;
}

void SetBLDCDirPWMU2(uint8_t pwm, bit dir) using 2
{
	BLDCReverse = dir;
	BLDCSpeed = pwm;
}


void SetElecCycleU2(uint8_t i) using 2
{
	eleccycle = i;
}

void SetElecCycleU3(uint8_t i) using 3
{
	eleccycle = i;
}

void SetBLDCPWM(uint8_t pwm) using 2
{
	BLDCSpeed = pwm;
}

bit GetBLDCDirection()
{return BLDCReverse;}

uint8_t GetBLDCDirectionU3() using 3
{return BLDCReverse;}

void UpdateHall() using 2
{	
	HA = HAPort;
	HB = HBPort;
	HC = HCPort;
}

void HallGpioInit()
{
	P1M1 &= 0XC7;
	P1M2 &= 0XC7;	
	TA = 0X0AA;
	TA = 0X55;
	SFRS = 1;
	P1S |= 0X18;
	TA = 0X0AA;
	TA = 0X55;
	SFRS = 0;
	HAPort = 1;
	HBPort = 1;
	HCPort = 1;
}

uint8_t DetermineCurrentElecCycle(bit reverse) using 3
{	
	if(HAPort)
	{
		if(Hall_Mode_60deg ^ HBPort)
		{
			if(HCPort)
			{
				if(reverse)
					return 5;
				else
					return 3;		
			}
			else
			{
				if(reverse)
					return 4;
				else
					return 2;
			}
		}
		else
		{
			if(HCPort)
			{
				return 0;		//error
			}
			else
			{
				if(reverse)
					return 3;
				else
					return 1;
			}
		}
	}
	else
	{
		if(Hall_Mode_60deg ^ HBPort)
		{
			if(HCPort)
			{
				if(reverse)
					return 6;
				else
					return 4;
			}
			else
			{
				return 0; //error
			}
		}
		else
		{
			if(HCPort)
			{
				if(reverse)
					return 1;
				else
					return 5;			
			}
			else
			{
				if(reverse)
					return 2;
				else
					return 6;	
			}
		}
	}
	return 0;
}

void UpdateBLDCInverter() using 1
{
	if(BLDC_Previous_Cycle!= eleccycle)
	{
		EA = 0;
		switch(eleccycle)
		{
			case 1: {			
				PMEN = 0X30;
				PWM0L = BLDCSpeed;
				PWM2L = 0;
				PWMCON0 |= 0X40;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 1;
				PWMINTC=0X10;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 0;
				ADCCON0 &= 0XCF;
				ADCCON0 |= 0X00;	// PWM0 trig
				break;
			}
			case 2: {
				PMEN = 0X0C;
				PWM0L = BLDCSpeed;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 1;
				PWM4L = 0;
				PWMINTC=0X10;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 0;
				PWMCON0 |= 0X40;
				ADCCON0 &= 0XCF;
				ADCCON0 |= 0X00;	// PWM0 trig
				break;
			}
			case 3: {
				PMEN = 0X03;
				PWM2L = BLDCSpeed;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 1;
				PWM4L = 0;
				PWMINTC=0X12;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 0;
				PWMCON0 |= 0X40;
				ADCCON0 &= 0XCF;
				ADCCON0 |= 0X10;	// PWM2 trig
				break;
			}
			case 4: {			
				PMEN = 0X30;
				PWM2L = BLDCSpeed;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 1;
				PWMINTC=0X12;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 0;
				PWM0L = 0;
				PWMCON0 |= 0X40;
				ADCCON0 &= 0XCF;
				ADCCON0 |= 0X10;	// PWM2 trig
				break;
			}
			case 5: {
				PMEN = 0X0C;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 1;
				PWM4L = BLDCSpeed;
				PWMINTC=0X14;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 0;
				PWM0L = 0;
				PWMCON0 |= 0X40;
				ADCCON0 &= 0XCF;
				ADCCON0 |= 0X20;	// PWM4 trig
				break;
			}
			case 6: {
				PMEN = 0X03;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 1;
				PWM4L = BLDCSpeed;		
				PWMINTC=0X14;
				TA = 0X0AA;
				TA = 0X55;
				SFRS = 0;
				PWM2L = 0;
				PWMCON0 |= 0X40;
				ADCCON0 &= 0XCF;
				ADCCON0 |= 0X20;	// PWM4 trig
				break;
			}
			case 0:
				break;
		}
		
		EA = 1;
		BLDC_Previous_Cycle= eleccycle;
	}
}	
