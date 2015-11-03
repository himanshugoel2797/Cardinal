#include "fpu.h"
#include "cpuid.h"
#include "managers.h"

uint32_t FPU_StateHandler(Registers *regs);

void FPU_Initialize()
{
    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_FPU))
        {
            //OS can't load
            asm volatile("mov $0, %eax\n\thlt");
            return;
        }
    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_SSE))
        {
            //OS can't load
            asm volatile("mov $1, %eax\n\thlt");
            return;
        }
    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_FXSR))
        {
            //OS can't load
            asm volatile("mov $2, %eax\n\thlt");
            return;
        }

    uint32_t bitmask = 0;
    //Get the current control register 0 value
    asm volatile ("mov %%cr0, %0" : "=r" (bitmask));

    bitmask &= ~(1 << 2); //Enable the FPU
    bitmask |= (1 << 5); //Enable FPU exceptions
    bitmask |= (1 << 1); //FWAIT should cause a state change

    //Set the control register to the new value
    asm volatile ("mov %0, %%cr0" :: "r" (bitmask));

    //Get the current control register 0 value
    asm volatile ("mov %%cr4, %0" : "=r" (bitmask));

    bitmask |= (1 << 9); //Enable FPU exceptions
    bitmask |= (1 << 10); //Allow CPU to backup FPU state
    bitmask |= (1 << 18); //FWAIT should cause a state change

    //Set the control register to the new value
    asm volatile ("mov %0, %%cr4" :: "r" (bitmask));

    Interrupts_RegisterHandler(7, 0, FPU_StateHandler);

    asm ("fninit");
}

void FPU_EnableInterrupts()
{
    uint32_t bitmask = 0;
    //Get the current control register 0 value
    asm volatile ("mov %%cr0, %0" : "=r" (bitmask));
    bitmask |= (1 << 3); //Allow CPU to backup FPU state
    //Set the control register to the new value
    asm volatile ("mov %0, %%cr0" :: "r" (bitmask));
}

uint32_t FPU_StateHandler(Registers *regs)
{
    //TODO FXSSAVE
    asm volatile ("clts");
    return 0;
}
