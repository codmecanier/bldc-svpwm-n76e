#define HAPort P15
#define HBPort P13 
#define HCPort P14

unsigned char DetermineCurrentElecCycle(bit reverse);

void UpdateHall();

void SetBLDCDirPWM(unsigned char pwm, bit dir);

void HallGpioInit();


void UpdateBLDCInverter(unsigned char eleccycle);

void BLDCTimerEventHandler();
