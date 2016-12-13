#include <cardinal/syscall_property.h>
#include <cardinal/ipc.h>
#include <cardinal/driver_utils.h>

#include "ps2_keyboard.h"
#include "priv_ps2.h"

int main() {

	//Request IO privileges
	SetProperty(CardinalProperty_IOPL, 0, 3);


	//Start the keyboard driver
	PS2Keyboard_Initialize();
	R01_RegisterForInterrupts(33, 1);

    uint8_t cfg = PS2_ReadConfig();
    outb(CMD_PORT, ENABLE_PORT1_CMD);
    cfg |= 1;
    cfg &= ~(1 << 4);
    WAIT_CMD_SENT;
    PS2_WriteConfig(cfg);

    int cnt = 0;

	while(1) 
	{
		CREATE_NEW_MESSAGE_PTR_TYPE(InterruptMessage, m);
		POLL_MESSAGE_MSGTYPE(m, CardinalMsgType_Interrupt);

		cnt++;

		if(cnt == 50)
			__asm__("hlt");

		while(IS_DATA_AVL)
		inb(DATA_PORT);

	}
}