#ifndef __BLDC_W_HALL__
#define __BLDC_W_HALL__

#include "typedef.h"

#define HAPort P15
#define HBPort P13 
#define HCPort P14

uint8_t DetermineCurrentElecCycle(bit reverse);

void UpdateHall();

void SetBLDCDirPWM(uint8_t pwm, bit dir);

void HallGpioInit();


void UpdateBLDCInverter();

void BLDCTimerEventHandler();

void SetBLDCPWM(uint8_t pwm);

void SetElecCycleU3(uint8_t i);

void SetElecCycleU2(uint8_t i);

void SetBLDCDirPWMU2(uint8_t pwm, bit dir);


#endif