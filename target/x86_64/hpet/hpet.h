#ifndef _CARDINAL_x86_64_HPET_H_
#define _CARDINAL_x86_64_HPET_H_

#include "types.h"

typedef enum {
    HPETError_None,
    HPETError_NotPresent
} HPETError;

HPETError
HPET_Initialize(void);

void
HPET_SetCounterEnableStatus(bool enableStatus);

uint64_t
HPET_GetElapsedTime(uint64_t diff);

uint64_t
HPET_GetPeriod(void);

uint64_t
HPET_GetCounterValue(void);

void
HPET_SetCounterValue(uint64_t val);

#endif