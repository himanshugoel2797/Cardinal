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
                        const char **argv,
                        uint32_t argc) {
    ElfInformation elf_info;
    ElfLoaderError err = LoadElf(elf_loc, elf_size, ElfLimitations_64Bit | ElfLimitations_LSB, pid, &elf_info);
    if(err != ElfLoaderError_Success)
        return -1;
    UID tid = 0;
    int (*entry_point)(void*) = (int(*)(void*))elf_info.entry_point;
    if(R0_CreateThread(pid, entry_point, NULL, &tid) != 0)
        return -6;

    //Start the process
    if(R0_StartProcess(pid) != 0)
        return -7;
}