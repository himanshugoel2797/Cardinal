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
static volatile CoreThreadState* coreState = NULL;
static uint64_t preempt_frequency;
static uint32_t preempt_vector;
static volatile _Atomic UID base_thread_ID = 1;

static UID
new_thd_uid(void) {
    return (UID)(uint32_t)base_thread_ID++;
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
PROPERTY_PROC_GET(ProcessInformation*, Parent, NULL)
PROPERTY_PROC_GET(List*, PendingMessages, NULL)
PROPERTY_PROC_GET(List*, ThreadIDs, NULL)
PROPERTY_PROC_GET(ProcessStatus, Status, ProcessStatus_Stopped)

PROPERTY_GET_SET(UID, ID, 0)

PROPERTY_GET_SET(ProcessInformation*, ParentProcess, NULL)

PROPERTY_GET_SET(ThreadState, State, 0)
PROPERTY_GET_SET(ThreadWakeCondition, WakeCondition, 0)
PROPERTY_GET_SET(ThreadPriority, Priority, 0)

PROPERTY_GET_SET(uint64_t, InterruptStackBase, 0)
PROPERTY_GET_SET(uint64_t, InterruptStackAligned, 0)
PROPERTY_GET_SET(uint64_t, KernelStackBase, 0)
PROPERTY_GET_SET(uint64_t, KernelStackAligned, 0)
PROPERTY_GET_SET(uint64_t, CurrentStack, 0)
PROPERTY_GET_SET(uint64_t, SleepDurationNS, 0)
PROPERTY_GET_SET(uint64_t, SleepStartTime, 0)
PROPERTY_GET_SET(uint64_t, Errno, 0)

PROPERTY_GET_SET(int32_t, CoreAffinity, 0)

PROPERTY_GET_SET(void*, SetChildTID, NULL)
PROPERTY_GET_SET(void*, ClearChildTID, NULL)
PROPERTY_GET_SET(void*, SetParentTID, NULL)
PROPERTY_GET_SET(void*, FPUState, NULL)
PROPERTY_GET_SET(void*, ArchSpecificData, NULL)

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

UID
GetCurrentProcessParentUID(void) {
    if(coreState != NULL && coreState->cur_thread != NULL && GET_PROPERTY_PROC_VAL(coreState->cur_thread, Parent) != NULL)
        return GET_PROPERTY_PROC_VAL(coreState->cur_thread, Parent)->ID;
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
        &user_stack_base,
        STACK_SIZE,
        MemoryAllocationType_Stack,
        MemoryAllocationFlags_Write | ((perm_level == ThreadPermissionLevel_User)?MemoryAllocationFlags_User : 0));

    if(user_stack_base == 0)while(1);

    MapPage(pInfo->PageTable,
            0,
            user_stack_base,
            STACK_SIZE,
            CachingModeWriteBack,
            MemoryAllocationType_Stack | MemoryAllocationType_ReservedAllocation,
            MemoryAllocationFlags_Write | ((perm_level == ThreadPermissionLevel_User)?MemoryAllocationFlags_User : 0)
           );

    UnlockSpinlock(pInfo->lock);
    return user_stack_base + STACK_SIZE - 16;
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
    regs.rbp = user_stack_base;
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
    regs.tls = NULL;
    regs.p_tid = NULL;
    regs.clear_tid = NULL;
    regs.set_tid = NULL;

    if(perm_level == ThreadPermissionLevel_User) {
        regs.ss = 0x20 | 3; //DPL: 3
        regs.cs = 0x28 | 3; //DPL: 3
    } else if(perm_level == ThreadPermissionLevel_Kernel) {
        regs.ss = 0x10 | 0; //DPL: 0
        regs.cs = 0x08 | 0; //DPL: 0
    }

    regs.set_tid = NULL;
    regs.clear_tid = NULL;
    regs.p_tid = NULL;
    regs.tls = NULL;

    return CreateThreadADV(parentProcess, &regs);
}

