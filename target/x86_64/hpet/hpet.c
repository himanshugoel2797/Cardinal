/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "acpi/acpi_tables.h"
#include "acpi/hpet.h"
#include "priv_hpet.h"
#include "hpet.h"
#include "virt_mem_manager/virt_mem_manager.h"

static
HPET_Main* hpet_regs = NULL;

HPETError
HPET_Initialize(void) {
    HPET *hpet = (HPET*)ACPITables_FindTable(HPET_SIG, 0);
    if(hpet == NULL)return HPETError_NotPresent;

    hpet_regs = (HPET_Main*)VirtMemMan_GetVirtualAddress(CachingModeUncachable, (void*)hpet->Address.address);

    HPET_SetCounterEnableStatus(DISABLE);
    HPET_SetCounterValue(0);
    HPET_SetCounterEnableStatus(ENABLE);

    return HPETError_None;
}

void
HPET_SetCounterEnableStatus(bool enableStatus) {
    hpet_regs->Configuration.GlobalEnable = enableStatus;
}

uint64_t
HPET_GetElapsedTime(uint64_t diff) {
    return (diff * (uint64_t)hpet_regs->Capabilities.ClockPeriod)/1000000;
}

uint64_t
HPET_GetPeriod(void) {
    return hpet_regs->Capabilities.ClockPeriod/1000000;
}

uint64_t
HPET_GetCounterValue(void) {
    return hpet_regs->CounterValue;
}

void
HPET_SetCounterValue(uint64_t val) {
    hpet_regs->CounterValue = val;
}