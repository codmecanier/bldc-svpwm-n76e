#define HAPort P15
#define HBPort P13 
#define HCPort P14

unsigned char DetermineCurrentElecCycle(bit reverse);

void UpdateHall();

void SetBLDCDirPWM(unsigned char pwm, bit dir);

void HallGpioInit();


void UpdateBLDCInverter();

void BLDCTimerEventHandler();

void SetBLDCPWM(unsigned char pwm);

unsigned char GetBLDCDirectionU3();

void SetElecCycleU3(unsigned char i);

void SetElecCycleU2(unsigned char i);
