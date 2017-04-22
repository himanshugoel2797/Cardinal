/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "thread.h"
#include "common/common.h"
#include "kmalloc.h"

#include "common/list.h"
#include "common/heap.h"
#include "common/btree.h"

#include "target/hal/thread.h"
#include "target/hal/interrupts.h"
#include "target/hal/syscall.h"
#include "target/hal/timer.h"

#include "debug_gfx.h"

typedef struct CoreThreadState {
    ThreadInfo *cur_thread;
    Heap *cur_heap;
    Heap *back_heap;
    uint32_t coreID;
} CoreThreadState;

Spinlock sync_lock;
Spinlock starving_lock;
Spinlock pending_lock;
Spinlock sleeping_lock;
static List *pending_thds, *sleeping_thds;
static BTree *thds;
static bool isInited = FALSE;
static volatile CoreThreadState *all_states = NULL;

static CoreInfo *cores;
static uint64_t *core_id = NULL;
static volatile _Atomic uint64_t core_id_counter = 0;

static uint32_t preempt_vector;

static UID new_thd_uid(void) {
    return (UID)BTree_GetKey(thds);
}

UID GetCurrentThreadUID(void) {
    if (all_states != NULL && all_states[*core_id].cur_thread != NULL) {

        if(LockSpinlock(all_states[*core_id].cur_thread->lock) == NULL)
            return -1;

        UID retVal = all_states[*core_id].cur_thread->ID;
        UnlockSpinlock(all_states[*core_id].cur_thread->lock);
        return retVal;
    } else
        return -1;
}

UID GetCurrentProcessUID(void) {
    if (all_states != NULL && all_states[*core_id].cur_thread != NULL) {

        if(LockSpinlock(all_states[*core_id].cur_thread->lock) == NULL)
            return -1;

        if(LockSpinlock(all_states[*core_id].cur_thread->Process->lock) == NULL) {
            UnlockSpinlock(all_states[*core_id].cur_thread->lock);
            return -1;
        }

        UID retVal = all_states[*core_id].cur_thread->Process->PID;

        UnlockSpinlock(all_states[*core_id].cur_thread->Process->lock);
        UnlockSpinlock(all_states[*core_id].cur_thread->lock);
        return retVal;
    } else
        return -1;
}

UID
GetProcessGroupID(UID pid) {
    ProcessInfo *pInfo = NULL;
    if(GetProcessReference(pid, &pInfo) != ThreadError_None)
        return -1;

    if(LockSpinlock(pInfo->lock) == NULL)
        return -1;

    UID retVal = pInfo->GroupID;

    UnlockSpinlock(pInfo->lock);
    return retVal;
}

uint64_t
SetProcessGroupID(UID pid, UID gid) {
    ProcessInfo *pInfo = NULL;
    if(GetProcessReference(pid, &pInfo) != ThreadError_None)
        return -1;

    if(LockSpinlock(pInfo->lock) == NULL)
        return -1;

    pInfo->GroupID = gid;

    UnlockSpinlock(pInfo->lock);
    return gid;
}

UID GetCurrentProcessParentUID(void) {
    if (all_states != NULL && all_states[*core_id].cur_thread != NULL) {

        if(LockSpinlock(all_states[*core_id].cur_thread->lock) == NULL)
            return -1;

        if(LockSpinlock(all_states[*core_id].cur_thread->Process->lock) == NULL) {
            UnlockSpinlock(all_states[*core_id].cur_thread->lock);
            return -1;
        }

        UID retVal = all_states[*core_id].cur_thread->Process->ParentID;

        UnlockSpinlock(all_states[*core_id].cur_thread->Process->lock);
        UnlockSpinlock(all_states[*core_id].cur_thread->lock);
        return retVal;
    } else
        return -1;
}

