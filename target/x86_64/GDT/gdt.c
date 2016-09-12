#include "types.h"
#include "gdt.h"
#include "common.h"
#include "memory.h"
#include "managers.h"

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

typedef struct GDT_CoreData {
//GDT Entry region
    GDTEntry gdt_entries[GDT_ENTRY_COUNT];
    GDTPtr gdt_table;
    tss_struct sys_tss; //Define the TSS as a global structure
} GDT_CoreData;
static volatile GDT_CoreData *coreLocalData;

void
GDT_Bootstrap(void) {
    __asm__ ("cli");
    coreLocalData = bootstrap_malloc(sizeof(GDT_CoreData));

    memset((void*)&coreLocalData->sys_tss, 0, sizeof(tss_struct));
    coreLocalData->sys_tss.iomap = sizeof(tss_struct);

    coreLocalData->gdt_table.limit = (sizeof(GDTEntry) * GDT_ENTRY_COUNT) - 1;
    coreLocalData->gdt_table.base = (uint64_t)&coreLocalData->gdt_entries;


    GDT_SetEntry(0, 0, 0, 0, 0);
    GDT_SetEntry(1, 0, 0xFFFFFFFF, 0x9B, 0xA0); // Code segment
    GDT_SetEntry(2, 0, 0xFFFFFFFF, 0x93, 0x00); // Data segment
    GDT_SetEntry(3, 0, 0xFFFFFFFF, 0xFB, 0xD0); // User mode code segment (32bit)
    GDT_SetEntry(4, 0, 0xFFFFFFFF, 0xF3, 0x00); // User mode data segment
    GDT_SetEntry(5, 0, 0xFFFFFFFF, 0xFB, 0xA0); // User mode code segment (64bit)
    GDT_SetTSS(6, (uint64_t)&coreLocalData->sys_tss, sizeof(tss_struct), 0xE9);

    __asm__ volatile ("lgdt (%0)" :: "r" (&coreLocalData->gdt_table));

    __asm__ volatile (
        "pushq %rax\n\t"
        "mov $flush_gdt_btstrp, %rax\n\t"
        "pushq %rax\n\t"
        "retq\n\t"
        "flush_gdt_btstrp:\n\t"
        "mov $0x10, %ax\n\t"
        "mov %ax, %ds\n\t"
        "mov %ax, %es\n\t"
        "mov %ax, %fs\n\t"
        "mov %ax, %gs\n\t"
        "mov %ax, %ss\n\t"

        //Set the TSS
        "mov $0x33, %ax\n\t"
        "ltr %ax\n\t"
        "popq %rax\n\t"
    );

}

void
GDT_InitializeMP(void) {
    coreLocalData = AllocateAPLSMemory(sizeof(GDT_CoreData));
}

__attribute__((optnone))
void GDT_Initialize() {
    //Make sure interrupts are disabled
    __asm__ ("cli");

    memset((void*)coreLocalData, 0, sizeof(GDT_CoreData));


    coreLocalData->sys_tss.iomap = sizeof(tss_struct);

    coreLocalData->gdt_table.limit = (sizeof(GDTEntry) * GDT_ENTRY_COUNT) - 1;
    coreLocalData->gdt_table.base = (uint64_t)&coreLocalData->gdt_entries;


    GDT_SetEntry(0, 0, 0, 0, 0);
    GDT_SetEntry(1, 0, 0xFFFFFFFF, 0x9B, 0xA0); // Code segment
    GDT_SetEntry(2, 0, 0xFFFFFFFF, 0x93, 0x00); // Data segment
    GDT_SetEntry(3, 0, 0xFFFFFFFF, 0xFB, 0xD0); // User mode code segment (32bit)
    GDT_SetEntry(4, 0, 0xFFFFFFFF, 0xF3, 0x00); // User mode data segment
    GDT_SetEntry(5, 0, 0xFFFFFFFF, 0xFB, 0xA0); // User mode code segment (64bit)
    GDT_SetTSS(6, (uint64_t)&coreLocalData->sys_tss, sizeof(tss_struct), 0xE9);

    __asm__ volatile ("lgdt (%0)" :: "r" (&coreLocalData->gdt_table));

    __asm__ volatile (
        "pushq %rax\n\t"
        "mov $flush, %rax\n\t"
        "pushq %rax\n\t"
        "retq\n\t"
        "flush:\n\t"
        "mov $0x10, %ax\n\t"
        "mov %ax, %ds\n\t"
        "mov %ax, %es\n\t"
        "mov %ax, %fs\n\t"
        "mov %ax, %gs\n\t"
        "mov %ax, %ss\n\t"

        //Set the TSS
        "mov $0x33, %ax\n\t"
        "ltr %ax\n\t"
        "popq %rax\n\t"
    );

    return; //Don't enable interrupts yet
}

void
GDT_SetTSS(int num,
           uint64_t base,
           uint32_t limit,
           uint8_t access) {
    GDT_SetEntry(num, (uint32_t)base, limit, access, 0);
    GDT_SetEntry(num + 1, 0, 0, 0, 0);

    coreLocalData->gdt_entries[num + 1].limit_low = (uint16_t)(base >> 32);
    coreLocalData->gdt_entries[num + 1].base_low = (uint16_t)(base >> (32 + 16));
}

void
GDT_SetEntry(int num,
             uint32_t base,
             uint32_t limit,
             uint8_t access,
             uint8_t gran) {
    coreLocalData->gdt_entries[num].base_low    = (base & 0xFFFF);
    coreLocalData->gdt_entries[num].base_mid = (base >> 16) & 0xFF;
    coreLocalData->gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    coreLocalData->gdt_entries[num].limit_low   = (limit & 0xFFFF);
    coreLocalData->gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    coreLocalData->gdt_entries[num].granularity |= gran & 0xF0;
    coreLocalData->gdt_entries[num].access     = access;
}

void
SetInterruptStack(void* sp) {
    coreLocalData->sys_tss.rsp0 = (uint64_t)sp;
}

void
GDT_SetIST(uint8_t ist,
           uint64_t stack) {
    switch(ist) {
    case 1:
        coreLocalData->sys_tss.ist1 = stack;
        break;
    case 2:
        coreLocalData->sys_tss.ist2 = stack;
        break;
    case 3:
        coreLocalData->sys_tss.ist3 = stack;
        break;
    case 4:
        coreLocalData->sys_tss.ist4 = stack;
        break;
    case 5:
        coreLocalData->sys_tss.ist5 = stack;
        break;
    case 6:
        coreLocalData->sys_tss.ist6 = stack;
        break;
    case 7:
        coreLocalData->sys_tss.ist7 = stack;
        break;
    default:
        break;
    }
}