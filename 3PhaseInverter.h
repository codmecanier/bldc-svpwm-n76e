#include <STC8FXX.h>

#define HINU_HIGH_ACTIVE 1
#define HINV_HIGH_ACTIVE 1
#define HINW_HIGH_ACTIVE 1

#define LINU_HIGH_ACTIVE 1
#define LINV_HIGH_ACTIVE 1
#define LINW_HIGH_ACTIVE 1

#define HINU P55
#define HINV P54
#define HINW P17

#define LINU P16
#define LINV P11
#define LINW P35

#define Switch_HU_ON HINU == HINU_HIGH_ACTIVE
#define Switch_HV_ON HINV == HINV_HIGH_ACTIVE
#define Switch_HW_ON HINW == HINW_HIGH_ACTIVE
	#define Switch_LU_ON LINU == LINU_HIGH_ACTIVE
#define Switch_LV_ON LINV == LINV_HIGH_ACTIVE
#define Switch_LW_ON LINW == LINW_HIGH_ACTIVE

#define TurnON_HU HINU = HINU_HIGH_ACTIVE
#define TurnON_HV HINV = HINV_HIGH_ACTIVE
#define TurnON_HW HINW = HINW_HIGH_ACTIVE
#define TurnON_LU LINU = LINU_HIGH_ACTIVE
#define TurnON_LV LINV = LINV_HIGH_ACTIVE
#define TurnON_LW LINW = LINW_HIGH_ACTIVE

#define TurnOFF_HU HINU = !HINU_HIGH_ACTIVE
#define TurnOFF_HV HINV = !HINV_HIGH_ACTIVE
#define TurnOFF_HW HINW = !HINW_HIGH_ACTIVE
#define TurnOFF_LU LINU = !LINU_HIGH_ACTIVE
#define TurnOFF_LV LINV = !LINV_HIGH_ACTIVE
#define TurnOFF_LW LINW = !LINW_HIGH_ACTIVE


void Inverter_ControlGPIO_Init();
void TurnOFFALLSwitches();
void CC2418();