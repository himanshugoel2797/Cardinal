#ifndef _CARDINAL_THREAD_H_
#define _CARDINAL_THREAD_H_

#include "types.h"
#include "managers/process_manager/process_info.h"
#include "libs/libc/include/signal.h"
#include "libs/libCardinal/include/registers.h"
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

typedef enum {
    ThreadPermissionLevel_User,
    ThreadPermissionLevel_Kernel
} ThreadPermissionLevel;

typedef void (*ThreadEntryPoint)(void*);

typedef struct ThreadInfo {

    UID                 ID;

    ProcessInformation  *ParentProcess;

    ThreadState         State;
    ThreadWakeCondition WakeCondition;
    ThreadPriority      Priority;

    uint64_t            InterruptStackBase;
    uint64_t            InterruptStackAligned;
    uint64_t            KernelStackBase;
    uint64_t            KernelStackAligned;
    uint64_t            CurrentStack;
    uint64_t            SleepDurationNS;
    uint64_t            SleepStartTime;
    uint64_t            Errno;

    int32_t             CoreAffinity;

    void                *SetChildTID;
    void                *ClearChildTID;
    void                *SetParentTID;
    void                *FPUState;
    void                *ArchSpecificData;

    uint32_t            reference_count;
    Spinlock            lock;
} ThreadInfo;

typedef struct CoreInfo {
    UID ID;
    int (*getCoreData)(void);
} CoreInfo;

void
Thread_Initialize(void);

void
SetChildTIDAddress(UID id,
                   void *address);

void
SetClearChildTIDAddress(UID id,
                        void *address);

void*
GetClearChildTIDAddress(UID id);

void*
GetChildTIDAddress(UID id);

void*
GetParentTIDAddress(UID id);

uint64_t
AllocateStack(UID parentProcess,
              ThreadPermissionLevel perm_level);

UID
CreateThread(UID parentProcess,
             ThreadPermissionLevel perm_level,
             ThreadEntryPoint entry_point,
             void *arg);

UID
CreateThreadADV(UID parentProcess,
                CRegisters *regs);

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

UID
GetCurrentProcessParentUID(void);

ThreadInfo*
GetCurrentThreadInfo(void);

void
TaskSwitch(uint32_t int_no,
           uint32_t err_code);

void
SetThreadIsPaused(UID tid, 
                  bool paused);

void
SetThreadErrno(UID tid,
               uint64_t errno);

uint64_t
GetThreadErrno(UID tid);

#endif