#ifndef _CARDINAL_ELF_SERVER_H_
#define _CARDINAL_ELF_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <cardinal/ipc.h>

typedef struct {
	Message m;
	UID targetPID;
	void *src;
} ElfLoadRequest;

#endif