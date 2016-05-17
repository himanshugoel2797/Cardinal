#include "thread.h"
#include "common/common.h"
#include "kmalloc.h"
#include "common/list.h"

typedef struct ThreadInfo {
    UID Parent;
    UID ID;
    ThreadEntryPoint entry_point;
    ThreadState state;
    ThreadPriority priority;
    int core_affinity;
} ThreadInfo;

typedef struct CoreInfo {
    UID ID;
    int (*getCoreData)(void);
} CoreInfo;

static List *vLow, *low, *medium, *neutral, *high, *vHigh, *max, *thds;
static List* cores;

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

    List_AddEntry(neutral, thd);
    List_AddEntry(thds, thd);
    return thd->ID;
}

void
SetThreadState(UID id,
               ThreadState state) {
    for(int i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->state = state;
            return;
        }
    }
}

ThreadState
GetThreadState(UID id) {
    for(int i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            return thd->state;
        }
    }
}

void
SetThreadBasePriority(UID id,
                      ThreadPriority priority) {
    for(int i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->priority = priority;
            return;
        }
    }
}

ThreadPriority
GetThreadPriority(UID id) {
    for(int i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            return thd->priority;
        }
    }
}


void
SetThreadCoreAffinity(UID id,
                      int coreID) {
    for(int i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            thd->core_affinity = coreID;
            return;
        }
    }
}

int
GetThreadCoreAffinity(UID id) {
    for(int i = 0; i < List_Length(thds); i++) {
        ThreadInfo *thd = (ThreadInfo*)List_EntryAt(thds, i);
        if( thd->ID == id) {
            return thd->core_affinity;
        }
    }
}

void
FreeThread(UID id) {

}

void
YieldThread(void) {

}

void
SwitchThread(void) {

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
GetCoreCount(void) {
    return List_Length(cores);
}

int
GetCoreLoad(int coreNum) {
    if(coreNum > List_Length(cores))return -1;
    return ((CoreInfo*)List_EntryAt(cores, coreNum))->getCoreData();
}