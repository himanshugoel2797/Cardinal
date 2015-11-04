#ifndef _CARDINAL_PARSE_BOOT_INFO_H_
#define _CARDINAL_PARSE_BOOT_INFO_H_

#include "types.h"
#include "bootinfo.h"

void
ParseAndSaveBootInformation(void *boot_info);

CardinalBootInfo*
GetBootInfo(void);

#endif
