/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _PRIV_X86_64_HPET_H_
#define _PRIV_X86_64_HPET_H_

#include "types.h"

typedef struct {
    uint64_t Rsv0 : 1;
    uint64_t InterruptType : 1;
    uint64_t InterruptEnable : 1;
    uint64_t TimerType : 1;
    uint64_t PeriodicCapable : 1;
    uint64_t Is64Bit : 1;
    uint64_t ValueSet : 1;
    uint64_t Rsv1 : 1;
    uint64_t Enable32BitMode : 1;
    uint64_t InterruptRoute : 5;
    uint64_t FSBInterruptEnable : 1;
    uint64_t FSBInterruptDelivery : 1;
    uint64_t Rsv2 : 16;
    uint64_t RoutingCapability : 32;
} __attribute__((packed)) HPET_TimerConfigRegister;

typedef struct {
    HPET_TimerConfigRegister Configuration;
    uint64_t ComparatorValue;
    uint64_t FSBInterruptRoute;
    uint64_t Rsv0;
} __attribute__((packed)) HPET_Timer;

typedef struct {
    uint8_t RevID;
    uint8_t TimerCount : 5;
    uint8_t Is64Bit : 1;
    uint8_t Rsv0 : 1;
    uint8_t LegacyReplacementCapable : 1;
    uint16_t VendorID;
    uint32_t ClockPeriod;
} __attribute__((packed)) HPET_CapRegister;

typedef struct {
    uint64_t GlobalEnable : 1;
    uint64_t LegacyReplacementEnable : 1;
    uint64_t Rsv0 : 62;
} __attribute__((packed)) HPET_ConfigRegister;

typedef struct {
    uint64_t T0_InterruptStatus : 1;
    uint64_t T1_InterruptStatus : 1;
    uint64_t T2_InterruptStatus : 1;
    uint64_t Rsv0 : 61;
} __attribute__((packed)) HPET_InterruptStatus;

typedef struct {
    HPET_CapRegister Capabilities;
    uint64_t Rsv0;
    HPET_ConfigRegister Configuration;
    uint64_t Rsv1;
    HPET_InterruptStatus InterruptStatus;
    uint64_t Rsv2;
    volatile uint64_t CounterValue;
    uint64_t Rsv3;
    HPET_Timer timer0;
    HPET_Timer timer1;
    HPET_Timer timer2;
} __attribute__((packed)) HPET_Main;

#endif