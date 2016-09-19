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
clone(int (*func)(void*),
      void *child_stack,
      uint64_t flags,
      void *arg,
      void *ptid,
      void *ctid,
      void *tls) {

    UID id = 0;
    UID parentProcess = GetCurrentProcessUID();

    if(flags & CLONE_PARENT)
        parentProcess = GetCurrentProcessParentUID();

    if(flags & CLONE_THREAD) {
        //Create a new thread

        CRegisters regs;
        regs.rip = (uint64_t)func;
        regs.rsp = (uint64_t)child_stack;
        regs.rbp = 0;
        regs.rax = 0;
        regs.rbx = 0;
        regs.rcx = 0;
        regs.rdx = 0;
        regs.rsi = 0;
        regs.rdi = (uint64_t)arg;
        regs.r8 = 0;
        regs.r9 = 0;
        regs.r10 = 0;
        regs.r11 = 0;
        regs.r12 = 0;
        regs.r13 = 0;
        regs.r14 = 0;
        regs.r15 = 0;
        regs.rflags = 0;
        regs.ss = 0x20;
        regs.cs = 0x28;
        if(flags & CLONE_SETTLS)regs.tls = tls;
        if(flags & CLONE_CHILD_SETTID)regs.set_tid = ctid;
        if(flags & CLONE_CHILD_CLEARTID)regs.clear_tid = ctid;
        if(flags & CLONE_PARENT_SETTID)regs.p_tid = ptid;
        id = CreateThreadADV(GetCurrentProcessUID(), &regs);
    } else {
        //Fork a new process
        UID cpid = ForkCurrentProcess();
        if(cpid == 0) {
            if(child_stack == NULL)
                id = CreateThread(GetCurrentProcessUID(), ThreadPermissionLevel_User, (ThreadEntryPoint)func, arg);
        } else {
            CRegisters regs;
            regs.rip = (uint64_t)func;
            regs.rsp = (uint64_t)child_stack;
            regs.rbp = 0;
            regs.rax = 0;
            regs.rbx = 0;
            regs.rcx = 0;
            regs.rdx = 0;
            regs.rsi = 0;
            regs.rdi = (uint64_t)arg;
            regs.r8 = 0;
            regs.r9 = 0;
            regs.r10 = 0;
            regs.r11 = 0;
            regs.r12 = 0;
            regs.r13 = 0;
            regs.r14 = 0;
            regs.r15 = 0;
            regs.rflags = 0;
            regs.ss = 0x20;
            regs.cs = 0x28;
            if(flags & CLONE_SETTLS)regs.tls = tls;
            if(flags & CLONE_CHILD_SETTID)regs.set_tid = ctid;
            if(flags & CLONE_CHILD_CLEARTID)regs.clear_tid = ctid;
            if(flags & CLONE_PARENT_SETTID)regs.p_tid = ptid;
            id = CreateThreadADV(GetCurrentProcessUID(), &regs);
        }
    }

    //Create the thread with the parentProcID
    return id;
}

uint64_t
Clone_Syscall(uint64_t UNUSED(instruction_pointer),
              uint64_t syscall_num,
              uint64_t *syscall_params) {
    if(syscall_num != Syscall_Clone)
        return -ENOSYS;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 1)
        return -EINVAL;

    uint64_t *params = (uint64_t*)data->params[0];

    return clone((int (*)(void*))params[0],
                 (void*)params[1],
                 params[2],
                 (void*)params[3],
                 (void*)params[4],
                 (void*)params[5],
                 (void*)params[6]);
}

uint64_t
R0Fork_Syscall(uint64_t UNUSED(instruction_pointer),
             uint64_t syscall_num,
             uint64_t *syscall_params) {
    if(syscall_num != Syscall_R0_Fork)
        return -ENOSYS;

    if(GetProcessGroupID(GetCurrentProcessUID()) != 0)
        return -EPERM;

    SyscallData *data = (SyscallData*)syscall_params;

    if(data->param_num != 0)
        return -EINVAL;

    return ForkCurrentProcess();
}