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

unsigned int code T1Array[]={1774,
1755,1737,1718,1698,1678,
1657,1636,1614,1592,1569,
1546,1522,1498,1473,1448,
1423,1397,1370,1344,1316,
1289,1261,1233,1204,1175,
1145,1115,1085,1055,1024,
993,961,930,898,866,
833,800,767,734,700,
667,633,599,565,530,
495,461,426,391,356,
320,285,250,214,178,
143,107,71,36,0,
36,71,107,143,178,
214,250,285,320,356,
391,426,461,495,530,
565,599,633,667,700,
734,767,800,833,866,
898,930,961,993,1024,
1055,1085,1115,1145,1175,
1204,1233,1261,1289,1316,
1344,1370,1397,1423,1448,
1473,1498,1522,1546,1569,
1592,1614,1636,1657,1678,
1698,1718,1737,1755,1774,
1755,1737,1718,1698,1678,
1657,1636,1614,1592,1569,
1546,1522,1498,1473,1448,
1423,1397,1370,1344,1316,
1289,1261,1233,1204,1175,
1145,1115,1085,1055,1024,
993,961,930,898,866,
833,800,767,734,700,
667,633,599,565,530,
495,461,426,391,356,
320,285,250,214,178,
143,107,71,36,0,
36,71,107,143,178,
214,250,285,320,356,
391,426,461,495,530,
565,599,633,667,700,
734,767,800,833,866,
898,930,961,993,1024,
1055,1085,1115,1145,1175,
1204,1233,1261,1289,1316,
1344,1370,1397,1423,1448,
1473,1498,1522,1546,1569,
1592,1614,1636,1657,1678,
1698,1718,1737,1755,1774,
1755,1737,1718,1698,1678,
1657,1636,1614,1592,1569,
1546,1522,1498,1473,1448,
1423,1397,1370,1344,1316,
1289,1261,1233,1204,1175,
1145,1115,1085,1055,1024,
993,961,930,898,866,
833,800,767,734,700,
667,633,599,565,530,
495,461,426,391,356,
320,285,250,214,178,
143,107,71,36,0,
36,71,107,143,178,
214,250,285,320,356,
391,426,461,495,530,
565,599,633,667,700,
734,767,800,833,866,
898,930,961,993,1024,
1055,1085,1115,1145,1175,
1204,1233,1261,1289,1316,
1344,1370,1397,1423,1448,
1473,1498,1522,1546,1569,
1592,1614,1636,1657,1678,
1698,1718,1737,1755,1774,
};

unsigned int code T2Array[]={
0,36,71,107,143,178,
214,250,285,320,356,
391,426,461,495,530,
565,599,633,667,700,
734,767,800,833,866,
898,930,961,993,1024,
1055,1085,1115,1145,1175,
1204,1233,1261,1289,1316,
1344,1370,1397,1423,1448,
1473,1498,1522,1546,1569,
1592,1614,1636,1657,1678,
1698,1718,1737,1755,1774,
1755,1737,1718,1698,1678,
1657,1636,1614,1592,1569,
1546,1522,1498,1473,1448,
1423,1397,1370,1344,1316,
1289,1261,1233,1204,1175,
1145,1115,1085,1055,1024,
993,961,930,898,866,
833,800,767,734,700,
667,633,599,565,530,
495,461,426,391,356,
320,285,250,214,178,
143,107,71,36,0,
36,71,107,143,178,
214,250,285,320,356,
391,426,461,495,530,
565,599,633,667,700,
734,767,800,833,866,
898,930,961,993,1024,
1055,1085,1115,1145,1175,
1204,1233,1261,1289,1316,
1344,1370,1397,1423,1448,
1473,1498,1522,1546,1569,
1592,1614,1636,1657,1678,
1698,1718,1737,1755,1774,
1755,1737,1718,1698,1678,
1657,1636,1614,1592,1569,
1546,1522,1498,1473,1448,
1423,1397,1370,1344,1316,
1289,1261,1233,1204,1175,
1145,1115,1085,1055,1024,
993,961,930,898,866,
833,800,767,734,700,
667,633,599,565,530,
495,461,426,391,356,
320,285,250,214,178,
143,107,71,36,0,
36,71,107,143,178,
214,250,285,320,356,
391,426,461,495,530,
565,599,633,667,700,
734,767,800,833,866,
898,930,961,993,1024,
1055,1085,1115,1145,1175,
1204,1233,1261,1289,1316,
1344,1370,1397,1423,1448,
1473,1498,1522,1546,1569,
1592,1614,1636,1657,1678,
1698,1718,1737,1755,1774,
1755,1737,1718,1698,1678,
1657,1636,1614,1592,1569,
1546,1522,1498,1473,1448,
1423,1397,1370,1344,1316,
1289,1261,1233,1204,1175,
1145,1115,1085,1055,1024,
993,961,930,898,866,
833,800,767,734,700,
667,633,599,565,530,
495,461,426,391,356,
320,285,250,214,178,
143,107,71,36,0,
};


void CalculateInverterVectorsWidth_Polar(unsigned int deg, unsigned int length)
{
	T1 = T1Array[deg];
	T2 = T2Array[deg];
	T07 = (2048 - T1 - T2);
	if((deg >= 0) && (deg < 60))
	{
		sectornum = 1;
		SVPFirstHalfSector = deg > 30;
	}
	if((deg >= 60) && (deg < 120))
	{
		sectornum = 2;
		SVPFirstHalfSector = deg < 90;
	}
	if((deg >= 120) && (deg < 180))
	{
		sectornum = 3;
		SVPFirstHalfSector = deg > 150;
	}
	if((deg >= 180) && (deg < 240))
	{
		sectornum = 4;
		SVPFirstHalfSector = deg < 210;
	}
	if((deg >= 240) && (deg < 300))
	{
		sectornum = 5;
		SVPFirstHalfSector = deg > 270;
	}
	if((deg >= 300) && (deg < 360))
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
	debug = 0;
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
	debug = 1;
	return CurrentSVPSectorTime;
}





