#define HA P04
#define HB P13 
#define HC P14

unsigned char DetermineCurrentElecCycle(bit reverse);

void HallGpioInit();


void UpdateBLDCInverter(unsigned char eleccycle);

void BLDCTimerEventHandler();

void SetBLDCSpeed(unsigned char speed);