void Thread_Initialize(void) {
    thds = BTree_Create(4);  // The thread tree is 4 levels deep
    BTree_GetKey(thds);      // Force keys to start from 1

    starving_lock = CreateSpinlock();
    pending_lock = CreateSpinlock();
    pending_thds = List_Create(pending_lock);

    sleeping_lock = CreateSpinlock();
    sleeping_thds = List_Create(sleeping_lock);

    sync_lock = CreateSpinlock();

    all_states = kmalloc(GetCoreCount() * sizeof(CoreThreadState));
    cores = kmalloc(GetCoreCount() * sizeof(CoreInfo));

    core_id = (uint64_t *)AllocateAPLSMemory(sizeof(uint64_t));


    ThreadInfo *root_thd = kmalloc(sizeof(ThreadInfo));
    ProcessInfo *pInfo = kmalloc(sizeof(ProcessInfo));
    root_thd->State = ThreadState_Created;
    root_thd->Process = pInfo;
    root_thd->ID = new_thd_uid();
    root_thd->lock = CreateSpinlock();
    pInfo->GroupID = 0;
    pInfo->PID = root_thd->ID;
    pInfo->ParentID = 0;
    pInfo->UserID = 0;
    pInfo->PageTable = GetActiveVirtualMemoryInstance();
    pInfo->lock = CreateSpinlock();
    pInfo->Status = ProcessStatus_Executing;

    BTree_Insert(thds, root_thd->ID, root_thd);


    isInited = TRUE;
}

bool
Thread_IsInitialized(void) {
    return isInited;
}

uint64_t AllocateStack(ProcessInfo *pInfo, ThreadPermissionLevel perm_level) {

    uint64_t stack_Size = (perm_level == ThreadPermissionLevel_User)
                          ? USER_STACK_SIZE
                          : KERNEL_STACK_SIZE;

    if(LockSpinlock(pInfo->lock) == NULL)
        return -1;

    // Setup the user stack
    uint64_t user_stack_base = 0;

    FindFreeVirtualAddress(
        pInfo->PageTable, &user_stack_base, stack_Size + 2 * PAGE_SIZE,
        MemoryAllocationType_Stack,
        MemoryAllocationFlags_Write | ((perm_level == ThreadPermissionLevel_User)
                                       ? MemoryAllocationFlags_User
                                       : 0));

    //TODO: debug block.
    if (user_stack_base == 0)
        while (1)
            ;

    // Add an offset in front for the guard page, the same has been done on the
    // back.
    user_stack_base += PAGE_SIZE;

    MapPage(pInfo->PageTable,
            (perm_level == ThreadPermissionLevel_User)
            ? 0
            : AllocatePhysicalPageCont(stack_Size / PAGE_SIZE,
                                       PhysicalMemoryAllocationFlags_None),
            user_stack_base, stack_Size, CachingModeWriteBack,
            MemoryAllocationType_Stack |
            ((perm_level == ThreadPermissionLevel_User)
             ? MemoryAllocationType_ReservedAllocation
             : 0),
            MemoryAllocationFlags_Write | MemoryAllocationFlags_Present |
            ((perm_level == ThreadPermissionLevel_User)
             ? MemoryAllocationFlags_User
             : MemoryAllocationFlags_Kernel));

    return user_stack_base;
}

uint64_t GetStackSize(ThreadPermissionLevel perm_level) {
    return (perm_level == ThreadPermissionLevel_User) ? USER_STACK_SIZE
           : KERNEL_STACK_SIZE;
}

ThreadError
CreateProcess(UID parentProcess, ProcessInfo **pInf) {

        ProcessInfo *pInfo = NULL;
        if(GetProcessReference(parentProcess, &pInfo) != ThreadError_None)
            return ThreadError_UIDNotFound;

        ProcessInfo *nProc = kmalloc(sizeof(ProcessInfo));
        if(nProc == NULL) {
            ReturnProcessReference(parentProcess);
            return ThreadError_Unknown;
        }

        nProc->lock = CreateSpinlock();
        nProc->PID = new_thd_uid();
        nProc->ParentID = parentProcess;
        nProc->UserID = pInfo->UserID;
        nProc->GroupID = pInfo->GroupID;
        nProc->Status = ProcessStatus_Executing;
        nProc->PageTable = kmalloc(sizeof(ManagedPageTable));
        if(nProc->PageTable == NULL) {
            FreeSpinlock(nProc->lock);
            kfree(nProc);
            ReturnProcessReference(parentProcess);
            return ThreadError_Unknown;
        }

        if(CreateVirtualMemoryInstance(nProc->PageTable) != MemoryAllocationErrors_None) {
            kfree(nProc->PageTable);
            FreeSpinlock(nProc->lock);
            kfree(nProc);
            ReturnProcessReference(parentProcess);
            return ThreadError_Unknown;
        }

        //TODO: Descriptor table

        *pInf = nProc;

        ReturnProcessReference(parentProcess);
        return ThreadError_None;
}

