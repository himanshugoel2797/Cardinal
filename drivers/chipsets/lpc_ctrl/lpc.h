#ifndef _LPC_DRIVER_H_
#define _LPC_DRIVER_H_

#include "types.h"
#include "managers.h"

uint32_t
LPC_Initialize(void);

uint32_t
LPC_TransitionPowerState(AOS_PowerStates ps);

#endif