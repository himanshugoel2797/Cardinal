#include "thread.h"
#include "common/common.h"
#include "kmalloc.h"
#include "synchronization.h"
#include "common/list.h"
#include "target/hal/thread.h"
#include "target/hal/interrupts.h"

typedef struct CoreThreadState {
    ThreadInfo *cur_thread;
} CoreThreadState;

static Spinlock vLow_s, low_s, medium_s, neutral_s, high_s, vHigh_s, max_s, thds_s, core_s;
static List *vLow, *low, *medium, *neutral, *high, *vHigh, *max, *thds;
static List* cores;
static CoreThreadState *coreState;
static uint64_t preempt_frequency;
static uint32_t preempt_vector;

UID
GetCurrentThreadUID(void) {
    return coreState->cur_thread->ID;
}

void
Thread_Initialize(void) {
    vLow = List_Create();
    low = List_Create();
    medium = List_Create();
    neutral = List_Create();
    high = List_Create();
    vHigh = List_Create();
    max = List_Create();
    thds = List_Create();

    cores = List_Create();

    vLow_s = CreateSpinlock();
    low_s = CreateSpinlock();
    medium_s = CreateSpinlock();
    neutral_s = CreateSpinlock();
    high_s = CreateSpinlock();
    vHigh_s = CreateSpinlock();
    max_s = CreateSpinlock();
    thds_s = CreateSpinlock();
    core_s = CreateSpinlock();
}

UID
CreateThread(UID parentProcess,
             ThreadEntryPoint entry_point) {
    ThreadInfo *thd = kmalloc(sizeof(ThreadInfo));
    thd->entry_point = entry_point;
    thd->state = ThreadState_Initialize;
    thd->priority = ThreadPriority_Neutral;
    thd->Parent = parentProcess;
    thd->sleep_duration_ms = 0;
    thd->fpu_state = kmalloc(GetFPUStateSize() + 16);

    uint64_t fpu_state_tmp = (uint64_t)thd->fpu_state;
    if(fpu_state_tmp % 16 != 0)
        fpu_state_tmp += 16 - fpu_state_tmp % 16;

    thd->fpu_state = (void*)fpu_state_tmp;
    SaveFPUState(thd->fpu_state);

    if(GetProcessReference(parentProcess, &thd->ParentProcess) == ProcessErrors_UIDNotFound)
        goto error_exit;

    if(thd->ParentProcess->TLSSize != 0) {
        uint64_t tls_base = 0;
        FindFreeVirtualAddress(thd->ParentProcess->PageTable,
                               &tls_base,
                               thd->ParentProcess->TLSSize,
                               MemoryAllocationType_Application,
                               MemoryAllocationFlags_Write | MemoryAllocationFlags_User
                              );

        if(tls_base != 0) {
            MemoryAllocationsMap *alloc = kmalloc(sizeof(MemoryAllocationsMap));
            if(alloc == NULL)
                goto error_exit;

            MapPage(thd->ParentProcess->PageTable,
                    alloc,
                    AllocatePhysicalPageCont(thd->ParentProcess->TLSSize/PAGE_SIZE),
                    tls_base,
                    thd->ParentProcess->TLSSize,
                    CachingModeWriteBack,
                    MemoryAllocationType_Application,
                    MemoryAllocationFlags_Write | MemoryAllocationFlags_User
                   );

            alloc->next = thd->ParentProcess->AllocationMap->next;
            thd->ParentProcess->AllocationMap->next = alloc;

            thd->tls_base = (void*)tls_base;
        } else goto error_exit;
    }
    thd->ID = new_uid();
    thd->stack = kmalloc(KiB(16));

    LockSpinlock(neutral_s);
    List_AddEntry(neutral, thd);
    UnlockSpinlock(neutral_s);

    LockSpinlock(thds_s);
    List_AddEntry(thds, thd);
    UnlockSpinlock(thds_s);

    return thd->ID;

error_exit:
    kfree(thd);
    return -1;
}

void
SetThreadState(UID id,
               ThreadState state) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->state = state;
            return;
        }
    }
}

void
SleepThread(UID id,
            uint64_t duration_ms) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->state = ThreadState_Sleep;
            thd->sleep_duration_ms = duration_ms;
            return;
        }
    }
}

ThreadState
GetThreadState(UID id) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            return thd->state;
        }
    }
    return -1;
}

void
SetThreadBasePriority(UID id,
                      ThreadPriority priority) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->priority = priority;
            return;
        }
    }
}

ThreadPriority
GetThreadPriority(UID id) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            return thd->priority;
        }
    }
    return -1;
}


void
SetThreadCoreAffinity(UID id,
                      int coreID) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->core_affinity = coreID;
            return;
        }
    }
}

int
GetThreadCoreAffinity(UID id) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            return thd->core_affinity;
        }
    }
    return -1;
}

int q = 0;

