/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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