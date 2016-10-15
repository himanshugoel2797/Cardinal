#ifndef _LIBCARDINAL_THREAD_H_
#define _LIBCARDINAL_THREAD_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"

#include <stdint.h>
#include <stdbool.h>

static __inline
UID
R0_CreateThread(UID parent_pid, 
				int (*entry_point)(void *arg), 
				void *arg) {
	return Syscall3(Syscall_R0_CreateThread, entry_point, arg);
}

static __inline
void
R0_SetThreadIsPaused(UID tid,
					 bool pause) {
	Syscall2(Syscall_R0_SetThreadIsPaused, tid, pause);
}

static __inline
uint64_t
R0_ExitDeleteThread(UID tid) {
	return Syscall1(Syscall_R0_ExitDeleteThread, tid);
}

//TODO Implement the following
//	Syscall_R0_SetThreadIsPaused,       //Allow pausing/resuming threads
//  Syscall_R0_ExitDeleteThread,        //Exit and delete a thread by its TID


#endif