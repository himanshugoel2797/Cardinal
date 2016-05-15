#include "cpuid.h"

static uint32_t eax, ebx, ecx, edx;
static uint32_t cache_line_size = 0;

void
CPUID_RequestInfo(uint32_t eax,
                  uint32_t ecx) {
    __asm__ volatile ("cpuid\n\t"
                      : "=a"(eax), "=b"(ebx), "=c" (ecx), "=d" (edx)
                      : "a" (eax), "c"(ecx));
}

uint32_t
CPUID_GetCacheLineSize(void)
{
    if(cache_line_size == 0){
        CPUID_RequestInfo(0x80000006, 0);
        cache_line_size = ecx & 0xFF;
    }
    return cache_line_size;
}

bool
CPUID_FeatureIsAvailable(CPUID_REG cpuid_register,
                         CPUID_FEAT feature) {
    switch(cpuid_register) {
    case CPUID_EAX:
        return (uint32_t)(eax & feature) == (uint32_t)feature;
    case CPUID_EBX:
        return (uint32_t)(ebx & feature) == (uint32_t)feature;
    case CPUID_ECX:
        return (uint32_t)(ecx & feature) == (uint32_t)feature;
    case CPUID_EDX:
        return (uint32_t)(edx & feature) == (uint32_t)feature;
    default:
        return FALSE;
    }
}

uint32_t
CPUID_GetValue(CPUID_REG cpuid_register) {
    switch(cpuid_register) {
    case CPUID_EAX:
        return eax;
    case CPUID_EBX:
        return ebx;
    case CPUID_ECX:
        return ecx;
    case CPUID_EDX:
        return edx;
    default:
        return 0;
    }
}