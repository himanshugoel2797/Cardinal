#ifndef _CARDINAL_BUS_MANAGER_H_
#define _CARDINAL_BUS_MANAGER_H_

#include "types.h"

typedef enum {
	BusMan_Errors_None = 0,
	BusMan_Errors_BusDoesNotExist
} BusMan_Errors;

void
BusMan_Initialize(void);

uint64_t
BusMan_CreateBus();

#endif