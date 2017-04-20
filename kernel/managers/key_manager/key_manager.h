// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _CARDINAL_KEY_MANAGER_H_
#define _CARDINAL_KEY_MANAGER_H_

#include "types.h"
#include "libs/libCardinal/include/keyman.h"

#define IDENTIFIER_COUNT 5

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

typedef enum {
    KeyIdentifier_Count = 3
} KeyIdentifierIndices;

typedef enum {
    KeyType_SharedMemoryKey = 1
} KeyType;

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
KeyMan_AllocateKey(uint64_t *identifier,
                   Key_t *key);

/**
 * @brief      Free an existing key.
 *
 * @param[in]  key   The key
 *
 * @return     Error code on failure, KeyManagerErrors_None on success.
 */
KeyManagerErrors
KeyMan_FreeKey(Key_t *key);

/**
 * @brief      Check if the key exists.
 *
 * @param[in]  key   The key to check.
 *
 * @return     0 if the key does not exist, 1 if the key exists.
 */
bool
KeyMan_KeyExists(const Key_t *key);

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
KeyMan_ReadKey(const Key_t *key,
               uint64_t *identifier);

/**
 * @brief      Write a key.
 *
 * @param[in]  key         The key
 * @param      identifier  The identifier
 *
 * @return     Error code on failure, KeyManagerErrors_None on success.
 */
KeyManagerErrors
KeyMan_WriteKey(const Key_t *key,
                uint64_t *identifier);

/**
 * @brief      Increment the key reference count.
 *
 * @param[in]  key   The key
 *
 * @return     { description_of_the_return_value }
 */
KeyManagerErrors
KeyMan_IncrementRefCount(const Key_t *key);

/**
 * @brief      Decrement the key reference count.
 *
 * @param[in]  key   The key
 *
 * @return     { description_of_the_return_value }
 */
KeyManagerErrors
KeyMan_DecrementRefCount(Key_t *key);

KeyManagerErrors
KeyMan_GetKeyUsageCount(Key_t *key, 
                        uint64_t *cnt);

#endif