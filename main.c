#include <N76E003.h>
#include <SVPWM.h>
#include <3PhaseInverter.h>
#include <BLDC with Hall.h>
#include <BLDC_Sensorless.h>
#include "intrins.h"
//#include "NTC.c"

bit SVPWMmode = 0;
bit SVPReverseSpin = 1;
bit ENABLE_SVPWM_FOR_SYNCM = 0;
unsigned char ElecAngleOffestCCW = 189;
unsigned char StableCount = 10;
unsigned char ElecAngleOffestCW = 215; // 238wm // 222
unsigned char SVPAngleStep = 1;
unsigned char SVPNextAngleStep = 1;
unsigned char SpeedRippleLimitforSVP = 2;
unsigned int pdata SpeedLowLimitforSVP = 6000;
unsigned int pdata SatiSCyclesSwSVP = 0;
unsigned char pdata Stablecnt = 0;
unsigned int pdata SpeedCount = 0;
unsigned char pdata PrevoiusMechinalCycle = 0;
unsigned int CalcElectricAngle = 0;
unsigned int SVPDriveAngle = 0;
unsigned char SVPWMCurPWM = 0;
unsigned char xdata ExecuteSVPBL_PWM = 0;
unsigned int pdata PulseCount = 0;
unsigned char SVP_Angle_Delay = 0;
unsigned int pdata Previous1CaptureCnt,Previous2CaptureCnt,Previous3CaptureCnt,Previous4CaptureCnt;
unsigned int pdata Previous1MechanicalDelay, Previous2MechanicalDelay, CurrentElectricAngle, PreviousElectricAngle;
unsigned int pdata External_Analog_ADC_Value = 0;
unsigned int Current_SENSE_ADC_Value = 0;
unsigned char pdata CurrentElectricCycle = 0;
unsigned char ADC_SampleTimes = 0;

unsigned char DC_Volt_ADC_Channel = 0;
unsigned char BEMF_Volt_ADC_Channel = 0;

static bit ADC_IsSampleCurrentFinishd = 0;
//unsigned int NTC_ADC_Value;

#define DC_VOLTAGE_SMPL 0
#define BEMF_SMPL       1
#define NTC_ADC         2	
#define EXTERNAL_ANALOG 5	
#define CALCULATE_BEMF  6

unsigned char data ADC_Sample_Sequence[]=
{
DC_VOLTAGE_SMPL ,
BEMF_SMPL       ,
CALCULATE_BEMF	,
NTC_ADC         ,	
DC_VOLTAGE_SMPL ,
BEMF_SMPL       ,
CALCULATE_BEMF	,
EXTERNAL_ANALOG ,	
};

unsigned char code number[]={'0','1','2','3','4','5','6','7','8','9',};	
sbit debug1 = P0^7;

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
//	CAPCON3 = 0X04;
	CAPCON3 = 0X08;
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
	
	IP |= 0x40;
	IPH |= 0x40;
	
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
//	SetMotorSpin(ExecuteSVPBL_PWM,1);
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
		else if(C0H < 4)
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
 

void UpdateSVPFreq(unsigned int n) using 3
{
	T3CON &= 0XE7;
	RL3 = ~(n & 0xff);
	RH3 = ~(n >> 8);
	T3CON |= 0X08;
}


