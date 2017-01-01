/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _IO_APIC_DRIVER_H_
#define _IO_APIC_DRIVER_H_

#include "types.h"

/**
 * \addtogroup ioapic_driver IO APIC driver
 * @{
*/

#define MAX_IOAPIC_COUNT 128
#define IOAPIC_PIN_COUNT 24

uint8_t
IOAPIC_Initialize(uint64_t baseAddr,
                  uint32_t global_int_base);

void
IOAPIC_MapIRQ(uint8_t global_irq,
              uint8_t apic_vector,
              uint64_t apic_id,
              uint8_t trigger_mode,
              uint8_t polarity,
              uint8_t delivery_mode);

void
IOAPIC_SetEnableMode(uint8_t vector,
                     bool active);

uint8_t
IOAPIC_GetVector(uint8_t global_irq,
                 uint8_t *vec);

void
IOAPIC_VirtualizeAll(void);

/**@}*/

#endif /* end of include guard: _IO_APIC_DRIVER_H_ */
