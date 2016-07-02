#include "thread.h"
#include "common/common.h"
#include "kmalloc.h"
#include "common/list.h"
#include "target/hal/thread.h"
#include "target/hal/interrupts.h"
#include "target/hal/syscall.h"

typedef struct CoreThreadState {
    ThreadInfo *cur_thread;
    uint32_t    coreID;
} CoreThreadState;

static Spinlock sync_lock;
static List *vLow, *low, *medium, *neutral, *high, *vHigh, *max, *thds;
static List* cores;
static volatile CoreThreadState *coreState = NULL;
static uint64_t preempt_frequency;
static uint32_t preempt_vector;

#define PROPERTY_GET(type, name, default_val) type get_thread_##name (ThreadInfo *t) \
                             { \
                                if(t == NULL)__asm__("add $0x20, %rsp\n\tpopq %rax\n\tpopq %rax\n\tcli\n\thlt"); \
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


#define PROPERTY_PROC_GET(type, name, default_val) type get_proc_##name (ProcessInformation *t) \
                             { \
                                if(t == NULL)__asm__("add $0x20, %rsp\n\tpopq %rax\n\tpopq %rax\n\tcli\n\thlt"); \
                                type tmp = default_val ; \
                                LockSpinlock(t->lock);  \
                                tmp = t-> name ; \
                                UnlockSpinlock(t->lock);    \
                                return tmp; \
                             }

#define PROPERTY_PROC_SET(type, name) void set_proc_##name (ProcessInformation *t, type v) \
                            { \
                                LockSpinlock(t->lock); \
                                t-> name = v; \
                                UnlockSpinlock(t->lock); \
                            }

#define PROPERTY_PROC_GET_SET(type, name, default_val) PROPERTY_PROC_GET(type, name, default_val) \
                            PROPERTY_PROC_SET(type, name)


#define SET_PROPERTY_PROC_VAL(t, name, val) set_proc_##name (GET_PROPERTY_VAL(t, ParentProcess), val)
#define GET_PROPERTY_PROC_VAL(t, name) get_proc_##name (GET_PROPERTY_VAL(t,ParentProcess))

PROPERTY_GET_SET(UID, Parent, 0)
PROPERTY_GET_SET(UID, ID, 0)
PROPERTY_GET_SET(ProcessInformation*, ParentProcess, NULL)
PROPERTY_PROC_GET(UID, ID, 0)
PROPERTY_PROC_GET(uint64_t, TLSSize, 0)
PROPERTY_PROC_GET(UID, PageTable, 0)
PROPERTY_PROC_GET(MemoryAllocationsMap*, AllocationMap, NULL)

PROPERTY_GET_SET(ThreadEntryPoint, entry_point, NULL)
PROPERTY_GET_SET(ThreadState, state, 0)
PROPERTY_GET_SET(ThreadPriority, priority, 0)
PROPERTY_GET_SET(uint64_t, interrupt_stack_base, 0)
PROPERTY_GET_SET(uint64_t, interrupt_stack_aligned, 0)
PROPERTY_GET_SET(uint64_t, user_stack_base, 0)
PROPERTY_GET_SET(uint64_t, kernel_stack_base, 0)
PROPERTY_GET_SET(uint64_t, kernel_stack_aligned, 0)
PROPERTY_GET_SET(uint64_t, current_stack, 0)
PROPERTY_GET_SET(void*, tls_base, NULL)
PROPERTY_GET_SET(int, core_affinity, 0)
PROPERTY_GET_SET(uint64_t, sleep_duration_ms, 0)
PROPERTY_GET_SET(bool, cur_executing, FALSE)
PROPERTY_GET_SET(void*, fpu_state, NULL)

UID
GetCurrentThreadUID(void) {
    return GET_PROPERTY_VAL(coreState->cur_thread, ID);
}

UID
GetCurrentProcessUID(void) {
    return GET_PROPERTY_VAL(coreState->cur_thread, ParentProcess)->ID;
}

void
Thread_Initialize(void) {
    vLow = List_Create(CreateSpinlock());
    low = List_Create(CreateSpinlock());
    medium = List_Create(CreateSpinlock());
    neutral = List_Create(CreateSpinlock());
    high = List_Create(CreateSpinlock());
    vHigh = List_Create(CreateSpinlock());
    max = List_Create(CreateSpinlock());
    thds = List_Create(CreateSpinlock());

    cores = List_Create(CreateSpinlock());
    sync_lock = CreateSpinlock();
}

#define STACK_SIZE KiB(8)

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
    SET_PROPERTY_VAL(thd, cur_executing, FALSE);
    SET_PROPERTY_VAL(thd, interrupt_stack_base, (uint64_t)kmalloc(STACK_SIZE) + STACK_SIZE);
    SET_PROPERTY_VAL(thd, kernel_stack_base, (uint64_t)kmalloc(STACK_SIZE) + STACK_SIZE);

    uint64_t kstack = GET_PROPERTY_VAL(thd, kernel_stack_base);
    kstack -= kstack % 16;
    SET_PROPERTY_VAL(thd, kernel_stack_aligned, kstack);

    uint64_t istack = GET_PROPERTY_VAL(thd, interrupt_stack_base);
    istack -= istack % 16;
    SET_PROPERTY_VAL(thd, interrupt_stack_aligned, istack);

    uint64_t fpu_state_tmp = (uint64_t)GET_PROPERTY_VAL(thd,fpu_state);
    if(fpu_state_tmp % 16 != 0)
        fpu_state_tmp += 16 - fpu_state_tmp % 16;

    SET_PROPERTY_VAL(thd, fpu_state, (void*)fpu_state_tmp);
    SaveFPUState(GET_PROPERTY_VAL(thd, fpu_state));

    ProcessInformation *pInfo = NULL;

    if(GetProcessReference(parentProcess, &pInfo) == ProcessErrors_UIDNotFound)
        goto error_exit;

    SET_PROPERTY_VAL(thd, ParentProcess, pInfo);

    if(GET_PROPERTY_PROC_VAL(thd, TLSSize) != 0) {
        uint64_t tls_base = 0;
        FindFreeVirtualAddress(GET_PROPERTY_PROC_VAL(thd, PageTable),
                               &tls_base,
                               GET_PROPERTY_PROC_VAL(thd, TLSSize),
                               MemoryAllocationType_Application,
                               MemoryAllocationFlags_Write | MemoryAllocationFlags_User
                              );

        if(tls_base != 0) {
            MemoryAllocationsMap *alloc = kmalloc(sizeof(MemoryAllocationsMap));
            if(alloc == NULL)
                goto error_exit;

            MapPage(GET_PROPERTY_PROC_VAL(thd, PageTable),
                    alloc,
                    AllocatePhysicalPageCont(GET_PROPERTY_PROC_VAL(thd, TLSSize)/PAGE_SIZE),
                    tls_base,
                    GET_PROPERTY_PROC_VAL(thd, TLSSize),
                    CachingModeWriteBack,
                    MemoryAllocationType_Application,
                    MemoryAllocationFlags_Write | MemoryAllocationFlags_User
                   );

            alloc->next = GET_PROPERTY_PROC_VAL(thd, AllocationMap)->next;
            GET_PROPERTY_PROC_VAL(thd, AllocationMap)->next = alloc;

            thd->tls_base = (void*)tls_base;
        } else goto error_exit;
    }
    SET_PROPERTY_VAL(thd, ID, new_uid());
    List_AddEntry(pInfo->ThreadIDs, (void*)GET_PROPERTY_VAL(thd, ID));

    uint64_t user_stack_base = 0;

    FindFreeVirtualAddress(
        GET_PROPERTY_PROC_VAL(thd, PageTable),
        (uint64_t*)&user_stack_base,
        STACK_SIZE,
        MemoryAllocationType_Stack,
        MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

    if(user_stack_base == 0)while(1);

    SET_PROPERTY_VAL(thd, user_stack_base, user_stack_base + STACK_SIZE - 128);

    MemoryAllocationsMap *alloc_stack = kmalloc(sizeof(MemoryAllocationsMap));
    if((uint64_t)alloc_stack == 0)while(1);
    MapPage(GET_PROPERTY_PROC_VAL(thd, PageTable),
            alloc_stack,
            AllocatePhysicalPageCont(STACK_SIZE/PAGE_SIZE),
            user_stack_base,
            STACK_SIZE,
            CachingModeWriteBack,
            MemoryAllocationType_Stack,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_User
           );

    alloc_stack->next = GET_PROPERTY_PROC_VAL(thd, AllocationMap)->next;
    GET_PROPERTY_PROC_VAL(thd, AllocationMap)->next = alloc_stack;

    List_AddEntry(neutral, thd);
    List_AddEntry(thds, thd);

    return GET_PROPERTY_VAL(thd, ID);

error_exit:
    FreeSpinlock(thd->lock);
    kfree(thd);
    return -1;
}

void
SetThreadState(UID id,
               ThreadState state) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, state, state);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
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
        return (void*)GET_PROPERTY_VAL(coreState->cur_thread, user_stack_base);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return (void*)GET_PROPERTY_VAL(thd, user_stack_base);
        }
    }
    return NULL;
}

