#include "math.h"
#include "NTC.h"

const float code NTC_Rp=10000.0; //10K
const float code NTC_T2 = (273.15+25.0);//T2
const float code NTC_Bx = 3950.0;//B
const float code NTC_Ka = 273.15;
unsigned int xdata NTC_ADC_Value = 0;

float Get_Temperature(unsigned long NTC_Rt)
{
	float temp;
	//like this R=5000, T2=273.15+25,B=3470, RT=5000*EXP(3470*(1/T1-1/(273.15+25)),  
	temp = NTC_Rt/NTC_Rp;
	temp = log(temp);//ln(Rt/Rp)
	temp/=NTC_Bx;//ln(Rt/Rp)/B
	temp+=(1/NTC_T2);
	temp = 1/(temp);
	temp-=NTC_Ka;
	return temp;
} 