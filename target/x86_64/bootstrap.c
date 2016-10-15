#include "common.h"
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

static volatile int smp_sync_base;

void
bootstrap_render(uint32_t color) {
    CardinalBootInfo *info = GetBootInfo();
    for(uint32_t y = 0; y < info->FramebufferHeight * info->FramebufferPitch; y+=4) {
        *(uint32_t*)(info->FramebufferAddress + y) = color;  //ARGB
    }
}

void
bootstrap_pagefault_handler(Registers *regs) {
    regs->int_no *= 16;
    bootstrap_render ((regs->int_no & 0xff) | (regs->int_no & 0xff << 8)| (regs->int_no & 0xff << 16)| (regs->int_no & 0xff << 24));
    __asm__ volatile("cli\n\thlt" :: "a"(regs->rip), "b"(regs->int_no / 16), "c"(regs->err_code), "d"(GetCurrentThreadUID()), "S"(regs->useresp), "D"(regs->rax));
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

    info->FramebufferAddress = (uint64_t)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)info->FramebufferAddress);  //Virtualize bootinfo addresses

    //Copy the initrd into the bootstrap memory pool
    info->InitrdStartAddress = (uint64_t)VirtMemMan_GetVirtualAddress(CachingModeWriteBack, (void*)info->InitrdStartAddress);
    void* tmp_initrd_addr = bootstrap_malloc(info->InitrdLength + PAGE_SIZE);

        uint64_t tmp_initrd_addr_u64 = (uint64_t)tmp_initrd_addr;
        if(tmp_initrd_addr_u64 % PAGE_SIZE != 0)tmp_initrd_addr_u64 += (PAGE_SIZE - tmp_initrd_addr_u64 % PAGE_SIZE);

        tmp_initrd_addr = (void*)tmp_initrd_addr_u64;

        memcpy(tmp_initrd_addr, (void*)info->InitrdStartAddress, info->InitrdLength);
        memset((void*)info->InitrdStartAddress, 0, info->InitrdLength);
        info->InitrdStartAddress = (uint64_t)tmp_initrd_addr;

    if(magic != MULTIBOOT_MAGIC) {
        bootstrap_kernel_panic(0xff);   //We weren't booted by a standards compliant bootloader, can't trust this environment
    }

    GDT_Bootstrap();
    IDT_Initialize();   //Setup the Bootstrap IDT
    for(int i = 0; i < 32; i++)
        IDT_RegisterHandler(i, bootstrap_pagefault_handler);


    FPU_Initialize();   //Setup the FPU

    MemMan_Initialize ();
    VirtMemMan_Initialize ();
    MemoryHAL_Initialize ();


    GDT_InitializeMP();
    GDT_Initialize();   //Setup the Bootstrap GDT

    uint64_t t_addr = ((uint64_t)bootstrap_malloc(KiB(16)) + KiB(16));
    t_addr -= t_addr % 16;
    SetInterruptStack((void*)t_addr);
    //Setup IST for important exceptions
    GDT_SetIST(0x1, (uint64_t)bootstrap_malloc(KiB(4)));
    IDT_ChangeEntry(0x8, 0x08, 0x8E, 0x1);  //Setup IST1 for Double fault

    GDT_SetIST(0x2, (uint64_t)bootstrap_malloc(KiB(4)));
    IDT_ChangeEntry(0x12, 0x08, 0x8E, 0x2); //Setup IST2 for Machine Check

    ACPITables_Initialize();    //Initialize the ACPI table data
    SMP_IncrementCoreCount();

    RTC_Initialize ();

    //Convert framebuffer address to writethrough caching
    info->FramebufferAddress = (uint64_t)VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), (void*)info->FramebufferAddress);
    info->FramebufferAddress = (uint64_t)VirtMemMan_GetVirtualAddress(CachingModeWriteThrough, (void*)info->FramebufferAddress);

    smp_sync_base = 1;
    APIC_Initialize();


    for(int i = 0; i < 32; i++) {
        IDT_ChangeEntry(i, 0x08, 0x8E, 0x2);
    }

    ManagedPageTable *pageTable = bootstrap_malloc(sizeof(ManagedPageTable));
    pageTable->PageTable = (UID)VirtMemMan_GetCurrent();
    pageTable->reference_count = 0;
    pageTable->lock = CreateBootstrapSpinlock();
    SetActiveVirtualMemoryInstance(pageTable);

    //Now that all the processors are booted up and ready to do their job
    //Initialize MTRRs, paging, enable debugging interfaces, find ACPI tables and report them to the kernel - Done
    //Initialize FPU - Done, setup threading code, provide interfaces to OS
    //Setup platform specific rendering code and supply interface to the OS (VESA driver?)
    //Initialize PCI, provide interface to OS
    //Initialize syscall mechanism, provide interface to OS for managing syscalls and jumping to user mode

    //When threading is up again, call kernel on new thread
    kernel_main_init();
    __asm__ volatile("cli\n\thlt\n\t");

    //We aren't supposed to reach here!
}

void
setup_preemption(void) {
    //Start the APIC timer here to act as a reference 'clock'
    //This is to be used along with the provided frequency to allow threads to sleep
}

void
target_device_setup(void) {
    //A pci device initialization is to be made into a thread spawn, the thread is freed when execution is complete
    //while(1){__asm__("hlt" :: "a"(APIC_GetTimerFrequency()));}
    //pci_Initialize();
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
    stack -= stack % 16;

    __asm__ volatile("mov %0, %%rsp":: "r"(stack)); //Switch to a new stack

    IDT_Initialize();   //Setup the IDT
    FPU_Initialize();   //Setup the FPU

    //Setup the page table for this core
    VirtMemMan_InitializeBootstrap();
    VirtMemMan_Initialize();

    GDT_Initialize();   //Setup the GDT
    SetInterruptStack((void*)((uint64_t)bootstrap_malloc(KiB(16)) + KiB(16) - 128));

    GDT_SetIST(0x1, (uint64_t)bootstrap_malloc(KiB(4)));
    IDT_ChangeEntry(0x8, 0x08, 0x8E, 0x1);  //Setup IST1 for Double fault

    GDT_SetIST(0x2, (uint64_t)bootstrap_malloc(KiB(4)));
    IDT_ChangeEntry(0x12, 0x08, 0x8E, 0x2); //Setup IST2 for Machine Check

    APIC_LocalInitialize();
    __asm__ volatile("sti");
    APIC_CallibrateTimer();
    __asm__ volatile("cli");
    SMP_IncrementCoreCount();
    int coreID = SMP_GetCoreCount();
    SMP_UnlockTrampoline();

    MemoryHAL_Initialize();
    while(smp_sync_base);

    ManagedPageTable *pageTable = bootstrap_malloc(sizeof(ManagedPageTable));
    pageTable->PageTable = (UID)VirtMemMan_GetCurrent();
    pageTable->reference_count = 0;
    pageTable->lock = CreateBootstrapSpinlock();
    SetActiveVirtualMemoryInstance(pageTable);

    smp_core_main(coreID, get_perf_counter);
    while(1);
}
