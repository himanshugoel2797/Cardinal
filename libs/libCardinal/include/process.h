#ifndef _LIB_CARDINAL_PROCESS_H_
#define _LIB_CARDINAL_PROCESS_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"

static __inline 
UID
R0_Fork(void) {
	return Syscall0(Syscall_R0_Fork);
}

static __inline
UID
R0_ExitDeleteProcess(UID pid) {
    return Syscall1(Syscall_R0_ExitDeleteProcess, pid);       //Exit and delete a process by its PID
}

//TODO implement the following without having to export all the kernel's structures:
//    Syscall_R0_GetProcessInfo,          //Get the kernel's process information
//    Syscall_R0_SetProcessInfo,          //Set the kernel's process information

#endif