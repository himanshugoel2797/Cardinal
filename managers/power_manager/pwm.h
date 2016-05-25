#ifndef _CARDINAL_POWER_MANAGER_H_
#define _CARDINAL_POWER_MANAGER_H_

#include "types.h"

typedef enum PowerState {
    PowerState_S0,
    PowerState_S0Away,
    PowerState_S1,
    PowerState_S2,
    PowerState_S3,
    PowerState_S4,
    PowerState_S5
} PowerState;

typedef enum PerformanceState {
    PerformanceState_P0,
    PerformanceState_P1,
    PerformanceState_P2,
    PerformanceState_P3
} PerformanceState;

typedef void (*PWM_StateChangeHandler)(PowerState pwr, PerformanceState perf);

typedef enum PWMError {
    PWMError_Error = 0,
    PWMError_Success = 1
} PWMError;

void
PWM_Initialize(void);

PWMError
PWM_RegisterStateChangeHandler(PWM_StateChangeHandler handler);

void
PWM_UnregisterHandler(PWM_StateChangeHandler handler);

#endif