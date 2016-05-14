#include "apic.h"
#include "priv_apic.h"
#include "memory.h"
#include "interrupts.h"
#include "IDT/idt.h"
#include "CPUID/cpuid.h"
#include "pit/pit.h"

#include "common.h"
#include "utils/native.h"
#include "managers.h"

static void
APIC_MainHandler(Registers *regs);
uint32_t *apic_base_addr = 0;

#define ICW4_8086 0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW1_ICW4 0x01    /* ICW4 (not) needed */
#define ICW1_INIT 0x10    /* Initialization - required! */

void
APIC_LockPIC(void)
{
    char a1 = inb(0x21);
    char a2 = inb(0xA1);

    outb(0x20, ICW1_INIT+ICW1_ICW4); // starts the initialization sequence (in cascade mode)
    outb(0xA0, ICW1_INIT+ICW1_ICW4);
    outb(0x21, 32);           // ICW2: Master PIC vector offset
    outb(0xA1, 40);           // ICW2: Slave PIC vector offset
    outb(0x21, 4);                 // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 2);                 // ICW3: tell Slave PIC its cascade identity (0000 0010)

    outb(0x21, ICW4_8086);
    outb(0xA1, ICW4_8086);

    outb(0x21, a1); // restore saved masks.
    outb(0xA1, a2);

    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);    //disable all interrupts from the PIC

}

static int32_t pit_ticks;
static uint32_t apic_timer_value;

static void
APIC_TimerCallibrate(uint32_t int_no,
                     uint32_t err_code)
{
    err_code = 0;
    if(int_no == IRQ(0) && pit_ticks >= 0){
        pit_ticks++;
        if(pit_ticks == 200)apic_timer_value = APIC_GetTimerValue();
    }

    APIC_SendEOI(int_no);
}

static uint32_t rollover_cnt;

static void
APIC_TimerCallibrateRollover(uint32_t int_no,
                             uint32_t err_code)
{
    if(int_no == IRQ(1))rollover_cnt++;
    APIC_SendEOI(int_no);
    if(err_code)return;
}

uint8_t
APIC_LocalInitialize(void)
{
    for(int i = 32; i < 256; i++)
        {
            IDT_RegisterHandler(i, APIC_MainHandler);
        }

    uint64_t apic_base_msr = rdmsr(IA32_APIC_BASE);
    apic_base_addr = (uint32_t*)GetVirtualAddress(CachingModeUncachable ,(void*)(apic_base_msr & 0xfffff000));

    apic_base_msr |= (1 << 11); //Enable the apic
    wrmsr(IA32_APIC_BASE, apic_base_msr);

    uint32_t tpr = APIC_Read(APIC_TPR);
    tpr &= ~(0xFF);
    APIC_Write(APIC_TPR, tpr);

    //Set the spurious vector
    uint32_t svr = APIC_Read(APIC_SVR);
    svr |= 0xFF;
    APIC_Write(APIC_SVR, svr);

    APIC_SetEnableMode(TRUE);
    return 0;
}

void
APIC_CallibrateTimer(void)
{

    //Callibrate the APIC Timer to the PIT
    rollover_cnt = 0;
    pit_ticks = -1;
    apic_timer_value = 0;

    AllocateAPLS(APIC_GetID());
    RegisterInterruptHandler(IRQ(1), APIC_TimerCallibrateRollover);
    APIC_SetVector(APIC_TIMER, IRQ(1));
    APIC_SetTimerValue(0);
    APIC_SetTimerDivisor(16);


    PIT_Initialize();
    RegisterInterruptHandler(IRQ(0), APIC_TimerCallibrate);
    APIC_SetTimerValue(-1);
    APIC_SetTimerMode(APIC_TIMER_PERIODIC);
    
    PIT_SetEnableMode(ENABLED);
    APIC_SetEnableInterrupt(APIC_TIMER, ENABLED);
    pit_ticks = 0;

    while(pit_ticks < 200);

    PIT_SetEnableMode(DISABLED);
    APIC_SetEnableInterrupt(APIC_TIMER, DISABLED);

    uint64_t apic_ticks = rollover_cnt * 0xFFFFFFFF + (0xFFFFFFFF - apic_timer_value);
    apic_ticks *= PIT_FREQUENCY_HZ;
    apic_ticks /= 200;

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

    if (divisor == 16) val |= 0b0011;
    else if (divisor == 1) val = 0b1011;
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
    return APIC_Read(APIC_TIMER_CURVAL);
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
APIC_SendEOI(uint8_t int_num)
{
    //Test if this is in service and send EOI
    uint32_t isr_msr = APIC_ISR_BASE + ((int_num / 32) * 0x10);
    uint32_t val = APIC_Read(isr_msr);
    if( CHECK_BIT(val, (int_num % 32)) )
        {
            APIC_Write(APIC_EOI, 0);
        }
}

void
APIC_SendIPI(uint32_t dest,
             uint8_t vector,
             uint8_t delivery_mode)
{
    uint32_t hi = (dest << 24);
    uint32_t lo = (1 << 14) | (delivery_mode & 7) << 8 | vector;

    APIC_Write(APIC_ICR_HI, hi);
    APIC_Write(APIC_ICR_LO, lo);
}

static void
APIC_MainHandler(Registers *regs)
{
    APIC_SendEOI(regs->int_no);
}
