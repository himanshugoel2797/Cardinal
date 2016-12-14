#include <cardinal/syscall_property.h>
#include <cardinal/ipc.h>

#include <cardinal/driver_utils.h>
#include "ps2_mouse.h"
#include "priv_ps2.h"

int main() {

    //Request IO privileges
    SetProperty(CardinalProperty_IOPL, 0, 3);

    //Start the mouse driver
    PS2Mouse_Initialize();

    R01_RegisterForInterrupts(44, 1);

    while(IS_DATA_AVL)
        inb(DATA_PORT);

    int cnt = 0;

    while(1) {
        CREATE_NEW_MESSAGE_PTR_TYPE(InterruptMessage, m);
        POLL_MESSAGE_MSGTYPE((Message*)m, CardinalMsgType_Interrupt);

        cnt++;

        if(cnt == 50)
            __asm__("hlt");

        while(IS_DATA_AVL)
            inb(DATA_PORT);
    }
}