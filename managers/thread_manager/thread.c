/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
static List *pending_thds, *sleeping_thds;
static BTree *thds;

static volatile CoreThreadState *all_states = NULL;

static CoreInfo* cores;
static uint64_t *core_id = NULL;
static volatile _Atomic uint64_t core_id_counter = 0;

static uint32_t preempt_vector;

static UID
new_thd_uid(void) {
    return (UID)BTree_GetKey(thds);
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
PROPERTY_PROC_GET(ProcessInformation*, Parent, NULL)
PROPERTY_PROC_GET(List*, PendingMessages, NULL)
PROPERTY_PROC_GET(List*, Threads, NULL)
PROPERTY_PROC_GET(ProcessStatus, Status, ProcessStatus_Stopped)

PROPERTY_GET_SET(UID, ID, 0)

PROPERTY_GET_SET(ProcessInformation*, ParentProcess, NULL)

PROPERTY_GET_SET(ThreadState, State, 0)
PROPERTY_GET_SET(ThreadWakeCondition, WakeCondition, 0)
PROPERTY_GET_SET(ThreadPermissionLevel, PermissionLevel, 0)

PROPERTY_GET_SET(uint64_t, KernelStackBase, 0)
PROPERTY_GET_SET(uint64_t, KernelStackAligned, 0)
PROPERTY_GET_SET(uint64_t, UserStackBase, 0)
PROPERTY_GET_SET(uint64_t, SleepDurationNS, 0)
PROPERTY_GET_SET(uint64_t, TargetMsgSourcePID, 0)
PROPERTY_GET_SET(uint64_t, SleepStartTime, 0)
PROPERTY_GET_SET(uint64_t, Errno, 0)

PROPERTY_GET_SET(int32_t, CoreAffinity, 0)

PROPERTY_GET_SET(void*, FPUState, NULL)
PROPERTY_GET_SET(void*, ArchSpecificData, NULL)

UID
GetCurrentThreadUID(void) {
    if(all_states != NULL && all_states[*core_id].cur_thread != NULL)
        return GET_PROPERTY_VAL(all_states[*core_id].cur_thread, ID);
    else
        return -1;
}

UID
GetCurrentProcessUID(void) {
    if(all_states != NULL && all_states[*core_id].cur_thread != NULL)
        return GET_PROPERTY_PROC_VAL(all_states[*core_id].cur_thread, ID);
    else
        return -1;
}

UID
GetCurrentProcessParentUID(void) {
    if(all_states != NULL && all_states[*core_id].cur_thread != NULL && GET_PROPERTY_PROC_VAL(all_states[*core_id].cur_thread, Parent) != NULL)
        return GET_PROPERTY_PROC_VAL(all_states[*core_id].cur_thread, Parent)->ID;
    else
        return -1;
}

void
Thread_Initialize(void) {
    thds = BTree_Create(4); //The thread tree is 4 levels deep
    BTree_GetKey(thds); //Force keys to start from 1

    starving_lock = CreateSpinlock();
    pending_thds = List_Create(CreateSpinlock());
    sleeping_thds = List_Create(CreateSpinlock());

    sync_lock = CreateSpinlock();

    all_states = kmalloc(GetCoreCount() * sizeof(CoreThreadState));
    cores = kmalloc(GetCoreCount() * sizeof(CoreInfo));

    core_id = (uint64_t*)AllocateAPLSMemory(sizeof(uint64_t));
}


uint64_t
AllocateStack(UID parentProcess,
              ThreadPermissionLevel perm_level) {

    ProcessInformation *pInfo = NULL;
    if(GetProcessReference(parentProcess, &pInfo) == ProcessErrors_UIDNotFound)
        return -1;

    uint64_t stack_Size = (perm_level == ThreadPermissionLevel_User)?USER_STACK_SIZE : KERNEL_STACK_SIZE;

    LockSpinlock(pInfo->lock);
    //Setup the user stack
    uint64_t user_stack_base = 0;

    FindFreeVirtualAddress(
        pInfo->PageTable,
        &user_stack_base,
        stack_Size + 2 * PAGE_SIZE,
        MemoryAllocationType_Stack,
        MemoryAllocationFlags_Write | ((perm_level == ThreadPermissionLevel_User)?MemoryAllocationFlags_User : 0));

    if(user_stack_base == 0)while(1);

    //Add an offset in front for the guard page, the same has been done on the back.
    user_stack_base += PAGE_SIZE;

    MapPage(pInfo->PageTable,
            (perm_level == ThreadPermissionLevel_User)?0:AllocatePhysicalPageCont(stack_Size/PAGE_SIZE, PhysicalMemoryAllocationFlags_None),
            user_stack_base,
            stack_Size,
            CachingModeWriteBack,
            MemoryAllocationType_Stack | ((perm_level == ThreadPermissionLevel_User)?MemoryAllocationType_ReservedAllocation:0),
            MemoryAllocationFlags_Write | MemoryAllocationFlags_Present |((perm_level == ThreadPermissionLevel_User)?MemoryAllocationFlags_User : MemoryAllocationFlags_Kernel)
           );

    UnlockSpinlock(pInfo->lock);
    return user_stack_base;
}

uint64_t
GetStackSize(ThreadPermissionLevel perm_level) {
    return (perm_level == ThreadPermissionLevel_User)?USER_STACK_SIZE : KERNEL_STACK_SIZE;
}

UID
CreateThread(UID parentProcess,
             ThreadPermissionLevel perm_level,
             ThreadEntryPoint entry_point,
             void *arg) {


    uint64_t user_stack_base = AllocateStack(parentProcess, perm_level);
    uint64_t user_stack_bottom = user_stack_base;

    user_stack_base += GetStackSize(perm_level) - 256;   //Subtract 256 to allocate space for red zone

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

    if(perm_level == ThreadPermissionLevel_User) {
        regs.ss = 0x20 | 3; //DPL: 3
        regs.cs = 0x28 | 3; //DPL: 3
    } else if(perm_level == ThreadPermissionLevel_Kernel) {
        regs.ss = 0x10 | 0; //DPL: 0
        regs.cs = 0x08 | 0; //DPL: 0
    }

    return CreateThreadADV(parentProcess, perm_level, user_stack_bottom, &regs, NULL);
}

UID
CreateThreadADV(UID parentProcess,
                ThreadPermissionLevel perm_level,
                uint64_t user_stack_bottom,
                Registers *regs,
                void *tls) {

    ThreadInfo *thd = kmalloc(sizeof(ThreadInfo));
    thd->lock = CreateSpinlock();
    LockSpinlock(thd->lock);
    LockSpinlock(sync_lock);

    RefInit(&thd->ref, (ReferenceFreeHandler)FreeThread, offsetof(ThreadInfo, ref));
    thd->TimeSlice = THREAD_TOTAL_TIMESLICE;

    SET_PROPERTY_VAL(thd, State, ThreadState_Initialize);
    SET_PROPERTY_VAL(thd, PermissionLevel, perm_level);
    SET_PROPERTY_VAL(thd, SleepDurationNS, 0);
    SET_PROPERTY_VAL(thd, FPUState, kmalloc(GetFPUStateSize() + 64));
    SET_PROPERTY_VAL(thd, KernelStackBase, AllocateStack(parentProcess, ThreadPermissionLevel_Kernel));
    SET_PROPERTY_VAL(thd, ArchSpecificData, kmalloc(ARCH_SPECIFIC_SPACE_SIZE));
    SET_PROPERTY_VAL(thd, Errno, 0);
    SET_PROPERTY_VAL(thd, UserStackBase, user_stack_bottom);

    //Setup kernel stack
    uint64_t kstack = GET_PROPERTY_VAL(thd, KernelStackBase) + GetStackSize(ThreadPermissionLevel_Kernel) - 256;
    SET_PROPERTY_VAL(thd, KernelStackAligned, kstack);

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
    RefInc(&thd->ParentProcess->ref);

    SetupArchSpecificData(thd, regs, tls);

    SET_PROPERTY_VAL(thd, ID, new_thd_uid());
    List_AddEntry(GET_PROPERTY_PROC_VAL(thd, Threads), (void*)thd->ID);

    BTree_Insert(thds, thd->ID, thd);

    List_AddEntry(pending_thds, thd);

    debug_gfx_writeLine("Thread: %x\r\n", GET_PROPERTY_VAL(thd, ID));

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



ThreadError
GetThreadReference(UID id, ThreadInfo **thd) {

    ThreadInfo *tmp_thd = (ThreadInfo*)BTree_GetValue(thds, id);
    if(tmp_thd == NULL)
        return ThreadError_UIDNotFound;

    if(tmp_thd->State == ThreadState_Exiting)
        return ThreadError_UIDNotFound;

    *thd = tmp_thd;
    RefInc(&tmp_thd->ref);
    return ThreadError_None;
}

void
SetThreadState(UID id,
               ThreadState state) {

    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);

    if(thd != NULL){
        SET_PROPERTY_VAL(thd, State, state);
        RefDec(&thd->ref);
    }
}

void
SleepThread(UID id,
            uint64_t duration_ns) {

    //TODO update this for new scheduler

    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);
    if(thd != NULL) {
        SET_PROPERTY_VAL(thd, State, ThreadState_Sleep);
        SET_PROPERTY_VAL(thd, WakeCondition, ThreadWakeCondition_SleepEnd);
        SET_PROPERTY_VAL(thd, SleepStartTime, GetTimerValue());
        SET_PROPERTY_VAL(thd, SleepDurationNS, duration_ns);

        //The scheduler will detect the state and move it into the appropriate queue
        //A kernel thread will loop through the threads and wake them up as necessary.
        return;
    }
}

