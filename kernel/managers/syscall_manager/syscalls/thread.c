/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "libs/libc/include/thread.h"
#include "libs/libCardinal/include/syscall.h"
#include "priv_syscalls.h"
#include "syscalls_all.h"

#include "managers.h"
uint64_t CreateProcess_Syscall(void *prog, uint32_t *keys, char *argv[],
                               int argc) {
  // Get the elf entry point
  uint64_t elf_entry_pt = 0;

  // Setup the argument passing

  // Create new process
  UID pid = CreateThread(GetCurrentProcessUID(), true,
                         ThreadPermissionLevel_User, elf_entry_pt, NULL);

  // Load ELF from prog into the created process

  // Apply keys

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