void*
GetThreadKernelStack(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return (void*)GET_PROPERTY_VAL(coreState->cur_thread, kernel_stack_aligned);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return (void*)GET_PROPERTY_VAL(thd, kernel_stack_aligned);
        }
    }
    return NULL;
}

void*
GetThreadCurrentStack(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return (void*)GET_PROPERTY_VAL(coreState->cur_thread, current_stack);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return (void*)GET_PROPERTY_VAL(thd, current_stack);
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
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, core_affinity, coreID);
        return;
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, core_affinity, coreID);
            return;
        }
    }
}

int
GetThreadCoreAffinity(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, core_affinity);
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, core_affinity);
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
GetNextThread(ThreadInfo *prevThread) {

    /*
    - Get a thread from the queue
    - Process the thread
    - Put the thread back once done
    */

    if(prevThread != NULL) {
        SET_PROPERTY_VAL(prevThread, cur_executing, FALSE);
        List_AddEntry(thds, prevThread);
    }

    ThreadInfo *next_thread = NULL;

    bool exit_loop = FALSE;
    while(!exit_loop) {

        LockSpinlock(sync_lock);
        next_thread = List_EntryAt(thds, 0);
        if(next_thread != NULL)List_Remove(thds, 0);
        UnlockSpinlock(sync_lock);

        if(next_thread == NULL)continue;

        switch(GET_PROPERTY_VAL(next_thread, state)) {
        case ThreadState_Exiting:
            if(GetSpinlockContenderCount(next_thread->lock) == 0) {
                LockSpinlock(next_thread->lock);
                kfree((void*)(next_thread->kernel_stack_base - STACK_SIZE));
                kfree((void*)(next_thread->interrupt_stack_base - STACK_SIZE));
                FreePhysicalPageCont((uint64_t)GetPhysicalAddress((void*)(next_thread->user_stack_base + 128 - STACK_SIZE)), 2);
                LockSpinlock(next_thread->ParentProcess->lock);
                UnmapPage(next_thread->ParentProcess->PageTable, next_thread->user_stack_base + 128 - STACK_SIZE, STACK_SIZE);

                for(uint64_t i = 0; i < List_Length(next_thread->ParentProcess->ThreadIDs); i++) {
                    if((uint64_t)List_EntryAt(next_thread->ParentProcess->ThreadIDs, i) == next_thread->ID) {
                        List_Remove(next_thread->ParentProcess->ThreadIDs, i);
                    }
                }

                uint64_t parent_thd_cnt = List_Length(next_thread->ParentProcess->ThreadIDs);
                ProcessStatus parent_status = next_thread->ParentProcess->Status;
                uint64_t parent_pid = next_thread->ParentProcess->ID;

                UnlockSpinlock(next_thread->ParentProcess->lock);
                if((parent_thd_cnt == 0) | (parent_status == ProcessStatus_Terminating))
                    KillProcess(parent_pid);

                FreeSpinlock(next_thread->lock);
                kfree(next_thread);
                next_thread = NULL;
            } else {
                List_AddEntry(thds, next_thread);
            }
            break;
        case ThreadState_Paused:
            break;
        case ThreadState_Sleep:
            LockSpinlock(next_thread->lock);
            next_thread->sleep_duration_ms -= (next_thread->sleep_duration_ms > preempt_frequency/1000)?preempt_frequency/1000 : next_thread->sleep_duration_ms;
            if(next_thread->sleep_duration_ms == 0) {
                next_thread->state = ThreadState_Running;
                exit_loop = TRUE;
                UnlockSpinlock(next_thread->lock);
            } else {
                UnlockSpinlock(next_thread->lock);
                List_AddEntry(thds, next_thread);
            }
            break;
        default:
            SET_PROPERTY_VAL(next_thread, cur_executing, TRUE);
            exit_loop = TRUE;
            break;
        }

    }


    return next_thread;
}

