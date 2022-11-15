#include "N76E003.h"
//#include <stdint.h>
#include "BLDC_Sensorless.h"

#define MagDecayPulseDct 3		//Magnent Decay Detect threshold
#define MagDecayPulseCnt 2		//Maagnent Decay Detect Count

#define PhaseSwitchCount 5

uint16_t data Prev_bemf;

uint8_t data Last_Rtn = 0;

bit Zero_Cross_Flag = 0;

uint8_t data PreviousBEMF_CH;
uint8_t data BEMF_Slope_Count;
uint16_t data Switch_Count = 0;
uint16_t data Zero_Cross_Count = 0;
uint16_t pdata Period_Count = 0;

const uint8_t BEMF_DCT_Params1[6][3] = {
	{0,2,0},
	{0,1,1},
	{1,0,0},
	{1,2,1},
	{2,1,0},
	{2,0,1},
};

uint8_t BEMF_Calculate(uint8_t eleccycle,uint16_t dcvolt,uint16_t bemf,bit pwmondct) using 1
{	
	bit bemf_cmp = 0;
	uint8_t eleci;
	eleccycle -= 1;	
	if(!pwmondct)
	{
		dcvolt = 0x08;
	}
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
				P07 = !P07;
		}
		if(Switch_Count >= (Zero_Cross_Count + (Period_Count >> 2) + (Period_Count >> 3)) && Zero_Cross_Flag)
		{				
			Zero_Cross_Flag = 0;
			eleci = eleccycle + 1;
			if(Last_Rtn != eleci)
			{	
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
		Zero_Cross_Flag = 0;
		Zero_Cross_Count = 0;
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
