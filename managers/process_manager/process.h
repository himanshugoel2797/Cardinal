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
KillProcess(UID pid);

ProcessErrors
SleepProcess(UID pid);

ProcessErrors
SetTLSSize(UID pid,
		   uint64_t tls_size);

ProcessErrors
RegisterSignalHandler(UID 		pid,
                      ProcessSignals 	signals,
                      ProcessSignalHandler sigHandler);

void
ProcessSys_Initialize(MemoryAllocationsMap *allocMap);

#endif
