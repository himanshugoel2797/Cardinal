/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "libs/libCardinal/include/syscall.h"
#include "types.h"
#include "thread.h"
#include "memory.h"

#include "libs/libc/include/asm/prctl.h"

uint64_t
ArchPrctl_Syscall(uint64_t target,
                  uint64_t val) {


    if(target == ARCH_SET_GS) SetBG_GSBase((void*)val);
    else if(target == ARCH_SET_FS) SetFSBase((void*)val);
    else if(target == ARCH_GET_GS) {
        MemoryAllocationFlags flags = 0;
        GetAddressPermissions(GetActiveVirtualMemoryInstance(), val, NULL, &flags, NULL);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User | MemoryAllocationFlags_Present))
            return -EINVAL;

        uint64_t* gs_base_target = (uint64_t*)val;
        *gs_base_target = (uint64_t)GetBG_GSBase();
    } else if(target == ARCH_GET_FS) {
        MemoryAllocationFlags flags = 0;
        GetAddressPermissions(GetActiveVirtualMemoryInstance(), val, NULL, &flags, NULL);

        if(flags != (MemoryAllocationFlags_Read | MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_User | MemoryAllocationFlags_Present))
            return -EINVAL;


        uint64_t* fs_base_target = (uint64_t*)val;
        *fs_base_target = (uint64_t)GetFSBase();
    }

    return 0;
}
