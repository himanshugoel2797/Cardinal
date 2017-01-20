/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_KEY_MANAGER_H_
#define _CARDINAL_KEY_MANAGER_H_

#include "types.h"
#include "libs/libCardinal/include/shared_memory.h"

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
                   uint8_t *key);

/**
 * @brief      Free an existing key.
 *
 * @param[in]  key   The key
 *
 * @return     Error code on failure, KeyManagerErrors_None on success.
 */
KeyManagerErrors
KeyMan_FreeKey(uint8_t *key);

/**
 * @brief      Check if the key exists.
 *
 * @param[in]  key   The key to check.
 *
 * @return     0 if the key does not exist, 1 if the key exists.
 */
bool
KeyMan_KeyExists(uint8_t *key);

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
KeyMan_ReadKey(uint8_t *key,
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
KeyMan_WriteKey(uint8_t *key,
                uint64_t *identifier);

/**
 * @brief      Increment the key reference count.
 *
 * @param[in]  key   The key
 *
 * @return     { description_of_the_return_value }
 */
KeyManagerErrors
KeyMan_IncrementRefCount(uint8_t *key);

/**
 * @brief      Decrement the key reference count.
 *
 * @param[in]  key   The key
 *
 * @return     { description_of_the_return_value }
 */
KeyManagerErrors
KeyMan_DecrementRefCount(uint8_t *key);

#endif