#include "program.h"
#include "common.h"
#include "memory.h"
#include "kmalloc.h"

void*
SetupApplicationStack(void *sp,
                      uint32_t argc,
                      const char **argv,
                      const char **envp,
                      const AUXVector *aux_vectors,
                      uint32_t auxv_cnt) {

    uint64_t *stack = (uint64_t*)sp;

    //calculate needed space
    uint64_t size = 0;
    if(envp)for(;envp[size]; size++);
    size += argc + 1 + (auxv_cnt * sizeof(AUXVector))/sizeof(uint64_t);

    stack -= size;

    //Copy the argument array and environment variables onto the stack first, then update the pointers
    uint64_t v_tmp_addr = 0;
    FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                           &v_tmp_addr,
                           PAGE_SIZE,
                           MemoryAllocationType_Heap,
                           MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

    MemoryAllocationsMap *alloc = kmalloc(sizeof(MemoryAllocationsMap));
    uint64_t phys_addr = AllocatePhysicalPage();

    MapPage(GetActiveVirtualMemoryInstance(),
            alloc,
            phys_addr,
            v_tmp_addr,
            PAGE_SIZE,
            CachingModeWriteBack,
            MemoryAllocationType_Heap,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_User
            );

    uint8_t *args = (uint8_t*)v_tmp_addr;
    uint64_t off = 0;

    for(uint32_t i = 0; i < argc; i++)
    {
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
    
    for(uint32_t i = 0; i < auxv_cnt; i++)
    {
      auxv->a_type = aux_vectors[i].a_type;
      auxv->a_un.a_val = aux_vectors[i].a_un.a_val;

      auxv++;
    }
    
    auxv->a_type = AUXVectorType_NULL;

    //__asm__("cli\n\thlt" :: "a"(&stack[0]));
    return (void*)stack;
}