#ifndef _CARDINAL_NAMESPACE_DB_H_
#define _CARDINAL_NAMESPACE_DB_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>

typedef struct {
    Message m;
    char namespace_name[1];
} NamespaceRegistrationRequest;

#endif