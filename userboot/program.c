#include "program.h"
#include <cardinal/memory.h>
#include <cardinal/process.h>
#include <cardinal/thread.h>
#include <string.h>
#include "elf.h"

//NOTE: Map userboot.bin into each process, elf loader just sets

int
LoadAndStartApplication(UID pid,
                        void *elf_loc,
                        uint64_t elf_size,
                        const char **argv,
                        uint32_t argc,
                        const char **envp) {
    ElfInformation elf_info;
    ElfLoaderError err = LoadElf(elf_loc, elf_size, ElfLimitations_64Bit | ElfLimitations_LSB, pid, &elf_info);
    if(err != ElfLoaderError_Success)
        return -1;

    // The following code is set to a thread entry point and stored in a
    // separate section. userboot.bin copies itself into the target process.
    // Calculates the entry point to the auxv setup routine. Creates a thread in
    // the process that will run the auxv setup on a newly allocated stack. Each
    // process is thus started as Group 0, and its permission level is lowered
    // correctly to its parent's level by the loader. Once the stack is setup,
    // the loader cleans itself up, increases the group level to the parent
    // process, swaps stacks and jumps to the actual elf entry point.

    uint32_t auxv_cnt = 0;
    AUXVector auxv[7];
    auxv[auxv_cnt].a_type = AUXVectorType_PGSZ;
    auxv[auxv_cnt++].a_val = PAGE_SIZE;

    auxv[auxv_cnt].a_type = AUXVectorType_UID;
    auxv[auxv_cnt++].a_val = (int64_t)tid;

    auxv[auxv_cnt].a_type = AUXVectorType_EUID;
    auxv[auxv_cnt++].a_val = (int64_t)tid;

    auxv[auxv_cnt].a_type = AUXVectorType_GID;
    auxv[auxv_cnt++].a_val = (int64_t)pid;

    auxv[auxv_cnt].a_type = AUXVectorType_EGID;
    auxv[auxv_cnt++].a_val = (int64_t)pid;

    auxv[auxv_cnt].a_type = AUXVectorType_RANDOM;
    auxv[auxv_cnt++].a_val = (int64_t)elf_info.entry_point;

    auxv[auxv_cnt].a_type = AUXVectorType_HWCAP;
    auxv[auxv_cnt++].a_val = (int64_t)0xBFEBFBFF;

    void* sp = SetupApplicationStack(pid, R0_GetThreadUserStack(GetCurrentThreadUID()), argc, argv, envp, auxv, auxv_cnt, &elf_info);

    //Get the user mode stack pointer for the process, call SetupApplicationStack on it
    //Start the process
}

void*
SetupApplicationStack(UID pid,
                      void *sp,
                      uint32_t argc,
                      const char **argv,
                      const char **envp,
                      const AUXVector *aux_vectors,
                      uint32_t auxv_cnt,
                      ElfInformation *elf) {

    uint64_t *stack = (uint64_t*)sp;

    //calculate needed space
    uint64_t size = 0;
    if(envp)for(; envp[size]; size++);
    size += argc + 2 + ((auxv_cnt + 5) * sizeof(AUXVector))/sizeof(uint64_t);

    stack -= size;

    //Copy the argument array and environment variables onto the stack first, then update the pointers

    uint64_t phys_addr = 0;
    if(R0_AllocatePages(1, &phys_addr) != 0) {
        return NULL;
    }

    uint64_t v_tmp_addr = 0;
    R0_Map(GetCurrentProcessUID(),
           phys_addr,
           &v_tmp_addr,
           PAGE_SIZE,
           CachingModeWriteBack,
           MemoryAllocationType_MMap,
           MemoryAllocationFlags_Write | MemoryAllocationFlags_User
          );


    R0_Map(pid,
           phys_addr,
           &v_tmp_addr,
           PAGE_SIZE,
           CachingModeWriteBack,
           MemoryAllocationType_MMap,
           MemoryAllocationFlags_Write | MemoryAllocationFlags_User
          );

    uint8_t *args = (uint8_t*)v_tmp_addr;
    uint64_t off = 0;

    for(uint32_t i = 0; i < argc; i++) {
        stack[i + 1] = (uint64_t)&args[off];
        strcpy((char*)&args[off], argv[i]);
        off += strlen(argv[i]);
    }

    stack[0] = argc;

    uint32_t offset = argc + 1;
    stack[offset++] = 0;

    if(envp != NULL)
        for(uint32_t i = 0; envp[i] != NULL; i++)
            stack[offset++] = (uint64_t)envp[i];

    stack[offset++] = 0;

    AUXVector *auxv = (AUXVector*)&stack[offset];

    for(uint32_t i = 0; i < auxv_cnt; i++) {
        auxv->a_type = aux_vectors[i].a_type;
        auxv->a_val = aux_vectors[i].a_val;

        auxv++;
    }

    memcpy(&args[off], elf->phdr_data, elf->phdr_num * elf->phdr_ent_size);

    auxv->a_type = AUXVectorType_PHDR;
    auxv->a_val = (int64_t)&args[off];
    auxv++;

    auxv->a_type = AUXVectorType_PHNUM;
    auxv->a_val = (int64_t)elf->phdr_num;
    auxv++;

    auxv->a_type = AUXVectorType_PHENT;
    auxv->a_val = (int64_t)elf->phdr_ent_size;
    auxv++;

    auxv->a_type = AUXVectorType_ENTRY;
    auxv->a_val = (int64_t)elf->entry_point;
    auxv++;

    auxv->a_type = AUXVectorType_NULL;
    auxv->a_val = 0;

    R0_Unmap(GetCurrentProcessUID(),
             v_tmp_addr,
             PAGE_SIZE);

    //__asm__("cli\n\thlt" :: "a"(&stack[0]));
    return (void*)stack;
}