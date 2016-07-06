#ifndef _CARDINAL_THREAD_H_
#define _CARDINAL_THREAD_H_

#include "types.h"
#include "managers/process_manager/process_info.h"
#include "synchronization.h"

//TODO: For the schedule manager, make sure to delete processes if their state suggests it

typedef enum {
    ThreadPriority_VeryLow = 0,
    ThreadPriority_Low = 1,
    ThreadPriority_Medium = 2,
    ThreadPriority_Neutral = 3,
    ThreadPriority_High = 4,
    ThreadPriority_VeryHigh = 5,
    ThreadPriority_Maximum = 6
} ThreadPriority;

typedef enum {
    ThreadState_Initialize,
    ThreadState_Running,
    ThreadState_Paused,
    ThreadState_Sleep,
    ThreadState_Exiting
} ThreadState;

typedef enum {
    ThreadWakeCondition_None,
    ThreadWakeCondition_SleepEnd,
    ThreadWakeCondition_Signal
} ThreadWakeCondition;

typedef void (*ThreadEntryPoint)(void);

typedef struct ThreadInfo {
    UID Parent;
    UID ID;
    ProcessInformation *ParentProcess;
    ThreadEntryPoint entry_point;
    ThreadState state;
    ThreadWakeCondition wakeCondition;
    ThreadPriority priority;
    uint64_t signal_mask0;
    uint64_t signal_mask1;
    uint64_t interrupt_stack_base;
    uint64_t kernel_stack_base;
    uint64_t user_stack_base;
    uint64_t current_stack;
    uint64_t kernel_stack_aligned;
    uint64_t interrupt_stack_aligned;
    int core_affinity;
    uint64_t sleep_duration_ns;
    uint64_t sleep_start_time;
    bool cur_executing;
    void *fpu_state;
    void *arch_specific_data;
    Spinlock lock;
} ThreadInfo;

typedef struct CoreInfo {
    UID ID;
    int (*getCoreData)(void);
} CoreInfo;

void
Thread_Initialize(void);

UID
CreateThread(UID parentProcess,
             ThreadEntryPoint entry_point);

void
SetThreadState(UID id,
               ThreadState state);

void
SleepThread(UID id,
            uint64_t duration_ms);

ThreadState
GetThreadState(UID id);

void*
GetThreadUserStack(UID id);

void*
GetThreadKernelStack(UID id);

void*
GetThreadCurrentStack(UID id);

void
SetThreadBasePriority(UID id,
                      ThreadPriority priority);

ThreadPriority
GetThreadPriority(UID id);

void
SetThreadCoreAffinity(UID id,
                      int coreID);

int
GetThreadCoreAffinity(UID id);

void
FreeThread(UID id);

void
YieldThread(void);

void
SetPeriodicPreemptVector(uint32_t irq,
                         uint64_t period);

void
SwitchThread(void);

void
RegisterCore(int id,
             int (*getCoreData)(void));

int
GetCoreLoad(int coreNum);

void
CoreUpdate();

UID
GetCurrentThreadUID(void);

UID
GetCurrentProcessUID(void);

ThreadInfo*
GetCurrentThreadInfo(void);

void
TaskSwitch(uint32_t int_no,
           uint32_t err_code);

#endif