/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "timer.h"
#include "hpet/hpet.h"
#include "apic/apic.h"

static uint8_t useTSC = 0;

void
InitializeTimer(void) {
    if(APIC_IsTSCReliable()) {
        useTSC = 1;
        return;
    }
    if(HPET_Initialize() == HPETError_NotPresent) {
        //No option but to rely on TSC anyway?
        //TODO reevaluate this situation

        useTSC = 1;
        return;
    }
    useTSC = 0;
}

uint64_t
GetTimerValue(void) {
    if(useTSC)return APIC_GetTSCValue();
    return HPET_GetCounterValue();
}

void
SetTimerEnableMode(bool enabled) {
    if(!useTSC)HPET_SetCounterEnableStatus(enabled);
}

uint64_t
GetTimerInterval_NS(uint64_t diff) {
    if(useTSC)return (diff * 100000000ULL) / APIC_GetTSCFrequency();
    return HPET_GetElapsedTime(diff);
}