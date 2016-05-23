#ifndef _CARDINAL_POWER_MANAGER_H_
#define _CARDINAL_POWER_MANAGER_H_

typedef enum PowerState {
    PowerState_S0,
    PowerState_S0Away
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
}

void
PWM_Initialize(void);


#endif