#ifndef _LIB_CARDINAL_PROCESS_H_
#define _LIB_CARDINAL_PROCESS_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"

/**
 * @brief      Exit and delete a process.
 *
 * @param[in]  pid   The pid
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t
R0_ExitDeleteProcess(UID pid) {
    Syscall1(Syscall_R0_ExitDeleteProcess, pid);       //Exit and delete a process by its PID
    return GetErrno();
}

static __inline uint64_t
R0_CreateProcess(UID *uid) {
    if(uid != NULL) {


        return GetErrno();
    }
    return -EINVAL;
}

static __inline uint64_t
R0_StartProcess(UID pid) {

}


//TODO implement the following without having to export all the kernel's structures:
//    Syscall_R0_GetProcessInfo,          //Get the kernel's process information
//    Syscall_R0_SetProcessInfo,          //Set the kernel's process information

#endif