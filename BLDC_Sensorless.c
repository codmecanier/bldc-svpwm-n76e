#include <N76E003.h>
#include <BLDC_Sensorless.h>

unsigned int DCBUS_Voltage = 0;

unsigned int data Phase_UVW_Voltage_ADC_Value[3][SAMPLE_BUFFER_LENGTH];
	
unsigned char data BEMF_BufIdx = 0;


void Set_Phase_U_Voltage_ADC_Value(unsigned int i) using 3
{
	Phase_UVW_Voltage_ADC_Value[0][BEMF_BufIdx] = i;
}
void Set_Phase_V_Voltage_ADC_Value(unsigned int i) using 3
{
	Phase_UVW_Voltage_ADC_Value[1][BEMF_BufIdx] = i;
}
void Set_Phase_W_Voltage_ADC_Value(unsigned int i) using 3
{
	Phase_UVW_Voltage_ADC_Value[2][BEMF_BufIdx] = i;
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
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[0][BEMF_BufIdx];
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
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[0][BEMF_BufIdx];
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
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[1][BEMF_BufIdx];
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
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[1][BEMF_BufIdx];
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
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[2][BEMF_BufIdx];
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
					DCBUS_Voltage = Phase_UVW_Voltage_ADC_Value[2][BEMF_BufIdx];
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
			bemf_cmp = Phase_UVW_Voltage_ADC_Value[bemf_channel][BEMF_BufIdx] < (DCBUS_Voltage >> 1);
			for(i = 0;i < BEMF_BufIdx;i += 1)
			{
				bemf_frc_idx = BEMF_BufIdx + i;
				if(bemf_frc_idx >= SAMPLE_BUFFER_LENGTH)
					bemf_frc_idx -= SAMPLE_BUFFER_LENGTH;
				if(bemf_frc_idx <= 0)
					bemf_frc_idxp = SAMPLE_BUFFER_LENGTH;
				else
					bemf_frc_idxp = bemf_frc_idx - 1;
				bemf_frc_idxp = bemf_frc_idx - 1;
				bemf_cmp = bemf_cmp && Phase_UVW_Voltage_ADC_Value[bemf_channel][bemf_frc_idx] < Phase_UVW_Voltage_ADC_Value[bemf_channel][bemf_frc_idxp];
			}
		}
		else
		{
			bemf_cmp = Phase_UVW_Voltage_ADC_Value[bemf_channel][BEMF_BufIdx] > (DCBUS_Voltage >> 1);			
			for(i = 0;i < BEMF_BufIdx;i += 1)
			{
				bemf_frc_idx = BEMF_BufIdx + i;
				if(bemf_frc_idx >= SAMPLE_BUFFER_LENGTH)
					bemf_frc_idx -= SAMPLE_BUFFER_LENGTH;
				if(bemf_frc_idx <= 0)
					bemf_frc_idxp = SAMPLE_BUFFER_LENGTH;
				else
					bemf_frc_idxp = bemf_frc_idx - 1;
				bemf_frc_idxp = bemf_frc_idx - 1;
				bemf_cmp = bemf_cmp && Phase_UVW_Voltage_ADC_Value[bemf_channel][bemf_frc_idx] > Phase_UVW_Voltage_ADC_Value[bemf_channel][bemf_frc_idxp];
			}
		}
		P07 = bemf_cmp;
		if(BEMF_BufIdx < SAMPLE_BUFFER_LENGTH - 1)
		{
			BEMF_BufIdx += 1;
		}
		else
		{
			BEMF_BufIdx = 0;
		}
	}
}	

void BEMF_Gpio_ADCIN_Init()
{
	P0M1 |= 0x70;
	P0M2 &= 0x8f;
	AINDIDS  |= 0x3E;
	BEMF_BufIdx = 0;
}
