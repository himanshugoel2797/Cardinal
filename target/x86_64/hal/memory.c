#include "memory.h"

void*
GetVirtualAddress(void *physicalAddress)
{
    return (uint8_t*)physicalAddress + 0xfffffffe00000000ull;
}