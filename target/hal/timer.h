#ifndef _CARDINAL_HAL_TIMER_H_
#define _CARDINAL_HAL_TIMER_H_

#include "types.h"

void
InitializeTimer(void);

uint64_t
GetTimerValue(void);

void
SetTimerValue(uint64_t val);

void
SetTimerEnableMode(bool enabled);

uint64_t
GetTimerInterval_NS(uint64_t diff);

#endif