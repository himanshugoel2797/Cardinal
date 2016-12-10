#ifndef _CARDINAL_NAMESPACE_DIR_SERVER_H_
#define _CARDINAL_NAMESPACE_DIR_SERVER_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>

#define NAMESPACE_NAME_LEN 64

typedef enum {
    NamespaceServerMessageType_Registration,
    NamespaceServerMessageType_Removal,
    NamespaceServerMessageType_Retrieval
} NamespaceServerMessageType;

typedef struct {
    Message m;
    NamespaceServerMessageType MsgType;
    char namespace_name[NAMESPACE_NAME_LEN];
} NamespaceRegistrationRequest;

typedef struct {
    Message m;
    uint64_t result;
    UID pid;
} NamespaceRegistrationResponse;

typedef NamespaceRegistrationRequest NamespaceRemovalRequest;
typedef NamespaceRegistrationResponse NamespaceRemovalResponse;

typedef NamespaceRegistrationRequest NamespaceRetrievalRequest;
typedef NamespaceRegistrationResponse NamespaceRetrievalResponse;

int
RegisterNamespace(char *name,
                  uint32_t *key);

int
UnregisterNamespace(char *name,
                    uint32_t *key);

int
RetrieveNamespace(char *name,
                  uint32_t *key);

int
IsNamespaceRetrieved(uint32_t key, UID *pid, uint64_t *error);

int
IsNamespaceRequestReady(uint32_t key, uint64_t *error);

#endif