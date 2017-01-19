/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "syscalls_all.h"
#include "priv_syscalls.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "kmalloc.h"

static Spinlock brk_lock = NULL, map_lock = NULL;

void
MemoryInitLocks(void) {
    map_lock = CreateSpinlock();
    brk_lock = CreateSpinlock();
}

uint64_t
Brk_Syscall(void *targ_brk_address) {

    LockSpinlock(brk_lock);

    ProcessInformation *p_info;
    GetProcessReference(GetCurrentProcessUID(), &p_info);

    if(targ_brk_address == NULL) {
        uint64_t addr = 0;
        FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                               &addr,
                               PAGE_SIZE,
                               MemoryAllocationType_Heap,
                               MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

        LockSpinlock(p_info->lock);
        if(p_info->HeapBreak == 0)p_info->HeapBreak = addr;
        else addr = p_info->HeapBreak;
        UnlockSpinlock(p_info->lock);

        SyscallSetErrno(0);
        UnlockSpinlock(brk_lock);
        return addr;
    }

    uint64_t area_base = GetMemoryAllocationTypeBase(MemoryAllocationType_Heap, MemoryAllocationFlags_User);
    uint64_t area_top = GetMemoryAllocationTypeTop(MemoryAllocationType_Heap, MemoryAllocationFlags_User);

    if(area_base <= (uint64_t)targ_brk_address && area_top > (uint64_t)targ_brk_address) {
        //expand the heap by a few pages and return the new heap break

        LockSpinlock(p_info->lock);

        if((uint64_t)targ_brk_address <= p_info->HeapBreak) {

            UnlockSpinlock(p_info->lock);
            SyscallSetErrno(0);
            UnlockSpinlock(brk_lock);
            return (uint64_t)targ_brk_address;
        }

        uint64_t size = (uint64_t)targ_brk_address - p_info->HeapBreak;
        uint64_t prev_heap_break = p_info->HeapBreak;

        if(size % PAGE_SIZE != 0)size += PAGE_SIZE - (size % PAGE_SIZE);
        p_info->HeapBreak += size;

        MapPage(GetActiveVirtualMemoryInstance(),
                0,
                prev_heap_break,
                size,
                CachingModeWriteBack,
                MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation, //Don't actually map in the page until it's used
                MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

        UnlockSpinlock(p_info->lock);

        SyscallSetErrno(0);
        UnlockSpinlock(brk_lock);
        return (uint64_t)targ_brk_address;
    }

    SyscallSetErrno(-ENOMEM);
    UnlockSpinlock(brk_lock);
    return p_info->HeapBreak;
}


uint64_t
R0Map_Syscall(struct MemoryMapParams *mmap_params) {
    
    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    ProcessInformation *p_info;
    if(GetProcessReference(mmap_params->TargetPID, &p_info) != ProcessErrors_None) {
        SyscallSetErrno(-EINVAL);
        return 0;
    }

#ifdef DEBUG
    ASSERT((mmap_params->PhysicalAddress % PAGE_SIZE != 0), "Physical address needs to be PAGE_SIZE aligned.")
#else
    if(mmap_params->PhysicalAddress % PAGE_SIZE){
        SyscallSetErrno(-EINVAL);
        return 0;
    }
#endif

    LockSpinlock(map_lock);
        
    //Prevent any attempts to map into kernel space
    mmap_params->AllocationFlags |= MemoryAllocationFlags_User;

    if(mmap_params->Length % PAGE_SIZE)
        mmap_params->Length += PAGE_SIZE - mmap_params->Length % PAGE_SIZE;

    uint64_t virt_addr = mmap_params->VirtualAddress;

    if(mmap_params->VirtualAddress == 0) {
        if(FindFreeVirtualAddress(p_info->PageTable,
                                  &virt_addr,
                                  mmap_params->Length,
                                  mmap_params->AllocationType,
                                  mmap_params->AllocationFlags) != MemoryAllocationErrors_None) {
            SyscallSetErrno(-ENOMEM);
            UnlockSpinlock(map_lock);
            return 0;
        }
    }

    if(MapPage(p_info->PageTable,
               mmap_params->PhysicalAddress,
               virt_addr,
               mmap_params->Length,
               mmap_params->CacheMode,
               mmap_params->AllocationType,
               mmap_params->AllocationFlags) != MemoryAllocationErrors_None) {

        SyscallSetErrno(-ENOMEM);
        UnlockSpinlock(map_lock);
        return 0;
    } else {

        SyscallSetErrno(0);
        UnlockSpinlock(map_lock);
        return virt_addr;
    }
}

uint64_t
R0_Unmap_Syscall(UID pid,
                uint64_t addr,
                uint64_t size) {

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return -1;
    }

    ProcessInformation *p_info;
    if(GetProcessReference(pid, &p_info) != ProcessErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    if(size % PAGE_SIZE)
        size += PAGE_SIZE - size % PAGE_SIZE;

    UnmapPage(p_info->PageTable,
              addr,
              size);

    return SyscallSetErrno(0);
}

uint64_t
Unmap_Syscall(uint64_t addr,
              uint64_t size) {

    ProcessInformation *p_info;
    if(GetProcessReference(GetCurrentProcessUID(), &p_info) != ProcessErrors_None) {
        SyscallSetErrno(-EINVAL);
        return -1;
    }

    if(size % PAGE_SIZE)
        size += PAGE_SIZE - size % PAGE_SIZE;

    UnmapPage(p_info->PageTable,
              addr,
              size);

    return SyscallSetErrno(0);
}

uint64_t
R0_AllocatePages_Syscall(uint64_t page_cnt,
                        PhysicalMemoryAllocationFlags flags) {
    
    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    uint64_t retVal = AllocatePhysicalPageCont(page_cnt, flags);
    if(retVal == 0) {
        SyscallSetErrno(-ENOMEM);
    } else
        SyscallSetErrno(0);

    return retVal;
}

uint64_t
R0_FreePages_Syscall(uint64_t UNUSED(instruction_pointer),
                    uint64_t syscall_num,
                    uint64_t *syscall_params) {
    if(syscall_num != Syscall_R0_FreePages) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0) {
        SyscallSetErrno(-EPERM);
        return -1;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-ENOSYS);
        return -1;
    }

    FreePhysicalPageCont(data->params[0], data->params[1]);
    return SyscallSetErrno(0);
}

uint64_t
R01_GetPhysicalAddress_Syscall(uint64_t UNUSED(instruction_pointer),
                              uint64_t syscall_num,
                              uint64_t *syscall_params) {

    if(syscall_num != Syscall_R01_GetPhysicalAddress) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 2) {
        SyscallSetErrno(-ENOSYS);
        return 0;
    }

    uint32_t gid = GetProcessGroupID(GetCurrentProcessUID());
    if(gid > 1) {
        SyscallSetErrno(-EPERM);
        return 0;
    }

    if(gid == 1)
        data->params[0] = GetCurrentProcessUID();

    ProcessInformation *p_info = NULL;
    if(GetProcessReference(data->params[0], &p_info) != ProcessErrors_None) {
        SyscallSetErrno(-EINVAL);
        return 0;
    }

    LockSpinlock(p_info->lock);

    SyscallSetErrno(0);
    uint64_t retVal = (uint64_t)GetPhysicalAddressPageTable(p_info->PageTable, (void*)(uint64_t*)data->params[1]);

    UnlockSpinlock(p_info->lock);
    return retVal;
}
