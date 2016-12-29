#ifndef _ROOT_SRV_DIR_H_
#define _ROOT_SRV_DIR_H_

#include <cardinal/cardinal_types.h>


void
RootDir_Initialize(void);

int
RootDir_AddMount(char *name, UID pid);

int
RootDir_GetMountPID(char *name, UID *pid);

int
RootDir_RemoveMount(char *name, UID pid);

int
RootDir_RemoveProcess(UID pid);

#endif