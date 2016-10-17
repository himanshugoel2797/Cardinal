#ifndef _LIB_CARDINAL_PROCESS_H_
#define _LIB_CARDINAL_PROCESS_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"

/**
 * \defgroup proc_syscall Process Management Syscalls
 * @{
 */

/**
 * @brief      Exit and delete a process. R0 process only.
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

/**
 * @brief      Create a new process without starting. R0 process only.
 *
 * @param[in]  parent  The parent PID
 * @param[in]  userID  The user ID
 * @param      uid     The uid of the created process
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t
R0_CreateProcess(UID parent, UID userID, UID *uid) {
    if(uid != NULL) {
    	*uid = Syscall2(Syscall_R0_CreateProcess, parent, userID);
        return GetErrno();
    }
    return -EINVAL;
}

/**
 * @brief      Start a created process. R0 process only.
 *
 * @param[in]  pid   The pid of the process to start
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t
R0_StartProcess(UID pid) {
	Syscall1(Syscall_R0_StartProcess, pid);
	return GetErrno();
}

/**@}*/

//TODO implement the following without having to export all the kernel's structures:
//    Syscall_R0_GetProcessInfo,          //Get the kernel's process information
//    Syscall_R0_SetProcessInfo,          //Set the kernel's process information

#endif