#include <N76E003.h>
#include <BLDC_Sensorless.h>

#define MagDecayPulseDct 3		//Magnent Decay Detect threshold
#define MagDecayPulseCnt 2		//Maagnent Decay Detect Count

#define PhaseSwitchCount 5

unsigned int data Prev_bemf;

unsigned char data Last_Rtn = 0;

bit Zero_Cross_Flag = 0;

unsigned char data PreviousBEMF_CH;
unsigned char data BEMF_Slope_Count;
unsigned int data Switch_Count = 0;
unsigned int data Zero_Cross_Count = 0;
unsigned int pdata Period_Count = 0;

const unsigned char BEMF_DCT_Params1[6][3] = {
	{0,2,0},
	{0,1,1},
	{1,0,0},
	{1,2,1},
	{2,1,0},
	{2,0,1},
};

/*
void Start_BEMF_Detect_ADC(unsigned char eleccycle, unsigned char times, bit pwm_on_sense) using 1
{
	if(pwm_on_sense)
		ADCCON1 = 0X01;
	else
		ADCCON1 = 0X03;
	ADCCON0 &= 0XF0;
	eleccycle -= 1;
	if(times == 1)
		ADCCON0 |= 0X03 + BEMF_DCT_Params1[eleccycle][DC_CH];
	else
		ADCCON0 |= 0X03 + BEMF_DCT_Params1[eleccycle][BEMF_CH];
	ADCDLY = 0;
	ADCCON2 = 0x00;
	if(pwm_on_sense)
		ADCCON0 |= 0X40;	//start adc
}	*/

/*
unsigned char BEMF_Calculate(unsigned char eleccycle) using 1
{	
	bit bemf_cmp = 0;
	unsigned char eleci;
	eleccycle -= 1;	
	if(PreviousBEMF_CH == eleccycle)
	{
		DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][DC_CH]];
//		if(BEMF_DCT_Params1[eleccycle][SLOPE])
//		{
//			bemf_cmp = Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] < (DCBUS_Voltage >> 1);
//			if(Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] < (DCBUS_Voltage >> MagDecayPulseDct))
//				BEMF_Slope_Count = 0;
//		}
//		else
//		{
//			bemf_cmp = Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] > (DCBUS_Voltage >> 1);	
//			if(Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] > (DCBUS_Voltage - (DCBUS_Voltage >> MagDecayPulseDct)))
//				BEMF_Slope_Count = 0;
//		}		DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][DC_CH]];
		if(BEMF_DCT_Params1[eleccycle][SLOPE])
		{
			if((Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] > (DCBUS_Voltage >> MagDecayPulseDct))&&(Prev_Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] > Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]]))
			{
					BEMF_Slope_Count++;
					if((BEMF_Slope_Count >= MagDecayPulseCnt) && Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] < (DCBUS_Voltage >> 1))
						bemf_cmp = 1;
			}
			else
				BEMF_Slope_Count = 0;
		}
		else
		{			
			if((Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] < (DCBUS_Voltage - (DCBUS_Voltage >> MagDecayPulseDct)))&&(Prev_Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] < Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]]))
			{
					BEMF_Slope_Count++;
					if((BEMF_Slope_Count >= MagDecayPulseCnt) && Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params1[eleccycle][BEMF_CH]] > (DCBUS_Voltage >> 1))
						bemf_cmp = 1;
			}
			else
				BEMF_Slope_Count = 0;
		}
		if(bemf_cmp)
		{
			eleci = eleccycle + 1;
			if(Last_Rtn != eleci)
			{	
	//			P07 = !P07;
				Last_Rtn = eleci;	
				return eleci;	
			}
		}
	}
	else
	{	
		BEMF_Slope_Count = 0;
//		P07 = !P07;
		PreviousBEMF_CH = eleccycle;
	}
//	P07 = 0;
	return 0;
}*/


unsigned char BEMF_Calculate(unsigned char eleccycle,unsigned int dcvolt,unsigned int bemf,bit pwmondct) using 1
{	
	bit bemf_cmp = 0;
	unsigned char eleci;
	eleccycle -= 1;	
	if(PreviousBEMF_CH == eleccycle)
	{
		
		if(BEMF_DCT_Params1[eleccycle][SLOPE])
		{
			if((!pwmondct||(bemf > (dcvolt >> MagDecayPulseDct)))&&(Prev_bemf > bemf))
			{
					BEMF_Slope_Count++;
					if((BEMF_Slope_Count >= MagDecayPulseCnt) && bemf < (dcvolt >> 1))
						bemf_cmp = 1;
			}
			else
				BEMF_Slope_Count = 0;
		}
		else
		{			
			if((!pwmondct || ((bemf < (dcvolt - (dcvolt >> MagDecayPulseDct)))))&&(Prev_bemf < bemf))
			{
					BEMF_Slope_Count++;
					if((BEMF_Slope_Count >= MagDecayPulseCnt) && bemf > (dcvolt >> 1))
						bemf_cmp = 1;
			}
			else
				BEMF_Slope_Count = 0;
		}
		Prev_bemf = bemf;
		if(bemf_cmp && (Zero_Cross_Count == 0))
		{
			Zero_Cross_Count = Switch_Count;
			Zero_Cross_Flag = 1;
		}
		if(Switch_Count >= (Zero_Cross_Count + (Period_Count >> 2) + (Period_Count >> 3)) && Zero_Cross_Flag)
		{				
			Zero_Cross_Flag = 0;
			eleci = eleccycle + 1;
			if(Last_Rtn != eleci)
			{	
				P07 = !P07;
				Last_Rtn = eleci;	
				Zero_Cross_Count = 0;
				return eleci;	
			}
		}
		Switch_Count ++;
	}
	else
	{	
		BEMF_Slope_Count = 0;
//		P07 = !P07;
		PreviousBEMF_CH = eleccycle;
		Period_Count = Switch_Count;
		Switch_Count = 0;
	}
//	P07 = 0;
	return 0;
}

void BEMF_Gpio_ADCIN_Init()
{
	P0M1 |= 0x70;
	P0M2 &= 0x8f;
	AINDIDS  |= 0x3E;
}