UID
CreateThreadADV(UID parentProcess,
                CRegisters *regs) {

    ThreadInfo *thd = kmalloc(sizeof(ThreadInfo));
    thd->lock = CreateSpinlock();
    LockSpinlock(thd->lock);
    LockSpinlock(sync_lock);

    SET_PROPERTY_VAL(thd, State, ThreadState_Initialize);
    SET_PROPERTY_VAL(thd, Priority, ThreadPriority_Neutral);
    SET_PROPERTY_VAL(thd, SleepDurationNS, 0);
    SET_PROPERTY_VAL(thd, FPUState, kmalloc(GetFPUStateSize() + 64));
    SET_PROPERTY_VAL(thd, InterruptStackBase, AllocateStack(parentProcess, ThreadPermissionLevel_Kernel));
    SET_PROPERTY_VAL(thd, KernelStackBase, AllocateStack(parentProcess, ThreadPermissionLevel_Kernel));
    SET_PROPERTY_VAL(thd, ArchSpecificData, kmalloc(ARCH_SPECIFIC_SPACE_SIZE));
    SET_PROPERTY_VAL(thd, ClearChildTID, regs->clear_tid);
    SET_PROPERTY_VAL(thd, SetChildTID, regs->set_tid);
    SET_PROPERTY_VAL(thd, SetParentTID, regs->p_tid);
    SET_PROPERTY_VAL(thd, Errno, 0);

    //Setup kernel stack
    uint64_t kstack = GET_PROPERTY_VAL(thd, KernelStackBase);
    SET_PROPERTY_VAL(thd, KernelStackAligned, kstack);

    //Setup interrupt stack
    uint64_t istack = GET_PROPERTY_VAL(thd, InterruptStackBase);
    SET_PROPERTY_VAL(thd, InterruptStackAligned, istack);

    //Setup FPU state
    uint64_t FPUState_tmp = (uint64_t)GET_PROPERTY_VAL(thd,FPUState);
    if(FPUState_tmp % 64 != 0)
        FPUState_tmp += 64 - FPUState_tmp % 64;

    SET_PROPERTY_VAL(thd, FPUState, (void*)FPUState_tmp);
    SaveFPUState(GET_PROPERTY_VAL(thd, FPUState));

    //Get the process information
    ProcessInformation *pInfo = NULL;
    if(GetProcessReference(parentProcess, &pInfo) == ProcessErrors_UIDNotFound)
        goto error_exit;

    SET_PROPERTY_VAL(thd, ParentProcess, pInfo);
    AtomicIncrement32(&thd->ParentProcess->reference_count);

    SetupArchSpecificData(thd, regs);



    uint64_t cur_stack_frame_vaddr = SetupTemporaryWriteMap(pInfo->PageTable, kstack - kstack % PAGE_SIZE, PAGE_SIZE);
    uint64_t *cur_stack_frame = (uint64_t*)(cur_stack_frame_vaddr + (kstack % PAGE_SIZE));
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

    uint64_t* kstack_p = (uint64_t*)kstack;

    UninstallTemporaryWriteMap(cur_stack_frame_vaddr, PAGE_SIZE);
    //Update the thread list
    //__asm__("cli\n\thlt" :: "a"(kstack), "b"(cur_stack_frame));
    SET_PROPERTY_VAL(thd, CurrentStack, (uint64_t)(&kstack_p[offset]));


    SET_PROPERTY_VAL(thd, ID, new_thd_uid());
    List_AddEntry(GET_PROPERTY_PROC_VAL(thd, ThreadIDs), (void*)GET_PROPERTY_VAL(thd, ID));
    List_AddEntry(thds, thd);
    List_AddEntry(neutral, thd);

    UnlockSpinlock(sync_lock);
    UnlockSpinlock(thd->lock);
    return GET_PROPERTY_VAL(thd, ID);

error_exit:
    UnlockSpinlock(sync_lock);
    UnlockSpinlock(thd->lock);
    FreeSpinlock(thd->lock);
    kfree(thd);
    return -1;
}

void
SetChildTIDAddress(UID id,
                   void *address) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, SetChildTID, address);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, SetChildTID, address);
            return;
        }
    }

}

void
SetClearChildTIDAddress(UID id,
                        void *address) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, ClearChildTID, address);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, ClearChildTID, address);
            return;
        }
    }

}

