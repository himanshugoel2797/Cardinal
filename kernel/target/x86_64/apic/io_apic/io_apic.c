/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "io_apic.h"
#include "priv_io_apic.h"
#include "managers.h"
#include "common.h"

static volatile IOAPIC_Desc ioapics[MAX_IOAPIC_COUNT];
static volatile uint32_t curIOAPIC_index = 0;

static volatile IOAPIC_InterruptMapEntry ioapic_interruptMap[256];

uint8_t
IOAPIC_Initialize(uint64_t baseAddr,
                  uint32_t global_int_base) {
    if(curIOAPIC_index >= MAX_IOAPIC_COUNT) return -1;
    ioapics[curIOAPIC_index].baseAddr = (uint32_t volatile *)baseAddr;
    ioapics[curIOAPIC_index].global_int_base = global_int_base;
    ioapics[curIOAPIC_index].ID = (IOAPIC_Read((uint32_t volatile *)baseAddr, 0x0) >> 24) & 0xF;
    ioapics[curIOAPIC_index].entry_count = (IOAPIC_Read((uint32_t volatile *)baseAddr, 0x01) >> 16) & 0xFF;


    curIOAPIC_index++;

    return 0;
}

uint32_t
IOAPIC_Read(uint32_t volatile* io_apic_baseAddr,
            uint32_t index) {
    io_apic_baseAddr[0] = index;
    return io_apic_baseAddr[4];
}

void
IOAPIC_Write(uint32_t volatile* io_apic_baseAddr,
             uint32_t index,
             uint32_t val) {
    io_apic_baseAddr[0] = index;
    io_apic_baseAddr[4] = val;
}

uint8_t
IOAPIC_GetVector(uint8_t global_irq,
                 uint8_t *vec) {
    uint32_t irq_pin = 0;
    uint32_t *baseAddr = NULL;
    //Determine which APIC this should map to
    for(uint32_t i = 0; i < curIOAPIC_index; i++) {
        if(global_irq >= ioapics[i].global_int_base && global_irq < (ioapics[i].global_int_base + ioapics[i].entry_count)) {
            //Found the IO APIC to map to
            irq_pin = global_irq - ioapics[i].global_int_base;
            baseAddr = (uint32_t*)ioapics[i].baseAddr;
        }
    }
    if(baseAddr == NULL) return -1; //No match found!

    const uint32_t low_index = 0x10 + irq_pin*2;

    uint32_t low = IOAPIC_Read(baseAddr, low_index);
    *vec = (low & 0xFF);
    return low & 0xff;
}

void
IOAPIC_MapIRQ(uint8_t global_irq,
              uint8_t apic_vector,
              uint64_t apic_id,
              uint8_t trigger_mode,
              uint8_t polarity,
              uint8_t delivery_mode) {
    uint32_t irq_pin = 0;
    uint32_t *baseAddr = NULL;
    //Determine which APIC this should map to
    for(uint32_t i = 0; i < curIOAPIC_index; i++) {
        if(global_irq >= ioapics[i].global_int_base && global_irq < (ioapics[i].global_int_base + ioapics[i].entry_count)) {
            //Found the IO APIC to map to
            irq_pin = global_irq - ioapics[i].global_int_base;
            baseAddr = (uint32_t*)ioapics[i].baseAddr;

            ioapic_interruptMap[apic_vector].ioapic_index = i;
            ioapic_interruptMap[apic_vector].ioapic_pin = irq_pin;
        }
    }
    if(baseAddr == NULL) return; //No match found!

    const uint32_t low_index = 0x10 + irq_pin*2;
    const uint32_t high_index = 0x10 + irq_pin*2 + 1;

    uint32_t high = IOAPIC_Read(baseAddr, high_index);
    // set APIC ID
    high &= ~0xff000000;
    high |= apic_id << 24;
    IOAPIC_Write(baseAddr, high_index, high);

    uint32_t low = IOAPIC_Read(baseAddr, low_index);

    // set the polarity
    low &= ~(1<<13);
    low |= ((polarity & 1) << 13);

    low &= ~(1<<15);
    low |= ((trigger_mode & 1) << 15);

    // set to physical delivery mode
    low &= ~(1<<11);

    // set to fixed delivery mode
    low &= ~0x700;

    // set delivery vector
    low &= ~0xff;
    low |= apic_vector;

    low |= (((uint32_t)delivery_mode & 7) << 8);

    IOAPIC_Write(baseAddr, low_index, low);
}

void
IOAPIC_SetEnableMode(uint8_t vector,
                     bool active) {
    //Make sure this interrupt has even been mapped to something!
    if(ioapic_interruptMap[vector].ioapic_index != 0xFFFFFFFF) {
        uint32_t index = 0x10 + ioapic_interruptMap[vector].ioapic_pin*2;
        uint32_t* baseAddr = (uint32_t*)ioapics[ioapic_interruptMap[vector].ioapic_index].baseAddr;

        uint32_t low = IOAPIC_Read(baseAddr, index);
        low = SET_VAL_BIT(low, 16, (~active & 1));
        IOAPIC_Write(baseAddr, index, low);
    }
}
