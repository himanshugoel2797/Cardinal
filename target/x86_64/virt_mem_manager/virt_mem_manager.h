#ifndef _VIRT_MEM_MAN_X86_64_H_
#define _VIRT_MEM_MAN_X86_64_H_

#include "types.h"

typedef uint64_t* PML_Instance;

typedef enum
{
    MEM_TYPE_UC = 2,	//Uncached
    MEM_TYPE_WB = 0,	//Writeback
    MEM_TYPE_WT = 1,	//Write through
    MEM_TYPE_WC = 3	//Write complete
} MEM_TYPES;		//Memory caching types

typedef enum
{
    MEM_WRITE = 2,	//Writable
    MEM_READ = 4,	//Readable
    MEM_EXEC = 1	//Executable
} MEM_ACCESS_PERMS;	//Memory Permissions

typedef enum
{
    MEM_KERNEL = 0,	//Kernel access only
    MEM_USER = 1	//User access only
} MEM_SECURITY_PERMS;	//Memory access permissions

void
VirtMemMan_Initialize(void);

#endif