UID CreateThread(UID parentProcess, bool newProcess, ThreadPermissionLevel perm_level,
                 ThreadEntryPoint entry_point, void *arg) {

    ProcessInfo *pInf = NULL;
    if(newProcess) {
        if(CreateProcess(parentProcess, &pInf) != ThreadError_None)
            return -1;

        LockSpinlock(pInf->lock);
    }else{
        if(GetProcessReference(parentProcess, &pInf) != ThreadError_None)
            return -1;

        if(LockSpinlock(pInf->lock) == NULL){
            ReturnProcessReference(parentProcess);
            return -1;
        }
    }

    uint64_t user_stack_base = AllocateStack(pInf, perm_level);
    uint64_t user_stack_bottom = user_stack_base;

    user_stack_base += GetStackSize(perm_level) -
                       256;  // Subtract 256 to allocate space for red zone

    Registers regs;
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

    if (perm_level == ThreadPermissionLevel_User) {
        regs.ss = 0x20 | 3;  // DPL: 3
        regs.cs = 0x28 | 3;  // DPL: 3
    } else if (perm_level == ThreadPermissionLevel_Kernel) {
        regs.ss = 0x10 | 0;  // DPL: 0
        regs.cs = 0x08 | 0;  // DPL: 0
    }

    UID retVal = CreateThreadADV(pInf, newProcess, perm_level, user_stack_bottom, &regs,
                           NULL);
    
    UnlockSpinlock(pInf->lock);
    if(!newProcess)
        ReturnProcessReference(parentProcess);

    return retVal;
}

UID CreateThreadADV(ProcessInfo *pInfo, bool newProcess, ThreadPermissionLevel perm_level,
                    uint64_t user_stack_bottom, Registers *regs, void *tls) {

    if(pInfo == NULL)
        return -1;

    ThreadInfo *thd = kmalloc(sizeof(ThreadInfo));
    thd->lock = CreateSpinlock();
    LockSpinlock(thd->lock);
    LockSpinlock(sync_lock);

    thd->TimeSlice = THREAD_TOTAL_TIMESLICE;
    thd->State = ThreadState_Created;
    thd->PermissionLevel = perm_level;
    thd->SleepDurationNS = 0;
    thd->KernelStackBase = AllocateStack(pInfo, ThreadPermissionLevel_Kernel);
    if(thd->KernelStackBase == (uint64_t)-1)
        goto error_exit;

    thd->FPUState = kmalloc(GetFPUStateSize() + 64);
    thd->ArchSpecificData = kmalloc(ARCH_SPECIFIC_SPACE_SIZE);
    thd->Errno = 0;
    thd->UserStackBase = user_stack_bottom;

    // Setup kernel stack
    uint64_t kstack = thd->KernelStackBase +
                      GetStackSize(ThreadPermissionLevel_Kernel) - 256;
    thd->KernelStackAligned = kstack;

    // Setup FPU state
    uint64_t FPUState_tmp = (uint64_t)thd->FPUState;
    if (FPUState_tmp % 64 != 0) FPUState_tmp += 64 - FPUState_tmp % 64;

    thd->FPUState = (void *)FPUState_tmp;
    SaveFPUState(thd->FPUState);


    SetupArchSpecificData(thd, regs, tls);

    if(newProcess)
        thd->ID = pInfo->PID;
    else
        thd->ID = new_thd_uid();

    // Setup process information pointer
    thd->Process = pInfo;

    BTree_Insert(thds, thd->ID, thd);

    LockSpinlock(pending_lock);
    List_AddEntry(pending_thds, thd);
    UnlockSpinlock(pending_lock);

    PrintDebugMessage("Thread: %x\r\n", thd->ID);

    UID tid = thd->ID;

    UnlockSpinlock(sync_lock);
    UnlockSpinlock(thd->lock);

    return tid;

error_exit:
    UnlockSpinlock(sync_lock);
    UnlockSpinlock(thd->lock);
    FreeSpinlock(thd->lock);
    kfree(thd);

    return -1;
}