void*
GetClearChildTIDAddress(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, ClearChildTID);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, ClearChildTID);
        }
    }

    return NULL;
}

void*
GetChildTIDAddress(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, SetChildTID);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, SetChildTID);
        }
    }

    return NULL;
}

void*
GetParentTIDAddress(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, SetParentTID);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, SetParentTID);
        }
    }

    return NULL;
}

void
SetThreadState(UID id,
               ThreadState state) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, State, state);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, State, state);
            return;
        }
    }
}

void
SleepThread(UID id,
            uint64_t duration_ns) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, State, ThreadState_Sleep);
        SET_PROPERTY_VAL(coreState->cur_thread, WakeCondition, ThreadWakeCondition_SleepEnd);
        SET_PROPERTY_VAL(coreState->cur_thread, SleepStartTime, GetTimerValue());
        SET_PROPERTY_VAL(coreState->cur_thread, SleepDurationNS, duration_ns);
        YieldThread();
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, State, ThreadState_Sleep);
            SET_PROPERTY_VAL(thd, WakeCondition, ThreadWakeCondition_SleepEnd);
            SET_PROPERTY_VAL(thd, SleepStartTime, GetTimerValue());
            SET_PROPERTY_VAL(thd, SleepDurationNS, duration_ns);
            return;
        }
    }
}

ThreadState
GetThreadState(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, State);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, State);
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
        return (void*)GET_PROPERTY_VAL(coreState->cur_thread, KernelStackAligned);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return (void*)GET_PROPERTY_VAL(thd, KernelStackAligned);
        }
    }
    return NULL;
}

void*
GetThreadCurrentStack(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return (void*)GET_PROPERTY_VAL(coreState->cur_thread, CurrentStack);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return (void*)GET_PROPERTY_VAL(thd, CurrentStack);
        }
    }
    return NULL;
}

void
SetThreadBasePriority(UID id,
                      ThreadPriority Priority) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, Priority, Priority);
        return;
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, Priority, Priority);
            return;
        }
    }
}

ThreadPriority
GetThreadPriority(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, Priority);
    }

    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, Priority);
        }
    }
    return -1;
}


void
SetThreadCoreAffinity(UID id,
                      int coreID) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, CoreAffinity, coreID);
        return;
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, CoreAffinity, coreID);
            return;
        }
    }
}

int
GetThreadCoreAffinity(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, CoreAffinity);
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, CoreAffinity);
        }
    }
    return -1;
}

void
FreeThread(UID id) {
    SetThreadState(id, ThreadState_Exiting);
}

void
SetThreadIsPaused(UID tid, bool paused) {
    if(paused)
        SetThreadState(tid, ThreadState_Paused);
    else
        SetThreadState(tid, ThreadState_Running);
}

void
YieldThread(void) {
    ResetPreemption();
    RaiseInterrupt(preempt_vector);
}

