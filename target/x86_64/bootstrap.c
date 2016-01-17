#include "main.h"
#include "types.h"
#include "GDT/gdt.h"
#include "IDT/idt.h"
#include "fpu.h"
#include "rtc/rtc.h"
#include "virt_mem_manager/virt_mem_manager.h"
#include "page_manager/phys_mem_manager.h"
#include "boot_information/boot_information.h"
#include "bootinfo.h"
#include "x86_64_common.h"
#include "memory.h"
#include "managers.h"
#include "apic/apic.h"
#include "acpi/acpi_tables.h"

#ifdef MULTIBOOT1
#include "multiboot.h"
#elif defined(MULTIBOOT2)
#include "multiboot2.h"
#endif

void
bootstrap_render(uint32_t color)
{
    CardinalBootInfo *info = GetBootInfo();
    for(uint32_t y = 0; y < info->framebuffer_height * info->framebuffer_pitch; y+=4)
                {
                    *(uint32_t*)(info->framebuffer_addr + y) = color;  //ARGB
                }
}

void
bootstrap_pagefault_handler(Registers *regs)
{
  	regs->int_no = -regs->int_no;
	bootstrap_render (0xffffffff);
  	regs->int_no = -regs->int_no;
}

void
bootstrap_kernel_panic(uint8_t severity)
{
    while(1)
        {
            bootstrap_render(0x00ff0000 | (0xff - severity));
        }
}

int coreCount = 1;

__attribute__((section(".entry_point")))	//Ensure that this is always the first thing in the .text section
void
bootstrap_kernel(void *param,
                 uint64_t magic)
{
    //Now unmap the bootstrap mappings
    uint64_t *pml = 0;
    __asm__ volatile("mov %%cr3, %%rax" : "=a"(pml));
    pml = GetVirtualAddress(pml);

    ParseAndSaveBootInformation(param);
    CardinalBootInfo *info = GetBootInfo();

    info->framebuffer_addr = (uint64_t)GetVirtualAddress((void*)info->framebuffer_addr);	//Virtualize bootinfo addresses

    if(magic != MULTIBOOT_MAGIC)
        {
            bootstrap_kernel_panic(0xff);	//We weren't booted by a standards compliant bootloader, can't trust this environment
        }
    else
        {
            for(uint32_t y = 0; y < info->framebuffer_height; y++)
                {
                    for(uint32_t x = 0; x < info->framebuffer_pitch; x+=4)
                        *(uint32_t*)(info->framebuffer_addr + x + y * info->framebuffer_pitch) =
                            (0x00ffcc00 * ((y % 16)/4 == 0)) | (0x00ffcc00 * ((x % 64)/16 == 0));	//ARGB
                }
        }

    GDT_Initialize();	//Setup the GDT
    IDT_Initialize();	//Setup the IDT
    IDT_RegisterHandler(14, bootstrap_pagefault_handler);
    FPU_Initialize();	//Setup the FPU
    ACPITables_Initialize();	//Initialize the ACPI table data

    APIC_Initialize();
    bootstrap_render (0xffff);
    MemMan_Initialize ();
    bootstrap_render (0xffff0000);
    VirtMemMan_Initialize ();
    bootstrap_render (0xff00);
    RTC_Initialize ();
    bootstrap_render (0xff0f);

    //Initialize MTRRs, paging, enable debugging interfaces, find ACPI tables and report them to the kernel
    //Setup platform specific rendering code and supply interface to the OS (VESA driver?)
    //Initialize FPU, setup threading code, provide interfaces to OS
    //Initialize PCI, provide interface to OS
    //Initialize syscall mechanism, provide interface to OS for managing syscalls and jumping to user mode

    //When threading is up again, call kernel on new thread
    kernel_main();	//Done initializing all arch specific stuff, call the kernel

    //We aren't supposed to reach here!

    if(coreCount == 1)bootstrap_kernel_panic(0x00);

    uint32_t color = 0x00ff0000, prev_col = 0x00ffff00, swap = 0;
    while(1)
        {
            for(uint32_t y = 0; y < info->framebuffer_height * info->framebuffer_pitch; y+=4)
                {
                    *(uint32_t*)(info->framebuffer_addr + y) = color;	//ARGB
                }
            swap++;
            if(swap == 0x80)
                {
                    swap = color;
                    color = prev_col;
                    prev_col = swap;

                    swap = 0;
                }
        }
    __asm__ volatile("cli\n\thlt\n\t");
}

__attribute__((section(".tramp_handler")))
void
smp_bootstrap(void)
{
    coreCount++;
    __asm__ volatile("hlt");

    //Allocate a new stack for this thread and put it into the scheduler's queue
    uint64_t stack = (uint64_t)bootstrap_malloc(KiB(16));
    stack += KiB(16);

    __asm__ volatile("mov %%rax, %%rsp":: "ra"(stack)); //Switch to a new stack

    GDT_Initialize();   //Setup the GDT
    IDT_Initialize();   //Setup the IDT
    FPU_Initialize();   //Setup the FPU

    while(1);
}
