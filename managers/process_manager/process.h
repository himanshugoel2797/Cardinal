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

ProcessErrors
GetDescriptor(UID pid,
              int d_num,
              Descriptor *desc);

ProcessErrors
CreateDescriptor(UID pid,
                 Descriptor *desc,
                 int *d_num);

ProcessErrors
CloseDescriptor(UID pid,
                int d_num);

#endif
