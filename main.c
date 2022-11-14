#include <N76E003.h>
#include <SVPWM.h>
#include <3PhaseInverter.h>
#include <BLDC with Hall.h>
#include <BLDC_Sensorless.h>
#include "intrins.h"
#include <stdint.h>
//#include "NTC.c"

uint8_t xdata StartUP_Process = 0;

uint8_t xdata	STARTUP_FREQUENCY = 10;
uint8_t xdata	STARTUP_END_FREQUENCY = 70;
uint8_t xdata	STARTUP_PWM =	23;
uint8_t xdata	STARTUP_END_PWM = 50;
uint16_t xdata ACCELERATION_TIME = 1000;
uint16_t xdata LOCK_POSITION_TIME = 500	;  
uint16_t xdata LOCK_POSITION_PWM = 26	;  
uint16_t xdata DIREACTION_CHANGE_DELAY = 400;	

uint16_t xdata MAX_FREQUENCY = 4000;
uint16_t xdata MIN_FREQUENCY = 1;
uint16_t xdata Current_Frequency = 0;
uint16_t xdata FREQUENCY_SECTION_CUT = 0;
uint16_t xdata VF_RATIO_1 = 3;
uint16_t xdata VF_RATIO_2 = 3;
uint16_t xdata REFERENCE_DC_VOLT = 0;
uint16_t xdata VF_ACCELERATION_HZS2 = 1000;
uint16_t xdata VF_DECELERATION_HZS2 = 1000;

bit SVPWMmode = 0;
bit SVPReverseSpin = 1;
bit ENABLE_SVPWM_FOR_SYNCM = 0;
bit BLDC_SENSORLESS = 1;
bit ASYNC_3_PHASE = 1;

bit SVPWMmode = 1;
bit SVPReverseSpin = 1;
volatile bit BEMF_PWM_ON_Detect = 1;

volatile bit data CShunt_ADC_Interrupt = 0;

uint8_t Adc_Smpl_Count = 0;

uint16_t xdata  DelayMsBetweenCurrentElectricalCycle = 0;
uint16_t xdata  UsedStartupTime = 0; 
uint16_t xdata  Accelerationtime = 0;
uint16_t xdata  AccelerationFrequency = 0;	 
uint16_t  xdata AccelerationPWM = 0;
unsigned long xdata  CurrentFrequency = 0;

uint8_t pdata BLDC_SNSLess_PWM = 0;
uint8_t adcbemfreg0s,adcalterreg0s,bcrtnv = 0;

uint8_t pdata ElecAngleOffestCCW = 189;
uint8_t pdata StableCount = 10;
uint8_t pdata ElecAngleOffestCW = 215; // 238wm // 222
uint8_t pdata SVPAngleStep = 1;
uint8_t pdata SVPNextAngleStep = 1;
uint8_t pdata SpeedRippleLimitforSVP = 2;
uint16_t pdata SpeedLowLimitforSVP = 6000;
uint16_t pdata SatiSCyclesSwSVP = 0;
uint8_t pdata Stablecnt = 0;
uint16_t pdata SpeedCount = 0;
uint8_t pdata PrevoiusMechinalCycle = 0;
uint16_t pdata CalcElectricAngle = 0;
uint16_t pdata  SVPDriveAngle = 0;
uint8_t pdata SVPWMCurPWM = 0;
uint8_t xdata ExecuteSVPBL_PWM = 0;
uint16_t pdata PulseCount = 0;
uint8_t pdata SVP_Angle_Delay = 0;
volatile uint16_t pdata Previous1MechanicalDelay,Previous2MechanicalDelay;
uint16_t pdata Previous1CaptureCnt,Previous2CaptureCnt,Previous3CaptureCnt,Previous4CaptureCnt;
uint16_t pdata CurrentElectricAngle, PreviousElectricAngle;

uint16_t data External_Analog_ADC_Value = 0;
uint16_t data Current_SENSE_ADC_Value = 0;
uint16_t data BEMF_ADC_Value = 0;
uint16_t data DCBUS_ADC_Value = 0;

