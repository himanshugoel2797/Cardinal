#include "types.h"
#include "gdt.h"
#include "common.h"
#include "memory.h"

//Describes a single GDT entry
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;

    uint8_t access;
    uint8_t granularity;

    uint8_t base_high;

} __attribute__((packed)) GDTEntry;

void
GDT_SetEntry(int num,
             uint32_t base,
             uint32_t limit,
             uint8_t access,
             uint8_t gran);

void
GDT_SetTSS(int num,
           uint64_t base,
           uint32_t limit,
           uint8_t access);

//Describes the pointer to the GDT entry
typedef struct {
    uint16_t limit; //Size
    uint64_t base; //Location
} __attribute__((packed)) GDTPtr;

//GDT Entry region
GDTEntry gdt_entries[GDT_ENTRY_COUNT];
GDTPtr gdt_table;
tss_struct sys_tss; //Define the TSS as a global structure

__attribute__((optnone))
void GDT_Initialize() {
    //Make sure interrupts are disabled
    __asm__ ("cli");

    memset((void*)&sys_tss, 0, sizeof(tss_struct));
    sys_tss.iomap = sizeof(tss_struct);

    gdt_table.limit = (sizeof(GDTEntry) * GDT_ENTRY_COUNT) - 1;
    gdt_table.base = (uint64_t)&gdt_entries;


    GDT_SetEntry(0, 0, 0, 0, 0);
    GDT_SetEntry(1, 0, 0xFFFFFFFF, 0x9A, 0x20); // Code segment
    GDT_SetEntry(2, 0, 0xFFFFFFFF, 0x92, 0x00); // Data segment
    GDT_SetEntry(3, 0, 0xFFFFFFFF, 0xFA, 0x20); // User mode code segment
    GDT_SetEntry(4, 0, 0xFFFFFFFF, 0xF2, 0x00); // User mode data segment
    GDT_SetTSS(5, (uint64_t)&sys_tss, sizeof(tss_struct), 0xE9);

    __asm__ ("lgdt (%0)" :: "r" (&gdt_table));

    __asm__ (
        "mov $flush, %rax\n\t"
        "push %rax\n\t"
        "ret\n\t"
        "flush:\n\t"
        "mov $0x10, %ax\n\t"
        "mov %ax, %ds\n\t"
        "mov %ax, %es\n\t"
        "mov %ax, %fs\n\t"
        "mov %ax, %gs\n\t"
        "mov %ax, %ss\n\t"

        //Set the TSS
        "mov $0x2B, %ax\n\t"
        "ltr %ax"
    );

    //__asm__ ("mov %0, %%rax\n\thlt"::"ra"(sizeof(tss_struct)));
    //TODO setup a 64bit TSS

    return; //Don't enable interrupts yet
}

void
GDT_SetTSS(int num,
           uint64_t base,
           uint32_t limit,
           uint8_t access) {
    GDT_SetEntry(num, (uint32_t)base, limit, access, 0);
    GDT_SetEntry(num + 1, 0, 0, 0, 0);

    gdt_entries[num + 1].limit_low = (uint16_t)(base >> 32);
    gdt_entries[num + 1].base_low = (uint16_t)(base >> (32 + 16));
}

void
GDT_SetEntry(int num,
             uint32_t base,
             uint32_t limit,
             uint8_t access,
             uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_mid = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access     = access;
}

void
SetKernelStack(void* sp) {
    sys_tss.rsp0 = (uint64_t)sp;
}

void
SetUserStack(void *sp) {
    sys_tss.rsp2 = (uint64_t)sp;
}


void
GDT_SetIST(uint8_t ist,
           uint64_t stack)
{
    switch(ist)
    {
        case 1:
        sys_tss.ist1 = stack;
        break;
        case 2:
        sys_tss.ist2 = stack;
        break;
        case 3:
        sys_tss.ist3 = stack;
        break;
        case 4:
        sys_tss.ist4 = stack;
        break;
        case 5:
        sys_tss.ist5 = stack;
        break;
        case 6:
        sys_tss.ist6 = stack;
        break;
        case 7:
        sys_tss.ist7 = stack;
        break;
        default:
        break;
    }
}