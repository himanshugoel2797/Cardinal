#include "timer.h"
#include "common.h"
#include "list.h"

typedef struct TimerSubscriberEntry {
    TimerEventHandler handler;
    void *val;
    uint64_t period;
    uint64_t registration_time;
    UID id;
} TimerSubscriberEntry;

typedef struct CoreLocalTimerData {
    uint64_t timer_frequency;
    uint64_t timer_tick_count;
    List *timer_subs;
} CoreLocalTimerData;

static volatile CoreLocalTimerData *timerData = NULL;


void
Timer_Initialize(void) {
    timer_subs = List_Create(CreateSpinlock());
}

TimerError
RegisterTimerHandler(TimerEventHandler handler,
                     void *param,
                     uint64_t period) {

}

TimerError
UnregisterTimerHandler(UID entry_id) {

}

void
HandleTimer(uint32_t int_no,
            uint32_t err_code) {
    timerData->timer_tick_count++;
}

uint64_t
GetTimerFrequency(void) {
    return timerData->timer_frequency;
}

uint64_t
GetTimerTickCount(void) {
    return timerData->timer_tick_count;
}
