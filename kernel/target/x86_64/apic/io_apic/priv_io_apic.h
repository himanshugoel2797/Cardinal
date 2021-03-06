/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _PRIV_IO_APIC_DRIVER_H_
#define _PRIV_IO_APIC_DRIVER_H_

#include "types.h"

/**
 * \defgroup ioapic_driver IO APIC driver
 * @{
*/

//! Read an IOAPIC configuration register

//! \param io_apic_baseAddr the base address of the IO APIC MMIO space
//! \param index the register index to read
//! \return the value of the register
uint32_t
IOAPIC_Read(uint32_t volatile* io_apic_baseAddr,
            uint32_t index);

//! Write an IOAPIC configuration register

//! \param io_apic_baseAddr the base address of the IO APIC MMIO space
//! \param index the register index to read
//! \param val the register value
void
IOAPIC_Write(uint32_t volatile* io_apic_baseAddr,
             uint32_t index,
             uint32_t val);


typedef struct {
    uint32_t volatile * baseAddr;
    uint32_t global_int_base;
    uint32_t ID;
    uint32_t entry_count;
} IOAPIC_Desc;

typedef struct {
    uint32_t ioapic_index;
    uint32_t ioapic_pin;
} IOAPIC_InterruptMapEntry;

/**@}*/

#endif /* end of include guard: _PRIV_IO_APIC_DRIVER_H_ */
