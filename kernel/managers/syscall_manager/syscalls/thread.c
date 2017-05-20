/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "libs/libc/include/thread.h"
#include "common.h"
#include "libs/libCardinal/include/syscall.h"
#include "managers.h"
#include "priv_syscalls.h"
#include "syscalls_all.h"

struct ElfSetupParameters {
  uint16_t ver;
  uint16_t size;
  uint16_t page_size;
  uint16_t argc;
  uint64_t rnd_seed;
  uint64_t elf_entry_point;
  char argv[1];
};

uint64_t CreateProcess_Syscall(void *prog, uint64_t sz, uint32_t *keys,
                               char *argv[], int argc) {
  // Get the elf entry point
  ElfInformation elfData;
  if (ReadElf(prog, sz, ElfLimitations_64Bit, &elfData) !=
      ElfLoaderError_Success) {
    SyscallSetErrno(-EINVAL);
    return -1;
  }

  // Create new process
  UID pid = CreateThread(GetCurrentProcessUID(), true,
                         ThreadPermissionLevel_User, elfData.entry_point, NULL);

  // Setup the argument passing
  uint64_t arg_len = 0;
  for (int i = 0; i < argc; i++) {
    arg_len += strlen(argv[i]) + 1;
  }

  uint64_t net_size =
      argc * sizeof(char *) + arg_len + sizeof(struct ElfSetupParameters);

  if (net_size % PAGE_SIZE) net_size += (PAGE_SIZE - (net_size % PAGE_SIZE));

  uint64_t phys_addr = 0;
  struct ElfSetupParameters *params = NULL;  // MAP

  phys_addr = AllocatePhysicalPageCont(net_size / PAGE_SIZE,
                                       PhysicalMemoryAllocationFlags_None);
  if (phys_addr == 0) {
    SyscallSetErrno(-ENOMEM);
    return -1;
    // TODO: delete thread
  }

  ProcessInfo *pInfo = NULL;
  if (GetProcessReference(pid, &pInfo) != ThreadError_None) {
    SyscallSetErrno(-EINVAL);
    return -1;
    // TODO: delete thread
  }

  // Map the memory into the target process
  uint64_t target_virt_addr = 0;

  FindFreeVirtualAddress(
      pInfo->PageTable, &target_virt_addr, net_size, MemoryAllocationType_MMap,
      MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Read |
          MemoryAllocationFlags_Write | MemoryAllocationFlags_User |
          MemoryAllocationFlags_Present);

  if (MapPage(pInfo->PageTable, phys_addr, target_virt_addr, net_size,
              CachingModeWriteBack, MemoryAllocationType_MMap,
              MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Read |
                  MemoryAllocationFlags_Write | MemoryAllocationFlags_User |
                  MemoryAllocationFlags_Present) !=
      MemoryAllocationErrors_None) {
    ReturnProcessReference(pid);
    SyscallSetErrno(-ENOMEM);
    return -1;
    // TODO: delete thread
  }

  params = (struct ElfSetupParameters *)SetupTemporaryWriteMap(
      pInfo->PageTable, target_virt_addr, net_size);
  params->ver = 1;
  params->size = net_size;
  params->page_size = PAGE_SIZE;
  params->argc = argc;
  params->rnd_seed = (uint64_t)rand() << 32 | rand();
  params->elf_entry_point = (uint64_t)elfData.entry_point;

  char **argv_base = (char **)params->argv;
  char *arg_loc = (char *)argv_base + sizeof(char *) * argc;

  for (int i = 0; i < argc; i++) {
    argv_base[i] =
        (char *)(target_virt_addr + ((uint64_t)arg_loc - (uint64_t)params));

    uint32_t len = strlen(argv[i]) + 1;
    strcpy(arg_loc, argv[i]);
    argv += len;
    arg_loc += len;
  }

  UninstallTemporaryWriteMap(target_virt_addr, net_size);
  ReturnProcessReference(pid);

  // Load ELF from prog into the created process
  if (LoadElf(prog, sz, ElfLimitations_64Bit, pid) != ElfLoaderError_Success) {
    SyscallSetErrno(-ENOMEM);

    // TODO: delete thread
    return -1;
  }

  // Apply keys
  uint32_t *k = keys;
  while (*k != (uint32_t)-1) {
    Key_t k_cur;
    if (GetKey(GetCurrentProcessUID(), *k, &k_cur) == ThreadError_None) {
      int32_t dummy = 0;
      AddKey(pid, &k_cur, &dummy);

      // Reduce the reference count now that the key has been added
      KeyMan_DecrementRefCount(&k_cur);
    } else {
      // TODO: delete thread
      SyscallSetErrno(-EINVAL);
      return -1;
    }
    k++;
  }

  // Start the process
  SetThreadState(pid, ThreadState_Initialize);

  SyscallSetErrno(0);
  return pid;
}

/*
uint64_t R0_GetThreadInfo_Syscall(void) {

}*/

uint64_t CreateThread_Syscall(UID parent, ThreadEntryPoint entry_point,
                              void *arg) {
  uint32_t gid = GetProcessGroupID(GetCurrentProcessUID());
  if (gid > 0) {
    SyscallSetErrno(-EPERM);
    return -1;
  }

  UID tid =
      CreateThread(parent, false, ThreadPermissionLevel_User, entry_point, arg);

  // TODO: interpret errors

  SetThreadState(tid, ThreadState_Initialize);

  SyscallSetErrno(0);
  return tid;
}

/*
uint64_t KillThread_Syscall(void) {}

uint64_t ExitDeleteThread_Syscall(void) {}

uint64_t SetThreadIsPaused_Syscall(void) {}
*/