void Input_Capture_Interrupt_ISR() interrupt 12 using 3
{
	bit ripple = 0;
	CAPCON0 &= 0XFE;
	if(SVPReverseSpin)
		SVPDriveAngle = ElecAngleOffestCW;
	else
		SVPDriveAngle = ElecAngleOffestCCW;	
	Previous2MechanicalDelay = Previous1MechanicalDelay;
	Previous1MechanicalDelay = ((int)C0H << 8)+ C0L;
	Previous4CaptureCnt = Previous3CaptureCnt;
	Previous3CaptureCnt = Previous2CaptureCnt;
	Previous2CaptureCnt = Previous1CaptureCnt;
	Previous1CaptureCnt = Previous1MechanicalDelay;
	if(SVP_Angle_Delay > 15)
	{
	//	ElecAngleOffestCW++;
		SVP_Angle_Delay = 0;
	}
	switch(T2MOD)
	{
		case 0x49: break;
		case 0x59: Previous1MechanicalDelay <<= 1; break;
		case 0x69: Previous1MechanicalDelay <<= 2; break;
	}
	if(Previous4CaptureCnt > Previous3CaptureCnt)
	{
		if(Previous4CaptureCnt - Previous3CaptureCnt > 200)
		{
			ripple = 1;
		}
	}
	else
	{		
		if(Previous3CaptureCnt - Previous4CaptureCnt > 200)
		{
			ripple = 1;
		}
	}
	if(Previous3CaptureCnt > Previous2CaptureCnt)
	{
		if(Previous3CaptureCnt - Previous2CaptureCnt > 200)
		{
			ripple = 1;
		}
	}
	else
	{		
		if(Previous2CaptureCnt - Previous3CaptureCnt > 200)
		{
			ripple = 1;
		}
	}
	if(Previous2CaptureCnt > Previous1CaptureCnt)
	{
		if(Previous2CaptureCnt - Previous1CaptureCnt > 200)
		{
			ripple = 1;
		}
	}
	else
	{		
		if(Previous1CaptureCnt - Previous2CaptureCnt > 200)
		{
			ripple = 1;
		}
	}
	if(ripple)
	{
		UpdateSVPFreq(Previous1CaptureCnt);	
	}
	else
	{
		UpdateSVPFreq((Previous1CaptureCnt + Previous2CaptureCnt + Previous3CaptureCnt + Previous4CaptureCnt) >> 2);	
	}
/*	if(Previous2MechanicalDelay < Previous1MechanicalDelay)
	{
		if(ElecAngleOffestCW < 255)
		{
			ElecAngleOffestCW ++;
		}
	}
	else
	{
		if(ElecAngleOffestCW > 0)
		{
			ElecAngleOffestCW --;
		}
	}*/
	SetSpeedRange_SVPrecision();
	if((Previous1MechanicalDelay <= SpeedLowLimitforSVP) && ((Previous1MechanicalDelay >= Previous2MechanicalDelay - (Previous2MechanicalDelay >> SpeedRippleLimitforSVP)) && (Previous1MechanicalDelay <= Previous2MechanicalDelay + (Previous2MechanicalDelay >> SpeedRippleLimitforSVP))))
	{
		if(Stablecnt >= 4)
		{
			if(ENABLE_SVPWM_FOR_SYNCM)
			{
				SVPWMmode = 1;
			}
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
	if(SVPDriveAngle < 255-SVPAngleStep)
		SVPDriveAngle += SVPAngleStep;
	else
	{
		SVPDriveAngle = 0;
		if(SVP_Angle_Delay < 255)
			SVP_Angle_Delay++;
	}
	CalcElectricAngle = SVPDriveAngle;
	if(SVPWMmode)
	{		
			if(SVPReverseSpin)
				CalcElectricAngle = 255 - CalcElectricAngle;
			CalculateInverterVectorsWidth_Polar(CalcElectricAngle);
	}
	else
	{
		CurrentElectricCycle = DetermineCurrentElecCycle(GetBLDCDirection());
		UpdateBLDCInverter(CurrentElectricCycle);
	}
}

void PWM_Interr_ISR() interrupt 13 using 2
{
	PWMF = 0;
}

void ADC_CurrentShunt_Compare_Start(unsigned char elecc) using 3
{
	ADCCON0 &= 0XF0;
	ADCCON0 |= 0X01;
	ADCCON1 = 0X07;
	ADCDLY = 5;
	ADCCON2 = 0x20;	//enable fault brake
//	ADCCON0 |= 0X40;
}

void ADC_Interrupt_ISR() interrupt 11 using 3
{
	unsigned char i;
	ADCF = 0;
	i = ADCCON0 & 0X07;
//	debug1 = !debug1;
	switch(i)
	{
		case 0:
		{
//			NTC_ADC_Value = ADCRH << 4 + ADCRL;
			break;   //temperature adc
		}
		case 1:
		{
			Current_SENSE_ADC_Value = (ADCRH << 4) + ADCRL;
			ADC_IsSampleCurrentFinishd = 1;
			break;	//current sense adc
		}
		case 2:
		{
			External_Analog_ADC_Value = (ADCRH << 4) + ADCRL;
			break;	//external analog input
		}
		case 3:
		{
			Set_Phase_U_Voltage_ADC_Value((ADCRH << 4) + ADCRL);
			break;	//bemf w channel
		}
		case 4:		
		{
			Set_Phase_V_Voltage_ADC_Value((ADCRH << 4) + ADCRL);
			break;	//bemf v channel
		}
		case 5:
		{
			Set_Phase_W_Voltage_ADC_Value((ADCRH << 4) + ADCRL);
			break;	//bemf u channel
		}
	}	
	EA = 0;
	if(ADC_IsSampleCurrentFinishd)
	{		
		switch(ADC_Sample_Sequence[ADC_SampleTimes])
		{
			case DC_VOLTAGE_SMPL:
			{
				Determine_BEMF_Detect_Channel(CurrentElectricCycle,1);
				break;
			}
			case BEMF_SMPL:
			{		
				Determine_BEMF_Detect_Channel(CurrentElectricCycle,2);			
				Determine_BEMF_Detect_Channel(CurrentElectricCycle,3);
				break;
			}
			case CALCULATE_BEMF:
			{
		//		debug1 = !debug1;	
				ADC_CurrentShunt_Compare_Start(CurrentElectricCycle);
				break;
			}
			case NTC_ADC:
			{
					ADCCON0 &= 0XF0;
					ADCCON0 |= 0X00;
					ADCCON1 = 0X01;////
					ADCDLY = 0;
					ADCCON2 = 0x00;
					ADCCON0 |= 0X40;	//start adc
				break;
			}
			case EXTERNAL_ANALOG:
			{
					ADCCON0 &= 0XF0;
					ADCCON0 |= 0X02;
					ADCCON1 = 0X01;////
					ADCDLY = 0;
					ADCCON2 = 0x00;
					ADCCON0 |= 0X40;	//start adc
				break;
			}
		}
	
		ADC_IsSampleCurrentFinishd = 0;
	}
	else
	{
		ADC_CurrentShunt_Compare_Start(CurrentElectricCycle);
	}
	EA = 1;
//	debug1 = !debug1;	
	if(ADC_SampleTimes >= 7)
	{
			ADC_SampleTimes = 0;
	}
	else
	{
		ADC_SampleTimes += 1;
	}
}

void Set_Currrent_Limit_Threshold(unsigned int th)
{
	ADCMPH = th >> 8; //current limit
	ADCMPL = th & 0xff;;
}

void ADCInit()
{
	Set_Currrent_Limit_Threshold(0xfff);
	ADC_CurrentShunt_Compare_Start(CurrentElectricCycle);
	EADC = 1;
}

void main(void)
{
	unsigned int i;
//	UartInit();
//  ES = 1;
//  EA = 1;
	Inverter_ControlGPIO_Init();
	HallGpioInit();
	BEMF_Gpio_ADCIN_Init();
	ADCInit();
	SetMotorSpin(253,1);
	TimerInit();
//	PWM_Interrupu_Init();
	
	P0M1 &= 0x7f;
	P0M2 |= 0x80;
		
	P1M1 &= 0Xb0;
	P1M2 &= 0Xb0;	
	
	debug1 = 1;
	
//  UartSendStr("DAS02418");
	while(1)
	{
		for(i = 0;i < 254;i += 1)
		{	
			
	//	BLDCTimerEventHandler();
	//		UpdateBLDCInverter(i);

			//CalculateInverterVectorsWidth_Polar(i);
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