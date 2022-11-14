#include <N76E003.h>
#include <SVPWM.h>
#include <3PhaseInverter.h>
#include <BLDC with Hall.h>
#include "intrins.h"

<<<<<<< Updated upstream
/************  The Configuration area  ***************
 
 
 
 
*****************************************************/

bit SVPWMmode = 0;
bit SVPReverseSpin = 1;
=======
unsigned char xdata StartUP_Process = 0;

unsigned char xdata	STARTUP_FREQUENCY = 10;
unsigned char xdata	STARTUP_END_FREQUENCY = 70;
unsigned char xdata	STARTUP_PWM =	23;
unsigned char xdata	STARTUP_END_PWM = 50;
unsigned int xdata ACCELERATION_TIME = 1000;
unsigned int xdata LOCK_POSITION_TIME = 500	;  
unsigned int xdata LOCK_POSITION_PWM = 26	;  
unsigned int xdata DIREACTION_CHANGE_DELAY = 400;	


bit SVPWMmode = 0;
bit SVPReverseSpin = 1;
bit ENABLE_SVPWM_FOR_SYNCM = 0;
bit BLDC_SENSORLESS = 1;
volatile bit BEMF_PWM_ON_Detect = 1;

volatile bit data CShunt_ADC_Interrupt = 0;

unsigned char Adc_Smpl_Count = 0;

unsigned int xdata  DelayMsBetweenCurrentElectricalCycle = 0;
unsigned int xdata  UsedStartupTime = 0; 
unsigned int xdata  Accelerationtime = 0;
unsigned int xdata  AccelerationFrequency = 0;	 
unsigned int  xdata AccelerationPWM = 0;
unsigned long xdata  CurrentFrequency = 0;

unsigned char pdata BLDC_SNSLess_PWM = 0;
unsigned char adcbemfreg0s,adcalterreg0s,bcrtnv = 0;
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
void UartSendStr(char *p)
{
    while (*p)
    {
        UartSend(*p++);
    }
}

=======
const unsigned char BEMF_DCT_Params[6][3] = {
	{0,2,0},
	{0,1,1},
	{1,0,0},
	{1,2,1},
	{2,1,0},
	{2,0,1},
};
>>>>>>> Stashed changes

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

//#define FOSC            30000000UL
//#define BRT             (65536 - FOSC / 115200 / 4)

//bit busy;
//char wptr;
//char rptr;
//char buffer[16];

//void UartIsr() interrupt 4 using 1
//{
//    if (TI)
//    {
//        TI = 0;
//        busy = 0;
//    }
//    if (RI)
//    {
//        RI = 0;
//        buffer[wptr++] = SBUF;
//        wptr &= 0x0f;
//    }
//}

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

//void UartSend(char dat)
//{
//    while (busy);
//    busy = 1;
//    SBUF = dat;
//}

//void UartSendStr(char *p)
//{
//    while (*p)
//    {
//        UartSend(*p++);
//    }
//}

//void UART_Write_Int_Value(unsigned int num)
//{
//	UartSend(number[num%1000/100]);
//	UartSend(number[num%100/10]);
//	UartSend(number[num%10]);
//}

void TimerInit()
{
//	CKCON |= 0X18;
	TMOD = 0x00;   
	T2MOD = 0X69; //low speed 69 high speed 49 mid speed 59 Corresponds to different scaling factor (256, 128, 64)
	RCMP2H = 0XFF;  // Reaload value of Timer2
	RCMP2H = 0XFE;

	CAPCON0 |= 0X10;
	CAPCON1 = 0X00;
	CAPCON2 = 0X10;
	CAPCON3 = 0X04;
	
	RL3 = 0X00;
	RH3 = 0XF0;
	EIE1 |= 0X02;
	T3CON &= 0XEF;
	T3CON |= 0X08;
	
	TH1 = 0x70;
	TL1 = 0x24;
	EIE |= 0X04;
	
	T2CON |= 0X04; // TR2 = 1, Timer 2 Run
	
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
<<<<<<< Updated upstream
=======
	BLDC_SNSLess_PWM = pwm;
	BEMF_PWM_ON_Detect = pwm > 100;
}

/*
void ADC_CurrentShunt_Compare_Start() using 1
{
	ADCCON1 = 0X07;		
	ADCCON0 &= 0X30;
	ADCDLY = 5;
	ADCCON2 = 0x20;	//enable fault brake
	CShunt_ADC_Interrupt = 1;
	EADC = 1;
//	ADCCON0 |= 0X40;
}*/

