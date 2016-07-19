#include "types.h"
#include "interrupts.h"
#include "common.h"
#include "IDT/idt.h"
#include "apic/io_apic/io_apic.h"
#include "apic/apic.h"
#include "memory.h"

static volatile InterruptHandler intHandlers[256] = {0};
static Registers volatile *regs_saved = NULL;
static uint64_t volatile int_stack = 0;

uint32_t
RequestInterruptVectorBlock(uint32_t vectorCount) {
    uint32_t score = 0;
    int i = 0;
    for(; i < 256; i++) {
        if(i > 0 && intHandlers[i - 1] != NULL)score = 0;
        if(intHandlers[i] == NULL)score++;
        if(score == vectorCount)break;
    }

    if(i + vectorCount >= 256)return -1;
    else return i;
}


void
ShadowInterruptHandler(Registers *regs) {
    if(regs_saved == NULL) {
        regs_saved = AllocateAPLSMemory(sizeof(Registers));
    }

    if(int_stack == 0)
        int_stack = (uint64_t)AllocateAPLSMemory(4096) + 4096;

    memcpy((void*)regs_saved, regs, sizeof(Registers));

    if(intHandlers[regs->int_no] != NULL) {
        __asm__ volatile("xchgq %%rax, %%rsp\n\t"
                         "pushq %%rax\n\t"
                         "callq *%%rbx\n\t"
                         "popq %%rax\n\t"
                         "xchgq %%rax, %%rsp\n\t"
                         :: "a"(int_stack), "b"(intHandlers[regs->int_no]),
                         "D"(regs->int_no), "S"(regs->err_code) :);
    }

    memset((void*)regs_saved, 0, sizeof(Registers));
    if(regs->int_no > 31)APIC_SendEOI(regs->int_no);
}

void
HandleInterruptNoReturn(uint32_t vector) {
    memset((void*)regs_saved, 0, sizeof(Registers));
    if(vector > 31)APIC_SendEOI(vector);
}

uint32_t
RegisterInterruptHandler(uint32_t int_no,
                         InterruptHandler handler) {
    if(int_no >= 256)return -1;
    intHandlers[int_no] = handler;
    IDT_RegisterHandler(int_no, ShadowInterruptHandler);
    return 0;
}

uint32_t
GetInterruptHandler(uint32_t int_no,
                    InterruptHandler *handler) {
    if(int_no >= 256)return -1;
    if(handler != NULL)*handler = intHandlers[int_no];
    return 0;
}

void
RemoveInterruptHandler(uint32_t id) {
    if(id < 256 && id > 31) {
        intHandlers[id] = NULL;
    }
}

void
SetInterruptEnableMode(uint32_t vector, bool enableMode) {
    IOAPIC_SetEnableMode((uint8_t)vector, enableMode);
}

void
RaiseInterrupt(uint32_t int_no) {
    IDT_RaiseInterrupt(int_no);
}

void*
GetSavedInterruptState(void) {
    return (void*)regs_saved;
}