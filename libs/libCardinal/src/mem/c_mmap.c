/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "cardinal_types.h"
#include "ipc.h"
#include "mem/server.h"

#include <string.h>



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