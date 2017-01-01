/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _PRIV_APIC_DRIVER_H_
#define _PRIV_APIC_DRIVER_H_

#include "types.h"

/**
 * \addtogroup apic_driver APIC driver
 * @{
*/

#define IA32_APIC_BASE 0x01B

#define APIC_ID 0x020
#define APIC_EOI 0x0B0
#define APIC_DFR 0x0E0
#define APIC_SVR 0x0F0
#define APIC_ISR_BASE 0x100
#define APIC_TPR 0x080
#define APIC_INITIAL_COUNT 0x380
#define APIC_ICR_LO 0x300
#define APIC_ICR_HI 0x310

typedef struct APIC_SMPThreads {
    uint32_t apic_id;
    uint32_t proc_id;
    struct APIC_SMPThreads *next;
} APIC_SMPThreads;

void
APIC_FillHWInterruptHandler(char *idt_handler,
                            uint8_t intNum,
                            uint8_t irqNum);

void
APIC_DefaultHandler(void);

void
APIC_Write(uint32_t reg,
           uint32_t val);

uint32_t
APIC_Read(uint32_t reg);

/**@}*/

#endif /* end of include guard: _PRIV_APIC_DRIVER_H_ */