ThreadError GetThreadReference(UID id, ThreadInfo **thd) {
    ThreadInfo *tmp_thd = (ThreadInfo *)BTree_GetValue(thds, id);
    if (tmp_thd == NULL) return ThreadError_UIDNotFound;
    *thd = tmp_thd;
    return ThreadError_None;
}

ThreadError GetProcessReference(UID pid, ProcessInfo **pInfo) {
    if(pInfo == NULL)
        return ThreadError_InvalidParams;

    ThreadInfo *thd = NULL;
    if(GetThreadReference(pid, &thd) != ThreadError_None)
        return ThreadError_UIDNotFound;

    if(LockSpinlock(thd->lock) == NULL)
        return ThreadError_Deleting;

    *pInfo = thd->Process;
    return ThreadError_None;
}

ThreadError
ReturnProcessReference(UID pid) {

    ThreadInfo *thd = NULL;
    if(GetThreadReference(pid, &thd) != ThreadError_None)
        return ThreadError_UIDNotFound;

    UnlockSpinlock(thd->lock);
    return ThreadError_None;
}

void SetThreadState(UID id, ThreadState state) {
    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);

    if (thd != NULL) {
        if(LockSpinlock(thd->lock) == NULL)
            return;

        thd->State = state;
        UnlockSpinlock(thd->lock);
    }
}

void SleepThread(UID id, uint64_t duration_ns) {
    // TODO update this for new scheduler

    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);
    if (thd != NULL) {

        if(LockSpinlock(thd->lock) == NULL)
            return;

        thd->State = ThreadState_Sleep;
        thd->WakeCondition = ThreadWakeCondition_SleepEnd;
        thd->SleepStartTime = GetTimerValue();
        thd->SleepDurationNS = duration_ns;

        // A kernel thread will loop through the threads and wake them up as
        // necessary.
        List_AddEntry(sleeping_thds, thd);

        UnlockSpinlock(thd->lock);
        if (id == GetCurrentThreadUID()) YieldThread();

        return;
    }
}

ThreadState GetThreadState(UID id) {
    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);
    if (thd != NULL) {

        if(LockSpinlock(thd->lock) == NULL)
            return -1;

        ThreadState state = thd->State;
        UnlockSpinlock(thd->lock);
        return state;
    }
    return -1;
}

void *GetThreadKernelStack(UID id) {
    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);
    if (thd != NULL) {

        if(LockSpinlock(thd->lock) == NULL)
            return NULL;

        void *kstack = (void *)thd->KernelStackAligned;
        UnlockSpinlock(thd->lock);

        return kstack;
    }
    return NULL;
}

int64_t AddThreadTimeSlice(UID tid, int64_t slice) {
    ThreadInfo *thd = NULL;
    GetThreadReference(tid, &thd);
    if (thd != NULL) {
        if(LockSpinlock(thd->lock) == NULL)
            return 0;

        thd->TimeSlice += slice;
        int64_t retVal = thd->TimeSlice;
        UnlockSpinlock(thd->lock);
        return retVal;
    }
    return 0;
}

void FreeThread(UID id) {
    SetThreadState(id, ThreadState_Exiting);
}

void SetThreadIsPaused(UID tid, bool paused) {
    if (paused)
        SetThreadState(tid, ThreadState_Paused);
    else
        SetThreadState(tid, ThreadState_Running);
}

