/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _APIC_DRIVER_H_
#define _APIC_DRIVER_H_

#include "types.h"
#include "io_apic/io_apic.h"

/**
 * \defgroup apic_driver APIC driver
 * @{
*/

#define APIC_TIMER 0x320
#define APIC_THERMAL_SENSOR 0x330
#define APIC_PERF_MON 0x340
#define APIC_LINT0    0x350
#define APIC_LINT1    0x360
#define APIC_ERR      0x370
#define APIC_TIMER_VAL 0x380
#define APIC_TIMER_CURVAL 0x390

#define APIC_TIMER_ONESHOT 0x0
#define APIC_TIMER_PERIODIC 0x1
#define APIC_TIMER_TSC      0x2

#define APIC_DESTINATION_SHORT_NONE 0x0
#define APIC_DESTINATION_SHORT_SELF 0x1
#define APIC_DESTINATION_SHORT_ALL 0x2
#define APIC_DESTINATION_SHORT_ALLBUTSELF 0x3

#define APIC_DELIVERY_MODE_FIXED 0x0
#define APIC_DELIVERY_MODE_SMI  0x2
#define APIC_DELIVERY_MODE_NMI  0x4
#define APIC_DELIVERY_MODE_EXTINT 0x7
#define APIC_DELIVERY_MODE_INIT  0x5
#define APIC_DELIVERY_MODE_START 0x6

#define APIC_TRIGGER_MODE_EDGE 0
#define APIC_TRIGGER_MODE_LEVEL 1

#define APIC_POLARITY_HIGH 0
#define APIC_POLARITY_LOW  1

void
APIC_LockPIC(void);

uint8_t
APIC_LocalInitialize(void);

void
APIC_CallibrateTimer(void);

uint32_t
APIC_Initialize(void);

void
APIC_SetEnableMode(uint8_t enabled);

uint8_t
APIC_GetID(void);

void
APIC_SetTimerMode(uint8_t mode);

void
APIC_SetDeliveryMode(uint32_t lvt_entry,
                     uint8_t mode);

void
APIC_SetTriggerMode(uint32_t lvt_entry,
                    uint8_t mode);

void
APIC_SetPolarity(uint32_t lvt_entry,
                 uint8_t mode);

void
APIC_SetTimerValue(uint32_t val);

uint32_t
APIC_GetTimerValue(void);

void
APIC_SetTimerDivisor(uint8_t divisor);

uint64_t
APIC_GetTimerFrequency(void);

void
APIC_SetEnableInterrupt(uint32_t interrupt,
                        int enableMode);

void
APIC_SetVector(uint32_t interrupt,
               uint8_t vector);

void
APIC_SendEOI(uint8_t int_num);

void
APIC_SendIPI(uint32_t dest,
             uint8_t destination_shorthand,
             uint8_t vector,
             uint8_t delivery_mode);

void
APIC_EnableTSC(void);

bool
APIC_IsTSCReliable(void);

uint64_t
APIC_GetTSCFrequency(void);

uint64_t
APIC_GetTSCValue(void);

/**@}*/

#endif /* end of include guard: _APIC_DRIVER_H_ */