ThreadInfo*
GetNextThread(ThreadInfo *prevThread) {

    /*
    - Get a thread from the queue
    - Process the thread
    - Put the thread back once done
    */

    LockSpinlock(sync_lock);

    if(prevThread != NULL) {
        LockSpinlock(sync_lock);
        List_AddEntry(neutral, prevThread);
        UnlockSpinlock(sync_lock);
    }

    ThreadInfo *next_thread = NULL;

    bool exit_loop = FALSE;
    while(!exit_loop) {

        LockSpinlock(sync_lock);
        next_thread = List_EntryAt(neutral, 0);
        if(next_thread != NULL)List_Remove(neutral, 0);
        UnlockSpinlock(sync_lock);

        if(next_thread == NULL)continue;

        //If the process is terminating, terminate the thread
        if(GET_PROPERTY_PROC_VAL(next_thread, Status) == ProcessStatus_Terminating && GetCurrentProcessUID() != GET_PROPERTY_PROC_VAL(next_thread, ID)) {
            SET_PROPERTY_VAL(next_thread, State, ThreadState_Exiting);
        }

        switch(GET_PROPERTY_VAL(next_thread, State)) {
        case ThreadState_Exiting:
            if(GetSpinlockContenderCount(next_thread->lock) == 0) {
                LockSpinlock(next_thread->lock);

                CachingMode cMode = 0;
                MemoryAllocationFlags cFlags = 0;
                GetAddressPermissions(GET_PROPERTY_PROC_VAL(next_thread, PageTable),
                                        (uint64_t)next_thread->SetChildTID,
                                        &cMode,
                                        &cFlags,
                                        NULL);

                if(cMode != 0 && cFlags != 0) {
                    if(cFlags & MemoryAllocationFlags_User)
                        WriteValueAtAddress32(GET_PROPERTY_PROC_VAL(next_thread, PageTable),
                                              (uint32_t*)next_thread->ClearChildTID,
                                              0);
                }

                kfree((void*)(next_thread->KernelStackBase - STACK_SIZE));
                kfree((void*)(next_thread->InterruptStackBase - STACK_SIZE));

                for(uint64_t i = 0; i < List_Length(thds); i++) {
                    ThreadInfo *tInfo = List_EntryAt(thds, i);
                    if(GET_PROPERTY_VAL(tInfo, ID) == next_thread->ID) {
                        List_Remove(thds, i);
                        break;
                    }
                }

                for(uint64_t i = 0; i < List_Length(GET_PROPERTY_PROC_VAL(next_thread, ThreadIDs)); i++) {
                    UID id = (UID)List_EntryAt(GET_PROPERTY_PROC_VAL(next_thread, ThreadIDs), i);

                    if(id == next_thread->ID) {
                        List_Remove(GET_PROPERTY_PROC_VAL(next_thread, ThreadIDs), i);
                        break;
                    }
                }

                LockSpinlock(next_thread->ParentProcess->lock);

                AtomicDecrement32(&next_thread->ParentProcess->reference_count);
                if(List_Length(GET_PROPERTY_PROC_VAL(next_thread, ThreadIDs)) == 0) {
                    TerminateProcess(GET_PROPERTY_PROC_VAL(next_thread, ID));
                }
                UnlockSpinlock(next_thread->lock);
                FreeSpinlock(next_thread->lock);


                kfree(next_thread);
                next_thread = NULL;
            } else {
                LockSpinlock(sync_lock);
                List_AddEntry(neutral, next_thread);
                UnlockSpinlock(sync_lock);
            }
            break;
        case ThreadState_Paused:
            break;
        case ThreadState_Sleep:
            LockSpinlock(next_thread->lock);
            uint64_t cur_time = GetTimerValue();
            if(next_thread->WakeCondition == ThreadWakeCondition_SleepEnd) {
                if(GetTimerInterval_NS(cur_time - next_thread->SleepStartTime) >= next_thread->SleepDurationNS) {
                    next_thread->State = ThreadState_Running;
                    exit_loop = TRUE;
                    UnlockSpinlock(next_thread->lock);
                } else {
                    UnlockSpinlock(next_thread->lock);
                    LockSpinlock(sync_lock);
                    List_AddEntry(neutral, next_thread);
                    UnlockSpinlock(sync_lock);
                }
            } else {
                UnlockSpinlock(next_thread->lock);
                LockSpinlock(sync_lock);
                List_AddEntry(neutral, next_thread);
                UnlockSpinlock(sync_lock);
            }
            break;
        default: {
            if(GET_PROPERTY_PROC_VAL(next_thread, Status) == ProcessStatus_Executing)
                exit_loop = TRUE;
            else {
                LockSpinlock(sync_lock);
                List_AddEntry(neutral, next_thread);
                UnlockSpinlock(sync_lock);
            }
        }
        break;
        }

    }

    UnlockSpinlock(sync_lock);
    return next_thread;
}

#include "boot_information/boot_information.h"

