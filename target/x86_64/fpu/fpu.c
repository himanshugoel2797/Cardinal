#include "fpu.h"
#include "interrupts.h"
#include "CPUID/cpuid.h"
#include "x86_64_common.h"

static bool xsave_available = FALSE;
static uint64_t state_area_size;

static void
FPU_SIMDException(uint32_t int_no,
                  uint32_t err_no) {
    if(int_no != 0x13)return;
    err_no = 0;

    __asm__ volatile("cli\n\tmov %0, %%rax\n\thlt" :: "ra"((uint64_t)0xB00B1E5));

}

static void
FPU_X87Exception(uint32_t int_no,
                 uint32_t err_no) {
    if(int_no != 0x10)return;
    err_no = 0;
    __asm__ volatile("cli\n\tmov %0, %%rax\n\thlt" :: "ra"((uint64_t)0xB00B5));
}

void
FPU_Initialize(void) {
    CPUID_RequestInfo(CPUID_EAX_FIRST_PAGE, CPUID_ECX_IGNORE);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_FPU))
        bootstrap_kernel_panic(0xff);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_SSE))
        bootstrap_kernel_panic(0xff);

    if(!CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_FXSR))
        bootstrap_kernel_panic(0xff);

    xsave_available = CPUID_FeatureIsAvailable(CPUID_ECX, CPUID_FEAT_ECX_XSAVE);

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

    bitmask |= (1 << 9); //Enable SSE support
    bitmask |= (1 << 10); //Enable #XF exception


    if(xsave_available)
        bitmask |= (1 << 18); //Enable XSAVE

    RegisterInterruptHandler(0x10, FPU_X87Exception);
    RegisterInterruptHandler(0x13, FPU_SIMDException);

    //Set the control register to the new value
    __asm__ volatile ("mov %0, %%cr4"
                      :: "ra" (bitmask));

    //Interrupts_RegisterHandler(7, 0, FPU_StateHandler);
    __asm__ volatile ("fninit");

    if(xsave_available)
    {
    //Determine the XSave area size, and setup XCR0
    CPUID_RequestInfo(0x0D, 0);
    __asm__ volatile("xsetbv" :: "d"(CPUID_GetValue(CPUID_EDX)), "a"(CPUID_GetValue(CPUID_EAX)), "c"((uint32_t)0));

    CPUID_RequestInfo(0x0D, 1);
    state_area_size = CPUID_GetValue(CPUID_EBX);
    __asm__ volatile("cli\n\thlt" :: "a"(state_area_size));
    }else{
        state_area_size = 512;
    }
}

uint64_t
FPU_GetStateSize(void)
{
    return state_area_size;
}

uint32_t
FPU_SaveState(void *target)
{
    if(xsave_available){
    __asm__ volatile("xsave %0" : "=m"(target) ::);
    }else{
        __asm__ volatile("fxsave %0" : "=m"(target) :: );
    }
    return 1;
}

void
FPU_LoadState(void *source)
{
    if(xsave_available){
    __asm__ volatile("xrstor %0" :: "m"(source) :);
}else{
    __asm__ volatile("fxrstor %0" :: "m"(source) :);
}
}

/*
void
FPU_SetLazyStateSaveEnableMode(bool enableMode)
{

}*/