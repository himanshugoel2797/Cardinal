#ifndef _CARDINAL_THREAD_H_
#define _CARDINAL_THREAD_H_

#include "types.h"

//TODO: Create threads, call HAL to handle context switching details
//TODO: Write a separate schedule manager that dictates thread switching in a process aware manner
//TODO: For the schedule manager, make sure to delete processes if their state suggests it

typedef enum{
	ThreadPriority_VeryLow = 0,
	ThreadPriority_Low = 1,
	ThreadPriority_Medium = 2,
	ThreadPriority_Neutral = 3,
	ThreadPriority_High = 4,
	ThreadPriority_VeryHigh = 5,
	ThreadPriority_Maximum = 6
} ThreadPriority;

typedef enum{
	ThreadState_Initialize,
	ThreadState_Running,
	ThreadState_Paused,
	ThreadState_Exiting
}ThreadState;

typedef void (*ThreadEntryPoint)(void);



void
Thread_Initialize(void);

UID
CreateThread(UID parentProcess,
			 ThreadEntryPoint entry_point);

void
SetThreadState(UID id,
			   ThreadState state);

ThreadState
GetThreadState(UID id);

void
SetThreadBasePriority(UID id,
					  ThreadPriority priority);

ThreadPriority
GetThreadPriority(UID id);

void
SetThreadCoreAffinity(UID id,
					  int coreID);

int
GetThreadCoreAffinity(UID id);

void
FreeThread(UID id);

void
YieldThread(void);

void
SwitchThread(void);

void
RegisterCore(int id,
			int (*getCoreData)(void));

int
GetCoreCount(void);

int
GetCoreLoad(int coreNum);

#endif