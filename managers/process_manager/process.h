#ifndef _CARDINAL_PROCESS_H_
#define _CARDINAL_PROCESS_H_

#include "types.h"
#include "memory.h"

#define MAX_PROCESS_NAME_LEN (256)

typedef enum{
  ProcessStatus_Stopped,
  ProcessStatus_Sleeping,
  ProcessStatus_Terminating,
  ProcessStatus_Executing
}ProcessStatus;

typedef enum{
  ProcessPriority_Normal,
  ProcessPriority_High
}ProcessPriority;

typedef enum{
  ProcessPermissions_None = 0,
  ProcessPermissions_NormalFileIO = (1 << 0),
  ProcessPermissions_RootFileIO = (1 << 1),
  ProcessPermissions_Kernel = (1 << 2),
  ProcessPermissions_User = (1 << 3),
  ProcessPermissions_SuperUser = (1 << 4),
  ProcessPermissions_Network = (1 << 5)
}ProcessPermissions;

typedef struct ProcessInformation{
  UID 				ID;
  char 				Name[MAX_PROCESS_NAME_LEN];
  ProcessStatus 		Status;
  ProcessPriority 		Priority;
  ProcessPermissions 		Permissions;
  UID 				PageTable;
  MemoryAllocationsMap 		*AllocationMap;
  struct ProcessInformation 	**children;
}ProcessInformation;

typedef enum{
  ProcessErrors_None = 0,
  ProcessErrors_Unknown = (1 << 0)
}ProcessErrors;

typedef enum{
  ProcessSignals_SIGABRT = (1 << 0),
  ProcessSignals_SIGALRM = (1 << 1),
  ProcessSignals_SIGVTALRM = (1 << 2),
  ProcessSignals_SIGPROF = (1 << 3),
  ProcessSignals_SIGBUS = (1 << 4),
  ProcessSignals_SIGCHLD = (1 << 5),
  ProcessSignals_SIGCONT = (1 << 6),
  ProcessSignals_SIGFPE = (1 << 7),
  ProcessSignals_SIGHUP = (1 << 8),
  ProcessSignals_SIGILL = (1 << 9),
  ProcessSignals_SIGINT = (1 << 10),
  ProcessSignals_SIGKILL = (1 << 11),
  ProcessSignals_SIGPIPE = (1 << 12),
  ProcessSignals_SIGPOLL = (1 << 13),
  ProcessSignals_SIGQUIT = (1 << 14),
  ProcessSignals_SIGSEGV = (1 << 15),
  ProcessSignals_SIGSTOP = (1 << 16),
  ProcessSignals_SIGSYS = (1 << 17),
  ProcessSignals_SIGTERM = (1 << 18),
  ProcessSignals_SIGTRAP = (1 << 19),
  ProcessSignals_SIGXFSZ = (1 << 20),
  ProcessSignals_SIGWINCH = (1 << 21)
}ProcessSignals;

ProcessErrors
ForkProcess(ProcessInformation *src,
	    ProcessInformation *dst);

ProcessErrors
GetProcessInformation(UID 			pid,
		      ProcessInformation	*procInfo);

ProcessErrors
KillProcess(UID pid);

ProcessErrors
SleepProcess(UID pid);

ProcessErrors
RegisterSignalHandler(UID 		pid,
		      ProcessSignals 	signals,
		      void 		(*sigHandler)(ProcessSignals));

#endif
