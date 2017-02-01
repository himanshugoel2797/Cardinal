/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <cardinal/mem/server.h>
#include <cardinal/memory.h>
#include <cardinal/ipc.h>

#include "mem_db.h"

static MemoryAllocationErrors
TranslateError(uint64_t err) {
    if(err == 0)return MemoryAllocationErrors_None;
    return MemoryAllocationErrors_Unknown;
}

static void
SendErrorMessage(uint64_t addr, MemoryAllocationErrors err, UID pid, uint32_t msgID) {
    CREATE_NEW_MESSAGE_PTR(msg);
    msg->MsgID = msgID;
    msg->MsgType = CardinalMsgType_Response;

    MMapResponse *resp = (MMapResponse*)msg;
    resp->error = err;
    resp->address = addr;

    PostIPCMessage(pid, msg);
}

void
mmap_handler(Message *m) {
    uint64_t error = 0;
    MemoryAllocationErrors err;

    MMapRequest *req = (MMapRequest*)m;

    uint64_t addr = req->address;
    uint64_t sz = req->size;

    if(sz % PAGE_SIZE)
        sz += PAGE_SIZE - (sz % PAGE_SIZE);

    MemoryAllocationFlags flags = req->flags | MemoryAllocationFlags_User | MemoryAllocationFlags_Present;
    MMapFlags mmap_flags = req->mmap_flags;
    CachingMode cacheMode = req->cachingMode;

    UID dst_id = req->m.SourcePID;

    if(mmap_flags & MMapFlags_Fixed) {

        if(addr == 0 | addr % PAGE_SIZE) {
            err = MemoryAllocationErrors_InvalidVirtualAddress;
            SendErrorMessage(0, err, m->SourcePID, m->MsgID);
        }

        //Ensure the entire range is free
        for(uint64_t tmp_addr = addr; tmp_addr < addr + sz; tmp_addr += PAGE_SIZE) {
            uint64_t p_addr = 0;
            error = R01_GetPhysicalAddress(m->SourcePID, addr, &p_addr);
            if(error) {
                err = TranslateError(error);
                SendErrorMessage(0, err, m->SourcePID, m->MsgID);
            }

            if(p_addr != 0) {
                SendErrorMessage(0, MemoryAllocationErrors_AlreadyMapped, m->SourcePID, m->MsgID);
            }
        }

        //Ok, this virtual address is safe to continue with.
    }

    PhysicalMemoryAllocationFlags memdb_alloc_flags = PhysicalMemoryAllocationFlags_None;
    if(mmap_flags & MMapFlags_DMA)
        memdb_alloc_flags = PhysicalMemoryAllocationFlags_32Bit;

    uint64_t address = 0;
    error = MemDB_AllocateMemory(m->SourcePID, sz, memdb_alloc_flags, &address);

    if(error != 0) {
        err = MemoryAllocationErrors_Unknown;
        if(error == -3)
            err = MemoryAllocationErrors_OutOfMemory;

        SendErrorMessage(0, err, m->SourcePID, m->MsgID);
    }


    error = R0_Map(m->SourcePID,
                   address,
                   &addr,
                   sz,
                   cacheMode,
                   MemoryAllocationType_MMap,
                   flags
                  );

    SendErrorMessage(addr, TranslateError(error), m->SourcePID, m->MsgID);
}
