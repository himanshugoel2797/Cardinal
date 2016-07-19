#include "process.h"
#include "kmalloc.h"
#include "memory.h"
#include "common.h"

static List *processes;
static ProcessInformation *root = NULL;

void
ProcessSys_Initialize(void) {
    root = kmalloc(sizeof(ProcessInformation));
    root->ID = 0;	//Root process ID is 0
    strcpy(root->Name, "Root Process");
    root->Status = ProcessStatus_Executing;
    root->Permissions = ProcessPermissions_None;
    root->PageTable = GetActiveVirtualMemoryInstance();
    root->SyscallFlags = ProcessSyscallFlags_PermissionsLocked;
    root->HeapBreak = 0;
    root->SignalHandlers = kmalloc(sizeof(sigaction) * SUPPORTED_SIGNAL_COUNT);
    memset(root->SignalHandlers, 0, sizeof(sigaction) * SUPPORTED_SIGNAL_COUNT);
    root->PendingSignals = List_Create(CreateSpinlock());
    root->Children = List_Create(CreateSpinlock());
    root->Parent = NULL;

    root->reference_count = 0;
    root->lock = CreateSpinlock();

    processes = List_Create(CreateSpinlock());
    List_AddEntry(processes, root);
}

bool
ProcessSys_IsInitialized(void) {
    return !(root == NULL);
}

ProcessErrors
ForkProcess(ProcessInformation *src,
            ProcessInformation **dest) {
    LockSpinlock(src->lock);
    ProcessInformation *dst = kmalloc(sizeof(ProcessInformation));
    dst->ID = new_uid();
    dst->Status = src->Status;
    dst->Permissions = src->Permissions;
    dst->PageTable = kmalloc(sizeof(ManagedPageTable));
    dst->SyscallFlags = ProcessSyscallFlags_None;
    dst->HeapBreak = 0;
    dst->SignalHandlers = kmalloc(sizeof(sigaction) * SUPPORTED_SIGNAL_COUNT);
    memcpy(dst->SignalHandlers, src->SignalHandlers, sizeof(sigaction) * SUPPORTED_SIGNAL_COUNT);

    dst->PendingSignals = List_Create(CreateSpinlock());
    dst->Children = List_Create(CreateSpinlock());
    List_AddEntry(src->Children, (void*)dst->ID);
    dst->Parent = src;

    ForkTable(src->PageTable, dst->PageTable);

    dst->reference_count = 0;
    dst->lock = CreateSpinlock();

    //Add dst to src's children
    UnlockSpinlock(src->lock);

    List_AddEntry(processes, dst);
    *dest = dst;

    return ProcessErrors_None;
}

UID
ForkCurrentProcess(void) {
    ProcessInformation *dst_proc = NULL;
    ProcessInformation *src_proc = NULL;
    GetProcessReference(GetCurrentProcessUID(), &src_proc);

    CRegisters regs;
    regs.rip = (uint64_t)__builtin_return_address(0);
    regs.rsp = (uint64_t)__builtin_frame_address(0);
    __asm__ ("movq %%rax, %%rax" : "=a"(regs.rax));
    __asm__ ("movq %%rbp, %%rax" : "=a"(regs.rbp));
    __asm__ ("movq %%rbx, %%rax" : "=a"(regs.rbx));
    __asm__ ("movq %%rcx, %%rax" : "=a"(regs.rcx));
    __asm__ ("movq %%rdx, %%rax" : "=a"(regs.rdx));
    __asm__ ("movq %%rsi, %%rax" : "=a"(regs.rsi));
    __asm__ ("movq %%rdi, %%rax" : "=a"(regs.rdi));
    __asm__ ("movq %%r8, %%rax" : "=a"(regs.r8));
    __asm__ ("movq %%r9, %%rax" : "=a"(regs.r9));
    __asm__ ("movq %%r10, %%rax" : "=a"(regs.r10));
    __asm__ ("movq %%r11, %%rax" : "=a"(regs.r11));
    __asm__ ("movq %%r12, %%rax" : "=a"(regs.r12));
    __asm__ ("movq %%r13, %%rax" : "=a"(regs.r13));
    __asm__ ("movq %%r14, %%rax" : "=a"(regs.r14));
    __asm__ ("movq %%r15, %%rax" : "=a"(regs.r15));
    __asm__ ("pushf\n\tpopq %%rax" : "=a"(regs.rflags));
    __asm__ ("movw %%ss, %%ax" : "=a"(regs.ss));
    __asm__ ("movw %%cs, %%ax" : "=a"(regs.cs));

    ForkProcess(src_proc, &dst_proc);
    CreateThreadADV(dst_proc->ID, &regs);

    return -1;
}

ProcessErrors
GetProcessInformation(UID           pid,
                      ProcessInformation	*procInfo) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            LockSpinlock(pInf->lock);
            memcpy(procInfo, pInf, sizeof(ProcessInformation));
            UnlockSpinlock(pInf->lock);
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
GetProcessReference(UID           pid,
                    ProcessInformation    **procInfo) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            *procInfo = pInf;
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}


ProcessErrors
SetProcessPermissions(UID pid,
                      ProcessPermissions perms) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            LockSpinlock(pInf->lock);
            pInf->Permissions = perms;
            UnlockSpinlock(pInf->lock);
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
SetProcessSyscallStatus(UID pid,
                        ProcessSyscallFlags flags) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            LockSpinlock(pInf->lock);
            pInf->SyscallFlags = flags;
            UnlockSpinlock(pInf->lock);
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
SetProcessSigaction(UID pid,
                    int sig_no,
                    const sigaction *sig) {
    if(sig_no >= SUPPORTED_SIGNAL_COUNT)
        return ProcessErrors_Unknown;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            LockSpinlock(pInf->lock);
            memcpy(&pInf->SignalHandlers[sig_no], sig, sizeof(sigaction));
            UnlockSpinlock(pInf->lock);
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
GetProcessSigaction(UID pid,
                    int sig_no,
                    sigaction *sig) {
    if(sig_no >= SUPPORTED_SIGNAL_COUNT)
        return ProcessErrors_Unknown;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            if(sig != NULL)
                memcpy(sig, &pInf->SignalHandlers[sig_no], sizeof(sigaction));
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

void
RaiseSignal(UID pid,
            int sig_no) {
    pid = 0;
    sig_no = 0;
}