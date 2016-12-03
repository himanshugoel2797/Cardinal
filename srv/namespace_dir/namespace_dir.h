#ifndef _NAMESPACE_SRV_DIR_H_
#define _NAMESPACE_SRV_DIR_H_

#include <cardinal/cardinal_types.h>


void
NamespaceDir_Initialize(void);

int
NamespaceDir_AddNamespace(char *name, UID pid);

int
NamespaceDir_GetNamespacePID(char *name, UID *pid);

int
NamespaceDir_RemoveNamespace(char *name, UID pid);

int
NamespaceDir_RemoveProcess(UID pid);

#endif