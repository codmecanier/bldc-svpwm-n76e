#include <STC8FXX.h>
#include <SVPWM.h>
#include <3PhaseInverter.h>
#include <BLDC with Hall.h>
#include "intrins.h"

bit SVPWMmode = 1;

unsigned int SpeedCount = 0;
unsigned char PrevoiusMechinalCycle = 0;
unsigned long CalcElectricAngle;

unsigned int Previous1MechanicalDelay, Previous2MechanicalDelay, CurrentElectricAngle, PreviousElectricAngle;

unsigned char code number[]={'0','1','2','3','4','5','6','7','8','9',};	
sbit debug = P1^0;

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
    SCON = 0x50;
    T2L = BRT;
    T2H = BRT >> 8;
    AUXR = 0x15;
    wptr = 0x00;
    rptr = 0x00;
    busy = 0;
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


void delay(unsigned int t)
{
	while(t--);
}


void TM0_Isr() interrupt 1 using 1
{
	unsigned int t;
	TR0 = 0;
	TF0 = 0;
	if(SVPWMmode)
	{	
	//	debug = !debug;
		while(1)
		{
			t = SwitchVector();
			if(t > 150)
			{
				TL0 = t & 0xff;
				TH0 = t >> 8; 
				TR0 = 1;
				break;
			}
			else
			{
				while(--t);
			}
		}
	}
	else
	{
		t = BLDCTimerEventHandler();
		t = ~t;
		TL0 = t & 0xff;
		TH0 = t >> 8; 
		TR0 = 1;
		
	//	UpdateBLDCInverter(DetermineCurrentElecCycle(1),1);
	}
}

void TM1_Isr() interrupt 3 using 0
{
	unsigned char CurrentMechinalCycle;
	TF1 = 0;
//		debug = !debug;		
	if(SpeedCount < 65535) SpeedCount++;
	CurrentMechinalCycle = DetermineCurrentElecCycle();
	if(PrevoiusMechinalCycle != CurrentMechinalCycle)
	{
		if(CurrentMechinalCycle == 3)
		{
			Previous1MechanicalDelay = SpeedCount;
			SpeedCount = 0;
		}
/*		switch(CurrentMechinalCycle)
		{
			case 6:		
				CalculateInverterVectorsWidth_Polar(233, 255); break;
			case 1:		
				CalculateInverterVectorsWidth_Polar(300, 255); break;
			case 2:		
				CalculateInverterVectorsWidth_Polar(355, 255); break;
			case 3:		
				CalculateInverterVectorsWidth_Polar(45, 255); break;
			case 4:		
				CalculateInverterVectorsWidth_Polar(121, 255); break;
			case 5:		
				CalculateInverterVectorsWidth_Polar(173, 255); break;
		}*/
	}
	PrevoiusMechinalCycle = CurrentMechinalCycle;
	Previous2MechanicalDelay = Previous1MechanicalDelay;
	CalcElectricAngle = (unsigned long)SpeedCount * 359 / (unsigned long)Previous1MechanicalDelay;
	if(PreviousElectricAngle != CalcElectricAngle)
	{	
		if((CalcElectricAngle < 360) && SVPWMmode)
		{
			CalculateInverterVectorsWidth_Polar(CalcElectricAngle, 255);
		}
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
	AUXR |= 0xC0;
	TMOD = 0x01;                             
	TL0 = 0x66;                             
	TH0 = 0xfc;
	TH1 = 0xf0;
	TL1 = 0x24;
	TR1 = 1;
	TR0 = 1;                                 
	ET0 = 1;   
	ET1 = 1;
	EA = 1;
}

void main(void)
{
	unsigned int i,j,k;
	UartInit();
  ES = 1;
  EA = 1;
	Inverter_ControlGPIO_Init();
	HallGpioInit();
	TimerInit();
	SetBLDCSpeed(255);
	if(SVPWMmode)
		CalculateInverterVectorsWidth_Polar(20,128);
  UartSendStr("DAS02418");
	while(1)
	{
		for(i = 1;i <= 359;i ++)
		{	
//			delay(800);

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