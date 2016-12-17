#include "program.h"
#include <cardinal/memory.h>
#include <cardinal/process.h>
#include <cardinal/thread.h>
#include <string.h>
#include "elf.h"

int
LoadAndStartApplication(UID pid,
                        void *elf_loc,
                        uint64_t elf_size,
                        const char *argv,
                        uint32_t argc,
                        uint64_t arg_len) {
    ElfInformation elf_info;
    ElfLoaderError err = LoadElf(elf_loc, elf_size, ElfLimitations_64Bit | ElfLimitations_LSB, pid, &elf_info);
    if(err != ElfLoaderError_Success)
        return -1;

    //Put argv, argc and elf info into at most 4 pages, take the physical address, unmap the page
    //Map the page to the same virtual address in the target process as read/write
    //Pass the pointer as a parameter to the thread

    uint64_t net_size = argc * sizeof(char*) + arg_len + sizeof(struct ElfSetupParameters);

    if(net_size % PAGE_SIZE)
        net_size += (PAGE_SIZE - (net_size % PAGE_SIZE));

    uint64_t phys_addr = 0;
    struct ElfSetupParameters *params = NULL;//MAP

    if(R0_AllocatePages(net_size / PAGE_SIZE, PhysicalMemoryAllocationFlags_None, &phys_addr) != 0)
        return -3;

    //Map the memory into the current process to allow editing
    if(R0_Map(GetCurrentProcessUID(),
              phys_addr,
              (uint64_t*)&params,
              net_size,
              CachingModeWriteBack,
              MemoryAllocationType_Application |
              MemoryAllocationType_Phys,
              MemoryAllocationFlags_NoExec |
              MemoryAllocationFlags_Read |
              MemoryAllocationFlags_Write |
              MemoryAllocationFlags_User |
              MemoryAllocationFlags_Present))
        return -4;

    //Map the memory into the target process
    uint64_t target_virt_addr = 0;
    if(R0_Map(pid,
              phys_addr,
              &target_virt_addr,
              net_size,
              CachingModeWriteBack,
              MemoryAllocationType_MMap,
              MemoryAllocationFlags_NoExec |
              MemoryAllocationFlags_Read |
              MemoryAllocationFlags_Write |
              MemoryAllocationFlags_User |
              MemoryAllocationFlags_Present))
        return -5;

    params->ver = 1;
    params->size = net_size;
    params->page_size = PAGE_SIZE;
    params->argc = argc;
    params->rnd_seed = 0;         //Set the seed to 0 for now, secondary loader will set it properly
    params->elf_entry_point = (uint64_t)elf_info.entry_point;

    uint8_t **argv_base = (uint8_t**)params->argv;
    uint8_t *arg_loc = (uint8_t*)argv_base + sizeof(char*) * argc;

    for(uint32_t i = 0; i < argc; i++) {
        argv_base[i] = (char*)(target_virt_addr + ((uint64_t)arg_loc - (uint64_t)params));

        uint32_t len = strlen(argv) + 1;
        strcpy(arg_loc, argv);
        argv += len;
        arg_loc += len;
    }

    R0_Unmap(GetCurrentProcessUID(), (uint64_t)params, net_size);

    // We're no longer following the SysV Linux ABI, so just map in the data and
    // pass the virtual address.


    UID tid = 0;
    int (*entry_point)(void*) = (int(*)(void*))elf_info.entry_point;
    if(R0_CreateThread(pid, entry_point, (void*)target_virt_addr, &tid) != 0)
        return -6;

    //Start the process
    if(R0_StartProcess(pid) != 0)
        return -7;

    return 0;
}