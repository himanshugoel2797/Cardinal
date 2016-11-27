#ifndef _CARDINAL_ELF_SERVER_H_
#define _CARDINAL_ELF_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <cardinal/ipc.h>

typedef struct {
	Message m;
	uint64_t response_key;
} ProcServ_CreateRequest;

typedef struct {
	union {
	uint64_t pid;
	uint64_t err_code;
	}
} ProcServ_CreateRequest_Response;

#endif