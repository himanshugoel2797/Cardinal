#ifndef _MEM_SRV_DB_H_
#define _MEM_SRV_DB_H_

#include <cardinal/cardinal_types.h>

void
MemDB_Initialize(void);

void
MemDB_AddProcess(UID pid);

int
MemDB_ProcessExists(UID pid);

int
MemDB_AllocateMemory(UID pid,
                     uint64_t len,
                     uint64_t *address);

void
MemDB_FreeMemory(UID pid,
                 uint64_t address,
                 uint64_t len);

int
MemDB_ShareMemory(UID src,
                  UID dst,
                  uint64_t address,
                  uint64_t len);

void
MemDB_FreeProcess(UID pid);

#endif