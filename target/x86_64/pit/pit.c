#include "pit.h"
#include "utils/native.h"
#include "IDT/idt.h"
#include "interrupts.h"

//NOTE: The PIT should be set to always be running at 10027 Hz, not high enough to constantly interrupt stuff, not too low to be too slow for general use


uint32_t curFrequency = 0;
void 
PIT_SetFrequency(uint8_t channel, 
                 uint8_t access, 
                 uint8_t mode, 
                 uint8_t valType, 
                 uint32_t frequency)
{
    uint32_t divisor = 1193180 / frequency;
    curFrequency = divisor * 1193180;

    channel &= 3;
    access &= 3;
    mode &= 7;
    valType &= 1;

    // Send the command byte.
    uint8_t cmd = 0;
    cmd = (channel << 6) | (access << 4) | (mode << 1) | valType;

    outb(PIT_CMD, cmd);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    if((access & PIT_ACCESS_LO_BYTE) == PIT_ACCESS_LO_BYTE)
        {
            uint8_t l = (uint8_t)(divisor & 0xFF);
            outb(0x40 + channel, l);
        }

    if((access & PIT_ACCESS_HI_BYTE) == PIT_ACCESS_HI_BYTE)
        {
            uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );
            outb(0x40 + channel, h);
        }
}

uint32_t pit_ticksToWait = 0;
uint32_t pit_handlerMode = 0; //0 = Sleep, 1 = Shutdown

static void
PIT_TempIntHandler(uint32_t int_no,
                   uint32_t err_code)
{
    if(int_no == IRQ(0) && err_code == 0){
    if(pit_handlerMode == 0)
        {
            if(pit_ticksToWait != 0) pit_ticksToWait--;
        }
    }
}

void 
PIT_Sleep(uint32_t interval)
{
    //Mask all PIT interrupts
    SetInterruptEnableMode(IRQ(0), DISABLED);

    //Backup the current highest priority handler
    InterruptHandler handler = NULL;
    GetInterruptHandler(IRQ(0), &handler);

    //Temporarily register an interrupt handler
    RegisterInterruptHandler(IRQ(0), PIT_TempIntHandler);

    pit_ticksToWait = (interval * curFrequency)/1000; //Convert into ticks
    pit_handlerMode = 0;

    //Make sure the PIT is at 1KHz
    PIT_SetFrequency(PIT_CH0, PIT_ACCESS_HI_BYTE | PIT_ACCESS_LO_BYTE, PIT_MODE_SQUARE_WAVE, PIT_VAL_16BIT, PIT_FREQUENCY_HZ);

    //Enable the PIT interrupt
    SetInterruptEnableMode(IRQ(0), ENABLED);

    while(1)
        {
            //Prevent race conditions
            __asm__ volatile ("cli");
            if(pit_ticksToWait == 0) break;
            __asm__ volatile ("sti");

            //Wait some time
            for(int a = 0; a < 50; a++)
                {
                    __asm__ volatile ("nop");
                    __asm__ volatile ("nop");
                    __asm__ volatile ("nop");
                    __asm__ volatile ("nop");
                    __asm__ volatile ("nop");
                }
        }

    SetInterruptEnableMode(IRQ(0), DISABLED);
    RegisterInterruptHandler(IRQ(0), handler);
    //Interrupts_SetInterruptEnableMode(IRQ(0), ENABLED);
    __asm__ volatile ("sti");
}

void 
PIT_SetEnableMode(bool enabled)
{
    SetInterruptEnableMode(IRQ(0), enabled);
}


void 
PIT_Initialize(void)
{
    PIT_SetFrequency(PIT_CH0, PIT_ACCESS_HI_BYTE | PIT_ACCESS_LO_BYTE, PIT_MODE_RATE, PIT_VAL_16BIT, PIT_FREQUENCY_HZ);
}