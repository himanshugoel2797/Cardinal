#include "ps2_keyboard.h"
#include "priv_ps2.h"
#include <cardinal/driver_utils.h>

uint8_t PS2Keyboard_Initialize()
{
    //Reset the keyboard
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xFF);
    WAIT_DATA_AVL;
    uint16_t reset_res = 0;
    inb(DATA_PORT);
    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(DATA_PORT, 0xF3);
    outb(DATA_PORT, 0x00);
    inb(DATA_PORT);

    outb(DATA_PORT, 0xF8);	//Enable make and break codes for all keys
    inb(DATA_PORT);
    inb(DATA_PORT);
}

void PS2Keyboard_SetLEDStatus(uint8_t led, uint8_t status)
{
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xED);
    WAIT_DATA_SENT;
    outb(DATA_PORT, status << led);
    WAIT_DATA_AVL;
    inb(DATA_PORT);
    return;
}