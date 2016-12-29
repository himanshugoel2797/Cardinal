#include <string.h>

#include "cardinal_types.h"
#include "ipc.h"
#include "root/server.h"
#include "namespace/server.h"

#include "helpers.h"

static uint64_t root_srv_pid = 0;

uint64_t
Mount(char *path,
      uint32_t *key,
      MountKey *shmem_key) {
    if(key == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(RootMountRequest, request);
    request->m.MsgID = RequestMessageID();
    request->m.MsgType = CardinalMsgType_Request;
    request->MsgType = RootServerMessageType_Mount;

    uint64_t path_len = strlen(path);
    uint64_t shmem_len = path_len;
    char * vAddr = NULL;

    if(__card_setup_shmem(&shmem_len, 0, &request->path_key, (uint64_t*)&vAddr) != 0)
        return -1;

    strcpy(vAddr, path);
    shmem_key->key = request->path_key;
    shmem_key->vAddr = (uint64_t)vAddr;
    shmem_key->len = shmem_len;

    Message *msg_p = (Message*)request;

    if(root_srv_pid == 0){

        uint32_t key = 0;
        uint64_t err = 0;
        RetrieveNamespace("root", &key);
        while(!IsNamespaceRetrieved(key, &root_srv_pid, &err));

        if(err != 0)
            return err;
    }

    int err = PostIPCMessages(root_srv_pid, &msg_p, 1);

    if(err != 1)
        return err;

    *key = request->m.MsgID;
    return 0;
}

uint64_t
Unmount(char *path,
        uint32_t *key,
        MountKey *shmem_key) {
    if(key == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(RootUnmountRequest, request);
    request->m.MsgID = RequestMessageID();
    request->m.MsgType = CardinalMsgType_Request;
    request->MsgType = RootServerMessageType_Unmount;
    
    uint64_t path_len = strlen(path);
    uint64_t shmem_len = path_len;
    char * vAddr = NULL;

    if(__card_setup_shmem(&shmem_len, 0, &request->path_key, (uint64_t*)&vAddr) != 0)
        return -1;

    strcpy(vAddr, path);
    shmem_key->key = request->path_key;
    shmem_key->vAddr = (uint64_t)vAddr;
    shmem_key->len = shmem_len;

    Message *msg_p = (Message*)request;

    if(root_srv_pid == 0){

        uint32_t key = 0;
        uint64_t err = 0;
        RetrieveNamespace("root", &key);
        while(!IsNamespaceRetrieved(key, &root_srv_pid, &err));

        if(err != 0)
            return err;
    }

    int err = PostIPCMessages(root_srv_pid, &msg_p, 1);

    if(err != 1)
        return err;

    *key = request->m.MsgID;
    return 0;
}

int
IsRootRequestReady(uint32_t key,
                   MountKey *shmem_key,
                   uint64_t *error) {
    if(error == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR(msg_buf);
    RootMountResponse *resp = (RootMountResponse*)msg_buf;

    //No need to attempt to resolve the root server pid here, no need to expect a response if we haven't talked to it.
    if(root_srv_pid == 0)
        return -1;

    int err = GetIPCMessageFrom((Message*)resp, root_srv_pid, key);

    if(err == 1) {
        *error = resp->result;
        FreeSharedMemoryKey(shmem_key->key);
        Unmap(shmem_key->vAddr, shmem_key->len);
    }

    return (err == 1);
}