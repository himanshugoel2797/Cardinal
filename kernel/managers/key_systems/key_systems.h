// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef CARDINAL_KEY_SYSTEMS_H
#define CARDINAL_KEY_SYSTEMS_H

#include "libs/libCardinal/include/keyman.h"
#include "shared_memory/shared_memory.h"
#include "signal/signal.h"
#include "types.h"

uint64_t KeySys_TryLockKey(Key_t* key);

uint64_t KeySys_UnlockKey(Key_t* key);

uint64_t KeySys_RegisterTagProvider(char* name, Key_t* signals);

uint64_t KeySys_GetWithTag(const char* cond, char* tags, uint32_t flags);

uint64_t KeySys_Read(Key_t* key, void* buff, uint32_t flags);

uint64_t KeySys_Write(Key_t* key, void* buff, uint64_t len, uint32_t flags);

uint64_t KeySys_CreateKey(KeyType_t type, uint32_t flags, void* params,
                          Key_t* key, uint32_t* key_cnt);

uint64_t KeySys_AddKey(Key_t* key);

uint64_t KeySys_Dup(int32_t src, int32_t dst);

uint64_t KeySys_RemoveKey(int32_t key);

uint64_t KeySys_GetKeyIndex(Key_t* key);

uint64_t KeySys_UseKey(Key_t* key);

uint64_t KeySys_GetKeyUsageCount(Key_t* key);

#endif