void
FreeThread(UID id) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->state = ThreadState_Exiting;
            break;
        }
    }

    if(id == coreState->cur_thread->ID) {
        q = 1;
        while(1);
    }
}

void
YieldThread(void) {
    RaiseInterrupt(preempt_vector);
}

ThreadInfo*
GetNextThread(void) {
    LockSpinlock(thds_s);

    ThreadInfo *next_thread = List_EntryAt(thds, 0);
    //if(next_thread->state == ThreadState_Exiting)__asm__ ("hlt" :: "a"(*((uint64_t*)thds + 3)));
    List_Remove(thds, 0);
    List_AddEntry(thds, next_thread);

    //Cleanup dead threads
    while(next_thread->state == ThreadState_Exiting) {
        List_Remove(thds, List_Length(thds) - 1);
        kfree(next_thread->stack);
        kfree(next_thread);

        next_thread = List_EntryAt(thds, 0);
        List_Remove(thds, 0);
        List_AddEntry(thds, next_thread);
    }

    //Skip paused threads
    while(next_thread->state == ThreadState_Paused) {
        next_thread = List_EntryAt(thds, 0);
        List_Remove(thds, 0);
        List_AddEntry(thds, next_thread);
    }

    while(next_thread->state == ThreadState_Sleep) {
        next_thread->sleep_duration_ms -= (next_thread->sleep_duration_ms > preempt_frequency/1000)?preempt_frequency/1000 : next_thread->sleep_duration_ms;
        if(next_thread->sleep_duration_ms == 0) {
            next_thread->state = ThreadState_Running;
            break;
        } else {
            next_thread = List_EntryAt(thds, 0);
            List_Remove(thds, 0);
            List_AddEntry(thds, next_thread);
        }
    }

    while(next_thread->cur_executing) {
        next_thread = List_EntryAt(thds, 0);
        List_Remove(thds, 0);
        List_AddEntry(thds, next_thread);
    }

    UnlockSpinlock(thds_s);

    return next_thread;
}

static void
TaskSwitch(uint32_t int_no,
           uint32_t err_code) {
    err_code = 0;


    ThreadInfo *tmp_cur_thread = coreState->cur_thread;
    coreState->cur_thread->cur_executing = FALSE;
    SaveFPUState(coreState->cur_thread->fpu_state);
    coreState->cur_thread = GetNextThread();
    RestoreFPUState(coreState->cur_thread->fpu_state);
    coreState->cur_thread->cur_executing = TRUE;
    SetActiveVirtualMemoryInstance(coreState->cur_thread->ParentProcess->PageTable);
    if(coreState->cur_thread->state == ThreadState_Running) {
        SwapThreadOnInterrupt(tmp_cur_thread, coreState->cur_thread);
    } else if(coreState->cur_thread->state == ThreadState_Initialize) {
        coreState->cur_thread->state = ThreadState_Running;
        HandleInterruptNoReturn(int_no);
        SwitchAndInitializeThread(coreState->cur_thread);
    }
}

void
SetPeriodicPreemptVector(uint32_t irq,
                         uint64_t frequency) {
    RegisterInterruptHandler(irq, TaskSwitch);
    preempt_vector = irq;
    preempt_frequency = frequency;
}

void
SwitchThread(void) {

    coreState->cur_thread = GetNextThread();
    RestoreFPUState(coreState->cur_thread->fpu_state);
    coreState->cur_thread->cur_executing = TRUE;
    SetActiveVirtualMemoryInstance(coreState->cur_thread->ParentProcess->PageTable);
    //Resume execution of the thread
    if(coreState->cur_thread->state == ThreadState_Initialize) {
        coreState->cur_thread->state = ThreadState_Running;
        SwitchAndInitializeThread(coreState->cur_thread);
    } else if(coreState->cur_thread->state == ThreadState_Running) {
    } else if(coreState->cur_thread->state == ThreadState_Paused)return;
}

void
CoreUpdate(int coreID) {
    //Obtain thread to process from the lists
    //TODO make kmalloc work on all threads by having it share the mappings on to all cores
    coreID = 0;
    while(TRUE) {
        SwitchThread();
    }
}

void
RegisterCore(int id,
             int (*getCoreData)(void)) {

    LockSpinlock(core_s);
    CoreInfo *cInfo = kmalloc(sizeof(CoreInfo));
    cInfo->ID = id;
    cInfo->getCoreData = getCoreData;

    List_AddEntry(cores, cInfo);

    coreState = (CoreThreadState*)AllocateAPLSMemory(sizeof(CoreThreadState));
    coreState->cur_thread = NULL;

    UnlockSpinlock(core_s);
}

int
GetCoreLoad(int coreNum) {
    if(coreNum > (int)List_Length(cores))return -1;
    return ((CoreInfo*)List_EntryAt(cores, coreNum))->getCoreData();
}