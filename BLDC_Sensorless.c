#include <N76E003.h>
#include <BLDC_Sensorless.h>

#define MagDecayPulseDct 2
#define MagDecayPulseCnt 4
unsigned int DCBUS_Voltage = 0;

unsigned int data Phase_UVW_Voltage_ADC_Value[3];
	
unsigned char data PreviousBEMF_CH;
unsigned char data PrevoiusBEMF_Value;
unsigned char data BEMF_Slope_Count;

void Set_Phase_U_Voltage_ADC_Value(unsigned int i) using 3
{
	Phase_UVW_Voltage_ADC_Value[0] = i;
}
void Set_Phase_V_Voltage_ADC_Value(unsigned int i) using 3
{
	Phase_UVW_Voltage_ADC_Value[1] = i;
}
void Set_Phase_W_Voltage_ADC_Value(unsigned int i) using 3
{
	Phase_UVW_Voltage_ADC_Value[2] = i;
}

void BEMF_ReadVoltage(unsigned char phase) using 3
{
	ADCCON1 = 0X01;////
	switch(phase)
	{
		case 1: 
		{
			ADCCON0 &= 0XF0;
			ADCCON0 |= 0X03;
			break;
		}
		case 2: 
		{
			ADCCON0 &= 0XF0;
			ADCCON0 |= 0X04;
			break;
		}
		case 3: 
		{
			ADCCON0 &= 0XF0;
			ADCCON0 |= 0X05;
			break;
		}
	}
	ADCDLY = 0;
	ADCCON2 = 0x00;
	ADCCON0 |= 0X40;	//start adc
}

void Determine_BEMF_Detect_Channel(unsigned char eleccycle, unsigned char times) using 3
{
	bit bemf_cmp = 0;
	bit slope_increase = 0;
	unsigned char i = 0;
	unsigned char bemf_frc_idx = 0;
	unsigned char bemf_frc_idxp = 0;
	unsigned char bemf_channel;

	switch(eleccycle)
	{
		case 1: {		
			switch(times)
			{
				case 1: BEMF_ReadVoltage(1); break;
				case 2:	BEMF_ReadVoltage(3); break;
				case 3:	
					bemf_channel = 2;
					slope_increase = 0;
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[0];
					break;
			}
				break;	//AB c decrease  
		}
		case 2: {
			switch(times)
			{
				case 1: BEMF_ReadVoltage(1); break;
				case 2:	BEMF_ReadVoltage(2); break;
				case 3:	
					bemf_channel = 1;
					slope_increase = 1;
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[0];
					break;
			}
				break;	//AC b increase
		}
		case 3: {
			switch(times)
			{
				case 1: BEMF_ReadVoltage(2); break;
				case 2:	BEMF_ReadVoltage(1); break;
				case 3:	
					bemf_channel = 0;
					slope_increase = 0;
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[1];
					break;
			}
				break;	//BC a decrease
		}
		case 4: {	
			switch(times)
			{
				case 1: BEMF_ReadVoltage(2); break;
				case 2:	BEMF_ReadVoltage(3); break;
				case 3:	
					bemf_channel = 2;
					slope_increase = 1;
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[1];
					break;
			}
				break;	//BA c increase
		}
		case 5: {
			switch(times)
			{
				case 1: BEMF_ReadVoltage(3); break;
				case 2:	BEMF_ReadVoltage(2); break;
				case 3:	
					bemf_channel = 1;
					slope_increase = 0;
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[2];
					break;
			}
				break;	//CA b decrease
		}
		case 6: {
			switch(times)				
			{
				case 1: BEMF_ReadVoltage(3); break;
				case 2:	BEMF_ReadVoltage(1); break;
				case 3:	
					bemf_channel = 0;
					slope_increase = 1;
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[2];
					break;
			}
				break;	//CB a increase
		}
		case 0:
			break;
	}
	if(times == 3)
	{
		if(slope_increase)
		{
			bemf_cmp = Phase_UVW_Voltage_ADC_Value[bemf_channel] < (DCBUS_Voltage >> 1);
//			if(Phase_UVW_Voltage_ADC_Value[bemf_channel] < (DCBUS_Voltage >> MagDecayPulseDct))
//				BEMF_Slope_Count = 0;
		}
		else
		{
			bemf_cmp = Phase_UVW_Voltage_ADC_Value[bemf_channel] > (DCBUS_Voltage >> 1);	
//			if(Phase_UVW_Voltage_ADC_Value[bemf_channel] > (DCBUS_Voltage - (DCBUS_Voltage >> MagDecayPulseDct)))
//				BEMF_Slope_Count = 0;
		}
		if(!bemf_cmp && (BEMF_Slope_Count <= (MagDecayPulseCnt >> 1)))
			BEMF_Slope_Count++;
		if(bemf_cmp && (BEMF_Slope_Count >= (MagDecayPulseCnt >> 1)) && (BEMF_Slope_Count <= MagDecayPulseCnt))
			BEMF_Slope_Count++;
		if(PreviousBEMF_CH != bemf_channel)
			BEMF_Slope_Count = 0;
		PreviousBEMF_CH = bemf_channel;
		PrevoiusBEMF_Value = Phase_UVW_Voltage_ADC_Value[bemf_channel];
		P07 = (BEMF_Slope_Count >= MagDecayPulseCnt) && bemf_cmp;
	}
}	

void BEMF_Gpio_ADCIN_Init()
{
	P0M1 |= 0x70;
	P0M2 &= 0x8f;
	AINDIDS  |= 0x3E;
}
