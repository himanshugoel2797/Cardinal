#include "thread.h"
#include "common/common.h"
#include "kmalloc.h"
#include "common/list.h"
#include "target/hal/thread.h"
#include "target/hal/interrupts.h"

typedef struct CoreThreadState {
    ThreadInfo *cur_thread;
    uint32_t    coreID;
} CoreThreadState;

static Spinlock vLow_s, low_s, medium_s, neutral_s, high_s, vHigh_s, max_s, thds_s, core_s;
static List *vLow, *low, *medium, *neutral, *high, *vHigh, *max, *thds;
static List* cores;
static volatile CoreThreadState *coreState;
static uint64_t preempt_frequency;
static uint32_t preempt_vector;

#define PROPERTY_GET(type, name, default_val) type get_thread_##name (ThreadInfo *t) \
                             { \
                                type tmp = default_val ; \
                                LockSpinlock(t->lock);  \
                                tmp = t-> name ; \
                                UnlockSpinlock(t->lock);    \
                                return tmp; \
                             } 

#define PROPERTY_SET(type, name) void set_thread_##name (ThreadInfo *t, type v) \
                            { \
                                LockSpinlock(t->lock); \
                                t-> name = v; \
                                UnlockSpinlock(t->lock); \
                            }

#define PROPERTY_GET_SET(type, name, default_val) PROPERTY_GET(type, name, default_val) \
                            PROPERTY_SET(type, name)


#define SET_PROPERTY_VAL(t, name, val) set_thread_##name (t, val)
#define GET_PROPERTY_VAL(t, name) get_thread_##name (t)

PROPERTY_GET_SET(UID, Parent, 0)
PROPERTY_GET_SET(UID, ID, 0)
PROPERTY_GET_SET(ProcessInformation*, ParentProcess, NULL)
PROPERTY_GET_SET(ThreadEntryPoint, entry_point, NULL)
PROPERTY_GET_SET(ThreadState, state, 0)
PROPERTY_GET_SET(ThreadPriority, priority, 0)
PROPERTY_GET_SET(void*, stack_base, NULL)
PROPERTY_GET_SET(void*, user_stack_base, NULL)
PROPERTY_GET_SET(void*, stack, NULL)
PROPERTY_GET_SET(void*, user_stack, NULL)
PROPERTY_GET_SET(void*, tls_base, NULL)
PROPERTY_GET_SET(int, core_affinity, 0)
PROPERTY_GET_SET(uint64_t, sleep_duration_ms, 0)
PROPERTY_GET_SET(bool, cur_executing, FALSE)
PROPERTY_GET_SET(void*, fpu_state, NULL)


UID
GetCurrentThreadUID(void) {
    return coreState->cur_thread->ID;
}

UID
GetCurrentProcessUID(void) {
    return coreState->cur_thread->ParentProcess->ID;
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
    thd->lock = CreateSpinlock();
    
    SET_PROPERTY_VAL(thd, entry_point, entry_point);
    SET_PROPERTY_VAL(thd, state, ThreadState_Initialize);
    SET_PROPERTY_VAL(thd, priority, ThreadPriority_Neutral);
    SET_PROPERTY_VAL(thd, Parent, parentProcess);
    SET_PROPERTY_VAL(thd, sleep_duration_ms, 0);
    SET_PROPERTY_VAL(thd, fpu_state, kmalloc(GetFPUStateSize() + 16));

    uint64_t fpu_state_tmp = (uint64_t)thd->fpu_state;
    if(fpu_state_tmp % 16 != 0)
        fpu_state_tmp += 16 - fpu_state_tmp % 16;

    SET_PROPERTY_VAL(thd, fpu_state, (void*)fpu_state_tmp);
    SaveFPUState(GET_PROPERTY_VAL(thd, fpu_state));

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
    SET_PROPERTY_VAL(thd, ID, new_uid());
    SET_PROPERTY_VAL(thd, stack_base, kmalloc(KiB(16)));
    SET_PROPERTY_VAL(thd, stack, (void*)((uint64_t)thd->stack_base + KiB(16) - 8));

    FindFreeVirtualAddress(
        thd->ParentProcess->PageTable,
        (uint64_t*)&thd->user_stack_base,
        PAGE_SIZE * 4,
        MemoryAllocationType_Stack,
        MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

    if((uint64_t)thd->user_stack_base == 0)while(1);

    MemoryAllocationsMap *alloc_stack = kmalloc(sizeof(MemoryAllocationsMap));
    if((uint64_t)alloc_stack == 0)while(1);
    MapPage(thd->ParentProcess->PageTable,
            alloc_stack,
            AllocatePhysicalPageCont(4),
            (uint64_t)thd->user_stack_base,
            PAGE_SIZE * 4,
            CachingModeWriteBack,
            MemoryAllocationType_Stack,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_User
           );

    SET_PROPERTY_VAL(thd, user_stack, (void*)((uint64_t)thd->user_stack_base + PAGE_SIZE * 4 - 128));
    alloc_stack->next = thd->ParentProcess->AllocationMap->next;
    thd->ParentProcess->AllocationMap->next = alloc_stack;

    LockSpinlock(neutral_s);
    List_AddEntry(neutral, thd);
    UnlockSpinlock(neutral_s);

    LockSpinlock(thds_s);
    List_AddEntry(thds, thd);
    UnlockSpinlock(thds_s);

    return GET_PROPERTY_VAL(thd, ID);

error_exit:
    FreeSpinlock(thd->lock);
    kfree(thd);
    return -1;
}

void
SetThreadState(UID id,
               ThreadState state) {
    if(id == coreState->cur_thread->ID) {
        coreState->cur_thread->state = state;
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            SET_PROPERTY_VAL(thd, state, state);
            return;
        }
    }
}

