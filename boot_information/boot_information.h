#ifndef _CARDINAL_PARSE_BOOT_INFO_H_
#define _CARDINAL_PARSE_BOOT_INFO_H_

#include "types.h"
#include "libs/libCardinal/include/bootinfo.h"

void
ParseAndSaveBootInformation(void *boot_info);

CardinalBootInfo*
GetBootInfo(void);

#endif