void
TaskSwitch(uint32_t int_no,
           uint32_t err_code) {

    //if(coreState->coreID == 1)
    {
        err_code = 0;

        LockSpinlock(sync_lock);

        SaveFPUState(GET_PROPERTY_VAL(coreState->cur_thread, FPUState));
        PerformArchSpecificTaskSave(coreState->cur_thread);
        SavePreviousThread(coreState->cur_thread);

        if(List_Length(thds) > 0)coreState->cur_thread = GetNextThread(coreState->cur_thread);

        RestoreFPUState(GET_PROPERTY_VAL(coreState->cur_thread, FPUState));
        SetInterruptStack((void*)coreState->cur_thread->InterruptStackAligned);
        SetKernelStack((void*)coreState->cur_thread->KernelStackAligned);
        SetActiveVirtualMemoryInstance(GET_PROPERTY_PROC_VAL(coreState->cur_thread, PageTable));
        PerformArchSpecificTaskSwitch(coreState->cur_thread);


        if(coreState->cur_thread->State == ThreadState_Initialize) {
            coreState->cur_thread->State = ThreadState_Running;

            CachingMode cMode = 0;
            MemoryAllocationFlags cFlags = 0;
            GetAddressPermissions(GET_PROPERTY_PROC_VAL(coreState->cur_thread, PageTable),
                                    (uint64_t)coreState->cur_thread->SetChildTID,
                                    &cMode,
                                    &cFlags,
                                    NULL);

            if(cMode != 0 && cFlags != 0) {
                if(cFlags & MemoryAllocationFlags_User)
                    *(uint32_t*)coreState->cur_thread->SetChildTID = (uint32_t)coreState->cur_thread->ID;
            }

            if(coreState->cur_thread->ParentProcess->Parent != NULL) {
                GetAddressPermissions(coreState->cur_thread->ParentProcess->Parent->PageTable,
                                        (uint64_t)coreState->cur_thread->SetParentTID,
                                        &cMode,
                                        &cFlags,
                                        NULL);

                if(cMode != 0 && cFlags != 0) {
                    if(cFlags & MemoryAllocationFlags_User)
                        WriteValueAtAddress32(coreState->cur_thread->ParentProcess->Parent->PageTable,
                                              (uint32_t*)coreState->cur_thread->SetParentTID,
                                              (uint32_t)coreState->cur_thread->ID);
                }
            }

        }
        HandleInterruptNoReturn(int_no);

        UnlockSpinlock(sync_lock);

        SwitchToThread(coreState->cur_thread);
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

    LockSpinlock(sync_lock);
    coreState->cur_thread = GetNextThread(NULL);

    RestoreFPUState(GET_PROPERTY_VAL(coreState->cur_thread, FPUState));
    SetInterruptStack((void*)coreState->cur_thread->InterruptStackAligned);
    SetKernelStack((void*)coreState->cur_thread->KernelStackAligned);

    SetActiveVirtualMemoryInstance(GET_PROPERTY_PROC_VAL(coreState->cur_thread, PageTable));
    PerformArchSpecificTaskSwitch(coreState->cur_thread);

    //Resume execution of the thread
    if(GET_PROPERTY_VAL(coreState->cur_thread, State) == ThreadState_Initialize) {
        SET_PROPERTY_VAL(coreState->cur_thread, State, ThreadState_Running);

        UnlockSpinlock(sync_lock);
        SwitchToThread(coreState->cur_thread);
    }
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
        coreState = (volatile CoreThreadState*)AllocateAPLSMemory(sizeof(CoreThreadState));

    coreState->cur_thread = NULL;
    coreState->coreID = id;
}

int
GetCoreLoad(int coreNum) {
    if(coreNum > (int)List_Length(cores))return -1;
    return ((CoreInfo*)List_EntryAt(cores, coreNum))->getCoreData();
}

void
SetThreadErrno(UID id,
               uint64_t errno) {

    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        SET_PROPERTY_VAL(coreState->cur_thread, Errno, errno);
        return;
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, Errno, errno);
            return;
        }
    }
}

uint64_t
GetThreadErrno(UID id) {
    if(id == GET_PROPERTY_VAL(coreState->cur_thread, ID)) {
        return GET_PROPERTY_VAL(coreState->cur_thread, Errno);
    }
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( GET_PROPERTY_VAL(thd, ID) == id) {
            return GET_PROPERTY_VAL(thd, Errno);
        }
    }
    return -1;
}