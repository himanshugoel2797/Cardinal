#ifndef _IDT_H_
#define _IDT_H_

#include "types.h"

#define IDT_ENTRY_COUNT 256
#define IDT_ENTRY_HANDLER_SIZE 0x20

#define IRQ(n) (n + 32)

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rdx, rcx, rbx, rax; // Pushed by pusha.
    uint64_t int_no, err_code; // Interrupt number and error code (if applicable)
    uint64_t rip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} __attribute__((packed)) Registers;

typedef void (*IDT_InterruptHandler)(Registers*);

//extern char idt_handlers[IDT_ENTRY_COUNT][IDT_ENTRY_HANDLER_SIZE];
//extern void(*idt_handler_calls[IDT_ENTRY_COUNT] ) (Registers*);

void
IDT_Initialize(void);

void
IDT_SetEntry(uint8_t index,
             uint64_t base,
             uint16_t selector,
             uint8_t flags,
             uint8_t ist);

void
IDT_ChangeEntry(uint8_t index,
                uint16_t selector,
                uint8_t flags,
                uint8_t ist);


void
IDT_MainHandler(Registers *regs);

void
IDT_RegisterHandler(uint8_t intNum,
                    void (*handler)(Registers*));


void 
IDT_RaiseInterrupt(uint32_t int_no);

#endif /* end of include guard: _IDT_H_ */