ThreadState
GetThreadState(UID id) {
    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);
    if(thd != NULL) {
        ThreadState state = GET_PROPERTY_VAL(thd, State);
        RefDec(&thd->ref);
        return state;
    }
    return -1;
}

void*
GetThreadKernelStack(UID id) {
    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);
    if(thd != NULL) {
        void *kstack = (void*)GET_PROPERTY_VAL(thd, KernelStackAligned);
        RefDec(&thd->ref);
        return kstack;
    }
    return NULL;
}

int64_t
AddThreadTimeSlice(UID tid,
                   int64_t slice) {
    
    ThreadInfo *thd = NULL;
    GetThreadReference(tid, &thd);
    if(thd != NULL) {
        LockSpinlock(thd->lock);
        thd->TimeSlice += slice;
        int64_t retVal = thd->TimeSlice;
        UnlockSpinlock(thd->lock);
        RefDec(&thd->ref);
        return retVal;
    }
    return 0;
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

uint64_t
GetCoreIndex(void) {
    if(core_id != NULL)
        return *core_id;

    return 0;
}

uint64_t
GetActiveCoreCount(void) {
    return core_id_counter;
}

void
TryAddThreads(void){
    for(int i = 0; i < MAX_THREADS_PER_SWAP; i++){     
        ThreadInfo *thd_ad = (ThreadInfo*)List_EntryAt(pending_thds, 0);
        List_Remove(pending_thds, 0);

        if(thd_ad != NULL){
            debug_gfx_writeLine("Adding: Thread ID: %x\r\n", thd_ad->ID);
            Heap_Insert(all_states[*core_id].back_heap, thd_ad->TimeSlice, thd_ad);
        }
    }
}

ThreadInfo*
GetNextThread(ThreadInfo *prevThread) {

    /*
    - Get a thread from the queue
    - Process the thread
    - Put the thread back once done
    */


    if(prevThread != NULL) {
        prevThread->TimeSlice -= THREAD_PREEMPTION_COST;
        
        switch(GET_PROPERTY_VAL(prevThread, State)){

            case ThreadState_Exiting:
                RefDec(&prevThread->ref);
            break;
            case ThreadState_Sleep:
            break;
            default:
                if(prevThread->TimeSlice <= 0){
                    prevThread->TimeSlice = THREAD_TOTAL_TIMESLICE;
                    Heap_Insert(all_states[*core_id].back_heap, prevThread->TimeSlice, prevThread);
                }
                else{
                    Heap_Insert(all_states[*core_id].cur_heap, prevThread->TimeSlice, prevThread);
                }
            break;
        }
    }

    ThreadInfo *next_thread = NULL;

    bool exit_loop = FALSE;
    while(!exit_loop) {
        
        //Exhausted all the items in the current set, switch to the back heap while picking up any new threads if available
        if(Heap_GetItemCount(all_states[*core_id].cur_heap) == 0)
        {
            debug_gfx_writeLine("Swap %x\r\n", Heap_GetItemCount(all_states[*core_id].back_heap));

            //Pick up 10 threads from the pending queue if any are available
            if(TryLockSpinlock(starving_lock) && List_Length(pending_thds) != 0)
            {
                TryAddThreads();
                UnlockSpinlock(starving_lock);
            }

            //Wait for more threads to be made pending
            while(Heap_GetItemCount(all_states[*core_id].back_heap) == 0){
                //TODO report that this core is waiting for threads, so other cores shouldn't bother pulling in more load.
                __asm__ volatile("sti");
                HALT(0);    //Halt for another core to wake us up for threads.
                LockSpinlock(starving_lock);
                while(List_Length(pending_thds) == 0)
                    __asm__("pause");

                TryAddThreads();
                UnlockSpinlock(starving_lock);
            }

            //Swap the front and back heaps
            Heap *new_front = all_states[*core_id].back_heap;
            all_states[*core_id].back_heap = all_states[*core_id].cur_heap;
            all_states[*core_id].cur_heap = new_front;
        }

        next_thread = Heap_Pop(all_states[*core_id].cur_heap, NULL);

        //debug_gfx_writeLine(" Trying: %x, TimeSlice: %x \r\n", next_thread->ParentProcess->ID, next_thread->TimeSlice);

        //If the process is terminating, terminate the thread
        if(GET_PROPERTY_PROC_VAL(next_thread, Status) == ProcessStatus_Terminating && GetCurrentProcessUID() != GET_PROPERTY_PROC_VAL(next_thread, ID)) {
            SET_PROPERTY_VAL(next_thread, State, ThreadState_Exiting);
        }

        switch(GET_PROPERTY_VAL(next_thread, State)) {
            case ThreadState_Exiting:
                RefDec(&next_thread->ref);
                break;
            case ThreadState_Paused:
                break;
            case ThreadState_Sleep:
                List_AddEntry(sleeping_thds, next_thread);
                break;
            default: {
                    if(GET_PROPERTY_PROC_VAL(next_thread, Status) == ProcessStatus_Executing)
                        exit_loop = TRUE;
                    else {  
                        List_AddEntry(sleeping_thds, next_thread);
                    }
                }
                break;
        }
    }

    return next_thread;
}

void
SchedulerCycle(Registers *regs) {

        if(all_states[*core_id].cur_thread != NULL) {
            SaveFPUState(GET_PROPERTY_VAL(all_states[*core_id].cur_thread, FPUState));
            SaveTask(all_states[*core_id].cur_thread, regs);
        
            all_states[*core_id].cur_thread = RefDec(&all_states[*core_id].cur_thread->ref);
        }

        UID prevId = GetCurrentProcessUID();
        UID prevTID = GetCurrentThreadUID();

        if(BTree_GetCount(thds) > 0)
            all_states[*core_id].cur_thread = GetNextThread(all_states[*core_id].cur_thread);
        //debug_gfx_writeLine("Front: %x Back: %x Core %x Thread From: %x:%x To: %x:%x \r\n", Heap_GetItemCount(all_states[*core_id].cur_heap) + 1, Heap_GetItemCount(all_states[*core_id].back_heap), *core_id, prevId, prevTID, GetCurrentProcessUID(), GetCurrentThreadUID());

        RefInc(&all_states[*core_id].cur_thread->ref);

        RestoreFPUState(GET_PROPERTY_VAL(all_states[*core_id].cur_thread, FPUState));
        SetKernelStack((void*)all_states[*core_id].cur_thread->KernelStackAligned);
        SetActiveVirtualMemoryInstance(GET_PROPERTY_PROC_VAL(all_states[*core_id].cur_thread, PageTable));

        if(all_states[*core_id].cur_thread->State == ThreadState_Initialize) {
            all_states[*core_id].cur_thread->State = ThreadState_Running;
        }
}



void
TaskSwitch(uint32_t int_no,
           uint32_t err_code) {

    {
        err_code = 0;
        SchedulerCycle(GetSavedInterruptState());
        ConfigurePreemption(MIN(THREAD_PREEMPTION_COST, all_states[*core_id].cur_thread->TimeSlice));
        HandleInterruptNoReturn(int_no);
        SwitchToTask(all_states[*core_id].cur_thread);
    }
}

void
SetPeriodicPreemptVector(uint32_t irq) {
    RegisterInterruptHandler(irq, TaskSwitch);
    preempt_vector = irq;
}

void
RegisterCore(int (*getCoreData)(void)) {

    LockSpinlock(sync_lock);
    *core_id = core_id_counter;


    cores[*core_id].ID = *core_id;
    cores[*core_id].getCoreData = getCoreData;

    all_states[*core_id].cur_thread = NULL;
    all_states[*core_id].cur_heap = Heap_Create();
    all_states[*core_id].back_heap = Heap_Create();
    all_states[*core_id].coreID = *core_id;

    debug_gfx_writeLine("Register Core: %x\r\n", *core_id);

    core_id_counter++;
    UnlockSpinlock(sync_lock);

    //Threads are placed into the associated heaps when they are scheduled.
    //Replace main thread structure with something with faster indexing
}

int
GetCoreLoad(int coreNum) {
    if(coreNum > GetCoreCount()) return -1;
    return cores[coreNum].getCoreData();
}

void
SetThreadErrno(UID id,
               uint64_t errno) {
    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);
    if(thd != NULL)
        SET_PROPERTY_VAL(thd, Errno, errno);
}

