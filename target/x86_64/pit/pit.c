#include "pit.h"
#include "utils/native.h"
#include "IDT/idt.h"
#include "apic/apic.h"
#include "interrupts.h"

//NOTE: The PIT should be set to always be running at 10027 Hz, not high enough to constantly interrupt stuff, not too low to be too slow for general use


uint32_t curFrequency = 0;
void
PIT_SetFrequency(uint8_t channel,
                 uint8_t access,
                 uint8_t mode,
                 uint8_t valType,
                 uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    curFrequency = 1193180 / divisor;

    channel &= 3;
    access &= 3;
    mode &= 7;
    valType &= 1;

    // Send the command byte.
    uint8_t cmd = 0;
    cmd = (channel << 6) | (access << 4) | (mode << 1) | valType;

    outb(PIT_CMD, cmd);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    if((access & PIT_ACCESS_LO_BYTE) == PIT_ACCESS_LO_BYTE) {
        uint8_t l = (uint8_t)(divisor & 0xFF);
        outb(0x40 + channel, l);
    }

    if((access & PIT_ACCESS_HI_BYTE) == PIT_ACCESS_HI_BYTE) {
        uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );
        outb(0x40 + channel, h);
    }
}

static volatile uint32_t pit_ticksToWait = 0;
static volatile uint32_t pit_handlerMode = 0; //0 = Sleep, 1 = Shutdown

static volatile uint32_t stop = 0;

static void
PIT_TempIntHandler(uint32_t int_no,
                   uint32_t err_code) {
    err_code = 0;
    //if(stop)__asm__ volatile("hlt");
    if(int_no == IRQ(0)) {
        if(pit_handlerMode == 0) {
            if(pit_ticksToWait != 0) {
                pit_ticksToWait -= 1;
            }
        }
    }
}

void
PIT_Sleep(uint32_t interval) {
    pit_handlerMode = 1;

    //Mask all PIT interrupts
    PIT_SetEnableMode(DISABLED);

    //Backup the current highest priority handler
    InterruptHandler handler = NULL;
    GetInterruptHandler(IRQ(0), &handler);

    //Temporarily register an interrupt handler
    RegisterInterruptHandler(IRQ(0), PIT_TempIntHandler);

    pit_ticksToWait = (interval * curFrequency)/1000; //Convert milliseconds into ticks

    //Enable the PIT interrupt
    PIT_SetEnableMode(ENABLED);

    pit_handlerMode = 0;
    __asm__ volatile ("sti");

    bool continueSleep = TRUE;
    while(continueSleep) {
        //Prevent race conditions
        pit_handlerMode = 1;
        if(pit_ticksToWait == 0) {
            continueSleep = FALSE;
        }
        pit_handlerMode = 0;

        //Wait some time
        for(int a = 0; a < 50; a++) {
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
        }
    }

    pit_handlerMode = 1;
    PIT_SetEnableMode(DISABLED);

    RegisterInterruptHandler(IRQ(0), handler);
    stop = 1;
}

void
PIT_SetEnableMode(bool enabled) {
    SetInterruptEnableMode(IRQ(0), enabled);

    if(!enabled)
        PIT_SetFrequency(PIT_CH0, PIT_ACCESS_HI_BYTE | PIT_ACCESS_LO_BYTE, PIT_MODE_INTERRUPT, PIT_VAL_16BIT, 1);
    else
        PIT_SetFrequency(PIT_CH0, PIT_ACCESS_HI_BYTE | PIT_ACCESS_LO_BYTE, PIT_MODE_RATE, PIT_VAL_16BIT, PIT_FREQUENCY_HZ);    
}


void
PIT_Initialize(void) {
    PIT_SetFrequency(PIT_CH0, PIT_ACCESS_HI_BYTE | PIT_ACCESS_LO_BYTE, PIT_MODE_RATE, PIT_VAL_16BIT, PIT_FREQUENCY_HZ);
}