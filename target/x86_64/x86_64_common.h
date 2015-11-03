#ifndef _X86_64_COMMON_H_
#define _X86_64_COMMON_H_

#include "types.h"


void
bootstrap_render(uint32_t color);

void
bootstrap_kernel_panic(uint8_t severity);

void
smp_bootstrap(void);

extern int coreCount;
#endif