#include <cardinal/syscall_property.h>
#include <cardinal/ipc.h>

#include "ps2_keyboard.h"

int main() {

	//Request IO privileges
	SetProperty(CardinalProperty_IOPL, 0, 3);

	//Start the mouse driver
	PS2Keyboard_Initialize();

	while(1) 
	{
		CREATE_NEW_MESSAGE_PTR_TYPE(InterruptMessage, m);
		POLL_MESSAGE_MSGTYPE(m, CardinalMsgType_Interrupt);

	}
}