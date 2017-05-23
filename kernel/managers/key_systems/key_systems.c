/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "key_systems.h"
#include "managers.h"

uint64_t KeySys_TryLockKey(Key_t* key) {
    if (key == NULL) return -EINVAL;

    KeyManagerErrors err = KeyMan_TryLockKey(key);

    if (err == KeyManagerErrors_None)
        return 1;
    else if (err == KeyManagerErrors_Unknown)
        return 0;

    return -EINVAL;
}

uint64_t KeySys_UnlockKey(Key_t* key) {
    if (key == NULL) return -EINVAL;

    KeyManagerErrors err = KeyMan_UnlockKey(key);

    if (err == KeyManagerErrors_None) return 0;
    return -EINVAL;
}

uint64_t KeySys_RegisterTagProvider(char* name, Key_t* signals) {}

uint64_t KeySys_GetWithTag(const char* cond, char* tags, uint32_t flags) {}

uint64_t KeySys_Read(Key_t* key, void* buff, uint32_t flags) {
    // Read the key to get its type, and call the appropriate read handler
    uint64_t idents[IDENTIFIER_COUNT];
    KeyManagerErrors err = KeyMan_ReadKey(key, idents);
    if (err != KeyManagerErrors_None) return -EINVAL;

    switch (idents[KeyIdentifier_Type]) {
    case KeyType_SharedMemoryKey:
        return KeySys_Shmem_Read(key, buff, flags);
        break;
    case KeyType_Signal:
        return KeySys_Signal_Read(key, buff, flags);
        break;
    default:
        return -EINVAL;
        break;
    }
}

uint64_t KeySys_Write(Key_t* key, void* buff, uint64_t len, uint32_t flags) {
    // Read the key to get its type, and call the appropriate write handler
    uint64_t idents[IDENTIFIER_COUNT];
    KeyManagerErrors err = KeyMan_ReadKey(key, idents);
    if (err != KeyManagerErrors_None) return -EINVAL;

    switch (idents[KeyIdentifier_Type]) {
    case KeyType_SharedMemoryKey:
        return KeySys_Shmem_Write(key, buff, len, flags);
        break;
    case KeyType_Signal:
        return KeySys_Signal_Write(key, buff, len, flags);
        break;
    default:
        return -EINVAL;
        break;
    }
}

uint64_t KeySys_Create(KeyType_t type, uint32_t flags, void* params, Key_t* key,
                       uint32_t* key_cnt) {
    // TODO: parse the arguments to validate them for the key type
    switch (type) {
    case KeyType_SharedMemoryKey:
        return KeySys_Shmem_CreateKey(type, flags, params, key, key_cnt);
        break;
    case KeyType_Signal:
        return KeySys_Signal_CreateKey(type, flags, params, key, key_cnt);
        break;
    default:
        return -EINVAL;
        break;
    }
}

uint64_t KeySys_AddKey(Key_t* key) {
    if (key == NULL) return -EINVAL;

    int32_t idx = -1;
    ThreadError err = AddKey(GetCurrentProcessUID(), key, &idx);
    if (err != ThreadError_None) return -1;

    return (uint64_t)idx;
}

uint64_t KeySys_Dup(int32_t src, int32_t dst) {
    Key_t srcKey;
    ThreadError err = GetKey(GetCurrentProcessUID(), src, &srcKey);
    if (err != ThreadError_None) return -EINVAL;
    err = AddKey(GetCurrentProcessUID(), &srcKey, &dst);
    if (err != ThreadError_None) return -EINVAL;

    return 0;
}

uint64_t KeySys_RemoveKey(int32_t key) {
    ThreadError err = RemoveKey(GetCurrentProcessUID(), key);

    if (err != ThreadError_None) return -EINVAL;
    return 0;
}

uint64_t KeySys_GetKeyIndex(Key_t* key) {
    if (key == NULL) return -EINVAL;
    int32_t idx = -1;
    ThreadError err = GetKeyIndex(GetCurrentProcessUID(), key, &idx);
    if (err != ThreadError_None) return -EINVAL;
    return idx;
}

// TODO: interpret error codes
uint64_t KeySys_UseKey(Key_t* key) {
    if (key == NULL) return -EINVAL;
    KeyManagerErrors err = KeyMan_UseKey(key);

    if (err != KeyManagerErrors_None) return err;
    return 0;
}

// TODO: interpret erorr codes
uint64_t KeySys_GetKeyUsageCount(Key_t* key) {
    if (key == NULL) return -EINVAL;

    uint64_t cnt = -1;
    KeyManagerErrors err = KeyMan_GetKeyUsageCount(key, &cnt);

    if (err != KeyManagerErrors_None) return err;
    return cnt;
}