uint64_t
GetThreadErrno(UID id) {
    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);
    if(thd != NULL)
        return GET_PROPERTY_VAL(thd, Errno);

    return -1;
}

void
SleepThreadForMessage(UID id,
                      MessageWaitType wait_type,
                      uint64_t val) {

    ThreadWakeCondition condition;
    switch(wait_type) {
    case MessageWaitType_Any:
        condition = ThreadWakeCondition_MatchMsgAny;
        break;
    case MessageWaitType_MsgType:
        condition = ThreadWakeCondition_MatchMsgType;
        break;
    case MessageWaitType_SourcePID:
        condition = ThreadWakeCondition_MatchMsgSourcePID;
        break;
    }


    LockSpinlock(sync_lock);
    ThreadInfo *thd = NULL;
    GetThreadReference(id, &thd);

    if(thd != NULL) {
        SET_PROPERTY_VAL(thd, State, ThreadState_Sleep);
        SET_PROPERTY_VAL(thd, WakeCondition, condition);
        SET_PROPERTY_VAL(thd, SleepStartTime, GetTimerValue());
        SET_PROPERTY_VAL(thd, TargetMsgSourcePID, val);

        //Remove the thread from the neutral list and put it into the sleeping list
        List_AddEntry(sleeping_thds, thd);
        //A kernel thread will loop through the threads and wake them up as necessary.

        ProcessCheckWakeThreads(GET_PROPERTY_PROC_VAL(thd, ID));

    }
    UnlockSpinlock(sync_lock);

    if(id == GetCurrentThreadUID())
        YieldThread();
}

