#define DEADTIME 80

#define DC_Compensation 1

void Switch_Vector1();
void Switch_Vector2();
void Switch_Vector3();
void Switch_Vector4();
void Switch_Vector5();
void Switch_Vector6();
void Switch_Vector7();
void Switch_Vector0();

void DeadTimeDelay();
void SetUdcValue(unsigned int udc);
void CalculateNextSpwmZoneTimeAndVector();
unsigned int SVPWMTimerEventHandler();
void CalculateInverterVectorsWidth_Polar(unsigned int deg, unsigned int length);
unsigned int SwinbchVector();