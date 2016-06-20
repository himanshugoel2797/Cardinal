#ifndef _CARDINAL_THREAD_H_
#define _CARDINAL_THREAD_H_

#include "types.h"
#include "managers/process_manager/process_info.h"
#include "synchronization.h"

//TODO: Create threads, call HAL to handle context switching details
//TODO: Write a separate schedule manager that dictates thread switching in a process aware manner
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

typedef void (*ThreadEntryPoint)(void);

typedef struct ThreadInfo {
    UID Parent;
    UID ID;
    ProcessInformation *ParentProcess;
    ThreadEntryPoint entry_point;
    ThreadState state;
    ThreadPriority priority;
    void *interrupt_stack_base;
    void *stack_base;
    void *user_stack_base;
    void *stack;
    void *user_stack;
    void *tls_base;
    int core_affinity;
    uint64_t sleep_duration_ms;
    bool cur_executing;
    void *fpu_state;
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

#endif