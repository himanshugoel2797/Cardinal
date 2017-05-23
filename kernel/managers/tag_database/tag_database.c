/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "tag_database.h"
#include "managers.h"
#include "synchronization.h"

#define MAX_ENTRIES_PER_TAG (1000000)  // Max 1 million entries per tag
#define MAX_TAG_COUNT (4099)           // Prime number close to 4096

static uint64_t **tags;
static uint32_t *tag_cnts;
static Spinlock *tag_locks;

typedef enum {
  HashStr_DJB2,
  HashStr_Sdbm,
} HashStrMethods;

static void sort(uint64_t *data, uint32_t len) {
  int64_t left = 0, right = len - 1, pivot = 0;
  uint64_t piv_itm = data[left];

  while (left < right) {
    while (data[left] <= piv_itm && left < len) left++;
    while (data[right] > piv_itm && right >= 0) right--;
    if (left < right) {
      uint64_t tmp = data[left];
      data[left] = data[right];
      data[right] = tmp;
    }
  }

  data[0] = data[right];
  data[right] = piv_itm;
}

static uint32_t binsearch_r(uint64_t *data, uint32_t lo, uint32_t hi,
                            uint64_t val) {
  if (lo - hi <= 1) return -1;

  uint32_t mid = (hi - lo) / 2 + lo;
  if (data[mid] == val)
    return mid;
  else if (data[mid] > val)
    return binsearch_r(data, lo, mid, val);
  else if (data[mid] < val)
    return binsearch_r(data, mid, hi, val);

  return -1;
}

static uint32_t binsearch(uint64_t *data, uint32_t len, uint64_t val) {
  return binsearch_r(data, 0, len, val);
}

