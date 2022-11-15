
#ifndef __MC_SVPWM_TABLE__
#define __MC_SVPWM_TABLE__

#define DC_Compensation 1

#include "typedef.h"

void ExecuteSVPWMatGivenAngle(uint16_t deg);

void CalculateSVPWMtableWithGivenVctLength(uint8_t length);

#endif