void ADC_CurrentShunt_Compare_Start() using 1
{
	ADCCON1 = 0X07;		
	if((CurrentElectricCycle == 1)||(CurrentElectricCycle == 2))
	{
		ADCCON0 = 0X00;	// PWM0 trig
	}
	else 
	{
		if((CurrentElectricCycle == 3)||(CurrentElectricCycle == 4))
			ADCCON0 = 0X10;	// PWM2 trig
		else
			ADCCON0 = 0X20;	// PWM4 trig
	}
	ADCDLY = 5;
	ADCCON2 = 0x20;	//enable fault brake
	CShunt_ADC_Interrupt = 1;
	EADC = 1;
	//ADCCON0 |= 0X40;
>>>>>>> Stashed changes
}

// GPIO 上升沿下降沿中断函数
void Pin_Interrupt_ISR() interrupt 7  // GPIO Rising edge or falling edge triggered interrupt
{
	// 8 PIFs corresponds to eight GPIO interrupt sources independently
	if(PIF & 0x38) // Interrupt triggered by bit {3,4,5}
	{	// The Hall sensor Communitation counts here
	}
	if(PIF & 0x40)	// Interrupt triggered by the external clocking speed input
	{
		PIF &= 0XB0;	// Clear External Clock input Flag
		if(PulseCount < 0xff)
			PulseCount++;	// Count the number of pulses in one clock period
	}
	PIF &= 0x00; // Clear All the GPIO rising edge or falling edge triggered Flags
}

void Timer0_ISR() interrupt 1  // Timer O Used for Speed ramping now
{
	TR0 = 0; // Stop Timer0
	TF0 = 0;
	TL0 = 0xAB;	 // Load Initial Values
	TH0 = 0x2F;
	PulseCount = 64;
	if(ExecuteSVPBL_PWM < PulseCount) ExecuteSVPBL_PWM++;
	if(ExecuteSVPBL_PWM > PulseCount) ExecuteSVPBL_PWM--;
	SetMotorSpin(ExecuteSVPBL_PWM,1);
	PulseCount = 0;
	TR0 = 1; 	//	Start Timer0
}

// Set the SVPWM Precision according to the current perios
void SetSpeedRange_SVPrecision() using 1   
{
	unsigned char i;
	switch(T2MOD)	// First read out the Period under differnet Timer scaling factors
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
		default :	// Full SVPWM precision (8 bits)
			T2MOD = 0x69;  // Set the timer scaling factor back
			SVPNextAngleStep = 1;
		break;
		
		case 1 :	// Halved SVPWM precision
			T2MOD = 0x59;
			SVPNextAngleStep = 2;
		break;
		
		case 2 :	// Quadarple reduced SVPWM Precision
			T2MOD = 0x49;
			SVPNextAngleStep = 4;
		break;
	}
}

void PWM_Interrupu_Init()
{
	EIE |= 0X08;
}
 

<<<<<<< Updated upstream
void UpdateSVPFreq(unsigned char th, unsigned char tl) using 3
{
	T3CON &= 0XE7;
	RL3 = tl;
	RH3 = th;
	T3CON |= 0X08;
}

=======
void UpdateSVPFreq(unsigned int n) using 3
{
	T3CON &= 0XE7;
	RL3 = ~(n & 0xff);
	RH3 = ~(n >> 8);
	T3CON |= 0X08;
}

void UpdateBLDC_Dly(unsigned int n) using 3
{
	T3CON &= 0XE7;
	T3CON &= 0xF8;
	T3CON |= 0X07;
	RL3 = ~(n & 0xff);
	RH3 = ~(n >> 8);
	T3CON |= 0X08;
}
>>>>>>> Stashed changes

void Input_Capture_Interrupt_ISR() interrupt 12 using 3
{
	CAPCON0 &= 0XFE;
	if(SVPReverseSpin)
		SVPDriveAngle = ElecAngleOffestCW;
	else
		SVPDriveAngle = ElecAngleOffestCCW;	
	Previous2MechanicalDelay = Previous1MechanicalDelay;

	// Capture the Angular Speed and compensate it according to the T2MOD value 
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
<<<<<<< Updated upstream
		if(Stablecnt >= 4)
=======
		UpdateBLDC_Dly(DelayMsBetweenCurrentElectricalCycle * 12);
	}  	
	if((UsedStartupTime > Accelerationtime))
	{
		//startup_failed
		BLDC_Sensorless_Status = BLDC_Run;			
		T3CON &= 0XE7;		//Timer3 Stop
		SetMotorSpin(150,1);
		SetBLDCPWM(BLDC_SNSLess_PWM);		
		
		BLDC_SNSLess_StepXL();
		SetElecCycleU2(CurrentElectricCycle);
		UpdateBLDCInverter();	
		if(BEMF_PWM_ON_Detect)
>>>>>>> Stashed changes
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

void Timer3_Interr_ISR() interrupt 16 using 1 // Timer 3 for incrementing the driving angle in SVPWM condition
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
			CalculateInverterVectorsWidth_Polar(CalcElectricAngle);
	}
