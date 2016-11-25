#ifndef _CARDINAL_USER_MANAGER_H_
#define _CARDINAL_USER_MANAGER_H_

#include "types.h"

typedef enum {
	UserMan_Errors_None = 0,
	UserMan_Errors_UserDoesNotExist
} UserMan_Errors;

void
UserMan_Initialize(void);

uint64_t
UserMan_CreateUser(UID *userID);

uint64_t
UserMan_SetUserSpecificSpecialDestination(uint64_t dest, UID pid);

bool
UserMan_UserExists(UID id);

uint64_t
UserMan_DeleteUser(UID id);

#endif