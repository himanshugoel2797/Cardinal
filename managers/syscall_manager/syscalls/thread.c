#include "syscalls_all.h"
#include "libs/libCardinal/include/syscall.h"
#include "libs/libc/include/thread.h"

#include "managers.h"

uint64_t
set_tid_address(void* address) {
    SetClearChildTIDAddress(GetCurrentThreadUID(),
                            address);

    return GetCurrentThreadUID();
}

uint64_t
SetTidAddress_Syscall(uint64_t UNUSED(instruction_pointer),
                      uint64_t syscall_num,
                      uint64_t *syscall_params) {
    if(syscall_num != Syscall_SetTidAddress)
        return ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1)
        return EINVAL;

    return set_tid_address((void*)data->params[0]);
}

uint64_t
clone(int (*func)(void*),
      void *child_stack,
      uint64_t flags,
      void *arg,
      void *ptid,
      void *ctid,
      void *tls) {
    
    UID parentProcID = GetCurrentProcessUID();
     if(flags & CLONE_THREAD)
     {
        //Create a new thread
     }else{
       //Fork a new process
        UID cpid = ForkCurrentProcess();
        if(cpid == 0)
        {
            if(child_stack == NULL)
              CreateThread(GetCurrentProcessUID(), ThreadPermissionLevel_User, (ThreadEntryPoint)func, arg);
              FreeThread(GetCurrentThreadUID());
              while(1);
            }else{

            }
        }
     }
    return -1;
    //Create the thread with the parentProcID
}

uint64_t
Clone_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params) {
    if(syscall_num != 0)
        return -1;

    syscall_params = NULL;
    return -1;
}