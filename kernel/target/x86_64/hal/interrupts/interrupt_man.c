/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "types.h"
#include "interrupts.h"
#include "common.h"
#include "IDT/idt.h"
#include "apic/io_apic/io_apic.h"
#include "apic/apic.h"
#include "memory.h"

#include "debug_gfx.h"

static volatile InterruptHandler intHandlers[256] = {0};
static Registers volatile *regs_saved = NULL;

uint32_t
RequestInterruptVectorBlock(uint32_t vectorCount) {
    uint32_t score = 0;
    int i = 0;
    for(; i < 256; i++) {
        if(i > 0 && intHandlers[i - 1] != NULL)score = 0;
        if(intHandlers[i] == NULL)score++;
        if(score == vectorCount)break;
    }

    if(i + vectorCount >= 256)return -1;
    else return i;
}


void
ShadowInterruptHandler(Registers *regs) {

    if(regs_saved == NULL) {
        regs_saved = AllocateAPLSMemory(sizeof(Registers));
    }

    memcpy((void*)regs_saved, regs, sizeof(Registers));

    if(intHandlers[regs->int_no] != NULL) {
        if(regs->int_no > 32)PrintDebugMessage("Interrupt %x, Handler %x\r\n", regs->int_no, (uint32_t)intHandlers[regs->int_no]);
        intHandlers[regs->int_no](regs->int_no, regs->err_code);
    } else {
        PrintDebugMessage("Warning: No handler for interrupt %x\r\n", regs->int_no);
    }

    HandleInterruptNoReturn(regs->int_no);
}

void
HandleInterruptNoReturn(uint32_t vector) {
    memset((void*)regs_saved, 0, sizeof(Registers));
    if(vector > 31)APIC_SendEOI(vector);
}

uint32_t
RegisterInterruptHandler(uint32_t int_no,
                         InterruptHandler handler) {
    if(int_no >= 256)return -1;
    intHandlers[int_no] = handler;
    IDT_RegisterHandler(int_no, ShadowInterruptHandler);
    return 0;
}

uint32_t
GetInterruptHandler(uint32_t int_no,
                    InterruptHandler *handler) {
    if(int_no >= 256)return -1;
    if(handler != NULL)*handler = intHandlers[int_no];
    return 0;
}

void
RemoveInterruptHandler(uint32_t id) {
    if(id < 256 && id > 31) {
        intHandlers[id] = NULL;
    }
}

void
SetInterruptEnableMode(uint32_t vector, bool enableMode) {
    IOAPIC_SetEnableMode((uint8_t)vector, enableMode);
}

void
RaiseInterrupt(uint32_t int_no) {
    IDT_RaiseInterrupt(int_no);
}

void*
GetSavedInterruptState(void) {
    return (void*)regs_saved;
}