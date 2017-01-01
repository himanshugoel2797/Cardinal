/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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

typedef struct APIC_APLS_Data {
    uint32_t volatile *apic_base_addr;
    uint64_t apic_frequency;
    uint64_t tsc_frequency;
    uint8_t tsc_useful;
} APIC_APLS_Data;
static volatile APIC_APLS_Data* apic_data = NULL;

#define ICW4_8086 0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW1_ICW4 0x01    /* ICW4 (not) needed */
#define ICW1_INIT 0x10    /* Initialization - required! */

#define TICK_CNT (PIT_FREQUENCY_HZ/100)

void
APIC_LockPIC(void) {

    outb(0x20, ICW1_INIT+ICW1_ICW4); // starts the initialization sequence (in cascade mode)
    outb(0xA0, ICW1_INIT+ICW1_ICW4);
    outb(0x21, 32);           // ICW2: Master PIC vector offset
    outb(0xA1, 40);           // ICW2: Slave PIC vector offset
    outb(0x21, 4);                 // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 2);                 // ICW3: tell Slave PIC its cascade identity (0000 0010)

    outb(0x21, ICW4_8086);
    outb(0xA1, ICW4_8086);

    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);    //disable all interrupts from the PIC

}

static volatile int32_t pit_ticks;
static volatile uint32_t apic_timer_value;
static volatile uint64_t final_tsc_value;

static void
APIC_TimerCallibrate(uint32_t int_no,
                     uint32_t err_code) {
    err_code = 0;
    if(int_no == IRQ(0) && pit_ticks >= 0) {
        pit_ticks++;
        if(pit_ticks == TICK_CNT) {
            apic_timer_value = APIC_GetTimerValue();
            final_tsc_value = APIC_GetTSCValue();
        }
    }
}

static uint32_t rollover_cnt;

static void
APIC_TimerCallibrateRollover(uint32_t int_no,
                             uint32_t err_code) {
    if(int_no == IRQ(1))rollover_cnt++;
    APIC_SendEOI(int_no);
    if(err_code)return;
}

