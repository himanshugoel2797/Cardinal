#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "memory.h"
#include "kmalloc.h"

#include <sys/mman.h>

uint64_t
MMap_Syscall(uint64_t UNUSED(instruction_pointer),
             uint64_t syscall_num,
             uint64_t *syscall_params) {
    if(syscall_num != Syscall_MMap)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 6)
        return ENOSYS;

    uint64_t addr = data->params[0];
    uint64_t target_len = data->params[1];
    uint64_t prot = data->params[2];
    uint64_t flags = data->params[3];
    //int64_t fd = data->params[4];
    //uint64_t offset = data->params[5];

    //TODO make this work once files are actually implemented
    if((flags & MAP_ANONYMOUS) == 0)
        __asm__ volatile("cli\n\thlt");

    if(flags & MAP_FIXED)
        __asm__ volatile("cli\n\thlt");


    MemoryAllocationFlags allocFlags = MemoryAllocationFlags_User;

    if(prot & PROT_EXEC)
        allocFlags |= MemoryAllocationFlags_Exec;

    if(prot & PROT_READ)
        allocFlags |= MemoryAllocationFlags_Read;

    if(prot & PROT_WRITE)
        allocFlags |= MemoryAllocationFlags_Write;

    MemoryAllocationType allocType = MemoryAllocationType_MMap;

    if(flags & MAP_32BIT)
        allocType = MemoryAllocationType_MMapLo;

    //Ignore the hint provided by the application
    FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                           &addr,
                           target_len,
                           allocType,
                           allocFlags);

    //TODO make the flags work once files are actually implemented

    if(flags & MAP_ANONYMOUS) {
        if(target_len % PAGE_SIZE != 0)
            target_len += PAGE_SIZE - (target_len % PAGE_SIZE);

        uint64_t target_phys_addr = AllocatePhysicalPageCont(target_len / PAGE_SIZE);
        if(target_phys_addr == 0)
            return ENOMEM;

        MapPage(GetActiveVirtualMemoryInstance(),
                target_phys_addr,
                (uint64_t)addr,
                target_len,
                CachingModeWriteBack,
                allocType,
                allocFlags);



        return (uint64_t)addr;
    }

    return EINVAL;
}

uint64_t
Brk_Syscall(uint64_t UNUSED(instruction_pointer),
            uint64_t syscall_num,
            uint64_t *syscall_params) {
    if(syscall_num != Syscall_Brk)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    ProcessInformation *p_info;
    GetProcessReference(GetCurrentProcessUID(), &p_info);

    if(data->param_num != 1)
        return ENOSYS;

    if(data->params[0] == 0) {
        uint64_t addr = 0;
        FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                               &addr,
                               PAGE_SIZE,
                               MemoryAllocationType_Heap,
                               MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

        LockSpinlock(p_info->lock);
        if(p_info->HeapBreak == 0)p_info->HeapBreak = addr;
        UnlockSpinlock(p_info->lock);
        return addr;
    }

    uint64_t area_base = GetMemoryAllocationTypeBase(MemoryAllocationType_Heap, MemoryAllocationFlags_User);
    uint64_t area_top = GetMemoryAllocationTypeTop(MemoryAllocationType_Heap, MemoryAllocationFlags_User);

    if(area_base <= data->params[0] && area_top > data->params[0]) {
        //TODO expand the heap by a few pages and return the new heap break

        LockSpinlock(p_info->lock);
        uint64_t size = data->params[0] - p_info->HeapBreak;
        uint64_t prev_heap_break = p_info->HeapBreak;
        p_info->HeapBreak += size;
        UnlockSpinlock(p_info->lock);

        if(size % PAGE_SIZE != 0)size += PAGE_SIZE - (size % PAGE_SIZE);

        MapPage(GetActiveVirtualMemoryInstance(),
                prev_heap_break,
                AllocatePhysicalPageCont(size/PAGE_SIZE),
                size,
                CachingModeWriteBack,
                MemoryAllocationType_Heap,
                MemoryAllocationFlags_Write | MemoryAllocationFlags_User); 

        return prev_heap_break + size;
    }

    return ENOMEM;
}