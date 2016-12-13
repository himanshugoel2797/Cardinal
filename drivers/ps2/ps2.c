#include "ps2.h"
#include "priv_ps2.h"
#include <cardinal/driver_utils.h>
#include <cardinal/io/server.h>
#include <cardinal/namespace/server.h>
#include <cardinal/proc/server.h>

uint8_t PS2_Initialize()
{
    //Disable the ports
    outb(CMD_PORT, DISABLE_PORT1_CMD);
    WAIT_CMD_SENT;
    outb(CMD_PORT, DISABLE_PORT2_CMD);
    WAIT_CMD_SENT;
    while(PS2_ReadStatus() & 1) inb(DATA_PORT);


    //Read the controller configuration byte
    uint8_t cfg = PS2_ReadConfig();

    cfg &= ~(1);
    cfg &= ~(1<<1);
    cfg &= ~(1<<6);

    uint8_t isDualChannel = (cfg & (1<<5)); //If clear, not dual channel

    //Sent the controller config
    PS2_WriteConfig(cfg);

    outb(CMD_PORT, PERFORM_SELFTEST);
    WAIT_DATA_AVL;
    uint8_t test_result = inb(DATA_PORT);

    //If test didn't pass, return -1
    if(test_result != 0x55) return -1;

    if(isDualChannel)   //If test showed it was dual channel first, check properly
        {
            outb(CMD_PORT, ENABLE_PORT2_CMD);
            cfg = PS2_ReadConfig();
            if(cfg & (1<<5)) isDualChannel = 0; //If bit is still set, not dual channel

            if(isDualChannel) outb(CMD_PORT, DISABLE_PORT2_CMD);
        }

    outb(CMD_PORT, PERFORM_PORT1TEST);
    WAIT_DATA_AVL;
    uint8_t port1_test_result = inb(DATA_PORT);
    uint8_t port2_test_result = 1;

    if(isDualChannel)
        {
            outb(CMD_PORT, PERFORM_PORT2TEST);
            WAIT_DATA_AVL;
            port2_test_result = inb(DATA_PORT);
        }

    //If both tests failed, return -1
    if(port1_test_result != 0 && port2_test_result != 0) return -1;

    uint32_t key = 0;
    uint64_t error = 0;
    uint8_t access_pass[KEY_BYTES];
    UID dst_pid = 0;
    RetrieveNamespace("initrd", &key);
    while(!IsNamespaceRetrieved(key, &dst_pid, &error));

    if(port1_test_result == 0)
        {
            //PS2Keyboard_Initialize();
            char* argv[] = {"ps2_keyboard.elf"};
            UID kb_pid = 0;
            //__asm__("hlt":: "a"(StartProcess(":ps2_keyboard.elf", argv, 1, dst_pid, access_pass, &kb_pid)));

        }

    if(port2_test_result == 0)
        {
            //PS2Mouse_Initialize();
            char* argv[] = {"ps2_mouse.elf"};
            UID mouse_pid = 0;

            //__asm__("hlt":: "a"(StartProcess(":ps2_mouse.elf", argv, 1, dst_pid, access_pass, &mouse_pid)));
        }

    cfg = PS2_ReadConfig();
    if(port1_test_result == 0)
        {
            outb(CMD_PORT, ENABLE_PORT1_CMD);
            cfg |= 1;
            cfg &= ~(1 << 4);
            WAIT_CMD_SENT;
        }

    if(port2_test_result == 0)
        {
            outb(CMD_PORT, ENABLE_PORT2_CMD);
            cfg |= 2;
            cfg &= ~(1 << 5);
            WAIT_CMD_SENT;
        }
    PS2_WriteConfig(cfg);


    return 0;
}

uint8_t PS2_ReadStatus()
{
    return inb(CMD_PORT);
}

uint8_t PS2_ReadConfig()
{
    outb(CMD_PORT, READ_CFG_CMD);
    WAIT_CMD_SENT;
    WAIT_DATA_AVL;
    return inb(DATA_PORT);
}

void PS2_WriteConfig(uint8_t cfg)
{
    outb(CMD_PORT, WRITE_CFG_CMD);
    WAIT_CMD_SENT;
    WAIT_DATA_SENT;
    outb(DATA_PORT, cfg);
    WAIT_CMD_SENT;
}
