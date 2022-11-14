#include <N76E003.h>
#include <BLDC_Sensorless.h>

#define MagDecayPulseDct 3		//Magnent Decay Detect threshold
#define MagDecayPulseCnt 4		//Maagnent Decay Detect Count

#define PhaseSwitchCount 5

unsigned int DCBUS_Voltage = 0;

unsigned int data Phase_UVW_Voltage_ADC_Value[3];

unsigned char data Last_Rtn = 0;

const unsigned char BEMF_DCT_Params[6][3] = {
	{0,2,0},
	{0,1,1},
	{1,0,0},
	{1,2,1},
	{2,1,0},
	{2,0,1},
};

#define DC_CH 0
#define BEMF_CH 1
#define SLOPE 2

unsigned char data PreviousBEMF_CH;
unsigned char data PrevoiusBEMF_Value;
unsigned char data BEMF_Slope_Count;

void Set_Phase_U_Voltage_ADC_Value(unsigned int i) using 1
{
	Phase_UVW_Voltage_ADC_Value[0] = i;
}
void Set_Phase_V_Voltage_ADC_Value(unsigned int i) using 1
{
	Phase_UVW_Voltage_ADC_Value[1] = i;
}
void Set_Phase_W_Voltage_ADC_Value(unsigned int i) using 1
{
	Phase_UVW_Voltage_ADC_Value[2] = i;
}

void Start_BEMF_Detect_ADC(unsigned char eleccycle, unsigned char times, bit pwm_on_sense) using 1
{
	if(pwm_on_sense)
		ADCCON1 = 0X01;
	else
		ADCCON1 = 0X03;
	ADCCON0 &= 0XF0;
	eleccycle -= 1;
	if(times == 1)
		ADCCON0 |= 0X03 + BEMF_DCT_Params[eleccycle][DC_CH];
	else
		ADCCON0 |= 0X03 + BEMF_DCT_Params[eleccycle][BEMF_CH];
	ADCDLY = 0;
	ADCCON2 = 0x00;
	if(pwm_on_sense)
		ADCCON0 |= 0X40;	//start adc
}	


unsigned char BEMF_Calculate(unsigned char eleccycle) using 1
{	
	bit bemf_cmp = 0;
	unsigned char eleci;
	eleccycle -= 1;	
	if(PreviousBEMF_CH == eleccycle)
	{
		DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params[eleccycle][DC_CH]];
		if(BEMF_DCT_Params[eleccycle][SLOPE])
		{
			bemf_cmp = Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params[eleccycle][BEMF_CH]] < (DCBUS_Voltage >> 1);
			if(Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params[eleccycle][BEMF_CH]] < (DCBUS_Voltage >> MagDecayPulseDct))
				BEMF_Slope_Count = 0;
		}
		else
		{
			bemf_cmp = Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params[eleccycle][BEMF_CH]] > (DCBUS_Voltage >> 1);	
			if(Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params[eleccycle][BEMF_CH]] > (DCBUS_Voltage - (DCBUS_Voltage >> MagDecayPulseDct)))
				BEMF_Slope_Count = 0;
		}
		if(!bemf_cmp && (BEMF_Slope_Count <= (MagDecayPulseCnt >> 1)))
			BEMF_Slope_Count++;
		if(bemf_cmp && (BEMF_Slope_Count >= (MagDecayPulseCnt >> 1)) && (BEMF_Slope_Count <= MagDecayPulseCnt))
			BEMF_Slope_Count++;
		PrevoiusBEMF_Value = Phase_UVW_Voltage_ADC_Value[BEMF_DCT_Params[eleccycle][BEMF_CH]];	
		if((BEMF_Slope_Count >= MagDecayPulseCnt) && bemf_cmp)
		{
			eleci = eleccycle + 1;
			if(Last_Rtn != eleci)
			{	
				P07 = !P07;
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
}

void BEMF_Gpio_ADCIN_Init()
{
	P0M1 |= 0x70;
	P0M2 &= 0x8f;
	AINDIDS  |= 0x3E;
}
