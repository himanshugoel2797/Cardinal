#include "thread.h"
#include "common/common.h"
#include "kmalloc.h"
#include "synchronization.h"
#include "common/list.h"
#include "target/hal/thread.h"

static Spinlock vLow_s, low_s, medium_s, neutral_s, high_s, vHigh_s, max_s, thds_s, core_s;
static List *vLow, *low, *medium, *neutral, *high, *vHigh, *max, *thds;
static List* cores;
static ThreadInfo *cur_thread;

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
    thd->ID = new_uid();
    thd->stack = kmalloc(KiB(16));

    LockSpinlock(&neutral_s);
    List_AddEntry(neutral, thd);
    UnlockSpinlock(&neutral_s);

    LockSpinlock(&thds_s);
    List_AddEntry(thds, thd);
    UnlockSpinlock(&thds_s);

    return thd->ID;
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

void
FreeThread(UID id) {
    for(uint64_t i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->state = ThreadState_Exiting;
        }
    }
}

void
YieldThread(void) {

}

void
TaskSwitch(uint32_t int_no,
           uint32_t err_code)
{
    int_no = 0;
    err_code = 0;
    SwapThreadOnInterrupt(cur_thread, cur_thread);
}

void
SwitchThread(void) {
    cur_thread = List_EntryAt(thds, 0);
    List_Remove(thds, 0);
    List_AddEntry(thds, cur_thread);

    //Resume execution of the thread
    if(cur_thread->state == ThreadState_Initialize)
    {
        SwitchAndInitializeThread(cur_thread);
    }
    else if(cur_thread->state == ThreadState_Running)
    {

    }
    else if(cur_thread->state == ThreadState_Paused)return;
}

void
CoreUpdate(int coreID)
{
    //Obtain thread to process from the lists
    //TODO make kmalloc work on all threads by having it share the mappings on to all cores
    coreID = 0;
    while(TRUE)
    {
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
}

int
GetCoreLoad(int coreNum) {
    if(coreNum > (int)List_Length(cores))return -1;
    return ((CoreInfo*)List_EntryAt(cores, coreNum))->getCoreData();
}