#include "timer.h"

void
InitializeTimer(void)
{
	if(timerData == NULL)
		timerData = AllocateAPLSMemory(sizeof(CoreLocalTimerData));

	timerData->timer_frequency = APIC_GetTimerFrequency()/1000;
	timerData->timer_tick_count = 0;

	Timer_Initialize();

	RegisterInterruptHandler(IRQ(1), TimerInterrupt);
	APIC_SetVector(APIC_TIMER, IRQ(1));
	APIC_SetTimerValue(timerData->timer_frequency);
	APIC_SetTimerMode(APIC_TIMER_PERIODIC);
	APIC_SetEnableInterrupt(APIC_TIMER, ENABLE);
}