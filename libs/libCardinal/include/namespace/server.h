/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_NAMESPACE_DIR_SERVER_H_
#define _CARDINAL_NAMESPACE_DIR_SERVER_H_

#include "../cardinal_types.h"
#include "../ipc.h"

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