volatile uint8_t pdata CurrentElectricCycle = 0;
uint8_t ADC_SampleTimes = 0;
uint8_t pdata BLDC_Sensorless_Status = 0;


uint8_t pdata DC_Volt_ADC_Channel = 0;
uint8_t pdata BEMF_Volt_ADC_Channel = 0;

//uint16_t NTC_ADC_Value;

#define DC_VOLTAGE_SMPL 0
#define BEMF_SMPL       1
#define NTC_ADC         2	
#define EXTERNAL_ANALOG 5	

void delay(uint16_t i)
{
	while(i--);
}
const uint8_t BEMF_DCT_Params[6][3] = {
	{0,2,0},
	{0,1,1},
	{1,0,0},
	{1,2,1},
	{2,1,0},
	{2,0,1},
};

const uint8_t ADC_Sample_Sequence[]=
{
	NTC_ADC,
	EXTERNAL_ANALOG ,	
};

//uint8_t code number[]={'0','1','2','3','4','5','6','7','8','9',};	
sbit debug1 = P0^7;

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

//void UART_Write_Int_Value(uint16_t num)
//{
//	UartSend(number[num%1000/100]);
//	UartSend(number[num%100/10]);
//	UartSend(number[num%10]);
//}

void TimerInit()
{
//	CKCON |= 0X18;
	TMOD = 0x00; 

	if(BLDC_SENSORLESS)
	{
		T2MOD = 0X60; //Set Timer2 Params
		TH2 = TL2 = 0;
	}
	else
	{
	T2MOD = 0X69; //low speed 69 high speed 49 mid speed 59 Corresponds to different scaling factor (256, 128, 64)
			
		CAPCON0 |= 0X10;	//Hall Signal Capture
		CAPCON1 = 0X00;
		CAPCON2 = 0X10;
	//	CAPCON3 = 0X04;
		CAPCON3 = 0X08;
		RCMP2H = 0XFF;
		RCMP2H = 0XFE;
		EIE |= 0X04;		//input capture interrupt enable
	}
	
//	RL3 = 0X00;
//	RH3 = 0XF0;
	EIE1 |= 0X02;
//	T3CON &= 0XEF;
//	T3CON |= 0X08;
	
	TH1 = 0x70;
	TL1 = 0x24;
	
	T2CON |= 0X04; // TR2 = 1, Timer 2 Run
	
	TMOD |= 0x01;	
	TL0 = 0xAB;	
	TH0 = 0x2F;
	TF0 = 0;
	TR0 = 1;
	ET0 = 1;
	
	//Pin interrupts settings
	PICON = 0XFD;				//P1 interrupts edge triggled
	PINEN |= 0X78;
	PIPEN |= 0X38;
	
	//IRQ Priority settings
	IP |= 0x00;				//ADC priority second
	IPH |= 0x40;
	
	EIP |= 0X80;
	EIPH |= 0X80;			//PWM priority first
	
	EIP1 |= 0x02;
	EIPH1 |= 0x02;
	
	EIE |= 0X02;			//Timer3 Interrupt enable
	
//	TR1 = 1;     
//	ET1 = 1;
	EA = 1;
}

void SetMotorSpin(uint8_t pwm, bit dir)
{
	uint16_t blpwm;
	blpwm = pwm;
	SetBLDCDirPWM(blpwm,dir);
	SetSVPWMValue(pwm);
	SVPReverseSpin = dir;
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
}

