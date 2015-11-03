#ifndef _HPET_DRIVER_H_
#define _HPET_DRIVER_H_

#include "types.h"

uint8_t HPET_Initialize();
uint8_t HPET_GetTimerCount();
uint64_t HPET_GetFrequency();

uint64_t HPET_GetGlobalCounter();
void HPET_SetGlobalCounter(uint64_t val);

void HPET_SetEnable(int enable);

uint32_t HPET_GetSupportedIRQs(uint8_t index);
uint8_t HPET_IsPeriodicCapable(uint8_t index);
void HPET_SetTimerConfig(uint8_t index, uint8_t irq, uint8_t enable_int, uint8_t periodic, uint8_t timer_value_set, uint64_t tick_count);

#endif /* end of include guard: _HPET_DRIVER_H_ */
