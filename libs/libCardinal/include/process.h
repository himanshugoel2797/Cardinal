/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _LIB_CARDINAL_PROCESS_H_
#define _LIB_CARDINAL_PROCESS_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_property.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup proc_syscall Process Management Syscalls
 * @{
 */

/**
 * @brief      Gets the current process uid.
 *
 * @return     The current process uid.
 */
static __inline UID GetCurrentProcessUID(void) {
    UID id = 0;
    GetProperty(CardinalProperty_PID, 0, &id);
    return id;
}

/**
 * @brief      Create a new process without starting. R0 process only.
 *
 * @param[in]  parent   The parent PID
 * @param[in]  groupID  The group id
 * @param      uid      The uid of the created process
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t CreateProcess(UID parent, UID groupID, UID* uid) {
    if (uid != NULL) {
        *uid = Syscall2(Syscall_CreateProcess, parent, groupID);
        return GetErrno();
    }
    return -EINVAL;
}

/**
 * @brief      Gets the process group id.
 *
 * @param[in]  pid   The pid
 * @param      val   The value
 *
 * @return     Error code on failure, 0 on success.
 */
static __inline uint64_t GetProcessGroupID(UID pid, uint64_t* val) {
    return GetProperty(CardinalProperty_R0_GroupID, pid, val);
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif