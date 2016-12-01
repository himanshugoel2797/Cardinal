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

#endif