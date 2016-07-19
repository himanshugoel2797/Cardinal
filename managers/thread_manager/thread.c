#include "thread.h"
#include "common/common.h"
#include "kmalloc.h"
#include "common/list.h"
#include "target/hal/thread.h"
#include "target/hal/interrupts.h"
#include "target/hal/syscall.h"
#include "target/hal/timer.h"

typedef struct CoreThreadState {
    ThreadInfo *cur_thread;
    uint32_t    coreID;
} CoreThreadState;

static Spinlock sync_lock;
static List *neutral, *thds;
static List* cores;
static volatile CoreThreadState *coreState = NULL;
static uint64_t preempt_frequency;
static uint32_t preempt_vector;
static volatile UID base_thread_ID = 0;

static UID
new_thd_uid(void) {
    register UID dummy = 1;
    __asm__ volatile("lock xadd %[dummy], (%[bs])" : [dummy]"=r"(dummy) : [dummy]"r"(dummy), [bs]"r"(&base_thread_ID));
    return (UID)(uint32_t)dummy;
}

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


#define PROPERTY_PROC_GET(type, name, default_val) type get_proc_##name (ProcessInformation *t) \
                             { \
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

PROPERTY_PROC_GET(UID, ID, 0)
PROPERTY_PROC_GET(ManagedPageTable*, PageTable, 0)
PROPERTY_PROC_GET_SET(uint32_t, reference_count, 0)

PROPERTY_GET_SET(UID, ID, 0)

PROPERTY_GET_SET(ProcessInformation*, ParentProcess, NULL)

PROPERTY_GET_SET(ThreadState, state, 0)
PROPERTY_GET_SET(ThreadWakeCondition, wakeCondition, 0)
PROPERTY_GET_SET(ThreadPriority, priority, 0)

PROPERTY_GET_SET(uint64_t, interrupt_stack_base, 0)
PROPERTY_GET_SET(uint64_t, interrupt_stack_aligned, 0)
PROPERTY_GET_SET(uint64_t, kernel_stack_base, 0)
PROPERTY_GET_SET(uint64_t, kernel_stack_aligned, 0)
PROPERTY_GET_SET(uint64_t, current_stack, 0)
PROPERTY_GET_SET(uint64_t, sleep_duration_ns, 0)
PROPERTY_GET_SET(uint64_t, sleep_start_time, 0)

PROPERTY_GET_SET(int32_t, core_affinity, 0)

PROPERTY_GET_SET(void*, set_child_tid, NULL)
PROPERTY_GET_SET(void*, clear_child_tid, NULL)
PROPERTY_GET_SET(void*, set_parent_tid, NULL)
PROPERTY_GET_SET(void*, fpu_state, NULL)
PROPERTY_GET_SET(void*, arch_specific_data, NULL)

UID
GetCurrentThreadUID(void) {
    if(coreState != NULL && coreState->cur_thread != NULL)
        return GET_PROPERTY_VAL(coreState->cur_thread, ID);
    else
        return -1;
}

UID
GetCurrentProcessUID(void) {
    if(coreState != NULL && coreState->cur_thread != NULL)
        return GET_PROPERTY_PROC_VAL(coreState->cur_thread, ID);
    else
        return -1;
}

void
Thread_Initialize(void) {
    Spinlock tmp = CreateSpinlock();
    neutral = List_Create(tmp);
    thds = List_Create(tmp);

    cores = List_Create(CreateSpinlock());
    sync_lock = CreateSpinlock();
}

#define STACK_SIZE KiB(16)

uint64_t
AllocateStack(UID parentProcess,
              ThreadPermissionLevel perm_level) {

    ProcessInformation *pInfo = NULL;
    if(GetProcessReference(parentProcess, &pInfo) == ProcessErrors_UIDNotFound)
        return -1;

    LockSpinlock(pInfo->lock);
    //Setup the user stack
    uint64_t user_stack_base = 0;

    FindFreeVirtualAddress(
        pInfo->PageTable,
        (uint64_t*)&user_stack_base,
        STACK_SIZE,
        MemoryAllocationType_Stack,
        MemoryAllocationFlags_Write | ((perm_level == ThreadPermissionLevel_User)?MemoryAllocationFlags_User : 0));

    if(user_stack_base == 0)while(1);

    MapPage(pInfo->PageTable,
            AllocatePhysicalPageCont(STACK_SIZE/PAGE_SIZE),
            user_stack_base,
            STACK_SIZE,
            CachingModeWriteBack,
            MemoryAllocationType_Stack,
            MemoryAllocationFlags_Write | ((perm_level == ThreadPermissionLevel_User)?MemoryAllocationFlags_User : 0)
           );

    UnlockSpinlock(pInfo->lock);
    return user_stack_base + STACK_SIZE - 128;
}

UID
CreateThread(UID parentProcess,
             ThreadPermissionLevel perm_level,
             ThreadEntryPoint entry_point,
             void *arg) {

    uint64_t user_stack_base = AllocateStack(parentProcess, perm_level);

    CRegisters regs;
    regs.rip = (uint64_t)entry_point;
    regs.rsp = user_stack_base;
    regs.rbp = 0;
    regs.rax = 0;
    regs.rbx = 0;
    regs.rcx = 0;
    regs.rdx = 0;
    regs.rsi = 0;
    regs.rdi = (uint64_t)arg;
    regs.r8 = 0;
    regs.r9 = 0;
    regs.r10 = 0;
    regs.r11 = 0;
    regs.r12 = 0;
    regs.r13 = 0;
    regs.r14 = 0;
    regs.r15 = 0;
    regs.rflags = 0;

    if(perm_level == ThreadPermissionLevel_User) {
        regs.ss = 0x20;
        regs.cs = 0x28;
    } else if(perm_level == ThreadPermissionLevel_Kernel) {
        regs.ss = 0x10;
        regs.cs = 0x8;
    }

    return CreateThreadADV(parentProcess, &regs);
}

UID
CreateThreadADV(UID parentProcess,
                CRegisters *regs) {

    ThreadInfo *thd = kmalloc(sizeof(ThreadInfo));
    thd->lock = CreateSpinlock();

    SET_PROPERTY_VAL(thd, state, ThreadState_Initialize);
    SET_PROPERTY_VAL(thd, priority, ThreadPriority_Neutral);
    SET_PROPERTY_VAL(thd, sleep_duration_ns, 0);
    SET_PROPERTY_VAL(thd, fpu_state, kmalloc(GetFPUStateSize() + 16));
    SET_PROPERTY_VAL(thd, interrupt_stack_base, (uint64_t)kmalloc(STACK_SIZE) + STACK_SIZE);
    SET_PROPERTY_VAL(thd, kernel_stack_base, (uint64_t)kmalloc(STACK_SIZE) + STACK_SIZE);
    SET_PROPERTY_VAL(thd, arch_specific_data, kmalloc(ARCH_SPECIFIC_SPACE_SIZE));
    SET_PROPERTY_VAL(thd, clear_child_tid, regs->clear_tid);
    SET_PROPERTY_VAL(thd, set_child_tid, regs->set_tid);
    SET_PROPERTY_VAL(thd, set_parent_tid, regs->p_tid);

    //Setup kernel stack
    uint64_t kstack = GET_PROPERTY_VAL(thd, kernel_stack_base);
    kstack -= kstack % 16;
    SET_PROPERTY_VAL(thd, kernel_stack_aligned, kstack);

    //Setup interrupt stack
    uint64_t istack = GET_PROPERTY_VAL(thd, interrupt_stack_base);
    istack -= istack % 16;
    SET_PROPERTY_VAL(thd, interrupt_stack_aligned, istack);

    //Setup FPU state
    uint64_t fpu_state_tmp = (uint64_t)GET_PROPERTY_VAL(thd,fpu_state);
    if(fpu_state_tmp % 16 != 0)
        fpu_state_tmp += 16 - fpu_state_tmp % 16;

    SET_PROPERTY_VAL(thd, fpu_state, (void*)fpu_state_tmp);
    SaveFPUState(GET_PROPERTY_VAL(thd, fpu_state));

    //Get the process information
    ProcessInformation *pInfo = NULL;
    if(GetProcessReference(parentProcess, &pInfo) == ProcessErrors_UIDNotFound)
        goto error_exit;

    SET_PROPERTY_VAL(thd, ParentProcess, pInfo);
    AtomicIncrement32(&thd->ParentProcess->reference_count);

    SetupArchSpecificData(thd, regs);


    uint64_t *cur_stack_frame = (uint64_t*)kstack;
    int offset = 0;
    cur_stack_frame[--offset] = regs->ss;
    cur_stack_frame[--offset] = regs->rsp;
    cur_stack_frame[--offset] = (uint64_t)regs->rflags | (1 << 9);
    cur_stack_frame[--offset] = regs->cs;
    cur_stack_frame[--offset] = regs->rip;
    cur_stack_frame[--offset] = 0;
    cur_stack_frame[--offset] = 0;
    cur_stack_frame[--offset] = regs->rax;
    cur_stack_frame[--offset] = regs->rbx;
    cur_stack_frame[--offset] = regs->rcx;
    cur_stack_frame[--offset] = regs->rdx;
    cur_stack_frame[--offset] = regs->rbp;
    cur_stack_frame[--offset] = regs->rsi;
    cur_stack_frame[--offset] = regs->rdi;
    cur_stack_frame[--offset] = regs->r8;
    cur_stack_frame[--offset] = regs->r9;
    cur_stack_frame[--offset] = regs->r10;
    cur_stack_frame[--offset] = regs->r11;
    cur_stack_frame[--offset] = regs->r12;
    cur_stack_frame[--offset] = regs->r13;
    cur_stack_frame[--offset] = regs->r14;
    cur_stack_frame[--offset] = regs->r15;
    //push ss
    //push rsp
    //push rflags
    //push cs
    //push rip
    //push 0
    //push 0
    //push rax
    //push rbx
    //push rcx
    //push rdx
    //push rbp
    //push rsi
    //push rdi
    //push r8
    //push r9
    //push r10
    //push r11
    //push r12
    //push r13
    //push r14
    //push r15

    //Update the thread list
    SET_PROPERTY_VAL(thd, current_stack, (uint64_t)&cur_stack_frame[offset]);


    SET_PROPERTY_VAL(thd, ID, new_thd_uid());
    List_AddEntry(thds, thd);
    List_AddEntry(neutral, thd);

    return GET_PROPERTY_VAL(thd, ID);

error_exit:
    FreeSpinlock(thd->lock);
    kfree(thd);
    return -1;
}

void
SetChildTIDAddress(UID id,
                   void *address) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, set_child_tid, address);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, set_child_tid, address);
            return;
        }
    }

}

