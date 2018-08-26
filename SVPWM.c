#include "SVPWM.h"
#include "3PhaseInverter.h"

bit PhaseU_Target;
bit PhaseV_Target;
bit PhaseW_Target;

sbit debug = P1^0;

unsigned int T07,T1,T2;

bit SVPFirstHalfSector;

long Udcdvsqrt3, TSwitch,Udc;

unsigned char SVPWMCurrentZone = 0;

unsigned char sectornum ;
unsigned char CurrentTargetVector = 0;
unsigned char NextTargetVector = 0;

unsigned int CurrentSVPSectorTime, NextSVPSectorTime;

unsigned int code T1Array[]={222,
219,217,215,212,210,
207,204,202,199,196,
193,190,187,184,181,
178,175,171,168,165,
161,158,154,150,147,
143,139,136,132,128,
124,120,116,112,108,
104,100,96,92,88,
83,79,75,71,66,
62,58,53,49,44,
40,36,31,27,22,
18,13,9,4,0,
4,9,13,18,22,
27,31,36,40,44,
49,53,58,62,66,
71,75,79,83,88,
92,96,100,104,108,
112,116,120,124,128,
132,136,139,143,147,
150,154,158,161,165,
168,171,175,178,181,
184,187,190,193,196,
199,202,204,207,210,
212,215,217,219,222,
219,217,215,212,210,
207,204,202,199,196,
193,190,187,184,181,
178,175,171,168,165,
161,158,154,150,147,
143,139,136,132,128,
124,120,116,112,108,
104,100,96,92,88,
83,79,75,71,66,
62,58,53,49,44,
40,36,31,27,22,
18,13,9,4,0,
4,9,13,18,22,
27,31,36,40,44,
49,53,58,62,66,
71,75,79,83,88,
92,96,100,104,108,
112,116,120,124,128,
132,136,139,143,147,
150,154,158,161,165,
168,171,175,178,181,
184,187,190,193,196,
199,202,204,207,210,
212,215,217,219,222,
219,217,215,212,210,
207,204,202,199,196,
193,190,187,184,181,
178,175,171,168,165,
161,158,154,150,147,
143,139,136,132,128,
124,120,116,112,108,
104,100,96,92,88,
83,79,75,71,66,
62,58,53,49,44,
40,36,31,27,22,
18,13,9,4,0,
4,9,13,18,22,
27,31,36,40,44,
49,53,58,62,66,
71,75,79,83,88,
92,96,100,104,108,
112,116,120,124,128,
132,136,139,143,147,
150,154,158,161,165,
168,171,175,178,181,
184,187,190,193,196,
199,202,204,207,210,
212,215,217,219,222
};

unsigned int code T2Array[]={0,
4,9,13,18,22,
27,31,36,40,44,
49,53,58,62,66,
71,75,79,83,88,
92,96,100,104,108,
112,116,120,124,128,
132,136,139,143,147,
150,154,158,161,165,
168,171,175,178,181,
184,187,190,193,196,
199,202,204,207,210,
212,215,217,219,222,
219,217,215,212,210,
207,204,202,199,196,
193,190,187,184,181,
178,175,171,168,165,
161,158,154,150,147,
143,139,136,132,128,
124,120,116,112,108,
104,100,96,92,88,
83,79,75,71,66,
62,58,53,49,44,
40,36,31,27,22,
18,13,9,4,0,
4,9,13,18,22,
27,31,36,40,44,
49,53,58,62,66,
71,75,79,83,88,
92,96,100,104,108,
112,116,120,124,128,
132,136,139,143,147,
150,154,158,161,165,
168,171,175,178,181,
184,187,190,193,196,
199,202,204,207,210,
212,215,217,219,222,
219,217,215,212,210,
207,204,202,199,196,
193,190,187,184,181,
178,175,171,168,165,
161,158,154,150,147,
143,139,136,132,128,
124,120,116,112,108,
104,100,96,92,88,
83,79,75,71,66,
62,58,53,49,44,
40,36,31,27,22,
18,13,9,4,0,
4,9,13,18,22,
27,31,36,40,44,
49,53,58,62,66,
71,75,79,83,88,
92,96,100,104,108,
112,116,120,124,128,
132,136,139,143,147,
150,154,158,161,165,
168,171,175,178,181,
184,187,190,193,196,
199,202,204,207,210,
212,215,217,219,222,
219,217,215,212,210,
207,204,202,199,196,
193,190,187,184,181,
178,175,171,168,165,
161,158,154,150,147,
143,139,136,132,128,
124,120,116,112,108,
104,100,96,92,88,
83,79,75,71,66,
62,58,53,49,44,
40,36,31,27,22,
18,13,9,4,0,
};


