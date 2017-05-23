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

    Syscall_Brk,  //!< Increase the process heap break.

    Syscall_R01_GetPhysicalAddress,  //!< Get a physical address for a virtual
    //! address for the provided pid.

    Syscall_CreateProcess,  //!< Load an executable from memory, along with
    //! descriptor copies. TODO: make it take a key

    Syscall_CreateThread,      //!< Create a new thread.
    Syscall_KillThread,        //!< Kill a thread by its TID.
    Syscall_ExitDeleteThread,  //!< Exit and delete the current thread.

    Syscall_WaitSignal,  //! TODO: make this wait for the specified key to be
    //! signalled (signal type key)
    Syscall_HandleSignal,
    Syscall_Signal,  //!< Send a signal to a process by its id.

    // NOTE: Allow character devices, block devices and tag providers under the
    // same IO system
    // Character devices have stateless read/write, block devices have stateless
    // read block/write block
    // Tag providers register under their unique tag

    // TODO: decouple key functions from rest of the code, refactor stuff to form
    // a 'key layer' on top as a separate manager.

    Syscall_TryLockKey,
    Syscall_UnlockKey,

    Syscall_RegisterTagProvider,  // TODO: get set of signal type keys for
    // receiving requests
    Syscall_GetWithTag,           // TODO: return keys of objects
    Syscall_Read,  // TODO: flags for tag, properties, content fields, ACL, key if
    // enough permissions
    Syscall_Write,  // TODO: flags for tag, properties, content fields, ACL, key
    // if
    // enough permissions

    Syscall_CreateKey,         //!< Create key.
    Syscall_AddKey,            //!< Add a key to a process, applying it.
    Syscall_Dup,               //!< Duplicate process key.
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