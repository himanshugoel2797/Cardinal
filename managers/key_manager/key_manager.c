#include "key_manager.h"
#include "memory.h"
#include "synchronization.h"
#include "common/common.h"

#define KEY_TABLE_SIZE GiB(1)

typedef struct {
    uint32_t key;
    KeyFlags flags;
    uint64_t identifier;
    UID parent_pid;
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
KeyMan_AllocateKey(UID pid,
                   uint64_t identifier,
                   KeyFlags flags,
                   uint64_t *key) {

    if(key == NULL)
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    //Generate the full key
    uint32_t rng_key = rand();
    *key = (uint64_t)rng_key;
    *key = *key << 32 | lastFreeKeyIndex;

    keyTable[lastFreeKeyIndex].key = rng_key;
    keyTable[lastFreeKeyIndex].flags = flags;
    keyTable[lastFreeKeyIndex].identifier = identifier;
    keyTable[lastFreeKeyIndex].parent_pid = pid;

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

    if(keyTable[index].key == 0) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    keyTable[index].key = 0;
    keyTable[index].flags = 0;
    keyTable[index].identifier = 0;
    keyTable[index].parent_pid = 0;

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

    return 1;
}


KeyManagerErrors
KeyMan_TransferKey(uint64_t key, UID targetPID) {

    uint32_t index = (uint32_t)key;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(keyTable[index].key == 0) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(keyTable[index].flags & KeyFlags_SingleTransfer) {
        keyTable[index].parent_pid = targetPID;
        keyTable[index].flags &= ~KeyFlags_SingleTransfer;

        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_None;
    } else if(keyTable[index].flags & KeyFlags_UnlimitedTransfer) {
        keyTable[index].parent_pid = targetPID;

        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_None;
    }

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_InvalidOperation;
}

KeyManagerErrors
KeyMan_ReadKey(uint64_t key,
               UID *pid,
               uint64_t *identifier,
               KeyFlags *keyFlags) {

    uint32_t index = (uint32_t)key;
    if(index >= KEY_TABLE_SIZE/sizeof(KeyEntry))
        return KeyManagerErrors_InvalidParams;

    LockSpinlock(keyman_lock);

    if(keyTable[index].key == 0) {
        UnlockSpinlock(keyman_lock);
        return KeyManagerErrors_KeyDoesNotExist;
    }

    if(pid != NULL)
        *pid = keyTable[index].parent_pid;

    if(identifier != NULL)
        *identifier = keyTable[index].identifier;

    if(keyFlags != NULL)
        *keyFlags = keyTable[index].flags;

    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_None;
}