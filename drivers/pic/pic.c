#include "pic.h"
#include "idt.h"
#include "utils/native.h"
#include "utils/common.h"

#define ICW1_ICW4 0x01    /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL  0x08    /* Level triggered (edge) mode */
#define ICW1_INIT 0x10    /* Initialization - required! */

#define ICW4_8086 0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM 0x10    /* Special fully nested (not) */

#define PIC_READ_IRR 0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b    /* OCW3 irq service next CMD read */
#define PIC_RESET 0x20  /*Reset signal*/

void PIC_FillHWInterruptHandler(char *idt_handler, uint8_t intNum, uint8_t irqNum);
void PIC_DefaultHandler();

uint32_t PIC_Initialize()
{
    PIC_SetOffset(32, 40); //Remap PIC1 to raise interrupts in 32-39 and PIC1 to raise interrupts in 40-47

    for(int i = 32; i < 48; i++)
        {
            PIC_FillHWInterruptHandler(idt_handlers[i], i, i - 32);
            IDT_SetEntry(i, (uint32_t)idt_handlers[i], 0x08, 0x8E);
        }
    return 0;
}

//Necessary for APIC operation
void PIC_MaskAll()
{
    outb(PIC2_DATA, 0xFF);
    outb(PIC1_DATA, 0xFF);
}

void PIC_MaskIRQ(uint8_t irq)
{
    uint16_t port = PIC1_DATA;
    if(irq >= 40)
        {
            port = PIC2_DATA;
            irq -= 40;
        }
    outb(port, SET_BIT(inb(port), (irq - 32)));
}

void PIC_UnMaskIRQ(uint8_t irq)
{
    uint16_t port = PIC1_DATA;
    if(irq >= 40)
        {
            port = PIC2_DATA;
            irq -= 40;
        }
    outb(port, CLEAR_BIT(inb(port), (irq - 32)));
}

void PIC_SetOffset(int offset1, int offset2)
{
    char a1 = inb(PIC1_DATA);
    char a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4); // starts the initialization sequence (in cascade mode)
    outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
    outb(PIC1_DATA, offset1);           // ICW2: Master PIC vector offset
    outb(PIC2_DATA, offset2);           // ICW2: Slave PIC vector offset
    outb(PIC1_DATA, 4);                 // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC2_DATA, 2);                 // ICW3: tell Slave PIC its cascade identity (0000 0010)

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, a1); // restore saved masks.
    outb(PIC2_DATA, a2);
}

uint16_t PIC_GetReg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
    * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

void PIC_FillHWInterruptHandler(char *idt_handler, uint8_t intNum, uint8_t irqNum)
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
    idt_handler[index++] = (int)PIC_DefaultHandler & 0x000000FF;  //Calculate the offset
    idt_handler[index++] = ((int)PIC_DefaultHandler >> 8)& 0x000000FF;  //Calculate the offset
    idt_handler[index++] = ((int)PIC_DefaultHandler >> 16)& 0x000000FF;  //Calculate the offset
    idt_handler[index++] = ((int)PIC_DefaultHandler >> 24)& 0x000000FF;  //Calculate the offset

    idt_handler[index++] = 0xC3;
}

__attribute__((naked, noreturn))
void PIC_DefaultHandler()
{
    asm (
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

    asm (
        "call PIC_MainHandler"
    );
    asm (
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

void PIC_SendEOI(uint8_t irq)
{
    if( ((PIC_GetReg(PIC_READ_ISR) >> irq) & 1) == 1)
        {
            if(irq >= 40)
                {
                    outb(PIC2_COMMAND, PIC_RESET);
                }
            outb(PIC1_COMMAND, PIC_RESET);
        }
}

void PIC_MainHandler(Registers *regs)
{
    if( ((PIC_GetReg(PIC_READ_ISR) >> regs->int_no) & 1) == 1)
        {
            IDT_MainHandler(regs);
        }
}
