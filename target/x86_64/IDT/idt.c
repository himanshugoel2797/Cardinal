#include "types.h"
#include "idt.h"

void
IDT_DefaultHandler(void);

void
IDT_FillSWInterruptHandler(char *idt_handler,
                           uint8_t intNum,
                           uint8_t pushToStack);


typedef struct {
    uint16_t base_lo; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero; // unused, set to 0 (bits (0,1,2) are IST)
    uint8_t type_attr; // type and attributes, see below
    uint16_t base_mid; // offset bits 16..31
    uint32_t base_hi;	//offset bits 32..63
    uint32_t zero0;		// unused, set to 0
} __attribute__((packed)) IDTEntry;

//Describes the pointer to the IDT entry
typedef struct {
    uint16_t limit; //Size
    void *base; //Location
} __attribute__((packed)) IDTPtr;


static IDTEntry
idt_entries[IDT_ENTRY_COUNT];

static IDTPtr
idt_table;

static char
idt_handlers[IDT_ENTRY_COUNT][IDT_ENTRY_HANDLER_SIZE];

static void
(*idt_handler_calls[IDT_ENTRY_COUNT]) (Registers*);

void
IDT_Initialize(void) {
    idt_table.limit = (sizeof(IDTEntry) * IDT_ENTRY_COUNT) - 1;
    idt_table.base = (void*)&idt_entries;

    uint64_t physAddr = (uint64_t)&idt_table;

    //ensure interrupts are disabled
    __asm__ volatile("cli");
    __asm__ volatile("lidt (%%rax)" :: "a" (physAddr));         //Load the IDT
    //__asm__ volatile("hlt");

    for(int i = 0; i < IDT_ENTRY_COUNT; i++) {
        IDT_SetEntry(0, 0, 0, 0, 0);
        idt_handler_calls[i] = NULL;
    }

    //Fill the IDT
    int pushesToStack = 1;
    for(int i = 0; i < IDT_ENTRY_COUNT; i++) {
        //Setup the hardware interrupts
        if(i == 8 || (i >= 10 && i <= 14)) pushesToStack = 0;
        IDT_FillSWInterruptHandler(idt_handlers[i], i, pushesToStack);  //If pushesToStack is non-zero, the value will be pushed to stack
        IDT_SetEntry(i, (uint64_t)idt_handlers[i], 0x08, 0x8E, 0);
        pushesToStack = 1;
    }

    return;
}

void IDT_SetEntry(uint8_t index, uint64_t base, uint16_t selector, uint8_t flags, uint8_t ist) {
    idt_entries[index].base_lo = base & 0x0000FFFF;
    idt_entries[index].base_mid = (base >> 16) & 0x0000FFFF;
    idt_entries[index].base_hi = (base >> 32);
    idt_entries[index].type_attr = flags;
    idt_entries[index].zero = ist & 0x7;
    idt_entries[index].selector = selector;
}

void IDT_ChangeEntry(uint8_t index, uint16_t selector, uint8_t flags, uint8_t ist) {
    idt_entries[index].type_attr = flags;
    idt_entries[index].zero = ist & 0x7;
    idt_entries[index].selector = selector;
}

void IDT_FillSWInterruptHandler(char *idt_handler, uint8_t intNum, uint8_t pushToStack) {
    int index = 0;

    //Push dummy error code if the interrupt doesn't do so
    if(pushToStack) {
        idt_handler[index++] = 0x6a; //Push
        idt_handler[index++] = pushToStack;
    }

    idt_handler[index++] = 0x6a; //Push
    idt_handler[index++] = intNum; //Push the interrupt number to stack

    //push jump address and ret
    idt_handler[index++] = 0x50;	//push %%rax
    idt_handler[index++] = 0x48;	//movq IDT_DefaultHandler, %%rax
    idt_handler[index++] = 0xb8;
    idt_handler[index++] = ((uint64_t)(IDT_DefaultHandler)) & 0xff;
    idt_handler[index++] = ((uint64_t)(IDT_DefaultHandler) >> (8)) & 0xff;
    idt_handler[index++] = ((uint64_t)(IDT_DefaultHandler) >> (16)) & 0xff;
    idt_handler[index++] = ((uint64_t)(IDT_DefaultHandler) >> (24)) & 0xff;
    idt_handler[index++] = ((uint64_t)(IDT_DefaultHandler) >> 32) & 0xff;
    idt_handler[index++] = ((uint64_t)(IDT_DefaultHandler) >> (32 + 8)) & 0xff;
    idt_handler[index++] = ((uint64_t)(IDT_DefaultHandler) >> (32 + 16)) & 0xff;
    idt_handler[index++] = ((uint64_t)(IDT_DefaultHandler) >> (32 + 24)) & 0xff;
    //*(uint32_t*)&idt_handler[index] = 0x11223344;
    //index += 3;
    idt_handler[index++] = 0x50;	//push %%rax
    idt_handler[index++] = 0xC3;	//retq
}

__attribute__((naked, noreturn))
void IDT_DefaultHandler() {
    __asm__ volatile(
        "pushq %rbx\n\t"
        "pushq %rcx\n\t"
        "pushq %rdx\n\t"
        "pushq %rbp\n\t"
        "pushq %rsi\n\t"
        "pushq %rdi\n\t"
        "pushq %r8\n\t"
        "pushq %r9\n\t"
        "pushq %r10\n\t"
        "pushq %r11\n\t"
        "pushq %r12\n\t"
        "pushq %r13\n\t"
        "pushq %r14\n\t"
        "pushq %r15\n\t"
        "movq %rsp, %rdi\n\t"
        "callq IDT_MainHandler\n\t"
        "popq %r15\n\t"
        "popq %r14\n\t"
        "popq %r13\n\t"
        "popq %r12\n\t"
        "popq %r11\n\t"
        "popq %r10\n\t"
        "popq %r9\n\t"
        "popq %r8\n\t"
        "popq %rdi\n\t"
        "popq %rsi\n\t"
        "popq %rbp\n\t"
        "popq %rdx\n\t"
        "popq %rcx\n\t"
        "popq %rbx\n\t"
        "popq %rax\n\t"
        "add $16, %rsp\n\t"
        "iretq\n\t"
    );
}

void IDT_MainHandler(Registers *regs) {
    //__asm__ volatile("hlt" :: "a"(regs->err_code));
    if(idt_handler_calls[regs->int_no] != NULL) idt_handler_calls[regs->int_no](regs);
}

void IDT_RegisterHandler(uint8_t intNum, void (*handler)(Registers*)) {
    idt_handler_calls[intNum] = handler;
}
