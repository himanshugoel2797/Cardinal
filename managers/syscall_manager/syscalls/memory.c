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
    __asm__("cli\n\thlt" :: "a"((size_t)data->params[1]));

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

    MemoryAllocationType allocType = MemoryAllocationType_Heap;

    if(flags & MAP_32BIT)
        allocType = MemoryAllocationType_HeapLo;

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

        MemoryAllocationsMap *alloc = kmalloc(sizeof(MemoryAllocationsMap));
        MapPage(GetActiveVirtualMemoryInstance(),
                alloc,
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