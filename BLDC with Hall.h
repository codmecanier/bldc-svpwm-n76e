#define HA P34
#define HB P33 //problem is here always high
#define HC P32

unsigned char DetermineCurrentElecCycle(bit reverse);

void HallGpioInit();


void UpdateBLDCInverter(unsigned char eleccycle, bit L_Enable);

unsigned int BLDCTimerEventHandler();

void SetBLDCSpeed(unsigned char speed);