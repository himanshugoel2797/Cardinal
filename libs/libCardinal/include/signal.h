#ifndef _CARDINAL_SIGNAL_H_
#define _CARDINAL_SIGNAL_H_

#include <stdint.h>

#include "ipc.h"

#define CARDINAL_SIGNAL_TYPE_SIGCHILD 1

struct SigChild{
	Message m;
	uint64_t signal_type;
	uint64_t exit_code;
};

#endif