void
SetClearChildTIDAddress(UID id,
                        void *address) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, clear_child_tid, address);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, clear_child_tid, address);
            return;
        }
    }

}

void*
GetClearChildTIDAddress(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, clear_child_tid);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, clear_child_tid);
        }
    }

    return NULL;
}

void*
GetChildTIDAddress(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, set_child_tid);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, set_child_tid);
        }
    }

    return NULL;
}

void*
GetParentTIDAddress(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, set_parent_tid);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, set_parent_tid);
        }
    }

    return NULL;
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
        SET_PROPERTY_VAL(coreState->cur_thread, wakeCondition, ThreadWakeCondition_SleepEnd);
        SET_PROPERTY_VAL(coreState->cur_thread, sleep_start_time, GetTimerValue());
        SET_PROPERTY_VAL(coreState->cur_thread, sleep_duration_ns, duration_ms);
        YieldThread();
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, state, ThreadState_Sleep);
            SET_PROPERTY_VAL(thd, wakeCondition, ThreadWakeCondition_SleepEnd);
            SET_PROPERTY_VAL(thd, sleep_start_time, GetTimerValue());
            SET_PROPERTY_VAL(thd, sleep_duration_ns, duration_ms);
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
        //Setup the user stack
        uint64_t user_stack_base = AllocateStack(GET_PROPERTY_PROC_VAL(coreState->cur_thread, ID),
                                   ThreadPermissionLevel_User);

        return (void*)(user_stack_base);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {

            uint64_t user_stack_base = AllocateStack(GET_PROPERTY_PROC_VAL(thd, ID),
                                       ThreadPermissionLevel_User);
            return (void*)(user_stack_base);
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
        List_AddEntry(neutral, prevThread);
    }

    ThreadInfo *next_thread = NULL;

    bool exit_loop = FALSE;
    while(!exit_loop) {

        LockSpinlock(sync_lock);
        next_thread = List_EntryAt(neutral, 0);
        if(next_thread != NULL)List_Remove(neutral, 0);
        UnlockSpinlock(sync_lock);

        if(next_thread == NULL)continue;

        switch(GET_PROPERTY_VAL(next_thread, state)) {
        case ThreadState_Exiting:
            if(GetSpinlockContenderCount(next_thread->lock) == 0) {
                LockSpinlock(next_thread->lock);

                CachingMode cMode = 0;
                MemoryAllocationFlags cFlags = 0;
                CheckAddressPermissions(GET_PROPERTY_PROC_VAL(next_thread, PageTable),
                                        (uint64_t)next_thread->set_child_tid,
                                        &cMode,
                                        &cFlags);

                if(cMode != 0 && cFlags != 0) {
                    if(cFlags & MemoryAllocationFlags_User)
                        WriteValueAtAddress32(GET_PROPERTY_PROC_VAL(next_thread, PageTable),
                                              (uint32_t*)next_thread->clear_child_tid,
                                              0);
                }

                kfree((void*)(next_thread->kernel_stack_base - STACK_SIZE));
                kfree((void*)(next_thread->interrupt_stack_base - STACK_SIZE));

                for(uint64_t i = 0; i < List_Length(thds); i++) {
                    ThreadInfo *tInfo = List_EntryAt(thds, i);
                    if(GET_PROPERTY_VAL(tInfo, ID) == next_thread->ID) {
                        List_Remove(thds, i);
                        break;
                    }
                }

                LockSpinlock(next_thread->ParentProcess->lock);

                AtomicDecrement32(&next_thread->ParentProcess->reference_count);
                if(next_thread->ParentProcess->reference_count == 0) {
                    //TODO Free process memory
                }

                UnlockSpinlock(next_thread->ParentProcess->lock);
                FreeSpinlock(next_thread->lock);


                kfree(next_thread);
                next_thread = NULL;
            } else {
                List_AddEntry(neutral, next_thread);
            }
            break;
        case ThreadState_Paused:
            break;
        case ThreadState_Sleep:
            LockSpinlock(next_thread->lock);
            uint64_t cur_time = GetTimerValue();
            if(next_thread->wakeCondition == ThreadWakeCondition_SleepEnd) {
                if(GetTimerInterval_NS(cur_time - next_thread->sleep_start_time) >= next_thread->sleep_duration_ns) {
                    next_thread->state = ThreadState_Running;
                    exit_loop = TRUE;
                    UnlockSpinlock(next_thread->lock);
                } else {
                    UnlockSpinlock(next_thread->lock);
                    List_AddEntry(neutral, next_thread);
                }
            } else {
                UnlockSpinlock(next_thread->lock);
                List_AddEntry(neutral, next_thread);
            }
            break;
        default:
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
    PerformArchSpecificTaskSave(coreState->cur_thread);
    SavePreviousThread(coreState->cur_thread);


    if(List_Length(thds) > 0)coreState->cur_thread = GetNextThread(coreState->cur_thread);

    RestoreFPUState(GET_PROPERTY_VAL(coreState->cur_thread, fpu_state));
    SetInterruptStack((void*)coreState->cur_thread->interrupt_stack_aligned);
    SetKernelStack((void*)coreState->cur_thread->kernel_stack_aligned);
    SetActiveVirtualMemoryInstance(GET_PROPERTY_PROC_VAL(coreState->cur_thread, PageTable));
    PerformArchSpecificTaskSwitch(coreState->cur_thread);

    if(coreState->cur_thread->state == ThreadState_Initialize) {
        coreState->cur_thread->state = ThreadState_Running;

        CachingMode cMode = 0;
        MemoryAllocationFlags cFlags = 0;
        CheckAddressPermissions(GET_PROPERTY_PROC_VAL(coreState->cur_thread, PageTable),
                                (uint64_t)coreState->cur_thread->set_child_tid,
                                &cMode,
                                &cFlags);

        if(cMode != 0 && cFlags != 0) {
            if(cFlags & MemoryAllocationFlags_User)
                *(uint32_t*)coreState->cur_thread->set_child_tid = (uint32_t)coreState->cur_thread->ID;
        }

        if(coreState->cur_thread->ParentProcess->Parent != NULL) {
            CheckAddressPermissions(coreState->cur_thread->ParentProcess->Parent->PageTable,
                                    (uint64_t)coreState->cur_thread->set_parent_tid,
                                    &cMode,
                                    &cFlags);

            if(cMode != 0 && cFlags != 0) {
                if(cFlags & MemoryAllocationFlags_User)
                    WriteValueAtAddress32(coreState->cur_thread->ParentProcess->Parent->PageTable,
                                          (uint32_t*)coreState->cur_thread->set_parent_tid,
                                          (uint32_t)coreState->cur_thread->ID);
            }
        }

    }
    HandleInterruptNoReturn(int_no);
    SwitchToThread(coreState->cur_thread);
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

    coreState->cur_thread = GetNextThread(NULL);

    RestoreFPUState(GET_PROPERTY_VAL(coreState->cur_thread, fpu_state));
    SetInterruptStack((void*)coreState->cur_thread->interrupt_stack_aligned);
    SetKernelStack((void*)coreState->cur_thread->kernel_stack_aligned);

    SetActiveVirtualMemoryInstance(GET_PROPERTY_PROC_VAL(coreState->cur_thread, PageTable));
    PerformArchSpecificTaskSwitch(coreState->cur_thread);

    //Resume execution of the thread
    if(GET_PROPERTY_VAL(coreState->cur_thread, state) == ThreadState_Initialize) {
        SET_PROPERTY_VAL(coreState->cur_thread, state, ThreadState_Running);
        SwitchToThread(coreState->cur_thread);
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

void
SetThreadSigmask(UID id,
                 const sigset_t *flags) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        LockSpinlock(coreState->cur_thread->lock);
        memcpy(&coreState->cur_thread->SignalMask, flags, sizeof(sigset_t));
        UnlockSpinlock(coreState->cur_thread->lock);
        return;
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            LockSpinlock(thd->lock);
            memcpy(&thd->SignalMask, flags, sizeof(sigset_t));
            UnlockSpinlock(thd->lock);
            return;
        }
    }
}

void
GetThreadSigmask(UID           id,
                 sigset_t    *procInfo) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        LockSpinlock(coreState->cur_thread->lock);
        memcpy(procInfo, &coreState->cur_thread->SignalMask, sizeof(sigset_t));
        UnlockSpinlock(coreState->cur_thread->lock);
        return;
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            LockSpinlock(thd->lock);
            memcpy(procInfo, &thd->SignalMask, sizeof(sigset_t));
            UnlockSpinlock(thd->lock);
            return;
        }
    }
}