uint64_t GetCoreIndex(void) {
    if (core_id != NULL) return *core_id;

    return 0;
}

uint64_t GetActiveCoreCount(void) {
    return core_id_counter;
}

void TryAddThreads(void) {
    for (int i = 0; i < MAX_THREADS_PER_SWAP; i++) {

        LockSpinlock(pending_lock);
        ThreadInfo *thd_ad = (ThreadInfo *)List_EntryAt(pending_thds, 0);
        List_Remove(pending_thds, 0);
        UnlockSpinlock(pending_lock);

        if (thd_ad != NULL) {
            PrintDebugMessage("Adding: Thread ID: %x\r\n", thd_ad->ID);
            Heap_Insert(all_states[*core_id].back_heap, thd_ad->TimeSlice, thd_ad);
        }
    }
}

ThreadInfo *GetNextThread(ThreadInfo *prevThread) {
    /*
    - Get a thread from the queue
    - Process the thread
    - Put the thread back once done
    */

    if (prevThread != NULL) {
        prevThread->TimeSlice -= THREAD_PREEMPTION_COST;

        if(LockSpinlock(prevThread->lock) != NULL) {
            switch (prevThread->State) {
            case ThreadState_Exiting: {
                //TODO: Delete the thread
                //TODO: Ensure that a thread's spinlock is deleted after every reference to the thread has been removed.
            }
            break;
            case ThreadState_Sleep:
                break;
            default:
                if (prevThread->TimeSlice <= 0) {
                    prevThread->TimeSlice = THREAD_TOTAL_TIMESLICE;
                    Heap_Insert(all_states[*core_id].back_heap, prevThread->TimeSlice,
                                prevThread);
                } else {
                    Heap_Insert(all_states[*core_id].cur_heap, prevThread->TimeSlice,
                                prevThread);
                }
                UnlockSpinlock(prevThread->lock);
                break;
            }
        }
    }

    // Determine how much load this thread ought to have
    int cur_load = 0, desired_load = 0, max_wait = 1000;

    do {
        cur_load = (Heap_GetItemCount(all_states[*core_id].cur_heap) * 100) /
                   BTree_GetCount(thds);
        desired_load = (100 / GetActiveCoreCount());

        LockSpinlock(pending_lock);
        ThreadInfo *thd_ad = (ThreadInfo *)List_EntryAt(pending_thds, 0);
        List_Remove(pending_thds, 0);
        UnlockSpinlock(pending_lock);

        if (thd_ad != NULL) {
            PrintDebugMessage("Adding: Thread ID: %x Core: %x\r\n", thd_ad->ID, *core_id);
            Heap_Insert(all_states[*core_id].cur_heap, thd_ad->TimeSlice, thd_ad);
        }

        max_wait--;
    } while (desired_load >= cur_load && List_Length(pending_thds) > 0 && max_wait > 0);

    ThreadInfo *next_thread = NULL;

    bool exit_loop = FALSE;
    while (!exit_loop) {
        // Exhausted all the items in the current set, switch to the back heap while
        // picking up any new threads if available
        if (Heap_GetItemCount(all_states[*core_id].cur_heap) == 0) {
            PrintDebugMessage("Swap %x Core %x\r\n",
                              Heap_GetItemCount(all_states[*core_id].back_heap), *core_id);

            // Pick up threads from the pending queue if any are available
            if (TryLockSpinlock(starving_lock) && List_Length(pending_thds) != 0) {
                TryAddThreads();
                UnlockSpinlock(starving_lock);
            }

            // Wait for more threads to be made pending
            while (Heap_GetItemCount(all_states[*core_id].back_heap) == 0) {
                // TODO report that this core is waiting for threads, so other cores
                // shouldn't bother pulling in more load.

                //Release 
                SetIF();
                HALT(0);  // Halt for another core to wake us up for threads.
                ClearIF();
                LockSpinlock(starving_lock);
                while (List_Length(pending_thds) == 0) PAUSE;

                TryAddThreads();
                UnlockSpinlock(starving_lock);
            }

            // Swap the front and back heaps
            Heap *new_front = all_states[*core_id].back_heap;
            all_states[*core_id].back_heap = all_states[*core_id].cur_heap;
            all_states[*core_id].cur_heap = new_front;
        }

        next_thread = Heap_Pop(all_states[*core_id].cur_heap, NULL);

        //Current thread is being deleted, pick another thread.
        if(LockSpinlock(next_thread->lock) == NULL) {
            continue;
        }

        // PrintDebugMessage(" Trying: %x, TimeSlice: %x \r\n",
        // next_thread->ParentProcess->ID, next_thread->TimeSlice);

        // If the process is terminating, terminate the thread
        // TODO: inspect this portion and determine if it is necessary to not be deleting the currently executing thread.
        bool currentlyLocked = FALSE;
        if(LockSpinlock(next_thread->Process->lock) == NULL) {
            next_thread->State = ThreadState_Exiting;
        } else if (next_thread->Process->Status == ProcessStatus_Terminating &&
                   GetCurrentProcessUID() != next_thread->Process->PID) {
            next_thread->State = ThreadState_Exiting;
            currentlyLocked = TRUE;
        }

        switch (next_thread->State) {
        case ThreadState_Exiting:
            //TODO: Delete the thread
            break;
        case ThreadState_Paused:
            break;
        case ThreadState_Sleep:
            break;
        case ThreadState_Created:
            break;
        default: {
            exit_loop = TRUE;
        }
        break;
        }

        if(!exit_loop) {
            if(currentlyLocked) UnlockSpinlock(next_thread->Process->lock);
            UnlockSpinlock(next_thread->lock);
        }
    }

    return next_thread;
}

