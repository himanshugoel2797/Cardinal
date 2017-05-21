/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "key_manager.h"
#include "common/common.h"
#include "memory.h"
#include "synchronization.h"

#define KEY_TABLE_SIZE (GiB(16))

typedef struct {
  Key_t key;
  uint32_t ref_count;
  _Atomic uint64_t identifier[IDENTIFIER_COUNT];
} KeyEntry;

static KeyEntry *keyTable = NULL;
static uint64_t lastFreeKeyIndex = 0;
static Spinlock keyman_lock;

static void KeyMan_GenerateKey(uint64_t idx, Key_t *key) {
  for (int i = 0; i < KEY_LENGTH; i++) {
    key->key[i] = (uint8_t)rand();
  }

  key->key_index = idx + 1;
}

bool KeyMan_AreKeysEqual(const Key_t *a, const Key_t *b) {
  for (int i = 0; i < KEY_LENGTH; i++) {
    if (a->key[i] != b->key[i]) return FALSE;
  }

  if (a->key_index != b->key_index) return FALSE;

  if (a->machine_id != b->machine_id) return FALSE;

  if (a->network_id != b->network_id) return FALSE;

  return TRUE;
}

bool KeyMan_VerifyKey(const Key_t *key) {
  if (key->key_index == 0) return false;

  if (key->key_index - 1 >= KEY_TABLE_SIZE / sizeof(KeyEntry)) return false;

  for (int i = 0; i < KEY_LENGTH; i++) {
    if (key->key[i] != keyTable[key->key_index - 1].key.key[i]) return false;
  }

  return true;
}

void KeyMan_Initialize(void) {
  uint64_t keyTable_addr = 0;

  if (FindFreeVirtualAddress(
          GetActiveVirtualMemoryInstance(), &keyTable_addr, KEY_TABLE_SIZE,
          MemoryAllocationType_Heap,
          MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
              MemoryAllocationFlags_Kernel) != MemoryAllocationErrors_None) {
    // TODO: go back and make the kernel panic on any initialization failures.
    PANIC("Key Table Initialization Failed.");
  }

  if (MapPage(
          GetActiveVirtualMemoryInstance(),
          0,  // AllocatePhysicalPageCont(KEY_TABLE_SIZE / PAGE_SIZE),
          keyTable_addr, KEY_TABLE_SIZE, CachingModeWriteBack,
          MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation,
          MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
              MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Present) !=
      MemoryAllocationErrors_None) {
    // TODO Panic
    PANIC("Key Table Initialization Failed.");
  }

  // The key table has now been allocated.
  keyTable = (KeyEntry *)keyTable_addr;
  keyman_lock = CreateSpinlock();
}

KeyManagerErrors KeyMan_AllocateKey(uint64_t *identifier, Key_t *key) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;

  LockSpinlock(keyman_lock);

  // Generate the full key
  KeyMan_GenerateKey(lastFreeKeyIndex, key);
  memcpy(&keyTable[lastFreeKeyIndex].key, key, sizeof(Key_t));

  for (int i = 0; i < IDENTIFIER_COUNT; i++)
    keyTable[lastFreeKeyIndex].identifier[i] = identifier[i];

  keyTable[lastFreeKeyIndex].ref_count = 1;

  while (keyTable[lastFreeKeyIndex].ref_count != 0 &&
         lastFreeKeyIndex < KEY_TABLE_SIZE / sizeof(KeyEntry)) {
    lastFreeKeyIndex++;
  }

  UnlockSpinlock(keyman_lock);

  return KeyManagerErrors_None;
}

KeyManagerErrors KeyMan_FreeKey(Key_t *key) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;

  if (key->key_index == 0) return KeyManagerErrors_InvalidParams;

  uint64_t index = key->key_index - 1;
  if (index >= KEY_TABLE_SIZE / sizeof(KeyEntry))
    return KeyManagerErrors_InvalidParams;

  LockSpinlock(keyman_lock);

  if (!KeyMan_VerifyKey(key)) {
    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_KeyDoesNotExist;
  }

  if (keyTable[index].ref_count != 0) {
    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_NonZeroReferenceCount;
  }

  keyTable[index].key.key_index = 0;
  keyTable[index].ref_count = 0;

  for (int i = 0; i < IDENTIFIER_COUNT; i++) keyTable[index].identifier[i] = 0;

  if (index < lastFreeKeyIndex) lastFreeKeyIndex = index;

  UnlockSpinlock(keyman_lock);
  return KeyManagerErrors_None;
}

