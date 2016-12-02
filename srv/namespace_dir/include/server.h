#ifndef _CARDINAL_NAMESPACE_DIR_SERVER_H_
#define _CARDINAL_NAMESPACE_DIR_SERVER_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>

typedef enum {
    NamespaceDirectoryMessageType_Registration,
    NamespaceDirectoryMessageType_Removal,
    NamespaceDirectoryMessageType_Retrieval
} NamespaceDirectoryMessageType;

typedef struct {
    Message m;
    uint32_t MsgType;
    char namespace_name[64];
} NamespaceRegistrationRequest;

typedef struct {
    Message m;
    uint64_t result;
} NamespaceRegistrationResponse;

typedef NamespaceRegistrationRequest NamespaceRemovalRequest;
typedef NamespaceRegistrationResponse NamespaceRemovalResponse;

typedef NamespaceRegistrationRequest NamespaceRetrievalRequest;
typedef NamespaceRegistrationResponse NamespaceRetrievalResponse;


#endif