#include "N76E003.h"
#include "SVPWM.h"
#include "3PhaseInverter.h"
#include "BLDC with Hall.h"
#include "intrins.h"

bit SVPWMmode = 0;
bit SVPReverseSpin = 1;

const ElecAngleOffestCCW = 189;
const StableCount = 4;
const ElecAngleOffestCW = 219; // 238wm // 222
unsigned char SVPAngleStep = 1;
unsigned char SVPNextAngleStep = 1;
unsigned char SpeedRippleLimitforSVP = 4;
unsigned int SpeedLowLimitforSVP = 6000;
unsigned int SatiSCyclesSwSVP = 0;
unsigned char Stablecnt = 0;
unsigned int SpeedCount = 0;
unsigned char PrevoiusMechinalCycle = 0;
unsigned int CalcElectricAngle = 0;
unsigned int SVPDriveAngle = 0;
unsigned char SVPWMCurPWM = 0;
unsigned char ExecuteSVPBL_PWM = 0;
unsigned int PulseCount = 0;
unsigned int Previous1MechanicalDelay, Previous2MechanicalDelay, CurrentElectricAngle, PreviousElectricAngle;

unsigned char code number[]={'0','1','2','3','4','5','6','7','8','9',};	
sbit debug1 = P0^2;

#define FOSC            30000000UL
#define BRT             (65536 - FOSC / 115200 / 4)

bit busy;
char wptr;
char rptr;
char buffer[16];

void UartIsr() interrupt 4 using 1
{
    if (TI)
    {
        TI = 0;
        busy = 0;
    }
    if (RI)
    {
        RI = 0;
        buffer[wptr++] = SBUF;
        wptr &= 0x0f;
    }
}

void UartInit()
{
 /*   SCON = 0x50;
    T2L = BRT;
    T2H = BRT >> 8;
    AUXR = 0x15;
    wptr = 0x00;
    rptr = 0x00;
    busy = 0;*/
}

void UartSend(char dat)
{
    while (busy);
    busy = 1;
    SBUF = dat;
}

void UartSendStr(char *p)
{
    while (*p)
    {
        UartSend(*p++);
    }
}


void delay(unsigned long t)
{
	while(t--);
}

void UART_Write_Int_Value(unsigned int num)
{
	UartSend(number[num%1000/100]);
	UartSend(number[num%100/10]);
	UartSend(number[num%10]);
}

void TimerInit()
{
//	CKCON |= 0X18;
	TMOD = 0x00;   
	T2MOD = 0X69; //low speed 69 high speed 49 mid speed 59
	CAPCON0 |= 0X10;
	CAPCON1 = 0X00;
	CAPCON2 = 0X10;
	CAPCON3 = 0X04;
	RCMP2H = 0XFF;
	RCMP2H = 0XFE;
	
	RL3 = 0X00;
	RH3 = 0XF0;
	EIE1 |= 0X02;
	T3CON &= 0XEF;
	T3CON |= 0X08;
	
	TH1 = 0x70;
	TL1 = 0x24;
	EIE |= 0X04;
	
	T2CON |= 0X04;
	
	EIPH |= 0X04;
	EIP &= 0XFB;
	EIP |= 0X80;
	EIPH &= 0X7F;
	
	TMOD |= 0x01;	
	TL0 = 0xAB;	
	TH0 = 0x2F;
	TF0 = 0;
	TR0 = 1;
	ET0 = 1;
	
	PICON = 0XFD;
	PINEN |= 0X58;
	PIPEN |= 0X38;
	EIE |= 0X02;
	
	EIPH |= 0X80;
	EIP |= 0X80;
	
	EIP |= 0X26;
	
	EIPH1 |= 0X02;
	
//	TR1 = 1;     
//	ET1 = 1;
	EA = 1;
}

void SetMotorSpin(unsigned char pwm, bit dir)
{
	unsigned int blpwm;
	blpwm = pwm;
	SetBLDCDirPWM(blpwm,dir);
	SetSVPWMValue(pwm);
	SVPReverseSpin = dir;
}


void Pin_Interrupt_ISR() interrupt 7
{
	if(PIF & 0x38)
	{
	}
	if(PIF & 0x40)
	{
		PIF &= 0XB0;
		if(PulseCount < 0xff)
			PulseCount++;
	}
	PIF &= 0x00;
}

void Timer0_ISR() interrupt 1
{
	TR0 = 0;
	TF0 = 0;
	TL0 = 0xAB;	
	TH0 = 0x2F;
	PulseCount = 64;
	if(ExecuteSVPBL_PWM < PulseCount) ExecuteSVPBL_PWM++;
	if(ExecuteSVPBL_PWM > PulseCount) ExecuteSVPBL_PWM--;
	//SetMotorSpin(ExecuteSVPBL_PWM,1);
	PulseCount = 0;
	TR0 = 1;
}