void
TaskSwitch(uint32_t int_no,
           uint32_t err_code) {
    err_code = 0;

    SaveFPUState(GET_PROPERTY_VAL(coreState->cur_thread, fpu_state));
    SavePreviousThread(coreState->cur_thread);

    if(List_Length(thds) > 0)coreState->cur_thread = GetNextThread(coreState->cur_thread);

    RestoreFPUState(GET_PROPERTY_VAL(coreState->cur_thread, fpu_state));
    SetInterruptStack((void*)coreState->cur_thread->interrupt_stack_aligned);
    SetKernelStack((void*)coreState->cur_thread->kernel_stack_aligned);

    SetActiveVirtualMemoryInstance(GET_PROPERTY_PROC_VAL(coreState->cur_thread, PageTable));

    if(coreState->cur_thread->state == ThreadState_Running) {
        HandleInterruptNoReturn(int_no);
        SwitchToThread(coreState->cur_thread);
    } else if(coreState->cur_thread->state == ThreadState_Initialize) {
        coreState->cur_thread->state = ThreadState_Running;
        HandleInterruptNoReturn(int_no);
        SwitchAndInitializeThread(coreState->cur_thread);
    } else {
        __asm__("cli\n\thlt" :: "a"(coreState->cur_thread->state));
    }
}

