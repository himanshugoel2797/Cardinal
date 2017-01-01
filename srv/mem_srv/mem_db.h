/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _MEM_SRV_DB_H_
#define _MEM_SRV_DB_H_

#include <cardinal/cardinal_types.h>
#include <cardinal/memory.h>

void
MemDB_Initialize(void);

void
MemDB_AddProcess(UID pid);

int
MemDB_ProcessExists(UID pid);

int
MemDB_AllocateMemory(UID pid,
                     uint64_t len,
                     PhysicalMemoryAllocationFlags flags,
                     uint64_t *address);

void
MemDB_FreeMemory(UID pid,
                 uint64_t address,
                 uint64_t len);

int
MemDB_ShareMemory(UID src,
                  UID dst,
                  uint64_t address,
                  uint64_t len);

void
MemDB_FreeProcess(UID pid);

#endif