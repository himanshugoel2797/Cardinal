#include "timer.h"
#include "hpet/hpet.h"
#include "apic/apic.h"

void
InitializeTimer(void) {
	if(APIC_IsTSCReliable())return;
    if(HPET_Initialize() == HPETError_NotPresent) {
        __asm__ volatile("cli\n\thlt");
    }
}

uint64_t
GetTimerValue(void) {
	if(APIC_IsTSCReliable())return APIC_GetTSCValue();
    return HPET_GetCounterValue();
}

void
SetTimerEnableMode(bool enabled) {
    if(!APIC_IsTSCReliable())HPET_SetCounterEnableStatus(enabled);
}

uint64_t
GetTimerInterval_NS(uint64_t diff) {
	if(APIC_IsTSCReliable())return (diff * 100000000ULL) / APIC_GetTSCFrequency();
    return HPET_GetElapsedTime(diff);
}