void CalculateInverterVectorsWidth_Polar(unsigned int deg, unsigned int length)
{
	length = length * 3 / 4;
	T1 = T1Array[deg] * length >> 5;
	T2 = T2Array[deg] *length >> 5;
	T07 = (1536 - T1 - T2);
	if((deg >= 0) && (deg < 60))
	{
		sectornum = 1;
		SVPFirstHalfSector = deg > 30;
	}
	else if((deg >= 60) && (deg < 120))
	{
		sectornum = 2;
		SVPFirstHalfSector = deg < 90;
	}
	else if((deg >= 120) && (deg < 180))
	{
		sectornum = 3;
		SVPFirstHalfSector = deg > 150;
	}
	else if((deg >= 180) && (deg < 240))
	{
		sectornum = 4;
		SVPFirstHalfSector = deg < 210;
	}
	else if((deg >= 240) && (deg < 300))
	{
		sectornum = 5;
		SVPFirstHalfSector = deg > 270;
	}
	else if((deg >= 300) && (deg < 360))
	{
		sectornum = 6;
		SVPFirstHalfSector = deg < 330;
	}
}

void CalculateNextSpwmZoneTimeAndVector()
{
	if(SVPWMCurrentZone < 2)
		SVPWMCurrentZone ++;
	else
		SVPWMCurrentZone = 0;
	if((SVPWMCurrentZone == 1))
	{
		if(SVPFirstHalfSector)
		{
			switch(sectornum)
			{
				case 1:
					NextTargetVector = 4;
					break;
				case 2:
					NextTargetVector = 2;
					break;
				case 3:
					NextTargetVector = 2;
					break;
				case 4:
					NextTargetVector = 1;
					break;
				case 5:
					NextTargetVector = 1;
					break;
				case 6:
					NextTargetVector = 4;
					break;
			}
			NextSVPSectorTime = T1;
		}
		else
		{		
			switch(sectornum)
			{
				case 1:
					NextTargetVector = 6;
					break;
				case 2:
					NextTargetVector = 6;
					break;
				case 3:
					NextTargetVector = 3;
					break;
				case 4:
					NextTargetVector = 3;
					break;
				case 5:
					NextTargetVector = 5;
					break;
				case 6:
					NextTargetVector = 5;
					break;
			}
			NextSVPSectorTime = T2;
		}
	}
	else if((SVPWMCurrentZone == 2))
	{
		if(SVPFirstHalfSector)
		{
			switch(sectornum)
			{
				case 1:
					NextTargetVector = 6;
					break;
				case 2:
					NextTargetVector = 6;
					break;
				case 3:
					NextTargetVector = 3;
					break;
				case 4:
					NextTargetVector = 3;
					break;
				case 5:
					NextTargetVector = 5;
					break;
				case 6:
					NextTargetVector = 5;
					break;
			}
			NextSVPSectorTime = T2;
		}
		else
		{
			switch(sectornum)
			{
				case 1:
					NextTargetVector = 4;
					break;
				case 2:
					NextTargetVector = 2;
					break;
				case 3:
					NextTargetVector = 2;
					break;
				case 4:
					NextTargetVector = 1;
					break;
				case 5:
					NextTargetVector = 1;
					break;
				case 6:
					NextTargetVector = 4;
					break;
			}
			NextSVPSectorTime = T1;
		}
	}
	else 
	{
		NextTargetVector = 0;
		NextSVPSectorTime = T07;
	}
	NextSVPSectorTime = ~NextSVPSectorTime;
}

unsigned int SwitchVector()
{
	CurrentSVPSectorTime = NextSVPSectorTime;
	switch(NextTargetVector)
	{
		case 0:
			TurnOFF_HU;
			TurnOFF_HV;
			TurnOFF_HW;
			CalculateNextSpwmZoneTimeAndVector();
			TurnON_LU;
			TurnON_LV;
			TurnON_LW;
			break;
		case 7:
			TurnOFF_LU;
			TurnOFF_LV;
			TurnOFF_LW;
			CalculateNextSpwmZoneTimeAndVector();
			TurnON_HU;
			TurnON_HV;
			TurnON_HW;
			break;
		case 1:	
			TurnOFF_HU;
			TurnOFF_HV;
			TurnOFF_LW;
			CalculateNextSpwmZoneTimeAndVector();
			TurnON_LU;
			TurnON_LV;
			TurnON_HW;
			break;
		case 2:
			TurnOFF_HU;
			TurnOFF_LV;
			TurnOFF_HW;
			CalculateNextSpwmZoneTimeAndVector();
			TurnON_LU;
			TurnON_HV;
			TurnON_LW;
			break;
		case 3:
			TurnOFF_HU;
			TurnOFF_LV;
			TurnOFF_LW;
			CalculateNextSpwmZoneTimeAndVector();
			TurnON_LU;
			TurnON_HV;
			TurnON_HW;
			break;
		case 4:
			TurnOFF_LU;
			TurnOFF_HV;
			TurnOFF_HW;
			CalculateNextSpwmZoneTimeAndVector();
			TurnON_HU;
			TurnON_LV;
			TurnON_LW;
			break;
		case 5:
			TurnOFF_LU;
			TurnOFF_HV;
			TurnOFF_LW;
			CalculateNextSpwmZoneTimeAndVector();
			TurnON_HU;
			TurnON_LV;
			TurnON_HW;
			break;
		case 6:
			TurnOFF_LU;
			TurnOFF_LV;
			TurnOFF_HW;
			CalculateNextSpwmZoneTimeAndVector();
			TurnON_HU;
			TurnON_HV;
			TurnON_LW;
			break;
		default:			
			CalculateNextSpwmZoneTimeAndVector();
			break;
	}
	return CurrentSVPSectorTime;
}





