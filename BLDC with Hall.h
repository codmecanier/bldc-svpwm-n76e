#define HAPort P04
#define HBPort P13 
#define HCPort P14

unsigned char DetermineCurrentElecCycle(bit reverse);

void UpdateHall();

void HallGpioInit();


void UpdateBLDCInverter(unsigned char eleccycle);

void BLDCTimerEventHandler();

void SetBLDCSpeed(unsigned char speed);