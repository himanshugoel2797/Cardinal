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

}ProcessPriority;

typedef struct{
  UID ID;
  char Name[MAX_PROCESS_NAME_LEN];
  ProcessStatus Status;
  ProcessPriority Priority;
  UID PageTable;
  MemoryAllocationsMap *AllocationMap;
}ProcessInformation;


#endif
