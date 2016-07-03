#ifndef _CARDINAL_MANAGERS_TIMER_H_
#define _CARDINAL_MANAGERS_TIMER_H_

#include "types.h"

typedef enum {
	TimerContinue_Stop = 0,
	TimerContinue_Continue = 1
}TimerContinue;

typedef enum {
	TimerError_None = 0,
	TimerError_ListFull = 1
}TimerError;

typedef TimerContinue (*TimerEventHandler)(void *val);

void
Timer_Initialize(void);

TimerError
RegisterTimerHandler(TimerEventHandler handler, 
					 void *param, 
					 uint64_t period,
					 UID *entry_id);

TimerError
UnregisterTimerHandler(UID entry_id);

void
HandleTimer(uint32_t int_no, 
			uint32_t err_code);

uint64_t
GetTimerFrequency(void);

uint64_t
GetTimerTickCount(void);

#endif