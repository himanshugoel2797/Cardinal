#include <cardinal/cardinal_types.h>
#include <cardinal/ipc.h>
#include <libipc/ipc.h>
#include <string.h>

#include "server.h"


MemoryAllocationErrors
MMap(uint64_t *address,
     uint64_t size,
     MemoryAllocationFlags flags,
     MMapFlags m_flags,
     CachingMode cacheMode) {

    if(address == NULL)
        return MemoryAllocationErrors_InvalidVirtualAddress;

    uint32_t key = 0;
    int err = RequestMMap(*address, size, flags, m_flags, cacheMode, &key);
    if(err != 0)
        return err;

    MemoryAllocationErrors error = 0;

    while(!IsMMapped(key, &error, address));

    return error;
}

int
RequestMMap(uint64_t address,
            uint64_t size,
            MemoryAllocationFlags flags,
            MMapFlags m_flags,
            CachingMode cacheMode,
            uint32_t *key) {
    if(key == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR_TYPE(MMapRequest, request);
    request->m.MsgID = RequestMessageID();
    request->m.MsgType = CardinalMsgType_Request;
    request->MsgType = MemoryServerMessageType_MMapRequest;
    request->address = address;
    request->size = size;
    request->flags = flags;
    request->mmap_flags = m_flags;
    request->cachingMode = cacheMode;

    Message *msg_p = (Message*)request;

    int err = PostIPCMessages(MEMORY_SRV_PID, &msg_p, 1);

    if(err != 1)
        return err;

    *key = request->m.MsgID;
    return 0;
}

int
IsMMapped(uint32_t id,
          MemoryAllocationErrors *error,
          uint64_t *address) {
    if(address == NULL | error == NULL)
        return -1;

    CREATE_NEW_MESSAGE_PTR(msg_buf);
    MMapResponse *mmap_resp = (MMapResponse*)msg_buf;

    int err = GetIPCMessageFrom((Message*)mmap_resp, MEMORY_SRV_PID, id);

    if(err == 1) {
        *error = mmap_resp->error;
        *address = mmap_resp->address;
    }

    return (err == 1);
}