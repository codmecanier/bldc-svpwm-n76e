#include <N76E003.h>
#include <BLDC_Sensorless.h>

unsigned int DCBUS_Voltage = 0;


unsigned int Phase_U_Voltage_ADC_Value;
unsigned int Phase_V_Voltage_ADC_Value;
unsigned int Phase_W_Voltage_ADC_Value;


void Set_Phase_U_Voltage_ADC_Value(unsigned int i)
{
	Phase_U_Voltage_ADC_Value = i;
}
void Set_Phase_V_Voltage_ADC_Value(unsigned int i)
{
	Phase_V_Voltage_ADC_Value = i;
}
void Set_Phase_W_Voltage_ADC_Value(unsigned int i)
{
	Phase_W_Voltage_ADC_Value = i;
}

void BEMF_ReadVoltage(unsigned char phase) using 3
{
	switch(phase)
	{
		case 1: ADCCON0 = 0X05; break;
		case 2: ADCCON0 = 0X04; break;
		case 3: ADCCON0 = 0X03; break;
	}
	ADCCON1 = 0X01;////
	ADCDLY = 0;
	ADCCON2 = 0x00;
	ADCCON0 |= 0X40;	//start adc
}

void Determine_BEMF_Detect_Channel(unsigned char eleccycle, unsigned char times) using 3
{
	bit bemf_cmp = 0;

	switch(eleccycle)
	{
		case 1: {		
			switch(times)
			{
				case 1: BEMF_ReadVoltage(1); break;
				case 2:	BEMF_ReadVoltage(3); break;
				case 3:	
					DCBUS_Voltage = Phase_U_Voltage_ADC_Value;
					bemf_cmp = Phase_W_Voltage_ADC_Value < (DCBUS_Voltage >> 1);
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
					DCBUS_Voltage = Phase_U_Voltage_ADC_Value;
					bemf_cmp = Phase_V_Voltage_ADC_Value < (DCBUS_Voltage >> 1);
					break;
			}
				break;	//AC b increase
		}
		case 3: {
			switch(times)
			{
				case 1: BEMF_ReadVoltage(2); break;
				case 2:	BEMF_ReadVoltage(3); break;
				case 3:	
					DCBUS_Voltage = Phase_V_Voltage_ADC_Value;
					bemf_cmp = Phase_U_Voltage_ADC_Value < (DCBUS_Voltage >> 1);
					break;
			}
				break;	//BC a decrease
		}
		case 4: {	
			switch(times)
			{
				case 1: BEMF_ReadVoltage(2); break;
				case 2:	BEMF_ReadVoltage(1); break;
				case 3:	
					DCBUS_Voltage = Phase_V_Voltage_ADC_Value;
					bemf_cmp = Phase_W_Voltage_ADC_Value < (DCBUS_Voltage >> 1);
					break;
			}
				break;	//BA c increase
		}
		case 5: {
			switch(times)
			{
				case 1: BEMF_ReadVoltage(3); break;
				case 2:	BEMF_ReadVoltage(1); break;
				case 3:	
					DCBUS_Voltage = Phase_W_Voltage_ADC_Value;
					bemf_cmp = Phase_V_Voltage_ADC_Value < (DCBUS_Voltage >> 1);
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
					DCBUS_Voltage = Phase_W_Voltage_ADC_Value;
					bemf_cmp = Phase_U_Voltage_ADC_Value < (DCBUS_Voltage >> 1);
					break;
			}
				break;	//CB a increase
		}
		case 0:
			break;
	}
	if(times == 3)
	P07 = bemf_cmp;
}	

void BEMF_Gpio_ADCIN_Init()
{
	P0M1 |= 0x70;
	P0M2 &= 0x8f;
	AINDIDS  |= 0x38;
}
