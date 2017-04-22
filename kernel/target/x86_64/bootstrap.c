/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "common.h"
#include "main.h"
#include "types.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
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
#include "thread.h"
#include "debug_gfx.h"

#ifdef MULTIBOOT1
#include "multiboot.h"
#elif defined(MULTIBOOT2)
#include "multiboot2.h"
#else
#error "Unsupported boot protocol for this target."
#endif

static volatile int smp_sync_base;
static uint64_t *int_stack;

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
    //debug_handle_trap();
    __asm__ volatile("cli\n\thlt" :: "a"(regs->rip), "b"(regs->int_no / 16), "c"(regs->err_code), "d"(GetCurrentProcessUID()), "S"(regs->useresp), "D"(regs->rax));
}

__attribute__((section(".entry_point")))    //Ensure that this is always the first thing in the .text section
void
bootstrap_kernel(void *param,
                 uint64_t magic) {

    if(param == NULL)
        bootstrap_render(0xff00ff00);

    //Initialize the bootstrap mappings
    VirtMemMan_InitializeBootstrap();

    ParseAndSaveBootInformation(param, magic);
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

    GDT_Bootstrap();
    IDT_Initialize();   //Setup the Bootstrap IDT
    for(int i = 0; i < 32; i++)
        IDT_RegisterHandler(i, bootstrap_pagefault_handler);

    debug_gfx_init();

    FPU_Initialize();   //Setup the FPU

    MemMan_Initialize ();
    VirtMemMan_Initialize ();
    MemoryHAL_Initialize ();


    GDT_InitializeMP();
    GDT_Initialize();   //Setup the Bootstrap GDT

    uint64_t t_addr = ((uint64_t)bootstrap_malloc(KiB(32)) + KiB(32) - 128) ;
    t_addr -= t_addr % 16;
    SetInterruptStack((void*)t_addr);

    t_addr = ((uint64_t)bootstrap_malloc(KiB(4)) + KiB(4));
    t_addr -= t_addr % 16;

    GDT_SetIST(0x3, t_addr);
    for(int i = 0; i < 32; i++) {
        IDT_ChangeEntry(i, 0x08, 0x8E, 0x3);
    }

    //Setup IST for important exceptions
    t_addr = ((uint64_t)bootstrap_malloc(KiB(4)) + KiB(4));
    t_addr -= t_addr % 16;

    GDT_SetIST(0x1, t_addr);
    IDT_ChangeEntry(0x8, 0x08, 0x8E, 0x1);  //Setup IST1 for Double fault


    t_addr = ((uint64_t)bootstrap_malloc(KiB(4)) + KiB(4));
    t_addr -= t_addr % 16;

    GDT_SetIST(0x2, t_addr);
    IDT_ChangeEntry(0x12, 0x08, 0x8E, 0x2); //Setup IST2 for Machine Check

    ACPITables_Initialize();    //Initialize the ACPI table data
    SMP_IncrementCoreCount();

    RTC_Initialize ();

    //Convert framebuffer address to writethrough caching
    info->FramebufferAddress = (uint64_t)VirtMemMan_GetPhysicalAddress(VirtMemMan_GetCurrent(), (void*)info->FramebufferAddress);
    info->FramebufferAddress = (uint64_t)VirtMemMan_GetVirtualAddress(CachingModeWriteThrough, (void*)info->FramebufferAddress);

    smp_sync_base = 1;
    APIC_Initialize();

    //Reinstall the TLB shootdown handler
    MemoryHAL_Initialize();

    ManagedPageTable *pageTable = bootstrap_malloc(sizeof(ManagedPageTable));
    pageTable->PageTable = (UID)VirtMemMan_GetCurrent();
    
    if(GetCoreCount() < 64)
        pageTable->SmallActivityBitmap = 0;
    else
        pageTable->LargeActivityBitmap = bootstrap_malloc(GetCoreCount() / 8 + 1);

    pageTable->lock = CreateBootstrapSpinlock();
    SetActiveVirtualMemoryInstance(pageTable);

    bootstrap_render(0xFFFFFFFF);

    //Now that all the processors are booted up and ready to do their job
    kernel_main_init();

    //We aren't supposed to reach here!
    PANIC("Unexpected return.");

}

int get_perf_counter(void) {
    return 0;
}

void
smp_unlock_cores(void) {
    smp_sync_base = 0;
}

#include "utils/native.h"
void
smp_bootstrap_stage2(void) {
    VirtMemMan_InitializeBootstrap();

    IDT_Initialize();   //Setup the IDT
    FPU_Initialize();   //Setup the FPU

    //Setup the page table for this core
    VirtMemMan_Initialize();
    MemoryHAL_Initialize();

    GDT_Initialize();   //Setup the GDT


    uint64_t t_addr = ((uint64_t)bootstrap_malloc(KiB(32)) + KiB(32) - 128);
    t_addr -= t_addr % 16;
    SetInterruptStack((void*)t_addr);


    t_addr = ((uint64_t)bootstrap_malloc(KiB(4)) + KiB(4));
    t_addr -= t_addr % 16;

    GDT_SetIST(0x1, t_addr);
    IDT_ChangeEntry(0x8, 0x08, 0x8E, 0x1);  //Setup IST1 for Double fault


    t_addr = ((uint64_t)bootstrap_malloc(KiB(4)) + KiB(4));
    t_addr -= t_addr % 16;

    GDT_SetIST(0x2, t_addr);
    IDT_ChangeEntry(0x12, 0x08, 0x8E, 0x2); //Setup IST2 for Machine Check


    APIC_LocalInitialize();

    SMP_IncrementCoreCount();
    SMP_UnlockTrampoline();

    __asm__ volatile("sti");
    APIC_CallibrateTimer();
    __asm__ volatile("cli");

    MemoryHAL_Initialize();

    ManagedPageTable *pageTable = bootstrap_malloc(sizeof(ManagedPageTable));
    pageTable->PageTable = (UID)VirtMemMan_GetCurrent();

    if(GetCoreCount() < 64)
        pageTable->SmallActivityBitmap = 0;
    else
        pageTable->LargeActivityBitmap = bootstrap_malloc(GetCoreCount() / 8 + 1);

    pageTable->lock = CreateBootstrapSpinlock();

    SetActiveVirtualMemoryInstance(pageTable);


    while(smp_sync_base);
    smp_core_main(get_perf_counter);
    while(1);
}


__attribute__((section(".tramp_handler")))
void
smp_bootstrap(void) {
    SMP_LockTrampoline();


    //Allocate a new stack for this thread and put it into the scheduler's queue
    uint64_t stack = (uint64_t)bootstrap_malloc(KiB(16));
    stack += KiB(16);
    stack -= stack % 16;

    __asm__ volatile("mov %0, %%rsp\n\tcall smp_bootstrap_stage2":: "r"(stack)); //Switch to a new stack
}
