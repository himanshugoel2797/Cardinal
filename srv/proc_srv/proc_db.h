#ifndef _PROC_SRV_DB_H_
#define _PROC_SRV_DB_H_

#include <cardinal/cardinal_types.h>

void
ProcDB_Initialize(void);

void
ProcDB_AddProcess(UID pid,
                  char *name);

void
ProcDB_RemoveProcess(UID pid);

void
ProcDB_SetCreateSubsFlag(UID pid, char val);

void
ProcDB_SetExitSubsFlag(UID pid, char val);

int
ProcDB_GetCreateSubsFlag(UID pid);

int
ProcDB_GetExitSubsFlag(UID pid);

#endif