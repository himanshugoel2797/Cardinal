#include "apic.h"
#include "priv_apic.h"
#include "pic/pic.h"

#include "idt.h"
#include "cpuid.h"
#include "serial/COM.h"

#include "utils/common.h"
#include "utils/native.h"
#include "managers.h"

uint32_t *apic_base_addr = 0;

uint8_t
APIC_LocalInitialize(void)
{
    PIC_MaskAll();    //disable all interrupts from the PIC

    //Register the APIC interrupt handlers
    for(int i = 32; i < IDT_ENTRY_COUNT; i++)
        {
            APIC_FillHWInterruptHandler(idt_handlers[i], i, i - 32);
            IDT_SetEntry(i, (uint32_t)idt_handlers[i], 0x08, 0x8E);
        }

    uint64_t apic_base_msr = rdmsr(IA32_APIC_BASE);
    apic_base_addr = (uint32_t*)((uint32_t)apic_base_msr & 0xfffff000);


    apic_base_msr |= (1 << 11); //Enable the apic
    wrmsr(IA32_APIC_BASE, apic_base_msr);

    uint32_t tpr = APIC_Read(APIC_TPR);
    tpr &= ~(0xFF);
    APIC_Write(APIC_TPR, tpr);

    //Set the spurious vector
    uint32_t svr = APIC_Read(APIC_SVR);
    svr |= 0xFF;
    APIC_Write(APIC_SVR, svr);

    APIC_SetEnableMode(1);

    return 0;
}

void
APIC_Virtualize(void)
{
    //Adjust the base address for virtual memory
    apic_base_addr = VIRTUALIZE_HIGHER_MEM_OFFSET(apic_base_addr);
}

void
APIC_Write(uint32_t reg,
           uint32_t val)
{
    apic_base_addr[reg/4] = val;
}

uint32_t
APIC_Read(uint32_t reg)
{
    return apic_base_addr[reg/4];
}

void
APIC_SetEnableInterrupt(uint32_t interrupt,
                        int enableMode)
{
    if (interrupt < APIC_TIMER || interrupt > APIC_ERR) return;
    uint32_t val = APIC_Read(interrupt);
    val = SET_VAL_BIT(val, 16, (~enableMode & 1));
    APIC_Write(interrupt, val);
}

void
APIC_SetVector(uint32_t interrupt,
               uint8_t vector)
{
    if (interrupt < APIC_TIMER || interrupt > APIC_ERR) return;
    uint32_t val = APIC_Read(interrupt);
    val |= vector;
    APIC_Write(interrupt, val);
}

void
APIC_SetDeliveryMode(uint32_t interrupt,
                     uint8_t vector)
{
    if (interrupt < APIC_TIMER || interrupt > APIC_LINT1) return;
    uint32_t val = APIC_Read(interrupt);
    val |= (((uint32_t)vector & 7) << 8);
    APIC_Write(interrupt, val);
}

void
APIC_SetTriggerMode(uint32_t interrupt,
                    uint8_t vector)
{
    if (interrupt < APIC_LINT0 || interrupt > APIC_LINT1) return;
    uint32_t val = APIC_Read(interrupt);
    val |= (((uint32_t)vector & 1) << 15);
    APIC_Write(interrupt, val);
}

void
APIC_SetPolarity(uint32_t interrupt,
                 uint8_t vector)
{
    if (interrupt < APIC_LINT0 || interrupt > APIC_LINT1) return;
    uint32_t val = APIC_Read (interrupt);
    val |= (((uint32_t)vector & 1) << 13);
    APIC_Write (interrupt, val);
}

void
APIC_SetTimerMode(uint8_t mode)
{
    uint32_t val = APIC_Read(APIC_TIMER);
    val |= ((((uint32_t)mode) & 3) << 17);
    APIC_Write(APIC_TIMER, val);
}

void
APIC_SetTimerDivisor(uint8_t divisor)
{
    uint8_t val = 0;
    //Convert the divisor into the code to be written
    if (divisor > 16)
        {
            val |= (1<<3);
            divisor /= 16;
        }

    if (divisor == 16) val |= 3;
    else if (divisor == 1) val = (1<<3)|3;
    else val |= ((divisor >> 2) & 3);
    APIC_Write (0x3E0, val);
}

void
APIC_SetTimerValue(uint32_t val)
{
    APIC_Write(APIC_TIMER_VAL, val);
}

uint32_t
APIC_GetTimerValue(void)
{
    return APIC_Read(APIC_TIMER_VAL);
}

void
APIC_SetEnableMode(uint8_t enabled)
{
    uint32_t svr = APIC_Read(APIC_SVR);
    svr = SET_VAL_BIT(svr, 8, (enabled & 1));
    APIC_Write(APIC_SVR, svr);
}

uint8_t
APIC_GetID(void)
{
    return (uint8_t)APIC_Read(APIC_ID);
}

void
APIC_FillHWInterruptHandler(char *idt_handler,
                            uint8_t intNum,
                            uint8_t irqNum)
{
    int index = 0;

    //Push dummy error code if the interrupt doesn't do so
    idt_handler[index++] = 0x68;         //Push
    idt_handler[index++] = irqNum;
    idt_handler[index++] = 0;
    idt_handler[index++] = 0;
    idt_handler[index++] = 0;

    idt_handler[index++] = 0x68; //Push
    idt_handler[index++] = intNum; //Push the interrupt number to stack
    idt_handler[index++] = 0;
    idt_handler[index++] = 0;
    idt_handler[index++] = 0;

    //push jump address and ret
    idt_handler[index++] = 0x68;
    idt_handler[index++] = (int)APIC_DefaultHandler & 0x000000FF;  //Calculate the offset
    idt_handler[index++] = ((int)APIC_DefaultHandler >> 8)& 0x000000FF;  //Calculate the offset
    idt_handler[index++] = ((int)APIC_DefaultHandler >> 16)& 0x000000FF;  //Calculate the offset
    idt_handler[index++] = ((int)APIC_DefaultHandler >> 24)& 0x000000FF;  //Calculate the offset

    idt_handler[index++] = 0xC3;
}

void
APIC_MainHandler(Registers *regs);

__attribute__((naked, noreturn))
void
APIC_DefaultHandler(void)
{
    asm volatile(
        "pusha\n\t"
        "mov %ds, %ax\n\t"
        "push %eax\n\t"
        "mov $0x10, %ax\n\t"
        "mov %ax, %ds\n\t"
        "mov %ax, %es\n\t"
        "mov %ax, %fs\n\t"
        "mov %ax, %gs\n\t"
        "push %esp\n\t"
    );

    asm volatile(
        "call APIC_MainHandler"
    );
    asm volatile(
        "pop %ebx\n\t"
        "pop %ebx\n\t"
        "mov %bx, %ds\n\t"
        "mov %bx, %es\n\t"
        "mov %bx, %fs\n\t"
        "mov %bx, %gs\n\t"
        "popa\n\t"
        "add $8, %esp\n\t"
        "iret\n\t"
    );
}

void
APIC_SendEOI(uint8_t int_num)
{
//Test if this is in service and send EOI
    uint32_t isr_msr = APIC_ISR_BASE + ((int_num / 32) * 0x10);
    uint32_t val = APIC_Read(isr_msr);
    if( CHECK_BIT(val, (int_num % 32)) )
        {
            APIC_Write(APIC_EOI, 0xDEADBEEF);
        }
}

void
APIC_MainHandler(Registers *regs)
{
    IDT_MainHandler(regs);
}
