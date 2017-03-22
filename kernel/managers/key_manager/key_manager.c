/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "key_manager.h"
#include "memory.h"
#include "synchronization.h"
#include "common/common.h"

#define KEY_TABLE_SIZE GiB(16)

typedef struct {
    Key_t key;
    uint32_t ref_count;
    _Atomic uint64_t identifier[IDENTIFIER_COUNT];
} KeyEntry;

static KeyEntry* keyTable = NULL;
static uint64_t lastFreeKeyIndex = 0;
static Spinlock keyman_lock;

void
KeyMan_GenerateKey(uint64_t idx, Key_t *key) {

    for(int i = 0; i < KEY_LENGTH; i++) {
        key->key[i] = (uint8_t)rand();
    }

    key->key_index = idx;
}

bool
KeyMan_VerifyKey(const Key_t *key) {

    if(key->key_index >= KEY_TABLE_SIZE / sizeof(KeyEntry))
        return false;

    for(int i = 0; i < KEY_LENGTH; i++) {
        if(key->key[i] != keyTable[key->key_index].key.key[i])
            return false;
    }

    return true;
}

void
KeyMan_Initialize(void) {

    uint64_t keyTable_addr = 0;

    if(FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                              &keyTable_addr,
                              KEY_TABLE_SIZE,
                              MemoryAllocationType_Heap,
                              MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Kernel
                             ) != MemoryAllocationErrors_None) {
        //TODO go back and make the kernel panic on any initialization failures.
    }

    if(MapPage(GetActiveVirtualMemoryInstance(),
               0,//AllocatePhysicalPageCont(KEY_TABLE_SIZE / PAGE_SIZE),
               keyTable_addr,
               KEY_TABLE_SIZE,
               CachingModeWriteBack,
               MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation,
               MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Present
              ) != MemoryAllocationErrors_None) {
        //TODO Panic
    }

    //The key table has now been allocated.
    keyTable = (KeyEntry*)keyTable_addr;
    keyman_lock = CreateSpinlock();
}


KeyManagerErrors
KeyMan_AllocateKey(uint64_t *identifier,
                   Key_t *key) {

    if(key == NULL)
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    //Generate the full key
    KeyMan_GenerateKey(lastFreeKeyIndex, key);
    memcpy(&keyTable[lastFreeKeyIndex].key, key, sizeof(Key_t));

    for(int i = 0; i < IDENTIFIER_COUNT; i++)
        keyTable[lastFreeKeyIndex].identifier[i] = identifier[i];

    keyTable[lastFreeKeyIndex].ref_count = 1;

    while(keyTable[lastFreeKeyIndex].ref_count != 0 && lastFreeKeyIndex < KEY_TABLE_SIZE/sizeof(KeyEntry)) {
        lastFreeKeyIndex++;
    }

    UnlockSpinlock(keyman_lock);

    return KeyManagerErrors_None;
}

KeyManagerErrors
KeyMan_FreeKey(Key_t *key) {
    uint64_t index = key->key_index;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(!KeyMan_VerifyKey(key)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(keyTable[index].ref_count != 0) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_NonZeroReferenceCount;
    }

    keyTable[index].key.key_index = 0;
    keyTable[index].ref_count = 0;

    for(int i = 0; i < IDENTIFIER_COUNT; i++)
        keyTable[index].identifier[i] = 0;

    if(index < lastFreeKeyIndex)
        lastFreeKeyIndex = index;

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}

bool
KeyMan_KeyExists(const Key_t *key) {
    uint64_t index = key->key_index;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return 0;

    LockSpinlock(keyman_lock);

    if(!KeyMan_VerifyKey(key)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    uint64_t k = keyTable[index].key.key_index;
    UnlockSpinlock(keyman_lock);

    if(k == 0)
        return 0;

    return (k == key->key_index);
}

KeyManagerErrors
KeyMan_ReadKey(const Key_t *key,
               uint64_t *identifier) {

    uint64_t index = key->key_index;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(!KeyMan_VerifyKey(key)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(identifier != NULL) {
        for(int i = 0; i < IDENTIFIER_COUNT; i++)
            identifier[i] = keyTable[index].identifier[i];
    }

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}


KeyManagerErrors
KeyMan_WriteKey(const Key_t *key,
                uint64_t *identifier) {

    uint64_t index = key->key_index;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(!KeyMan_VerifyKey(key)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(identifier != NULL) {
        for(int i = 0; i < IDENTIFIER_COUNT; i++)
            keyTable[index].identifier[i] = identifier[i];
    }

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}

KeyManagerErrors
KeyMan_IncrementRefCount(const Key_t *key) {

    uint64_t index = key->key_index;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(!KeyMan_VerifyKey(key)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    //Prevent overflow
    if(keyTable[index].ref_count == (uint32_t)-1)
        return KeyManagerErrors_Unknown;

    keyTable[index].ref_count++;

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}

KeyManagerErrors
KeyMan_DecrementRefCount(Key_t *key) {
    uint64_t index = key->key_index;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(!KeyMan_VerifyKey(key)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(keyTable[index].ref_count != 0)
        keyTable[index].ref_count--;

    if(keyTable[index].ref_count == 0) {
        KeyMan_FreeKey(key);
    }

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}