void SchedulerCycle(Registers *regs) {
    if (all_states[*core_id].cur_thread != NULL) {
        if(LockSpinlock(all_states[*core_id].cur_thread->lock) != NULL) {
            SaveFPUState(all_states[*core_id].cur_thread->FPUState);
            SaveTask(all_states[*core_id].cur_thread, regs);
            UnlockSpinlock(all_states[*core_id].cur_thread->lock);
        }
    }

    UID prevId = GetCurrentProcessUID();
    UID prevTID = GetCurrentThreadUID();

    if (BTree_GetCount(thds) > 0)
        all_states[*core_id].cur_thread =
            GetNextThread(all_states[*core_id].cur_thread);

     /*PrintDebugMessage("Front: %x Back: %x Core %x Thread From: %x:%x To: %x:%x \r\n", Heap_GetItemCount(all_states[*core_id].cur_heap) + 1,
        Heap_GetItemCount(all_states[*core_id].back_heap), *core_id, prevId,
        prevTID, GetCurrentProcessUID(), GetCurrentThreadUID());*/

    RestoreFPUState(all_states[*core_id].cur_thread->FPUState);
    SetKernelStack((void *)all_states[*core_id].cur_thread->KernelStackAligned);

    // Only switch virtual address space if necessary.
    if (prevId != GetCurrentProcessUID())
        SetActiveVirtualMemoryInstance(all_states[*core_id].cur_thread->Process->PageTable);


    if (all_states[*core_id].cur_thread->State == ThreadState_Initialize) {
        all_states[*core_id].cur_thread->State = ThreadState_Running;
    }

    UnlockSpinlock(all_states[*core_id].cur_thread->Process->lock);
    UnlockSpinlock(all_states[*core_id].cur_thread->lock);
}

void TaskSwitch(uint32_t int_no, uint32_t err_code) {
    {
        err_code = 0;
        SchedulerCycle(GetSavedInterruptState());
        ConfigurePreemption(MIN(THREAD_PREEMPTION_COST,
                                all_states[*core_id].cur_thread->TimeSlice));
        HandleInterruptNoReturn(int_no);
        SwitchToTask(all_states[*core_id].cur_thread);
    }
}

void SetPeriodicPreemptVector(uint32_t irq) {
    RegisterInterruptHandler(irq, TaskSwitch);
    preempt_vector = irq;
}