bool KeyMan_KeyExists(const Key_t *key) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;

  if (key->key_index == 0) return KeyManagerErrors_InvalidParams;

  uint64_t index = key->key_index - 1;
  if (index >= KEY_TABLE_SIZE / sizeof(KeyEntry)) return 0;

  LockSpinlock(keyman_lock);

  if (!KeyMan_VerifyKey(key)) {
    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_KeyDoesNotExist;
  }

  uint64_t k = keyTable[index].key.key_index;
  UnlockSpinlock(keyman_lock);

  if (k == 0) return 0;

  return (k == key->key_index);
}

KeyManagerErrors KeyMan_ReadKey(const Key_t *key, uint64_t *identifier) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;

  if (key->key_index == 0) return KeyManagerErrors_InvalidParams;

  uint64_t index = key->key_index - 1;
  if (index >= KEY_TABLE_SIZE / sizeof(KeyEntry))
    return KeyManagerErrors_InvalidParams;

  LockSpinlock(keyman_lock);

  if (!KeyMan_VerifyKey(key)) {
    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_KeyDoesNotExist;
  }

  if (identifier != NULL) {
    for (int i = 0; i < IDENTIFIER_COUNT; i++)
      identifier[i] = keyTable[index].identifier[i];
  }

  UnlockSpinlock(keyman_lock);
  return KeyManagerErrors_None;
}

KeyManagerErrors KeyMan_WriteKey(const Key_t *key, uint64_t *identifier) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;

  if (key->key_index == 0) return KeyManagerErrors_InvalidParams;

  uint64_t index = key->key_index - 1;
  if (index >= KEY_TABLE_SIZE / sizeof(KeyEntry))
    return KeyManagerErrors_InvalidParams;

  LockSpinlock(keyman_lock);

  if (!KeyMan_VerifyKey(key)) {
    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_KeyDoesNotExist;
  }

  if (identifier != NULL) {
    for (int i = 0; i < IDENTIFIER_COUNT; i++)
      keyTable[index].identifier[i] = identifier[i];
  }

  UnlockSpinlock(keyman_lock);
  return KeyManagerErrors_None;
}

KeyManagerErrors KeyMan_IncrementRefCount(const Key_t *key) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;

  if (key->key_index == 0) return KeyManagerErrors_InvalidParams;

  uint64_t index = key->key_index - 1;
  if (index >= KEY_TABLE_SIZE / sizeof(KeyEntry))
    return KeyManagerErrors_InvalidParams;

  LockSpinlock(keyman_lock);

  if (!KeyMan_VerifyKey(key)) {
    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_KeyDoesNotExist;
  }

  // Prevent overflow
  if (keyTable[index].ref_count == (uint32_t)-1)
    return KeyManagerErrors_Unknown;

  keyTable[index].ref_count++;

  UnlockSpinlock(keyman_lock);
  return KeyManagerErrors_None;
}

KeyManagerErrors KeyMan_DecrementRefCount(Key_t *key) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;

  if (key->key_index == 0) return KeyManagerErrors_InvalidParams;

  uint64_t index = key->key_index - 1;
  if (index >= KEY_TABLE_SIZE / sizeof(KeyEntry))
    return KeyManagerErrors_InvalidParams;

  LockSpinlock(keyman_lock);

  if (!KeyMan_VerifyKey(key)) {
    UnlockSpinlock(keyman_lock);
    return KeyManagerErrors_KeyDoesNotExist;
  }

  if (keyTable[index].ref_count != 0) keyTable[index].ref_count--;

  if (keyTable[index].ref_count == 0) {
    KeyMan_FreeKey(key);
  }

  UnlockSpinlock(keyman_lock);
  return KeyManagerErrors_None;
}

KeyManagerErrors KeyMan_GetKeyUsageCount(Key_t *key, uint64_t *cnt) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;
  if (cnt == NULL) return KeyManagerErrors_InvalidParams;

  uint64_t identifiers[IDENTIFIER_COUNT];
  KeyManagerErrors err = KeyMan_ReadKey(key, identifiers);
  if (err != KeyManagerErrors_None) return err;

  *cnt = identifiers[KeyIdentifier_Count];
  return KeyManagerErrors_None;
}

KeyManagerErrors KeyMan_UseKey(Key_t *key) {
  if (key == NULL) return KeyManagerErrors_InvalidParams;

  uint64_t identifiers[IDENTIFIER_COUNT];
  KeyManagerErrors err = KeyMan_ReadKey(key, identifiers);
  if (err != KeyManagerErrors_None) return err;

  identifiers[KeyIdentifier_Count]++;

  KeyMan_WriteKey(key, identifiers);
  return KeyManagerErrors_None;
}