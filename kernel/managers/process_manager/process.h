/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_PROCESS_H_
#define _CARDINAL_PROCESS_H_

#include "types.h"
#include "memory.h"
#include "managers.h"
#include "list.h"
#include "process_info.h"
#include "libs/libCardinal/include/ipc.h"
#include "libs/libCardinal/include/signal.h"

/**
 * \defgroup proc_man Process Management
 * @{
 */

/**
 * @brief      Creates a process.
 *
 * @param[in]  parent    The parent PID
 * @param[in]  group_id  The group identifier
 * @param[out] pid       The PID
 *
 * @return     Error code on failure, ProcessErrors_None on success.
 */
ProcessErrors
CreateProcess(UID parent, UID group_id, UID *pid);

/**
 * @brief      Starts a process.
 *
 * @param[in]  pid   The pid
 *
 * @return     Error code on failure, ProcessErrors_None on success.
 */
ProcessErrors
StartProcess(UID pid);

/**
 * @brief      Terminate the specified process.
 *
 * @param[in]  pid        The pid
 * @param[in]  exit_code  The exit code
 *
 * @return     Error code on failure, ProcessErrors_None on success.
 */
ProcessErrors
TerminateProcess(ProcessInformation *pInfo);

/**
 * @brief      Gets the process information.
 *
 * @param[in]  pid       The pid
 * @param      procInfo  The proc information
 *
 * @return     The process information.
 */
ProcessErrors
GetProcessInformation(UID           pid,
                      ProcessInformation    *procInfo);

/**
 * @brief      Gets the process reference.
 *
 * @param[in]  pid       The pid
 * @param      procInfo  The proc information
 *
 * @return     The process reference.
 */
ProcessErrors
GetProcessReference(UID           pid,
                    ProcessInformation    **procInfo);

/**
 * @brief      Posts messages.
 *
 * @param[in]  dstPID   The destination pid
 * @param      msg      The message
 *
 * @return     < 0 on error, TRUE on success.
 */
uint64_t
PostMessage(UID dstPID, Message *msg);

/**
 * @brief      Gets the ipc message from the source.
 *
 * @param      msg        A preallocated message buffer of size MAX_MESSAGE_SIZE
 * @param[in]  SourcePID  The source pid (0 for any)
 * @param[in]  msg_id     The message identifier (0 for any)
 *
 * @return     0 if no message to return, 1 if a message has been returned.
 */
bool
GetMessageFrom(Message *msg,
               UID SourcePID,
               uint32_t msg_id);

/**
 * @brief      Gets the ipc message based on the type.
 *
 * @param      msg      The message
 * @param[in]  msgType  The message type
 *
 * @return     The message from type.
 */
bool
GetMessageFromType(Message *msg,
                   CardinalMsgType msgType);

/**
 * @brief      Initialize the process system.
 */
void
ProcessSys_Initialize(void);

/**
 * @brief      Check if the process system has been initialized.
 *
 * @return     TRUE if the system has been initialized, else FALSE.
 */
bool
ProcessSys_IsInitialized(void);

/**
 * @brief      Gets the process group id.
 *
 * @param[in]  pid   The pid
 *
 * @return     The process group id.
 */
uint64_t
GetProcessGroupID(UID pid);

/**
 * @brief      Sets the process group id.
 *
 * @param[in]  pid   The pid
 * @param[in]  id    The identifier
 *
 * @return     The process group id.
 */
uint64_t
SetProcessGroupID(UID pid, uint64_t id);

/**
 * @brief      Schedule the process for termination.
 *
 * @param[in]  pid        The pid
 * @param[in]  exit_code  The exit code
 *
 * @return     Error code on failure, ProcessErrors_None on success.
 */
uint64_t
ScheduleProcessForTermination(UID pid, uint32_t exit_code);

/**
 * @brief      Check if any of the process's threads can be woken.
 *
 * @param[in]  pid   The pid
 */
void
ProcessCheckWakeThreads(UID pid);

/**@}*/

#endif