uint8_t
APIC_LocalInitialize(void) {
    for(int i = 32; i < 256; i++) {
        InterruptHandler h = NULL;
        GetInterruptHandler(i, &h);
        RegisterInterruptHandler(i, h);
    }

    uint64_t apic_base_msr = rdmsr(IA32_APIC_BASE);
    if(apic_data == NULL)apic_data = (APIC_APLS_Data*)AllocateAPLSMemory(sizeof(APIC_APLS_Data));


    apic_data->apic_base_addr = (uint32_t*)GetVirtualAddress(CachingModeUncachable ,(void*)(apic_base_msr & 0xfffff000));

    apic_base_msr |= (1 << 11); //Enable the apic
    wrmsr(IA32_APIC_BASE, apic_base_msr);

    uint32_t dfr = APIC_Read(APIC_DFR);
    dfr &= ~(0x00);
    APIC_Write(APIC_DFR, dfr);

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
APIC_CallibrateTimer(void) {

    //Callibrate the APIC Timer to the PIT
    rollover_cnt = 0;
    pit_ticks = -1;
    apic_timer_value = 0;

    apic_data->tsc_useful = 0xff;
    APIC_IsTSCReliable();
    APIC_EnableTSC();

    RegisterInterruptHandler(IRQ(1), APIC_TimerCallibrateRollover);
    APIC_SetVector(APIC_TIMER, IRQ(1));
    APIC_SetTimerValue(0);
    APIC_SetTimerDivisor(16);


    RegisterInterruptHandler(IRQ(0), APIC_TimerCallibrate);
    APIC_SetTimerValue(-1);
    APIC_SetTimerMode(APIC_TIMER_PERIODIC);

    PIT_SetEnableMode(ENABLED);
    APIC_SetEnableInterrupt(APIC_TIMER, ENABLED);
    uint64_t init_tsc_value = APIC_GetTSCValue();

    pit_ticks = 0;

    while(pit_ticks < TICK_CNT);

    PIT_SetEnableMode(DISABLED);
    APIC_SetEnableInterrupt(APIC_TIMER, DISABLED);
    APIC_SetTimerValue(0);  //Disable the timer

    uint64_t apic_ticks = rollover_cnt * 0xFFFFFFFF + (0xFFFFFFFF - apic_timer_value);
    //__asm__("cli\n\thlt" :: "a"(apic_ticks));

    apic_ticks *= PIT_FREQUENCY_HZ;
    apic_ticks /= TICK_CNT;

    uint64_t tsc_ticks = final_tsc_value - init_tsc_value;
    tsc_ticks *= PIT_FREQUENCY_HZ;
    tsc_ticks /= TICK_CNT;

    apic_data->apic_frequency = apic_ticks;
    apic_data->tsc_frequency = tsc_ticks;
}

uint64_t
APIC_GetTimerFrequency(void) {
    return apic_data->apic_frequency;
}

void
APIC_EnableTSC(void) {
    if(apic_data->tsc_useful) {
        register uint64_t dummy = 0;
        __asm__("mov %%cr4, %[dummy]\n\t"
                "or $4, %[dummy]\n\t"
                "mov %[dummy], %%cr4\n\t" :: [dummy]"r"(dummy));
    }
}

bool
APIC_IsTSCReliable(void) {

    if(apic_data->tsc_useful == 0xff) {
        //Check for TSC support, then check for invariant TSC support
        apic_data->tsc_useful = 0;

        CPUID_RequestInfo(1, 0);    //First CPUID page
        if(CPUID_FeatureIsAvailable(CPUID_EDX, CPUID_FEAT_EDX_TSC)) {


            CPUID_RequestInfo(0x80000007, 0);
            if(CPUID_FeatureIsAvailable(CPUID_EDX, (1 << 8))) {
                apic_data->tsc_useful = 1;
            }
        }
    }

    return apic_data->tsc_useful;
}

uint64_t
APIC_GetTSCFrequency(void) {
    return apic_data->tsc_frequency;
}

uint64_t
APIC_GetTSCValue(void) {
    uint64_t lo = 0;
    uint64_t hi = 0;
    if(apic_data->tsc_useful)__asm__("rdtsc" : "=a"(lo), "=d"(hi));

    return (hi << 32) | lo;
}

void
APIC_Write(uint32_t reg,
           uint32_t val) {
    apic_data->apic_base_addr[reg/sizeof(uint32_t)] = val;
}

uint32_t
APIC_Read(uint32_t reg) {
    return apic_data->apic_base_addr[reg/sizeof(uint32_t)];
}

void
APIC_SetEnableInterrupt(uint32_t interrupt,
                        int enableMode) {
    if (interrupt < APIC_TIMER || interrupt > APIC_ERR) return;
    uint32_t val = APIC_Read(interrupt);
    val = SET_VAL_BIT(val, 16, (~enableMode & 1));
    APIC_Write(interrupt, val);
}

void
APIC_SetVector(uint32_t interrupt,
               uint8_t vector) {
    if (interrupt < APIC_TIMER || interrupt > APIC_ERR) return;
    uint32_t val = APIC_Read(interrupt);
    val &= ~0xFF;
    val |= vector;
    APIC_Write(interrupt, val);
}

void
APIC_SetDeliveryMode(uint32_t interrupt,
                     uint8_t vector) {
    if (interrupt < APIC_TIMER || interrupt > APIC_LINT1) return;
    uint32_t val = APIC_Read(interrupt);
    val |= (((uint32_t)vector & 7) << 8);
    APIC_Write(interrupt, val);
}

void
APIC_SetTriggerMode(uint32_t interrupt,
                    uint8_t vector) {
    if (interrupt < APIC_LINT0 || interrupt > APIC_LINT1) return;
    uint32_t val = APIC_Read(interrupt);
    val |= (((uint32_t)vector & 1) << 15);
    APIC_Write(interrupt, val);
}

void
APIC_SetPolarity(uint32_t interrupt,
                 uint8_t vector) {
    if (interrupt < APIC_LINT0 || interrupt > APIC_LINT1) return;
    uint32_t val = APIC_Read (interrupt);
    val |= (((uint32_t)vector & 1) << 13);
    APIC_Write (interrupt, val);
}

void
APIC_SetTimerMode(uint8_t mode) {
    uint32_t val = APIC_Read(APIC_TIMER);
    val &= ~(3 << 17);
    val |= ((((uint32_t)mode) & 3) << 17);
    APIC_Write(APIC_TIMER, val);
}

void
APIC_SetTimerDivisor(uint8_t divisor) {
    uint8_t val = 0;
    //Convert the divisor into the code to be written
    if (divisor > 16) {
        val |= (1<<3);
        divisor /= 16;
    }

    if (divisor == 16) val |= 0b0011;
    else if (divisor == 1) val = 0b1011;
    else val |= ((divisor >> 2) & 3);
    APIC_Write (0x3E0, val);
}

void
APIC_SetTimerValue(uint32_t val) {
    APIC_Write(APIC_TIMER_VAL, val);
}

uint32_t
APIC_GetTimerValue(void) {
    return APIC_Read(APIC_TIMER_CURVAL);
}

void
APIC_SetEnableMode(uint8_t enabled) {
    uint32_t svr = APIC_Read(APIC_SVR);
    svr = SET_VAL_BIT(svr, 8, (enabled & 1));
    APIC_Write(APIC_SVR, svr);
}

uint8_t
APIC_GetID(void) {
    if(apic_data != NULL)
        return (uint8_t)APIC_Read(APIC_ID);
    else
        return (uint8_t)-1;
}

void
APIC_SendEOI(uint8_t int_num) {
    //Test if this is in service and send EOI
    uint32_t isr_msr = APIC_ISR_BASE + ((int_num / 32) * 0x10);
    uint32_t val = APIC_Read(isr_msr);
    if( CHECK_BIT(val, (int_num % 32)) ) {
        APIC_Read(APIC_EOI);
        APIC_Write(APIC_EOI, 0);
    }
}

void
APIC_SendIPI(uint32_t dest,
             uint8_t destination_shorthand,
             uint8_t vector,
             uint8_t delivery_mode) {
    uint32_t hi = (dest << 24);
    uint32_t lo = ((destination_shorthand & 3) << 18) | (1 << 14) | (delivery_mode & 7) << 8 | vector;

    APIC_Write(APIC_ICR_HI, hi);
    APIC_Write(APIC_ICR_LO, lo);
}

static void
APIC_MainHandler(Registers *regs) {
    __asm__ volatile("mov %0, %%rax\n\thlt" :: "ra"(regs->int_no));
    APIC_SendEOI(regs->int_no);
}
