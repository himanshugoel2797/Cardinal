#include "types.h"
#include "chipsets.h"

void
Chipset_IdentifyAndRegister(void)
{
    if(ICH9_Detect())return;

}