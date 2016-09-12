#ifndef _CARDINAL_PROCESS_H_
#define _CARDINAL_PROCESS_H_

#include "types.h"
#include "memory.h"
#include "managers.h"
#include "list.h"
#include "process_info.h"
#include "libs/libCardinal/include/ipc.h"


#define MAX_PENDING_MESSAGE_CNT 256

ProcessErrors
ForkProcess(ProcessInformation *src,
            ProcessInformation **dst);

void
TerminateProcess(UID pid, uint32_t exit_code);

uint32_t
ReapProcess(UID pid);

UID
ForkCurrentProcess(void);

ProcessErrors
GetProcessInformation(UID           pid,
                      ProcessInformation    *procInfo);

ProcessErrors
GetProcessReference(UID           pid,
                    ProcessInformation    **procInfo);

void*
GetProcessLocalStorage(UID pid,
					   size_t minSize);

char*
GetCurrentProcessWorkingDirectory(void);

ProcessErrors
SetProcessPermissions(UID pid,
                      ProcessPermissions perms);

ProcessErrors
SetProcessSyscallStatus(UID pid,
                        ProcessSyscallFlags flags);

uint64_t
PostMessages(Message **msg, uint64_t msg_cnt);

bool
GetMessageFrom(Message *msg,
               UID SourcePID,
               uint64_t msg_id);

bool
SetSpecialDestinationPID(UID specialID);

void
ProcessSys_Initialize(void);

bool
ProcessSys_IsInitialized(void);

#endif
