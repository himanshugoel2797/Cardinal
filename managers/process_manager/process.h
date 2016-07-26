#ifndef _CARDINAL_PROCESS_H_
#define _CARDINAL_PROCESS_H_

#include "types.h"
#include "memory.h"
#include "managers.h"
#include "list.h"
#include "process_info.h"


ProcessErrors
ForkProcess(ProcessInformation *src,
            ProcessInformation **dst);

UID
ForkCurrentProcess(void);

ProcessErrors
GetProcessInformation(UID           pid,
                      ProcessInformation    *procInfo);

ProcessErrors
GetProcessReference(UID           pid,
                    ProcessInformation    **procInfo);

char*
GetCurrentProcessWorkingDirectory(void);

ProcessErrors
SetProcessPermissions(UID pid,
                      ProcessPermissions perms);

ProcessErrors
SetProcessSyscallStatus(UID pid,
                        ProcessSyscallFlags flags);

ProcessErrors
SetProcessSigaction(UID pid,
                    int sig_no,
                    const struct sigaction *sig);

ProcessErrors
GetProcessSigaction(UID pid,
                    int sig_no,
                    struct sigaction *sig);

void
RaiseSignal(UID pid,
            int sig_no);

bool
PostMessage(Message *msg);

bool
GetMessage(Message *msg);

bool
GetMessageFrom(Message *msg,
               UID SourcePID);

void
ProcessSys_Initialize(void);

bool
ProcessSys_IsInitialized(void);

#endif
