// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _LIB_CARDINAL_SYSCALL_LIST
#define _LIB_CARDINAL_SYSCALL_LIST

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  Syscall_NumStart = 1,

  Syscall_Nanosleep = Syscall_NumStart,
  Syscall_R0_GetBootInfo,  //!< Get the boot information.

  Syscall_SetProperty,
  Syscall_GetProperty,

  Syscall_GetErrno,  //!< Get the error for the latest syscall

  Syscall_AllocateSharedMemory,     //!< Allocate shared memory.
  Syscall_R0_AllocateSharedMemory,  //!< Allocate shared memory, controlling the
  //! physical address.
  Syscall_ApplySharedMemoryKey,  //!< Apply an existing shared memory key. TODO:
                                 //! remove this, move functionality to AddKey
  Syscall_GetSharedMemoryKey,    //!< Get a shared memory key.

  Syscall_R0_Map,  //!< Kernel level memory map that allows direct access to the
  //! kernel arbitrary memory mapping functions.
  Syscall_R0_Unmap,  //!< Unmap a mapping.
  Syscall_Unmap,     //!< Unmap any shared memory region.
  Syscall_Brk,       //!< Increase the process heap break.

  Syscall_R0_AllocatePages,        //!< Allocate a continuous range of pages.
  Syscall_R0_FreePages,            //!< Free a continuous range of pages.
  Syscall_R01_GetPhysicalAddress,  //!< Get a physical address for a virtual
  //! address for the provided pid.

  Syscall_CreateProcess,  //!< Load an executable from memory, along with
  //! descriptor copies. TODO: make it take a key

  Syscall_R0_GetThreadInfo,   //!< Get the kernel's thread information.
  Syscall_CreateThread,       //!< Create a new thread.
  Syscall_KillThread,         //!< Kill a thread by its TID.
  Syscall_ExitDeleteThread,   //!< Exit and delete the current thread.
  Syscall_SetThreadIsPaused,  //!< Allow pausing/resuming threads.

  Syscall_R01_AllocateInterrupts,  //!< Allocate a block of interrupts TODO:
                                   //! make this return keys for each interrupt
  //! signal
  Syscall_R01_RegisterForInterrupts,  //!< Register for a block of interrupts
                                      //! TODO: make this return keys for each
  //! interrupt signal

  Syscall_WaitSignal,  //! TODO: make this wait for the specified key to be
                       //! signalled (signal type key)
  Syscall_HandleSignal,
  Syscall_Signal,  //!< Send a signal to a process by its id.

  // TODO: revamp key system to be just a plain guid, with a system for fast
  // lookup. In kernel we can store the table indexes, but nowhere else.

  // TODO: track keys for objects in kernel as well.

  // TODO: consider making PIDs and TIDs also be keys outside of kernel?

  // TODO: allow locking/unlocking of keys providing a global synchronization
  // mechanism

  // NOTE: Allow character devices, block devices and tag providers under the
  // same IO system
  // Character devices have stateless read/write, block devices have stateless
  // read block/write block
  // Tag providers register under their unique tag

  Syscall_RegisterTagProvider,  // TODO: get set of signal type keys for
                                // receiving requests
  Syscall_GetTagList,           // TODO: get list of tags
  Syscall_GetWithTag,           // TODO: return keys of objects
  Syscall_Read,    // TODO: flags for tag, properties, content fields, ACL if
                   // enough permissions
  Syscall_Write,   // TODO: flags for tag, properties, content fields, ACL if
                   // enough permissions
  Syscall_Create,  // TODO: creation, flags for devices, depending on flags,
                   // different number of keys are returned
  Syscall_Remove,  // TODO: object removal

  // TODO: provide a syscall to read the state of a key.

  Syscall_CreateKey,         //!< Create general purpose key.
  Syscall_AddKey,            //!< Add a key to a process, applying it.
  Syscall_RemoveKey,         //!< Remove a key from the process, unapplying it.
  Syscall_GetKeyIndex,       //!< Get the index of a given key in the process.
  Syscall_UseKey,            //!< Increment the use count of a key.
  Syscall_GetKeyUsageCount,  //!< Get the number of times a key has been used.

  Syscall_NumEnd = Syscall_GetKeyUsageCount,
} SyscallFunctions;

#ifdef __cplusplus
}
#endif

#endif