void
SleepThread(UID id,
            uint64_t duration_ms) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, state, ThreadState_Sleep);
        SET_PROPERTY_VAL(coreState->cur_thread, sleep_duration_ms, duration_ms);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, state, ThreadState_Sleep);
            SET_PROPERTY_VAL(thd, sleep_duration_ms, duration_ms);
            return;
        }
    }
}

ThreadState
GetThreadState(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, state);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, state);
        }
    }
    return -1;
}

void*
GetThreadUserStack(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, user_stack);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, user_stack);
        }
    }
    return NULL;
}

void*
GetThreadKernelStack(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, stack);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, stack);
        }
    }
    return NULL;
}

void
SetThreadBasePriority(UID id,
                      ThreadPriority priority) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, priority, priority);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, priority, priority);
            return;
        }
    }
}

ThreadPriority
GetThreadPriority(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, priority);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, priority);
        }
    }
    return -1;
}


void
SetThreadCoreAffinity(UID id,
                      int coreID) {
    if(id == coreState->cur_thread->ID) {
        coreState->cur_thread->core_affinity = coreID;
        return;
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {

            LockSpinlock(thd->lock);
            thd->core_affinity = coreID;
            UnlockSpinlock(thd->lock);
            return;
        }
    }
}

int
GetThreadCoreAffinity(UID id) {
    if(id == coreState->cur_thread->ID) {
        return coreState->cur_thread->core_affinity;
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            return thd->core_affinity;
        }
    }
    return -1;
}

void
FreeThread(UID id) {
    SetThreadState(id, ThreadState_Exiting);
}

void
YieldThread(void) {
    RaiseInterrupt(preempt_vector);
}

ThreadInfo*
GetNextThread(void) {

    while(List_Length(thds) == 0);

    LockSpinlock(thds_s);
    ThreadInfo *next_thread = NULL;

    bool exit_loop = FALSE;
    while(!exit_loop) {
        do {
            next_thread = List_EntryAt(thds, 0);
            List_Remove(thds, 0);
            List_AddEntry(thds, next_thread);
        } while(GET_PROPERTY_VAL(next_thread, cur_executing));

        switch(GET_PROPERTY_VAL(next_thread, state)) {
        case ThreadState_Exiting:
            List_Remove(thds, List_Length(thds) - 1);
            kfree(next_thread->stack_base);
            FreeSpinlock(next_thread->lock);
            kfree(next_thread);
            break;
        case ThreadState_Paused:
            break;
        case ThreadState_Sleep:
            LockSpinlock(next_thread->lock);
            next_thread->sleep_duration_ms -= (next_thread->sleep_duration_ms > preempt_frequency/1000)?preempt_frequency/1000 : next_thread->sleep_duration_ms;
            if(next_thread->sleep_duration_ms == 0) {
                next_thread->state = ThreadState_Running;
                exit_loop = TRUE;
                if(!next_thread->cur_executing) {
                    exit_loop = TRUE;
                    next_thread->cur_executing = TRUE;
                }
            }
            UnlockSpinlock(next_thread->lock);
            break;
        default:
            SET_PROPERTY_VAL(next_thread, cur_executing, TRUE);
            exit_loop = TRUE;
            break;
        }

    }

    UnlockSpinlock(thds_s);

    return next_thread;
}

static void
TaskSwitch(uint32_t int_no,
           uint32_t err_code) {
    err_code = 0;
    ThreadInfo *tmp_cur_thread = coreState->cur_thread;
    if(GET_PROPERTY_VAL(coreState->cur_thread, state) == ThreadState_Exiting)
        tmp_cur_thread = NULL;
    else SaveFPUState(coreState->cur_thread->fpu_state);
    
    SavePreviousThread(coreState->cur_thread);
    SET_PROPERTY_VAL(coreState->cur_thread, cur_executing, FALSE);
    coreState->cur_thread = GetNextThread();

    RestoreFPUState(coreState->cur_thread->fpu_state);
    SetActiveVirtualMemoryInstance(coreState->cur_thread->ParentProcess->PageTable);
    HandleInterruptNoReturn(int_no);
    if(GET_PROPERTY_VAL(coreState->cur_thread, state) == ThreadState_Running) {
        SwitchToThread(coreState->cur_thread);
    } else if(GET_PROPERTY_VAL(coreState->cur_thread, state) == ThreadState_Initialize) {
        SET_PROPERTY_VAL(coreState->cur_thread, state, ThreadState_Running);
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
    SetActiveVirtualMemoryInstance(coreState->cur_thread->ParentProcess->PageTable);
    //Resume execution of the thread
    if(coreState->cur_thread->state == ThreadState_Initialize) {
        coreState->cur_thread->state = ThreadState_Running;
        SwitchAndInitializeThread(coreState->cur_thread);
    } else if(coreState->cur_thread->state == ThreadState_Running) {
    } else if(coreState->cur_thread->state == ThreadState_Paused)return;
}

void
CoreUpdate() {
    //Obtain thread to process from the lists
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
    coreState->coreID = id;

    UnlockSpinlock(core_s);
}

int
GetCoreLoad(int coreNum) {
    if(coreNum > (int)List_Length(cores))return -1;
    return ((CoreInfo*)List_EntryAt(cores, coreNum))->getCoreData();
}