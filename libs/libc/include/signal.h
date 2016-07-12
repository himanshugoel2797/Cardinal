#ifndef _CARDINAL_SIGNAL_H_
#define _CARDINAL_SIGNAL_H_

#include "types.h"

#define SUPPORTED_SIGNAL_COUNT 1024

typedef struct {
	uint64_t val[SUPPORTED_SIGNAL_COUNT/(8 * sizeof(uint64_t))];
} sigset_t;

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#endif