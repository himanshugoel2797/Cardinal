#include "priv_ps2.h"
#include "utils/native.h"

static uint8_t mode;

uint8_t
PS2Mouse_Initialize(void)
{
    mode = 0;

    WAIT_DATA_SENT;
    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xFF);

    int res = 0;
    while(res != 0xAA)
        {
            WAIT_DATA_AVL;
            res = inb(DATA_PORT);
        }

    inb(DATA_PORT);

    //Initialize ScrollWheel
    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF3);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 200);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF3);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 100);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF3);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 80);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    //The scroll wheel has been setup, check if this is the case
    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF2);

    WAIT_DATA_AVL;
    inb(DATA_PORT);
    mode = inb(DATA_PORT);

    if(mode == 0x3 || mode == 0x4)COM_WriteStr("Scroll Wheel Initialized!\r\n");



    //Initialize extra buttons
    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF3);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 200);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF3);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 200);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF3);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 80);

    WAIT_DATA_AVL;
    inb(DATA_PORT);

    //The extra buttons have been setup, check if this is the case
    outb(CMD_PORT, 0xD4);	//Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF2);

    WAIT_DATA_AVL;
    inb(DATA_PORT);
    mode = inb(DATA_PORT);

    if(mode == 0x4)COM_WriteStr("5-button mouse support Initialized!\r\n");

    //Activate the mouse

    //Set the streaming rate to 40
    outb(CMD_PORT, 0xD4);   //Select the mouse
    WAIT_DATA_SENT;
    outb(DATA_PORT, 40);

    outb(CMD_PORT, 0xD4);
    WAIT_DATA_SENT;
    outb(DATA_PORT, 0xF4);	//Enable streaming mode

    inb(DATA_PORT);

    return 0;
}

bool
PS2Mouse_HasScrollWheel(void)
{
    return (mode == 0x4 || mode == 0x3);
}

bool
PS2Mouse_IsFiveButton(void)
{
    return (mode == 0x4);
}