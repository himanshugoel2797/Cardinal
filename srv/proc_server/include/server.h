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


//All keys are stored in kernel, in a 'sparse' array, a total of 1GiB in size in virtual memory.
//Each process can create up to 16384 keys (hard hard limit), hard limit of 4096, soft limit of 1024, each being a struct of 32 bytes.
//This allows for a maximum of 2048 simultaneous processes across all users on the computer.
//The lower 32 bits of every key represent its index into the table, with the higher parts being random.
//The key is the first 8 bytes of every entry, with the second 8 bytes being an identifier provided by the application allocating the descriptor.
//The next 8 bytes is the creating process's UID, and the last 8 bytes represent various flags relating to the persistence of the key.
//
#endif