/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "helpers.h"


int
__card_setup_shmem(uint64_t *len,
                   MemoryAllocationFlags target,
                   uint64_t *key,
                   uint64_t *vAddress) {

    if(*len % PAGE_SIZE)
        *len += PAGE_SIZE - *len % PAGE_SIZE;

    if(AllocateSharedMemory(*len,
                            CachingModeWriteBack,
                            MemoryAllocationType_MMap,
                            MemoryAllocationFlags_Write | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                            vAddress) != 0)
        return -1;

    if(GetSharedMemoryKey(*vAddress,
                          *len,
                          CachingModeWriteBack,
                          target | MemoryAllocationFlags_Read | MemoryAllocationFlags_Present,
                          key) != 0) {
        Unmap(*vAddress, *len);
        return -1;
    }

    memset((void*)*vAddress, 0, *len);

    return 0;
}