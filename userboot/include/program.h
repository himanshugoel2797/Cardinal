#ifndef _CARDINAL_HAL_PROGRAM_H_
#define _CARDINAL_HAL_PROGRAM_H_

#include <stdint.h>
#include <stddef.h>

#include "elf.h"

typedef enum {
    AUXVectorType_NULL = 0,
    AUXVectorType_IGNORE,
    AUXVectorType_EXECFD,
    AUXVectorType_PHDR,
    AUXVectorType_PHENT,
    AUXVectorType_PHNUM,
    AUXVectorType_PGSZ,
    AUXVectorType_BASE,
    AUXVectorType_FLAGS,
    AUXVectorType_ENTRY,
    AUXVectorType_NOTELF,
    AUXVectorType_UID,
    AUXVectorType_EUID,
    AUXVectorType_GID,
    AUXVectorType_EGID,
    AUXVectorType_PLATFORM,
    AUXVectorType_HWCAP,
    AUXVectorType_CLKTCK,
    AUXVectorType_FPUCW,
    AUXVectorType_DCACHEBSIZE,
    AUXVectorType_ICACHEBSIZE,
    AUXVectorType_UCACHEBSIZE,
    AUXVectorType_IGNOREPPC,
    AUXVectorType_SECURE,
    AUXVectorType_BASEPLATFORM,
    AUXVectorType_RANDOM,
    AUXVectorType_EXECFN = 31
} AUXVectorType;

typedef struct {
    int32_t a_type;
    union {
        int64_t a_val;
        void *a_ptr;
        void (*a_func)();
    };
} AUXVector;

void
LoadAndStartApplication(void *elfLoc,
                        uint64_t elf_Size,
                        const char **argv,
                        uint32_t argc,
                        const char **envp);

void*
SetupApplicationStack(void *sp,
                      uint32_t argc,
                      const char **argv,
                      const char **envp,
                      const AUXVector *aux_vectors,
                      uint32_t auxv_cnt,
                      ElfInformation *elf);

#endif