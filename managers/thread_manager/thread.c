#include "thread.h"
#include "common/common.h"
#include "kmalloc.h"
#include "common/list.h"

typedef struct ThreadInfo
{
	UID ID;
	ThreadEntryPoint entry_point;
	ThreadState state;
	ThreadPriority priority;
}ThreadInfo;

typedef struct CoreInfo
{
	UID ID;
	int (*getCoreData)(void);
}CoreInfo;

static List *vLow, *low, *medium, *neutral, *high, *vHigh, *max;
static List* cores;

void
Thread_Initialize(void)
{
	vLow = List_Create();
	low = List_Create();
	medium = List_Create();
	neutral = List_Create();
	high = List_Create();
	vHigh = List_Create();
	max = List_Create();

	cores = List_Create();
}

UID
CreateThread(UID parentProcess,
			 ThreadEntryPoint entry_point)
{
	ThreadInfo *thd = kmalloc(sizeof(ThreadInfo));
	
}

void
SetThreadState(UID id,
			   ThreadState state)
{

}

ThreadState
GetThreadState(UID id)
{

}

void
SetThreadBasePriority(UID id,
					  ThreadPriority priority)
{

}

ThreadPriority
GetThreadPriority(UID id)
{

}


void
SetThreadCoreAffinity(UID id,
					  int coreID)
{

}

int
GetThreadCoreAffinity(UID id)
{

}

void
FreeThread(UID id)
{

}

void
YieldThread(void)
{

}

void
SwitchThread(void)
{

}

void
RegisterCore(int id,
			 int (*getCoreData)(void))
{
	CoreInfo *cInfo = kmalloc(sizeof(CoreInfo));
	cInfo->ID = id;
	cInfo->getCoreData = getCoreData;

	List_AddEntry(cores, cInfo);
}

int
GetCoreCount(void)
{
	return List_Length(cores);
}

int
GetCoreLoad(int coreNum)
{
	if(coreNum > List_Length(cores))return -1;
	return ((CoreInfo*)List_EntryAt(cores, coreNum))->getCoreData();
}