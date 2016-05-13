#include "fpu.h"
#include "CPUID/cpuid.h"
#include "x86_64_common.h"

void
FPU_Initialize(void)
{
    CPUID_RequestInfo(CPUID_EAX_FIRST_PAGE, CPUID_ECX_IGNORE);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_FPU))
        bootstrap_kernel_panic(0xff);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_SSE))
        bootstrap_kernel_panic(0xff);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_FXSR))
        bootstrap_kernel_panic(0xff);

    uint64_t bitmask = 0;
    //Get the current control register 0 value
    __asm__ volatile ("mov %%cr0, %0"
                      : "=ra" (bitmask));

    bitmask &= ~(1 << 2); //Enable the FPU
    bitmask |= (1 << 5); //Enable FPU exceptions
    bitmask |= (1 << 1); //FWAIT should cause a state change

    //Set the control register to the new value
    __asm__ volatile ("mov %0, %%cr0"
                      :: "ra" (bitmask));

    //Get the current control register 0 value
    __asm__ volatile ("mov %%cr4, %0"
                      : "=ra" (bitmask));

    bitmask |= (1 << 9); //Enable FPU exceptions
    bitmask |= (1 << 10); //Allow CPU to backup FPU state
    //bitmask |= (1 << 18); //FWAIT should cause a state change

    bootstrap_render(-1);
    //while(1);
    //Set the control register to the new value
    __asm__ volatile ("mov %0, %%cr4"
                      :: "ra" (bitmask));

    //Interrupts_RegisterHandler(7, 0, FPU_StateHandler);
    __asm__ volatile ("fninit");
}

/*
uint32_t
FPU_SaveState(void *target,
              uint32_t size)
{

}

void
FPU_LoadState(void *source,
              uint32_t size)
{

}

void
FPU_SetLazyStateSaveEnableMode(bool enableMode)
{

}*/