// GPIO �������½����жϺ���
void Pin_Interrupt_ISR() interrupt 7  // GPIO Rising edge or falling edge triggered interrupt
{
	// 8 PIFs corresponds to eight GPIO interrupt sources independently
	if(PIF & 0x38) // Interrupt triggered by bit {3,4,5}
	{	// The Hall sensor Communitation counts here
		PIF &= 0x00;
		//These codes used only for Square Wave BLDC Drive
		//if(!BLDC_SENSORLESS && !SVPWMmode)
		if(!0 && !SVPWMmode)
		{
			CurrentElectricCycle = DetermineCurrentElecCycle(GetBLDCDirectionU3());
			SetElecCycleU3(CurrentElectricCycle);
			UpdateBLDCInverter();		
			ADC_CurrentShunt_Compare_Start();
		}
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
//	SetMotorSpin(ExecuteSVPBL_PWM,1);
	PulseCount = 0;
	TR0 = 1; 	//	Start Timer0
}

// Set the SVPWM Precision according to the current perios
void SetSpeedRange_SVPrecision() using 1   
{
	uint8_t i;
	switch(T2MOD)	// First read out the Period under differnet Timer scaling factors
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
 

void UpdateSVPFreq(uint16_t n) using 3
{
	T3CON &= 0XE7;
	RL3 = ~(n & 0xff);
	RH3 = ~(n >> 8);
	T3CON |= 0X08;
}

void SetASYNCSVPFTimerReg(uint16_t n)
{
	T3CON &= 0XE7;
	RL3 = ~(n & 0xff);
	RH3 = ~(n >> 8);
	T3CON |= 0X08;
}

void UpdateBLDC_Dly(uint16_t n) using 3
{
	T3CON &= 0XE7;
	T3CON &= 0xF8;
	T3CON |= 0X07;
	RL3 = ~(n & 0xff);
	RH3 = ~(n >> 8);
	T3CON |= 0X08;
}

void Input_Capture_Interrupt_ISR() interrupt 12 using 3
{
	bit ripple = 0;
	CAPCON0 &= 0XFE;
	Previous2MechanicalDelay = Previous1MechanicalDelay;
	EADC = 0;
	Previous1MechanicalDelay = ((int)C0H << 8)+ C0L;
	EADC = 1;
	if(ENABLE_SVPWM_FOR_SYNCM)
	{
		if(SVPReverseSpin)
			SVPDriveAngle = ElecAngleOffestCW;
		else
			SVPDriveAngle = ElecAngleOffestCCW;	
		Previous4CaptureCnt = Previous3CaptureCnt;
		Previous3CaptureCnt = Previous2CaptureCnt;
		Previous2CaptureCnt = Previous1CaptureCnt;
		Previous1CaptureCnt = Previous1MechanicalDelay;
		if(SVP_Angle_Delay > 15)
		{
			SVP_Angle_Delay = 0;
	// Capture the Angular Speed and compensate it according to the T2MOD value 
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
}

void BLDC_SNSLess_StepXL() using 2
{
		//30DEG delay Counting Start
		if(0)
		{
			if(CurrentElectricCycle<6)	CurrentElectricCycle+=1;
			else		
			{
				CurrentElectricCycle = 1;
			}
		}
		else
		{ 
			if(CurrentElectricCycle>1)	CurrentElectricCycle-=1;
			else		
			{
				CurrentElectricCycle = 6;	
			}
		}
}


void ADC_PWM_FallEdge_BEMF_Dct() using 1
{
	ADCCON2 = 0x00;	//disable fault brake		
	ADCCON1 = 0X03;		//PWM fallinmg Edge trig	
	ADCCON0 &= 0X70;
	if((CurrentElectricCycle == 1)||(CurrentElectricCycle == 2))
	{
		ADCCON0 |= (0X03 + BEMF_DCT_Params[CurrentElectricCycle - 1][DC_CH]);	// PWM0 trig
	}
	else 
	{
		if((CurrentElectricCycle == 3)||(CurrentElectricCycle == 4))
			ADCCON0 |= (0X03 + BEMF_DCT_Params[CurrentElectricCycle - 1][DC_CH]);	// PWM2 trig
		else
			ADCCON0 |= (0X03 + BEMF_DCT_Params[CurrentElectricCycle - 1][DC_CH]);	// PWM4 trig
	}
	ADCDLY = 20;
	CShunt_ADC_Interrupt = 0;
	EADC = 1;
}


void BLDC_StartUP_OnProcCalc() using 2
{
	CurrentFrequency = 	STARTUP_FREQUENCY + (UsedStartupTime / (Accelerationtime / AccelerationFrequency ) ) ;
	BLDC_SNSLess_PWM = STARTUP_PWM + (UsedStartupTime / (Accelerationtime / AccelerationPWM ) ) ;	
	DelayMsBetweenCurrentElectricalCycle = 10000 /  CurrentFrequency;	
	if(UsedStartupTime == 0)
	{
		UpdateBLDC_Dly(LOCK_POSITION_TIME * 125);			
		BLDC_SNSLess_PWM = LOCK_POSITION_PWM;
	}	
	else 
	{
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
		{
			ADC_CurrentShunt_Compare_Start();
		}
		else
		{
			ADC_PWM_FallEdge_BEMF_Dct();
		}	
	}
	else
		UsedStartupTime = UsedStartupTime + DelayMsBetweenCurrentElectricalCycle;	
}
 // Timer 3 for incrementing the driving angle in SVPWM condition
void Timer3_Interr_ISR() interrupt 16 using 2
{	
	T3CON &= 0XEF;	//clear timer interrupt
	if(BLDC_SENSORLESS)
	{	
		SetBLDCPWM(BLDC_SNSLess_PWM);		
		BLDC_SNSLess_StepXL();
		if(BLDC_Sensorless_Status == BLDC_Run)
		{
			T3CON &= 0XE7;		//Timer3 Stop	
		}
		SetElecCycleU2(CurrentElectricCycle);
		UpdateBLDCInverter();	
		ADC_CurrentShunt_Compare_Start();
		
		//Calculate Startup Process
		if(BLDC_Sensorless_Status == BLDC_Startup)
		{
			BLDC_StartUP_OnProcCalc();
		}
	}
	if(ENABLE_SVPWM_FOR_SYNCM || ASYNC_3_PHASE)
	{
		//These codes used only for SVPWM mode
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
	}
}
// The PWM edge Triggering functions interrupt
void PWM_Interr_ISR() interrupt 13 using 0
{
	PWMF = 0;
}



void ADC_Interrupt_ISR() interrupt 11 using 1
{
//	debug1 = 1;
	EADC = 0;
	ADCF = 0;
	ADCCON1 = 0X01;		//Enable ADC and Start Converting immediately
	
//	debug1 = !debug1;
	if(BLDC_SENSORLESS)
	{
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

void Set_Currrent_Limit_Threshold(uint16_t th)
{
	ADCMPH = th >> 8; //current limit
	ADCMPL = th & 0xff;;
}

void ADCInit()
{
	Set_Currrent_Limit_Threshold(0xfff);
	ADC_CurrentShunt_Compare_Start();
	EADC = 1;
}

void BLDC_SNSless_Parms_Calc()
{
	Accelerationtime = ACCELERATION_TIME * 10;
	AccelerationFrequency = STARTUP_END_FREQUENCY - STARTUP_FREQUENCY;
	AccelerationPWM	 = STARTUP_END_PWM - STARTUP_PWM ;
}

void main(void)
{
	uint16_t i;
//	UartInit();
	Inverter_ControlGPIO_Init();
	HallGpioInit();
	BEMF_Gpio_ADCIN_Init();
	ADCInit();
	SetMotorSpin(30,1);
	TimerInit();
	BLDC_SNSless_Parms_Calc();
	
	BLDC_Sensorless_Status = BLDC_Startup;
	
	UpdateBLDC_Dly(418);
	
//	PWM_Interrupu_Init();
	
	
	P0M1 &= 0x7f;
	P0M2 |= 0x80;
		
	P1M1 &= 0Xb0;
	P1M2 &= 0Xb0;	
	
	debug1 = 1;
	
	SetSVPWMValue(92);
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