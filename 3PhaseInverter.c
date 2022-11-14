#include "3PhaseInverter.h"

void TurnOFFALLSwitches()
{	
	PMEN = 0X3f;
}

void Inverter_ControlGPIO_Init()
{
	P0M1 &= 0XF4;
	P0M2 |= 0X0B;
	P1M1 &= 0XF8;

	P1M2 |= 0X07;
/*	P11 = 1;
	P12 = 1;
	P10 = 1;
	P00 = 1;
	P01 = 1;
	P03 = 1;*/
	PIOCON0 = 0X3F;
	PWMCON1 = 0X51;
	PMEN = 0X3f;
	PMD = 0x00;
	PNP = 0X00;
	FBD = 0X00;
	PWMPL = 0Xff;
	PWMPH = 0X00;
	PWM2L = 0X00;
	PWM2H = 0X00;
	PWM0L = 0X00;
	PWM0H = 0X00;
	TA = 0X0AA;
	TA = 0X55;
	SFRS = 1;
	PWM4L = 0X00;
	PWM4H = 0X00;
	TA = 0X0AA;
	TA = 0X55;
	SFRS = 0;
	TA = 0X0AA;
	TA = 0X55;
	PDTEN = 0X07;
	TA = 0X0AA;
	TA = 0X55;
	PDTCNT = DEADTIME;
	PWMCON0 = 0XC0;
//	while(1);
	TurnOFFALLSwitches();
}