void
SwitchThread(void) {

    coreState->cur_thread = GetNextThread(NULL);

    RestoreFPUState(GET_PROPERTY_VAL(coreState->cur_thread, fpu_state));
    SetInterruptStack((void*)coreState->cur_thread->interrupt_stack_aligned);
    SetKernelStack((void*)coreState->cur_thread->kernel_stack_aligned);

    SetActiveVirtualMemoryInstance(GET_PROPERTY_PROC_VAL(coreState->cur_thread, PageTable));
    //Resume execution of the thread
    if(GET_PROPERTY_VAL(coreState->cur_thread, state) == ThreadState_Initialize) {
        SET_PROPERTY_VAL(coreState->cur_thread, state, ThreadState_Running);
        SwitchAndInitializeThread(coreState->cur_thread);
    }

    __asm__ ("cli\n\thlt");
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

    CoreInfo *cInfo = kmalloc(sizeof(CoreInfo));
    cInfo->ID = id;
    cInfo->getCoreData = getCoreData;

    List_AddEntry(cores, cInfo);

    if(coreState == NULL)
        coreState = (CoreThreadState*)AllocateAPLSMemory(sizeof(CoreThreadState));

    coreState->cur_thread = NULL;
    coreState->coreID = id;
}

int
GetCoreLoad(int coreNum) {
    if(coreNum > (int)List_Length(cores))return -1;
    return ((CoreInfo*)List_EntryAt(cores, coreNum))->getCoreData();
}