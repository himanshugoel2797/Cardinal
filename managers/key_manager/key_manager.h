#ifndef _CARDINAL_KEY_MANAGER_H_
#define _CARDINAL_KEY_MANAGER_H_

#include "types.h"

typedef enum {
	KeyManagerErrors_None,
	KeyManagerErrors_Unknown,
	KeyManagerErrors_InvalidParams,
	KeyManagerErrors_KeyDoesNotExist,
} KeyManagerErrors;

typedef enum {
	KeyFlags_None = 0,
} KeyFlags;

void
KeyMan_Initialize(void);

/**
 * @brief      Allocate a new key.
 *
 * @param[in]  pid         The parent pid
 * @param[in]  identifier  The identifier
 * @param[in]  flags       The key flags
 * @param      key         The key
 *
 * @return     Error code on failure, KeyManagerErrors_None on success.
 */
KeyManagerErrors
KeyMan_AllocateKey(UID pid,
				   uint64_t identifier,
				   KeyFlags flags,
				   uint64_t *key);

/**
 * @brief      Free an existing key.
 *
 * @param[in]  key   The key
 *
 * @return     Error code on failure, KeyManagerErrors_None on success.
 */
KeyManagerErrors
KeyMan_FreeKey(uint64_t key);

/**
 * @brief      Check if the key exists.
 *
 * @param[in]  key   The key to check.
 *
 * @return     0 if the key does not exist, 1 if the key exists.
 */
bool
KeyMan_KeyExists(uint64_t key);

#endif