static uint32_t hash_str(HashStrMethods hashMthd, const char *str,
                         uint32_t tableLen) {
  uint32_t hash = 0;

  switch (hashMthd) {
    case HashStr_DJB2: {  // djb2 hash
      hash = 5381;
      int c;

      while ((c = *str++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    } break;
    case HashStr_Sdbm: {  // sdbm hash
      int c;

      while ((c = *str++)) hash = c + (hash << 6) + (hash << 16) - hash;
    } break;
  }

  return hash % tableLen;
}

static TagDatabaseErrors TagDB_RemoveFromHash(uint32_t tagHash, uint64_t obj) {
  LockSpinlock(tag_locks[tagHash]);

  if (tag_cnts[tagHash] == 0) {
    UnlockSpinlock(tag_locks[tagHash]);
    return TagDatabaseErrors_InvalidTag;
  }

  // find the object in the table
  uint32_t idx = binsearch(tags[tagHash], tag_cnts[tagHash], obj);
  if (idx >= tag_cnts[tagHash]) {
    // Object not found
    UnlockSpinlock(tag_locks[tagHash]);
    return TagDatabaseErrors_None;
  }

  // remove it and sort the table
  // removal is done by setting the value to -1, thus it gets sorted to the
  // end,
  tags[tagHash][idx] = -1;
  sort(tags[tagHash], tag_cnts[tagHash]);

  // update the tag_cnts
  tag_cnts[tagHash]--;

  // remove the backing memory if the count is 0
  if (tag_cnts[tagHash] == 0) {
    UnmapPage(GetActiveVirtualMemoryInstance(), (uint64_t)tags[tagHash],
              MAX_ENTRIES_PER_TAG * sizeof(uint64_t));
  }

  UnlockSpinlock(tag_locks[tagHash]);
  return TagDatabaseErrors_None;
}

TagDatabaseErrors TagDB_Initialize(void) {
  // Setup the tag table size
  {
    uint64_t tagTable_addr = 0;

    if (FindFreeVirtualAddress(
            GetActiveVirtualMemoryInstance(), &tagTable_addr,
            MAX_TAG_COUNT * sizeof(uint64_t *), MemoryAllocationType_Heap,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
                MemoryAllocationFlags_Kernel) != MemoryAllocationErrors_None) {
      // TODO: go back and make the kernel panic on any initialization
      // failures.
      PANIC("Tag Table Initialization Failed.");
    }

    if (MapPage(
            GetActiveVirtualMemoryInstance(), 0, tagTable_addr,
            MAX_TAG_COUNT * sizeof(uint64_t *), CachingModeWriteBack,
            MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
                MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Present) !=
        MemoryAllocationErrors_None) {
      // TODO: Panic
      PANIC("Tag Table Initialization Failed.");
    }

    // The tag table has now been allocated.
    tags = (uint64_t **)tagTable_addr;
  }

  // Setup the tag counts
  {
    uint64_t tagTable_addr = 0;

    if (FindFreeVirtualAddress(
            GetActiveVirtualMemoryInstance(), &tagTable_addr,
            MAX_TAG_COUNT * sizeof(uint32_t), MemoryAllocationType_Heap,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
                MemoryAllocationFlags_Kernel) != MemoryAllocationErrors_None) {
      // TODO: go back and make the kernel panic on any initialization
      // failures.
      PANIC("Tag Count Table Initialization Failed.");
    }

    if (MapPage(
            GetActiveVirtualMemoryInstance(), 0, tagTable_addr,
            MAX_TAG_COUNT * sizeof(uint32_t), CachingModeWriteBack,
            MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
                MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Present) !=
        MemoryAllocationErrors_None) {
      // TODO: Panic
      PANIC("Tag Count Table Initialization Failed.");
    }

    // The tag table has now been allocated.
    tag_cnts = (uint32_t *)tagTable_addr;
  }

  // Setup the tag locks
  {
    uint64_t tagTable_addr = 0;

    if (FindFreeVirtualAddress(
            GetActiveVirtualMemoryInstance(), &tagTable_addr,
            MAX_TAG_COUNT * sizeof(Spinlock), MemoryAllocationType_Heap,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
                MemoryAllocationFlags_Kernel) != MemoryAllocationErrors_None) {
      // TODO: go back and make the kernel panic on any initialization
      // failures.
      PANIC("Tag Lock Table Initialization Failed.");
    }

    if (MapPage(
            GetActiveVirtualMemoryInstance(), 0, tagTable_addr,
            MAX_TAG_COUNT * sizeof(Spinlock), CachingModeWriteBack,
            MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
                MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Present) !=
        MemoryAllocationErrors_None) {
      // TODO: Panic
      PANIC("Tag Lock Table Initialization Failed.");
    }

    // The tag table has now been allocated.
    tag_locks = (Spinlock *)tagTable_addr;
  }

  // Now initialize the counts
  for (int i = 0; i < MAX_TAG_COUNT; i++) {
    tag_cnts[i] = 0;
    tag_locks[i] = CreateSpinlock();
  }

  return TagDatabaseErrors_None;
}

TagDatabaseErrors TagDB_AddObjectToTag(const char *tag, uint64_t obj) {
  uint32_t tagHash = hash_str(HashStr_DJB2, tag, MAX_TAG_COUNT);

  LockSpinlock(tag_locks[tagHash]);
  if (tag_cnts[tagHash] == 0) {
    // Setup the tag entry
    {
      uint64_t tagTable_addr = 0;

      if (FindFreeVirtualAddress(
              GetActiveVirtualMemoryInstance(), &tagTable_addr,
              MAX_ENTRIES_PER_TAG * sizeof(uint64_t), MemoryAllocationType_Heap,
              MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
                  MemoryAllocationFlags_Kernel) !=
          MemoryAllocationErrors_None) {
        // TODO: go back and make the kernel panic on any initialization
        // failures.
        PANIC("Tag Entry Table Initialization Failed.");
      }

      if (MapPage(GetActiveVirtualMemoryInstance(), 0, tagTable_addr,
                  MAX_ENTRIES_PER_TAG * sizeof(uint64_t), CachingModeWriteBack,
                  MemoryAllocationType_Heap |
                      MemoryAllocationType_ReservedAllocation,
                  MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec |
                      MemoryAllocationFlags_Kernel |
                      MemoryAllocationFlags_Present) !=
          MemoryAllocationErrors_None) {
        // TODO: Panic
        PANIC("Tag Entry Table Initialization Failed.");
      }

      // The tag hash table has now been allocated.
      tags[tagHash] = (uint64_t *)tagTable_addr;
    }
  }

  if (tag_cnts[tagHash] < MAX_ENTRIES_PER_TAG - 1) {
    tags[tagHash][tag_cnts[tagHash]] = obj;
    tag_cnts[tagHash]++;

    sort(tags[tagHash], tag_cnts[tagHash]);

    UnlockSpinlock(tag_locks[tagHash]);
    return TagDatabaseErrors_None;
  }

  UnlockSpinlock(tag_locks[tagHash]);
  return TagDatabaseErrors_EntryFull;
}

TagDatabaseErrors TagDB_RemoveObjectFromTag(const char *tag, uint64_t obj) {
  uint32_t tagHash = hash_str(HashStr_DJB2, tag, MAX_TAG_COUNT);
  return TagDB_RemoveFromHash(tagHash, obj);
}

TagDatabaseErrors TagDB_RemoveObject(uint64_t obj) {
  for (uint32_t i = 0; i < MAX_TAG_COUNT; i++) TagDB_RemoveFromHash(i, obj);
  return TagDatabaseErrors_None;
}

TagDatabaseErrors TagDB_EvaluateQuery(const char *path, uint64_t *cnt,
                                      uint64_t *objs) {
  path = NULL;
  cnt = NULL;
  objs = NULL;
  // TODO: write a path parser to go into libCardinal, import it here to
  // evaluate the query
  // TODO: cache past query per process to reduce the reexecution cost
  return TagDatabaseErrors_None;
}