void SetSpeedRange_SVPrecision() using 1
{
	unsigned char i;
	switch(T2MOD)
	{
		case 0x69:
		{
		if(C0H < 1)
			i = 2;
		else if(C0H < 3)
			i = 1;
		else
			i = 0;
		break;
		}
		case 0x59:
		{
		if(C0H < 2)
			i = 2;
		else if(C0H < 6)
			i = 1;
		else
			i = 0;
		break;
		}
		case 0x49:
		{
		if(C0H < 4)
			i = 2;
		else if(C0H < 12)
			i = 1;
		else
			i = 0;
		break;
		}
	}
	SVPAngleStep = SVPNextAngleStep;
	switch(i)
	{
		default :
			T2MOD = 0x69;
			SVPNextAngleStep = 1;
		break;
		
		case 1 :
			T2MOD = 0x59;
			SVPNextAngleStep = 2;
		break;
		
		case 2 :
			T2MOD = 0x49;
			SVPNextAngleStep = 4;
		break;
	}
}

void PWM_Interrupu_Init()
{
	EIE |= 0X08;
}
 

void UpdateSVPFreq(unsigned char th, unsigned char tl) using 3
{
	T3CON &= 0XE7;
	RL3 = tl;
	RH3 = th;
	T3CON |= 0X08;
}


void Input_Capture_Interrupt_ISR() interrupt 12 using 3
{
	CAPCON0 &= 0XFE;
	if(SVPReverseSpin)
		SVPDriveAngle = ElecAngleOffestCW;
	else
		SVPDriveAngle = ElecAngleOffestCCW;	
	Previous2MechanicalDelay = Previous1MechanicalDelay;
	Previous1MechanicalDelay = ((int)C0H << 8)+ C0L;
	switch(T2MOD)
	{
		case 0x49: break;
		case 0x59: Previous1MechanicalDelay <<= 1; break;
		case 0x69: Previous1MechanicalDelay <<= 2; break;
	}
	UpdateSVPFreq(255-C0H,255-C0L);	
	SetSpeedRange_SVPrecision();
	if((Previous1MechanicalDelay <= SpeedLowLimitforSVP) && ((Previous1MechanicalDelay >= Previous2MechanicalDelay - (Previous2MechanicalDelay >> SpeedRippleLimitforSVP)) && (Previous1MechanicalDelay <= Previous2MechanicalDelay + (Previous2MechanicalDelay >> SpeedRippleLimitforSVP))))
	{
		if(Stablecnt >= 4)
		{
			SVPWMmode = 1;
		}
		else
			Stablecnt += 1;
	}
	else
	{
		Stablecnt = 0;
		SVPWMmode = 0;
	}
}

void Timer3_Interr_ISR() interrupt 16 using 1
{	
	T3CON &= 0XEF;
	if(SVPDriveAngle < 251)
		SVPDriveAngle += SVPAngleStep;
	else
		SVPDriveAngle = 0;
	CalcElectricAngle = SVPDriveAngle;
	if(SVPWMmode)
	{		
			if(SVPReverseSpin)
				CalcElectricAngle = 255 - CalcElectricAngle;
		//	CalculateInverterVectorsWidth_Polar(CalcElectricAngle);
	}
	else
	{
		//BLDCTimerEventHandler();
	}
}

void PWM_Interr_ISR() interrupt 13 using 2
{
	PWMF = 0;
	ADCCON0 |= 0X40;
}


void ADCInit()
{
	ADCCON0 = 0X04;
	ADCCON1 = 0X07;
	ADCMPH = 0X19;
	ADCMPL = 0x00;
	ADCDLY = 28;
	ADCCON2 = 0xa0;
}

void main(void)
{
	unsigned int i;
//	UartInit();
//  ES = 1;
//  EA = 1;
	Inverter_ControlGPIO_Init();
	HallGpioInit();
	//ADCInit();
	SetMotorSpin(65,1);
	//TimerInit();
	//PWM_Interrupu_Init();
	
	P0M1 &= 0xfb;
	P0M2 |= 0x04;
		
	P1M1 &= 0Xb0;
	P1M2 &= 0Xb0;	
	
	debug1 = 1;
	
//  UartSendStr("DAS02418");
	while(1)
	{
		for(i = 0;i < 254;i += 1)
		{	
			
	if(SVPDriveAngle < 251)
		SVPDriveAngle += SVPAngleStep;
	else
		SVPDriveAngle = 0;
	//	BLDCTimerEventHandler();
	//		UpdateBLDCInverter(i);
			delay(19);

		CalculateInverterVectorsWidth_Polar(i);
/*			UART_Write_Int_Value(CalcElectricAngle);
			if(HA)
				UartSendStr("HA+");
			else	
				UartSendStr("HA-");
			
			if(HB)
				UartSendStr("HB+");
			else	
				UartSendStr("HB-");
			
			if(HC)
				UartSendStr("HC+");
			else	
				UartSendStr("HC-");
			switch(DetermineCurrentElecCycle(0))
			{
				case 0:
					UartSendStr("HALL ERROR\r\n"); break;
				case 1:
					UartSendStr("HALL 1\r\n");break;
				case 2:
					UartSendStr("HALL 2\r\n");break;
				case 3:
					UartSendStr("HALL 3\r\n");break;
				case 4:
					UartSendStr("HALL 4\r\n");break;
				case 5:
					UartSendStr("HALL 5\r\n");break;
				case 6:
					UartSendStr("HALL 6\r\n");break;
			}*/
		}
	}
}