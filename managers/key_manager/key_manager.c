#include "key_manager.h"
#include "memory.h"
#include "synchronization.h"
#include "common/common.h"

#define KEY_TABLE_SIZE GiB(1)

typedef struct {
    uint32_t key;
    uint32_t ref_count;
    uint64_t identifier[IDENTIFIER_COUNT];
} KeyEntry;

static KeyEntry* keyTable = NULL;
static uint32_t lastFreeKeyIndex = 0;
static Spinlock keyman_lock;

void
KeyMan_Initialize(void) {

    uint64_t keyTable_addr = 0;

    if(FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                              &keyTable_addr,
                              KEY_TABLE_SIZE,
                              MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation,
                              MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Kernel
                             ) != MemoryAllocationErrors_None) {
        //TODO go back and make the kernel panic on any initialization failures.
    }

    if(MapPage(GetActiveVirtualMemoryInstance(),
               0,
               keyTable_addr,
               KEY_TABLE_SIZE,
               CachingModeWriteBack,
               MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation,
               MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Kernel
              ) != MemoryAllocationErrors_None) {
        //TODO Panic
    }

    //The key table has now been allocated.
    keyTable = (KeyEntry*)keyTable_addr;
    keyman_lock = CreateSpinlock();
}


KeyManagerErrors
KeyMan_AllocateKey(uint64_t *identifier,
                   uint64_t *key) {

    if(key == NULL)
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    //Generate the full key
    uint32_t rng_key = rand();
    *key = (uint64_t)rng_key;
    *key = *key << 32 | lastFreeKeyIndex;

    keyTable[lastFreeKeyIndex].key = rng_key;

    for(int i = 0; i < IDENTIFIER_COUNT; i++)
        keyTable[lastFreeKeyIndex].identifier[i] = identifier[i];
    
    keyTable[lastFreeKeyIndex].ref_count = 0;

    while(keyTable[lastFreeKeyIndex].key != 0 && lastFreeKeyIndex < KEY_TABLE_SIZE/sizeof(KeyEntry)) {
        lastFreeKeyIndex++;
    }

    UnlockSpinlock(keyman_lock);

    return KeyManagerErrors_None;
}

KeyManagerErrors
KeyMan_FreeKey(uint64_t key) {
    uint32_t index = (uint32_t)key;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(keyTable[index].key != (uint32_t)(key >> 32)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(keyTable[index].ref_count != 0) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_NonZeroReferenceCount;
    }

    keyTable[index].key = 0;
    keyTable[index].ref_count = 0;

    for(int i = 0; i < IDENTIFIER_COUNT; i++)
        keyTable[index].identifier[i] = 0;

    if(index < lastFreeKeyIndex)
        lastFreeKeyIndex = index;

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}

bool
KeyMan_KeyExists(uint64_t key) {

    uint32_t index = (uint32_t)key;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return 0;

    LockSpinlock(keyman_lock);
    uint32_t k = keyTable[index].key;
    UnlockSpinlock(keyman_lock);

    if(k == 0)
        return 0;

    return (k == (uint32_t)(key >> 32));
}

KeyManagerErrors
KeyMan_ReadKey(uint64_t key,
               uint64_t *identifier) {

    uint32_t index = (uint32_t)key;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(keyTable[index].key != (uint32_t)(key >> 32)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(identifier != NULL)
    {
        for(int i = 0; i < IDENTIFIER_COUNT; i++)
            identifier[i] = keyTable[index].identifier[i];
    }

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}

KeyManagerErrors
KeyMan_IncrementRefCount(uint64_t key) {
    
    uint32_t index = (uint32_t)key;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(keyTable[index].key != (uint32_t)(key >> 32)) {
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
KeyMan_DecrementRefCount(uint64_t key) {
    uint32_t index = (uint32_t)key;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(keyTable[index].key != (uint32_t)(key >> 32)) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(keyTable[index].ref_count != 0)
        keyTable[index].ref_count--;

    if(keyTable[index].ref_count == 0)
    {
        KeyMan_FreeKey(key);
    }

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}