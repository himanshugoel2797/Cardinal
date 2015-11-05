#include "memory.h"

void*
GetVirtualAddress(void *physicalAddress)
{
    return (void*)((uint64_t)physicalAddress + 0xfffffffe00000000ull);
}

void*
GetPhysicalAddress(void *virtualAddress)
{
    return (void*)((uint64_t)virtualAddress  - 0xfffffffe00000000ull);
}
