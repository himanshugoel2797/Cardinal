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
R0_KillProcess(UID pid) {
    Syscall1(Syscall_R0_KillProcess, pid);       //Exit and delete a process by its PID
    return GetErrno();
}

/**
 * @brief      Exit the executing process. R0 process only.
 *
 * @param[in]  exit_code  The exit code
 *
 * @return     Error code on failure, does not return on success.
 */
static __inline uint64_t
R0_ExitProcess(uint64_t exit_code) {
    SetProperty(CardinalProperty_R0_Exit, 0, exit_code);
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

/**
 * @brief      Gets the current process uid.
 *
 * @return     The current process uid.
 */
static __inline UID
GetCurrentProcessUID(void) {
    UID id = 0;
    GetProperty(CardinalProperty_PID, 0, &id);
    return id;
}

/**
 * @brief      Gets the process group id.
 *
 * @param[in]  pid   The pid
 * @param      val   The value
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t
GetProcessGroupID(UID pid, uint64_t* val) {
    return GetProperty(CardinalProperty_R0_GroupID, pid, val);
}

/**@}*/

//TODO implement the following without having to export all the kernel's structures:
//    Syscall_R0_GetProcessInfo,          //Get the kernel's process information
//    Syscall_R0_SetProcessInfo,          //Set the kernel's process information

#endif