<<<<<<< Updated upstream
	else
=======
	if(ENABLE_SVPWM_FOR_SYNCM)
>>>>>>> Stashed changes
	{
		BLDCTimerEventHandler();
	}
}

void PWM_Interr_ISR() interrupt 13 using 2  // The PWM edge Triggering functions interrupt
{
	PWMF = 0;
	ADCCON0 |= 0X40;
}


<<<<<<< Updated upstream
=======

void ADC_Interrupt_ISR() interrupt 11 using 1
{
//	debug1 = 1;
	EADC = 0;
	ADCF = 0;
	ADCCON1 = 0X01;		//Enable ADC and Start Converting immediately
	
//	debug1 = !debug1;
	
	if(CShunt_ADC_Interrupt)
	{
		Current_SENSE_ADC_Value  = (ADCRH << 4) + ADCRL;
		//  BEMF_PWM_ON_Detect
		if(BEMF_PWM_ON_Detect)
		{
			ADCCON0 &= 0X70;
			ADCCON0 |= (0X03 + BEMF_DCT_Params[CurrentElectricCycle - 1][DC_CH]) | 0x40;
		}
		else
		{
			ADC_PWM_FallEdge_BEMF_Dct();
		}
		//Start ADC for DC_Volt Detection
		adcbemfreg0s = (0X03 + BEMF_DCT_Params[CurrentElectricCycle  - 1][BEMF_CH]) | 0x40 ;
		if(Adc_Smpl_Count < 1)
			Adc_Smpl_Count += 1;
		else
			Adc_Smpl_Count = 0;
		adcalterreg0s = ADC_Sample_Sequence[Adc_Smpl_Count] | 0x40 ;
		if(!BEMF_PWM_ON_Detect)
		{
			return;
		}
		while(ADCS);
	}
	DCBUS_ADC_Value = (ADCRH << 4) + ADCRL;
	ADCCON0 &= 0X70;
	ADCCON0 |= adcbemfreg0s;	//Start ADC for BEMF Detection
	while(ADCS);
	
	BEMF_ADC_Value = (ADCRH << 4) + ADCRL;
	ADCCON0 &= 0X70;
	ADCCON0 |= adcalterreg0s;	//Start ADC for Alternate Detection
	bcrtnv = BEMF_Calculate(CurrentElectricCycle,DCBUS_ADC_Value,BEMF_ADC_Value,BEMF_PWM_ON_Detect);
	while(ADCS);
	
	if((bcrtnv == CurrentElectricCycle)&&(BLDC_Sensorless_Status == BLDC_Run))
	{		
		SetBLDCPWM(BLDC_SNSLess_PWM);		
		BLDC_SNSLess_StepXL();
		SetElecCycleU2(CurrentElectricCycle);
		UpdateBLDCInverter();	
	}
	ADC_CurrentShunt_Compare_Start();
//	debug1 = 0;
}

void Set_Currrent_Limit_Threshold(unsigned int th)
{
	ADCMPH = th >> 8; //current limit
	ADCMPL = th & 0xff;;
}

>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
//  ES = 1;
//  EA = 1;
	Inverter_ControlGPIO_Init();
	HallGpioInit();
	ADCInit();
	SetMotorSpin(0,1);
	TimerInit();
=======
	Inverter_ControlGPIO_Init();
	HallGpioInit();
	BEMF_Gpio_ADCIN_Init();
	ADCInit();
	SetMotorSpin(30,1);
	TimerInit();
	BLDC_SNSless_Parms_Calc();
	
	BLDC_Sensorless_Status = BLDC_Startup;
	
	UpdateBLDC_Dly(418);
	
>>>>>>> Stashed changes
//	PWM_Interrupu_Init();
	
	P0M1 &= 0xfb;
	P0M2 |= 0x04;
		
	P1M1 &= 0Xb0;
	P1M2 &= 0Xb0;	
	
	debug1 = 1;
	
//  UartSendStr("DAS02418");
	while(1)
	{
<<<<<<< Updated upstream
		for(i = 0;i < 255;i += 1)
=======
		for(i = 0;i < 254;i += 1)
>>>>>>> Stashed changes
		{	
			
	//	BLDCTimerEventHandler();
	//		UpdateBLDCInverter(i);
<<<<<<< Updated upstream
			delay(3000);

	//	CalculateInverterVectorsWidth_Polar(i);
=======

			//CalculateInverterVectorsWidth_Polar(i);
>>>>>>> Stashed changes
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