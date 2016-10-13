#ifndef _CARDINAL_SYSINIT_INITRD_H_
#define _CARDINAL_SYSINIT_INITRD_H_

#include <stdint.h>
#include <stddef.h>

void
ImportInitrd(void);

int
GetFile(const char *file, 
		void **data, 
		size_t *len);

#endif