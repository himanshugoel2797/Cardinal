/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "mem_db.h"
#include <list/list.h>
#include <stdlib.h>
#include <cardinal/memory.h>

static List *processes;

typedef struct {
    UID pid;
    uint64_t totalMemory_allocated;
    uint64_t memoryBudget;
    List *allocations;
} ProcessMemoryData;

typedef struct {
    uint64_t address;
    uint64_t length;
    uint32_t refCount;
} ProcessMemoryAllocation;


static void
privMemDB_FreeMemory(ProcessMemoryData *mem_data,
                     uint64_t address,
                     uint64_t len) {
    //Quick check for invalid arguments
    if(mem_data->totalMemory_allocated  < len)
        return;

    for(uint64_t j = 0; j < List_Length(mem_data->allocations); j++) {
        ProcessMemoryAllocation *alloc = List_EntryAt(mem_data->allocations, j);

        if(alloc->address <= address && alloc->address + alloc->length >= address + len) {
            if(alloc->refCount != 1 && alloc->address != address && len != alloc->length)
                return;	//Can't free partial sets
            else if(alloc->refCount != 1) {
                alloc->refCount--;	//Decrement the reference count
                return;
            }

            ProcessMemoryAllocation *top = malloc(sizeof(ProcessMemoryAllocation));
            top->address = address + len;
            top->length = (alloc->address + alloc->length) - (address + len);

            alloc->length = address - alloc->address;

            if(top->length != 0)
                List_AddEntry(mem_data->allocations,top);
            else
                free(top);

            if(alloc->length == 0) {
                List_Remove(mem_data->allocations, j);
                free(alloc);
            }

            //if(R0_FreePages(address, len / PAGE_SIZE) != 0)
            //    return;

            break;
        }
    }
}

void
MemDB_Initialize(void) {
    processes = List_Create();
}

void
MemDB_AddProcess(UID pid) {
    ProcessMemoryData *mem_data = malloc(sizeof(ProcessMemoryData));
    mem_data->pid = pid;
    mem_data->allocations = List_Create();
    mem_data->totalMemory_allocated = 0;
    mem_data->memoryBudget = GiB(1);

    List_AddEntry(processes, mem_data);
}

int
MemDB_ProcessExists(UID pid) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessMemoryData *mem_data = List_EntryAt(processes, i);
        if(mem_data->pid == pid)
            return 1;
    }
    return 0;
}

int
MemDB_AllocateMemory(UID pid,
                     uint64_t len,
                     PhysicalMemoryAllocationFlags flags,
                     uint64_t *address) {

    if(len % PAGE_SIZE)
        return -1;

    if(address == NULL)
        return -1;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessMemoryData *mem_data = List_EntryAt(processes, i);
        if(mem_data->pid == pid) {
            if(mem_data->totalMemory_allocated + len > mem_data->memoryBudget)
                return -2;

            if(R0_AllocatePages(len / PAGE_SIZE, flags, address) != 0)
                return -2;

            ProcessMemoryAllocation *alloc = malloc(sizeof(ProcessMemoryAllocation));
            alloc->address = *address;
            alloc->length = len;
            alloc->refCount = 1;

            mem_data->totalMemory_allocated += len;
            List_AddEntry(mem_data->allocations, alloc);
            return 0;
        }
    }
    return -3;
}

void
MemDB_FreeMemory(UID pid,
                 uint64_t address,
                 uint64_t len) {

    if(len % PAGE_SIZE)
        return;

    if(address % PAGE_SIZE)
        return;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessMemoryData *mem_data = List_EntryAt(processes, i);
        if(mem_data->pid == pid) {
            privMemDB_FreeMemory(mem_data, address, len);
        }
    }
    return;
}

int
MemDB_ShareMemory(UID src,
                  UID dst,
                  uint64_t address,
                  uint64_t len) {

    if(len % PAGE_SIZE)
        return -1;

    if(address % PAGE_SIZE)
        return -1;

    ProcessMemoryAllocation *sharedAlloc = NULL;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessMemoryData *mem_data = List_EntryAt(processes, i);
        if(mem_data->pid == src) {
            //Quick check for invalid arguments
            if(mem_data->totalMemory_allocated  < len)
                return -1;

            for(uint64_t j = 0; j < List_Length(mem_data->allocations); j++) {
                ProcessMemoryAllocation *alloc = List_EntryAt(mem_data->allocations, j);

                if(alloc->address == address && alloc->length == len) {
                    sharedAlloc = alloc;
                    sharedAlloc->refCount++;
                }
            }
        }
    }

    if(sharedAlloc == NULL)
        return -1;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessMemoryData *mem_data = List_EntryAt(processes, i);
        if(mem_data->pid == dst) {
            if(mem_data->totalMemory_allocated + len > mem_data->memoryBudget)
                return -2;

            mem_data->totalMemory_allocated += len;
            List_AddEntry(mem_data->allocations, sharedAlloc);
            return 0;
        }
    }

    return -1;
}

void
MemDB_FreeProcess(UID pid) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessMemoryData *mem_data = List_EntryAt(processes, i);

        if(mem_data->pid == pid) {

            uint64_t len = List_Length(mem_data->allocations);

            for(uint64_t j = 0; j < len; j++) {
                ProcessMemoryAllocation *alloc = List_EntryAt(mem_data->allocations, 0);
                privMemDB_FreeMemory(mem_data, alloc->address, alloc->length);
            }

        }
    }
}