void RegisterCore(int (*getCoreData)(void)) {
    LockSpinlock(sync_lock);
    *core_id = core_id_counter;

    cores[*core_id].ID = *core_id;
    cores[*core_id].getCoreData = getCoreData;

    all_states[*core_id].cur_thread = NULL;
    all_states[*core_id].cur_heap = Heap_Create();
    all_states[*core_id].back_heap = Heap_Create();
    all_states[*core_id].coreID = *core_id;

    PrintDebugMessage("Register Core: %x\r\n", *core_id);

    core_id_counter++;
    UnlockSpinlock(sync_lock);

    // Threads are placed into the associated heaps when they are scheduled.
    // Replace main thread structure with something with faster indexing
}

int GetCoreLoad(int coreNum) {
    if (coreNum > GetCoreCount()) return -1;
    return cores[coreNum].getCoreData();
}

void SetThreadErrno(UID id, uint64_t errno) {
    ThreadInfo *thd = NULL;
    if(GetThreadReference(id, &thd) != ThreadError_None)
        return;

    if (thd != NULL) {
        if(LockSpinlock(thd->lock) == NULL)
            return;

        thd->Errno = errno;
        UnlockSpinlock(thd->lock);
    }
}

uint64_t GetThreadErrno(UID id) {
    ThreadInfo *thd = NULL;
    if(GetThreadReference(id, &thd) != ThreadError_None)
        return -1;

    if (thd != NULL) {

        if(LockSpinlock(thd->lock) == NULL)
            return -1;

        uint64_t retVal = thd->Errno;

        UnlockSpinlock(thd->lock);
        return retVal;
    }

    return -1;
}

void WakeThread(UID id) {
    LockSpinlock(sleeping_lock);

    List_RotPrev(sleeping_thds);
    for (uint64_t i = 0; i < List_Length(sleeping_thds); i++) {
        ThreadInfo *thd = (ThreadInfo *)List_RotNext(sleeping_thds);

        if(LockSpinlock(thd->lock) == NULL) {
            List_Remove(sleeping_thds, List_GetLastIndex(sleeping_thds));
            continue;
        }

        if (thd->ID == id) {
            thd->State = ThreadState_Running;

            List_Remove(sleeping_thds, List_GetLastIndex(sleeping_thds));
            List_AddEntry(pending_thds, thd);

            UnlockSpinlock(thd->lock);
            break;
        }

        UnlockSpinlock(thd->lock);
    }

    UnlockSpinlock(sleeping_lock);
}

void DeleteThread(ThreadInfo *thd) {

    if(FinalLockSpinlock(thd->lock) == NULL)
        return; //Another thread is already freeing this thread

    FreeMapping((void *)thd->KernelStackBase,
                GetStackSize(ThreadPermissionLevel_Kernel));
    FreeMapping((void *)thd->UserStackBase, GetStackSize(thd->PermissionLevel));

    BTree_RemoveEntry(thds, thd->ID);

    //TODO: delete process if this thread is the owner, killing all sibling processes.

    UnlockSpinlock(thd->lock);
    FreeSpinlock(thd->lock);
    kfree(thd);
}

void WakeReadyThreads(void) {
    LockSpinlock(sleeping_lock);

    ThreadInfo *thd = (ThreadInfo *)List_RotNext(sleeping_thds);

    if (thd != NULL) {
        if(LockSpinlock(thd->lock) == NULL) {
            UnlockSpinlock(sleeping_lock);
            return;
        }

        uint64_t cur_time = GetTimerValue();

        if (thd->WakeCondition == ThreadWakeCondition_SleepEnd) {
            if (GetTimerInterval_NS(cur_time - thd->SleepStartTime) >=
                    thd->SleepDurationNS) {
                thd->State = ThreadState_Running;

                List_Remove(sleeping_thds, List_GetLastIndex(sleeping_thds));
                List_AddEntry(pending_thds, thd);
            }
        }

        UnlockSpinlock(thd->lock);
    }

    UnlockSpinlock(sleeping_lock);
}