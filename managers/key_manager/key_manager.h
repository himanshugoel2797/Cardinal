#ifndef _CARDINAL_KEY_MANAGER_H_
#define _CARDINAL_KEY_MANAGER_H_

#include "types.h"

typedef enum {
    KeyManagerErrors_None,
    KeyManagerErrors_Unknown,
    KeyManagerErrors_InvalidParams,
    KeyManagerErrors_KeyDoesNotExist,
    KeyManagerErrors_InvalidOperation,
    KeyManagerErrors_NonZeroReferenceCount,
} KeyManagerErrors;

typedef enum {
    KeyFlags_None = 0,
    KeyFlags_SingleTransfer = (1 << 0),
    KeyFlags_UnlimitedTransfer = (1 << 1),
} KeyFlags;

void
KeyMan_Initialize(void);

/**
 * @brief      Allocate a new key.
 *
 * @param[in]  identifier  The identifier
 * @param      key         The key
 *
 * @return     Error code on failure, KeyManagerErrors_None on success.
 */
KeyManagerErrors
KeyMan_AllocateKey(uint64_t identifier,
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

/**
 * @brief      Read a key.
 *
 * @param[in]  key         The key
 * @param      pid         The pid
 * @param      identifier  The identifier
 * @param      keyFlags    The key flags
 *
 * @return     Error code on failure, KeyManagerErrors_None on success.
 */
KeyManagerErrors
KeyMan_ReadKey(uint64_t key,
               uint64_t *identifier);

/**
 * @brief      Increment the key reference count.
 *
 * @param[in]  key   The key
 *
 * @return     { description_of_the_return_value }
 */
KeyManagerErrors
KeyMan_IncrementRefCount(uint64_t key);

/**
 * @brief      Decrement the key reference count.
 *
 * @param[in]  key   The key
 *
 * @return     { description_of_the_return_value }
 */
KeyManagerErrors
KeyMan_DecrementRefCount(uint64_t key);

#endif