#include "main.h"
#include "types.h"
#include "GDT/gdt.h"
#include "IDT/idt.h"
#include "fpu/fpu.h"
#include "rtc/rtc.h"
#include "smp/smp.h"
#include "virt_mem_manager/virt_mem_manager.h"
#include "page_manager/phys_mem_manager.h"
#include "boot_information/boot_information.h"
#include "bootinfo.h"
#include "x86_64_common.h"
#include "memory.h"
#include "synchronization.h"
#include "managers.h"
#include "apic/apic.h"
#include "acpi/acpi_tables.h"
#include "pci/pci.h"
#include "thread.h"

#ifdef MULTIBOOT1
#include "multiboot.h"
#elif defined(MULTIBOOT2)
#include "multiboot2.h"
#endif

static int smp_sync_base;

void
bootstrap_render(uint32_t color) {
    CardinalBootInfo *info = GetBootInfo();
    for(uint32_t y = 0; y < info->framebuffer_height * info->framebuffer_pitch; y+=4) {
        *(uint32_t*)(info->framebuffer_addr + y) = color;  //ARGB
    }
}

void
bootstrap_pagefault_handler(Registers *regs) {
    regs->int_no = -regs->int_no;
    bootstrap_render (0xffffff00);
    __asm__ volatile("hlt" :: "a"(regs->rip), "b"(regs->int_no));
    regs->int_no = -regs->int_no;
}

void
bootstrap_kernel_panic(uint8_t severity) {
    while(1) {
        bootstrap_render(0x00ff0000 | (0xff - severity));
    }
}

__attribute__((section(".entry_point")))    //Ensure that this is always the first thing in the .text section
void
bootstrap_kernel(void *param,
                 uint64_t magic) {
    //Initialize the bootstrap mappings
    VirtMemMan_InitializeBootstrap();

    ParseAndSaveBootInformation(param);
    CardinalBootInfo *info = GetBootInfo();

    info->framebuffer_addr = (uint64_t)GetVirtualAddress(CachingModeWriteBack, (void*)info->framebuffer_addr);  //Virtualize bootinfo addresses

    if(magic != MULTIBOOT_MAGIC) {
        bootstrap_kernel_panic(0xff);   //We weren't booted by a standards compliant bootloader, can't trust this environment
    }

    GDT_Initialize();   //Setup the Bootstrap GDT
    IDT_Initialize();   //Setup the Bootstrap IDT
    for(int i = 0; i < 31; i++)
        IDT_RegisterHandler(i, bootstrap_pagefault_handler);

    //Setup IST for important exceptions
    GDT_SetIST(0x1, (uint64_t)bootstrap_malloc(KiB(4)));
    IDT_ChangeEntry(0x8, 0x08, 0x8E, 0x1);  //Setup IST1 for Double fault

    GDT_SetIST(0x2, (uint64_t)bootstrap_malloc(KiB(4)));
    IDT_ChangeEntry(0x12, 0x08, 0x8E, 0x2); //Setup IST2 for Machine Check

    FPU_Initialize();   //Setup the FPU


    MemMan_Initialize ();
    VirtMemMan_Initialize ();

    ACPITables_Initialize();    //Initialize the ACPI table data
    SMP_IncrementCoreCount();

    RTC_Initialize ();

    //Convert framebuffer address to writethrough caching
    info->framebuffer_addr = (uint64_t)GetPhysicalAddress((void*)info->framebuffer_addr);
    info->framebuffer_addr = (uint64_t)GetVirtualAddress(CachingModeWriteThrough, (void*)info->framebuffer_addr);
    info->initrd_start_addr = (uint64_t)GetVirtualAddress(CachingModeWriteBack, (void*)info->initrd_start_addr);

    SetKernelStack((void*)((uint64_t)bootstrap_malloc(KiB(4)) + KiB(4)));
    SetUserStack((void*)((uint64_t)bootstrap_malloc(KiB(4)) + KiB(4)));

    smp_sync_base = 1;
    APIC_Initialize();
    //__asm__ ("hlt");

    //Now that all the processors are booted up and ready to do their job

    //Initialize MTRRs, paging, enable debugging interfaces, find ACPI tables and report them to the kernel - Done
    //Initialize FPU - Done, setup threading code, provide interfaces to OS
    //Setup platform specific rendering code and supply interface to the OS (VESA driver?)
    //Initialize PCI, provide interface to OS
    //Initialize syscall mechanism, provide interface to OS for managing syscalls and jumping to user mode

    //When threading is up again, call kernel on new thread
    kernel_main_init();

    //We aren't supposed to reach here!
    __asm__ volatile("cli\n\thlt\n\t");
}

void
target_device_setup(void)
{
    //Start the APIC timer here to act as a reference 'clock'
    //This is to be used along with the provided frequency to allow threads to sleep
    //A pci device initialization is to be made into a thread spawn, the thread is freed when execution is complete
    SetPeriodicPreemptVector(IRQ(1), APIC_GetTimerFrequency()/1000);
    APIC_SetVector(APIC_TIMER, IRQ(1));
    APIC_SetTimerValue(APIC_GetTimerFrequency()/1000);
    APIC_SetTimerMode(APIC_TIMER_PERIODIC);
    APIC_SetEnableInterrupt(APIC_TIMER, ENABLE);
    //while(1){__asm__("hlt" :: "a"(APIC_GetTimerFrequency()));}
    pci_Initialize();
}

int get_perf_counter(void) {
    return 0;
}

void
smp_unlock_cores(void) {
    smp_sync_base = 0;
}

__attribute__((section(".tramp_handler")))
void
smp_bootstrap(void) {
    SMP_LockTrampoline();

    //Allocate a new stack for this thread and put it into the scheduler's queue
    uint64_t stack = (uint64_t)bootstrap_malloc(KiB(16));
    stack += KiB(16);

    __asm__ volatile("mov %%rax, %%rsp":: "a"(stack)); //Switch to a new stack

    GDT_Initialize();   //Setup the GDT
    IDT_Initialize();   //Setup the IDT
    FPU_Initialize();   //Setup the FPU

    //Setup the page table for this core
    VirtMemMan_InitializeBootstrap();
    VirtMemMan_Initialize();

    int coreID = SMP_GetCoreCount();

    APIC_LocalInitialize();
    __asm__ volatile("sti");
    APIC_CallibrateTimer();
    __asm__ volatile("cli");
    SMP_IncrementCoreCount();
    SMP_UnlockTrampoline();

    while(smp_sync_base);

    smp_core_main(coreID, get_perf_counter);
    while(1);
}
