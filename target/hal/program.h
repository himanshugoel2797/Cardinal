#ifndef _CARDINAL_HAL_PROGRAM_H_
#define _CARDINAL_HAL_PROGRAM_H_

#include "types.h"

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
    AUXVectorType_EGID
} AUXVectorType;

typedef struct {
    int32_t a_type;
    union {
        int64_t a_val;
        void *a_ptr;
        void (*a_func)();
    } a_un;
} AUXVector;

void
SetupApplicationStack(void *sp,
                      uint32_t argc,
                      const char **argv,
                      const char **envp,
                      const AUXVector *aux_vectors,
                      uint32_t auxv_cnt);

#endif