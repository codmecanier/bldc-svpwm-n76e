#include <N76E003.h>
#include <SVPWM.h>
#include <3PhaseInverter.h>
#include <BLDC with Hall.h>
#include "intrins.h"

bit SVPWMmode = 0;
bit ReverseSpin = 1;

const ElecAngleOffestCCW = 267;
const StableCount = 4;
const ElecAngleOffestCW = 318;
unsigned char SpeedRippleLimitforSVP = 3;
unsigned int SpeedLowLimitforSVP = 700;
unsigned int SatiSCyclesSwSVP = 0;
unsigned char Stablecnt = 0;

unsigned int SpeedCount = 0;
unsigned char PrevoiusMechinalCycle = 0;
unsigned long CalcElectricAngle = 0;
unsigned int Previous1MechanicalDelay, Previous2MechanicalDelay, CurrentElectricAngle, PreviousElectricAngle;

unsigned char code number[]={'0','1','2','3','4','5','6','7','8','9',};	
sbit debug = P0^6;

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


void TM1_Isr() interrupt 3 using 0
{
	unsigned char CurrentMechinalCycle;		
	
	TR1 = 0;
	TF1 = 0;
	TH1 = 0xB3;
	TL1 = 0x24;
	TR1 = 1;      
	
	if(SpeedCount < 60000) SpeedCount++;
	if(SpeedCount >= Previous1MechanicalDelay + (Previous1MechanicalDelay >> SpeedRippleLimitforSVP))
	{
		if(SVPWMmode)
		{
			debug = 1;
			SVPWMmode = 0;
			Stablecnt = 0;
		}
	}
	CurrentMechinalCycle = DetermineCurrentElecCycle(0);
	if(PrevoiusMechinalCycle != CurrentMechinalCycle)
	{
		if(ReverseSpin)
			switch(CurrentMechinalCycle)
			{
				case 6:
					Previous2MechanicalDelay = Previous1MechanicalDelay;
					Previous1MechanicalDelay = SpeedCount;
					SpeedCount = 0;	
					if((Previous1MechanicalDelay <= SpeedLowLimitforSVP) && ((Previous1MechanicalDelay >= Previous2MechanicalDelay - (Previous2MechanicalDelay >> SpeedRippleLimitforSVP)) && (Previous1MechanicalDelay <= Previous2MechanicalDelay + (Previous2MechanicalDelay >> SpeedRippleLimitforSVP))))
					{
						debug = 0;
						if(Stablecnt >= 4)
						{
							SVPWMmode = 1;
						}
						else
							Stablecnt += 1;
					}
					else
					{
						debug = 1;
						Stablecnt = 0;
						SVPWMmode = 0;
					}
					break;
		/*		case 5:
					SpeedCount = Previous1MechanicalDelay * 1 / 6;		
					break;
				case 4:
					SpeedCount = Previous1MechanicalDelay * 2 / 6;		
					break;
				case 3:
					SpeedCount = Previous1MechanicalDelay * 3 / 6;		 
					break;
				case 2:
					SpeedCount = Previous1MechanicalDelay * 4 / 6;		
					break;
				case 1:
					SpeedCount = Previous1MechanicalDelay * 5 / 6;		
					break;*/
			}
		else switch(CurrentMechinalCycle)
			{
				case 1:
					Previous2MechanicalDelay = Previous1MechanicalDelay;
					Previous1MechanicalDelay = SpeedCount;
					SpeedCount = 0;	
					if((Previous1MechanicalDelay <= SpeedLowLimitforSVP) && ((Previous1MechanicalDelay >= Previous2MechanicalDelay - (Previous2MechanicalDelay >> SpeedRippleLimitforSVP)) && (Previous1MechanicalDelay <= Previous2MechanicalDelay + (Previous2MechanicalDelay >> SpeedRippleLimitforSVP))))
					{		
						debug = 0;
						if(Stablecnt >= 4)
						{
							SVPWMmode = 1;
						}
						else
						Stablecnt += 1;
					}
					else
					{
						debug = 1;
						SVPWMmode = 0;
						Stablecnt = 0;
					}
					break;
			/*	case 2:
					SpeedCount = Previous1MechanicalDelay * 1 / 6;		
					break;
				case 3:
					SpeedCount = Previous1MechanicalDelay * 2 / 6;		
					break;
				case 4:
					SpeedCount = Previous1MechanicalDelay * 3 / 6;		 
					break;
				case 5:
					SpeedCount = Previous1MechanicalDelay * 4 / 6;		
					break;
				case 6:
					SpeedCount = Previous1MechanicalDelay * 5 / 6;		
					break;*/
			}
		}
	PrevoiusMechinalCycle = CurrentMechinalCycle;
	CalcElectricAngle = (unsigned long)SpeedCount * 719 / ((unsigned long)Previous1MechanicalDelay + (unsigned long)Previous2MechanicalDelay);
	if(SVPWMmode)
	{		
			if(CalcElectricAngle <= 360)
			{
				//CalcElectricAngle = CalcElectricAngle % 360;
			if(ReverseSpin)
				CalcElectricAngle += ElecAngleOffestCW;
			else
				CalcElectricAngle += ElecAngleOffestCCW;
			if(CalcElectricAngle >= 360) 
				CalcElectricAngle -= 360;
			if(ReverseSpin)
				CalcElectricAngle = 360 - CalcElectricAngle;
			CalculateInverterVectorsWidth_Polar(CalcElectricAngle, 128);
		}
	}
	else
	{
		BLDCTimerEventHandler();
	}
	PreviousElectricAngle = CalcElectricAngle;
}

void UART_Write_Int_Value(unsigned int num)
{
	UartSend(number[num%1000/100]);
	UartSend(number[num%100/10]);
	UartSend(number[num%10]);
}

void TimerInit()
{
	CKCON |= 0X18;
	TMOD = 0x00;       
	TH1 = 0x01;
	TL1 = 0x24;
	TR1 = 1;      
	ET1 = 1;
	EA = 1;
}

void PWM_Interrupu_Init()
{
	EIE |= 0X08;
}

void PWM_Interr_ISR() interrupt 13
{
	UpdateHall();
}

void main(void)
{
	unsigned int i;
//	UartInit();
//  ES = 1;
//  EA = 1;
	Inverter_ControlGPIO_Init();
	HallGpioInit();
	TimerInit();
	PWM_Interrupu_Init();
	SetBLDCSpeed(128);
//  UartSendStr("DAS02418");
	while(1)
	{
		for(i = 0;i <= 360;i ++)
		{	
			
	//	BLDCTimerEventHandler();
	//		UpdateBLDCInverter(i);
			delay(362);

		//	CalculateInverterVectorsWidth_Polar(i, 12);
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