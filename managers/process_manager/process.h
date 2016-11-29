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
 * @param[in]  parent  The parent PID
 * @param[in]  userID  The user ID
 * @param[out] pid     The PID
 *
 * @return     Error code on failure, ProcessErrors_None on success.
 */
ProcessErrors
CreateProcess(UID parent, UID userID, UID *pid);

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
TerminateProcess(UID pid);

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
 * @param[in]  msg_cnt  The message count
 *
 * @return     If 'i' is the number of messages sent, -(i + 1) on error, i on
 *             success.
 */
uint64_t
PostMessages(UID dstPID, Message **msg, uint64_t msg_cnt);

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
 * @brief      Sets the special destination pid.
 *
 * @param[in]  specialID  The special id
 *
 * @return     TRUE if the set operation succeeded, FALSE otherwise.
 */
bool
SetSpecialDestinationPID(UID specialID);

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
 * @brief      Creates a response buffer key.
 *
 * @param[in]  pid     The pid
 * @param[in]  offset  The offset
 * @param[in]  length  The length
 *
 * @return     0 on failure, key on success.
 */
uint64_t
CreateResponseBufferKey(UID pid,
                        uint32_t offset,
                        uint32_t length);

/**
 * @brief      Submit to a response buffer.
 *
 * @param[in]  key     The key
 * @param      buffer  The buffer
 *
 * @return     Error code on failure, ProcessErrors_None on success.
 */
ProcessErrors
SubmitToResponseBuffer(uint64_t key,
                       void *buffer,
                       uint32_t buf_len);


/**
 * @brief      Queries a response key length.
 *
 * @param[in]  key     The key
 * @param      length  The length
 *
 * @return     Error code on failure, ProcessErrors_None on success.
 */
ProcessErrors
QueryResponseKeyLength(uint64_t key,
                       uint32_t *length);

/**@}*/

#endif