void
WakeThread(UID id) {

    LockSpinlock(sync_lock);

    List_RotPrev(sleeping_thds);
    for(uint64_t i = 0; i < List_Length(sleeping_thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_RotNext(sleeping_thds);

        if(GET_PROPERTY_VAL(thd, ID) == id) {
            SET_PROPERTY_VAL(thd, State, ThreadState_Running);

            List_Remove(sleeping_thds, List_GetLastIndex(sleeping_thds));
            List_AddEntry(pending_thds, thd);
            break;
        }
    }

    UnlockSpinlock(sync_lock);
}

void
DeleteThread(ThreadInfo *thd) {

    LockSpinlock(sync_lock);

    FreeMapping((void*)thd->KernelStackBase, GetStackSize(ThreadPermissionLevel_Kernel));
    FreeMapping((void*)thd->UserStackBase, GetStackSize(thd->PermissionLevel));

    BTree_RemoveEntry(thds, thd->ID);

    for(uint64_t i = 0; i < List_Length(GET_PROPERTY_PROC_VAL(thd, Threads)); i++) {
        ThreadInfo* id = (ThreadInfo*)List_EntryAt(GET_PROPERTY_PROC_VAL(thd, Threads), i);

        if(id->ID == thd->ID) {
            List_Remove(GET_PROPERTY_PROC_VAL(thd, Threads), i);
            break;
        }
    }

    FreeSpinlock(thd->lock);

    memset(thd, 0, sizeof(ThreadInfo));
    kfree(thd);

    //LockSpinlock(next_thread->ParentProcess->lock);

    RefDec(&thd->ParentProcess->ref);
    UnlockSpinlock(sync_lock);
}

void
WakeReadyThreads(void) {

    ThreadInfo *thd = (ThreadInfo*)List_RotNext(sleeping_thds);

    if(thd != NULL) {
        LockSpinlock(thd->lock);
        uint64_t cur_time = GetTimerValue();

        if(thd->WakeCondition == ThreadWakeCondition_SleepEnd) {
            if(GetTimerInterval_NS(cur_time - thd->SleepStartTime) >= thd->SleepDurationNS) {
                SET_PROPERTY_VAL(thd, State, ThreadState_Running);

                List_Remove(sleeping_thds, List_GetLastIndex(sleeping_thds));
                List_AddEntry(pending_thds, thd);
            }
        } else {
            ProcessCheckWakeThreads(thd->ParentProcess->ID);
        }

        UnlockSpinlock(thd->lock);
    }
}