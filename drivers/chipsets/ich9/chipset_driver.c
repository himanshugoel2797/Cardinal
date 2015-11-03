#include "chipset_driver.h"
#include "../lpc_ctrl/lpc.h"
#include "drivers.h"

bool
ICH9_Detect(void)
{
    //Test if the current chipset is ICH9 by testing for the LPC controller
    if(LPC_Initialize())return TRUE;
    COM_WriteStr("LPC not initialized\r\n");
    return FALSE;
}