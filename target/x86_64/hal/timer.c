#include "timer.h"
#include "hpet/hpet.h"
#include "apic/apic.h"

static uint8_t useTSC = 0;

void
InitializeTimer(void) {
    if(APIC_IsTSCReliable()){
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