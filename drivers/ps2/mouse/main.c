#include <cardinal/syscall_property.h>
#include <cardinal/ipc.h>

#include "ps2_mouse.h"

int main() {

	//Request IO privileges
	SetProperty(CardinalProperty_IOPL, 0, 3);

	//Start the mouse driver
	PS2Mouse_Initialize();

	__asm__("hlt");

	while(1) 
	{
		CREATE_NEW_MESSAGE_PTR_TYPE(InterruptMessage, m);
		POLL_MESSAGE_MSGTYPE((Message*)m, CardinalMsgType_Interrupt);

	}
}