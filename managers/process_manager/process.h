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

ProcessErrors
GetProcessInformation(UID           pid,
                      ProcessInformation    *procInfo);

ProcessErrors
GetProcessReference(UID           pid,
                    ProcessInformation    **procInfo);

ProcessErrors
SetProcessPermissions(UID pid,
                      ProcessPermissions perms);

ProcessErrors
SetProcessSyscallStatus(UID pid,
                        ProcessSyscallFlags flags);


ProcessErrors
SetProcessSigmask(UID pid,
                  const sigset_t *flags);

ProcessErrors
GetProcessSigmask(UID           pid,
                  sigset_t    *procInfo);

void
ProcessSys_Initialize(void);

bool
ProcessSys_IsInitialized(void);

#endif
