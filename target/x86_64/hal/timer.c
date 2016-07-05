#include "timer.h"
#include "hpet/hpet.h"

void
InitializeTimer(void) 
{
    if(HPET_Initialize() == HPETError_NotPresent)
    {
        __asm__ volatile("cli\n\thlt");
    }
}

uint64_t
GetTimerValue(void)
{
    return HPET_GetCounterValue();
}

void
SetTimerValue(uint64_t val)
{
    HPET_SetCounterValue(val);
}

void
SetTimerEnableMode(bool enabled)
{
    HPET_SetCounterEnableStatus(enabled);
}

uint64_t
GetTimerInterval_NS(uint64_t diff)
{
    return HPET_GetElapsedTime(diff);
}