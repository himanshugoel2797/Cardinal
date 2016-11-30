#ifndef _CARDINAL_ELF_SERVER_H_
#define _CARDINAL_ELF_SERVER_H_

#include <stddef.h>
#include <stdint.h>
#include <cardinal/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Message m;

} ProcServ_CreateRequest;

typedef struct {
	Message m;
    union {
        uint64_t pid;
        uint64_t err_code;
    }
} ProcServ_CreateRequest_Response;

#ifdef __cplusplus
}
#endif

#endif