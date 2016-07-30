#include "process.h"
#include "kmalloc.h"
#include "memory.h"
#include "common.h"

static List *processes;
static ProcessInformation *root = NULL;
static volatile UID baseID = 0;

static UID
new_proc_uid(void) {
    register UID dummy = 1;
    __asm__ volatile("lock xadd %[dummy], (%[bs])" : [dummy]"+r"(dummy) : [bs]"r"(&baseID));
    return (UID)(uint32_t)dummy;
}

void
ProcessSys_Initialize(void) {
    root = kmalloc(sizeof(ProcessInformation));
    root->ID = new_proc_uid();	//Root process ID is 0
    strcpy(root->Name, u8"Root Process");
    root->Status = ProcessStatus_Executing;
    root->Permissions = ProcessPermissions_None;
    root->PageTable = GetActiveVirtualMemoryInstance();
    root->SyscallFlags = ProcessSyscallFlags_PermissionsLocked;
    root->HeapBreak = 0;
    root->SignalHandlers = kmalloc(sizeof(struct sigaction) * SUPPORTED_SIGNAL_COUNT);
    memset(root->SignalHandlers, 0, sizeof(struct sigaction) * SUPPORTED_SIGNAL_COUNT);
    root->PendingSignals = List_Create(CreateSpinlock());
    root->Children = List_Create(CreateSpinlock());
    root->Parent = NULL;

    root->PendingMessages = List_Create(CreateSpinlock());
    root->MessageLock = CreateSpinlock();

    root->Descriptors = List_Create(CreateSpinlock());
    root->PendingMessages = List_Create(CreateSpinlock());
    root->MessageLock = CreateSpinlock();

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
    dst->ID = new_proc_uid();
    dst->Status = src->Status;
    dst->Permissions = src->Permissions;
    dst->PageTable = kmalloc(sizeof(ManagedPageTable));
    dst->SyscallFlags = ProcessSyscallFlags_None;
    dst->HeapBreak = src->HeapBreak;
    dst->SignalHandlers = kmalloc(sizeof(struct sigaction) * SUPPORTED_SIGNAL_COUNT);
    memcpy(dst->SignalHandlers, src->SignalHandlers, sizeof(struct sigaction) * SUPPORTED_SIGNAL_COUNT);

    dst->PendingMessages = List_Create(CreateSpinlock());
    dst->PendingSignals = List_Create(CreateSpinlock());

    dst->MessageLock = CreateSpinlock();

    dst->Children = List_Create(CreateSpinlock());
    List_AddEntry(src->Children, (void*)(uint64_t)dst->ID);
    dst->Parent = src;

    ForkTable(src->PageTable, dst->PageTable);

    dst->Descriptors = List_Create(CreateSpinlock());

    for(uint64_t i = 0; i < List_Length(src->Descriptors); i++) {
        Descriptor *desc = (Descriptor*)List_EntryAt(src->Descriptors, i);


        Descriptor *dst_desc = kmalloc(sizeof(Descriptor));

        if(desc->Flags & DescriptorFlags_CloseOnExec) {
            dst_desc->Flags = DescriptorFlags_Free;
        } else {
            dst_desc->Flags = desc->Flags;
            dst_desc->AdditionalData = desc->AdditionalData;
        }

        List_AddEntry(dst->Descriptors, dst_desc);
    }

    dst->reference_count = 0;
    dst->lock = CreateSpinlock();

    src->reference_count++;

    //Add dst to src's children
    UnlockSpinlock(src->lock);

    List_AddEntry(processes, dst);
    *dest = dst;

    return ProcessErrors_None;
}

UID
ForkCurrentProcess(void) {

    CRegisters regs;
    regs.set_tid = NULL;
    regs.clear_tid = NULL;
    regs.p_tid = NULL;
    regs.tls = NULL;
    regs.rip = (uint64_t)__builtin_return_address(0);
    regs.rbp = *(uint64_t*)__builtin_frame_address(0);
    regs.rsp = (uint64_t)__builtin_frame_address(0);
    regs.rsp += 16;
    regs.rax = 0;
    __asm__ volatile("movq %%rbx, %%rax" : "=a"(regs.rbx));
    __asm__ volatile("movq %%rcx, %%rax" : "=a"(regs.rcx));
    __asm__ volatile("movq %%rdx, %%rax" : "=a"(regs.rdx));
    __asm__ volatile("movq %%rsi, %%rax" : "=a"(regs.rsi));
    __asm__ volatile("movq %%rdi, %%rax" : "=a"(regs.rdi));
    __asm__ volatile("movq %%r8, %%rax" : "=a"(regs.r8));
    __asm__ volatile("movq %%r9, %%rax" : "=a"(regs.r9));
    __asm__ volatile("movq %%r10, %%rax" : "=a"(regs.r10));
    __asm__ volatile("movq %%r11, %%rax" : "=a"(regs.r11));
    __asm__ volatile("movq %%r12, %%rax" : "=a"(regs.r12));
    __asm__ volatile("movq %%r13, %%rax" : "=a"(regs.r13));
    __asm__ volatile("movq %%r14, %%rax" : "=a"(regs.r14));
    __asm__ volatile("movq %%r15, %%rax" : "=a"(regs.r15));
    __asm__ volatile("pushf\n\tpopq %%rax" : "=a"(regs.rflags));
    __asm__ volatile("movw %%ss, %%ax" : "=a"(regs.ss));
    __asm__ volatile("movw %%cs, %%ax" : "=a"(regs.cs));

    regs.set_tid = NULL;
    regs.clear_tid = NULL;
    regs.p_tid = NULL;
    regs.tls = NULL;

    ProcessInformation *dst_proc = NULL;
    ProcessInformation *src_proc = NULL;
    GetProcessReference(GetCurrentProcessUID(), &src_proc);

    ForkProcess(src_proc, &dst_proc);
    CreateThreadADV(dst_proc->ID, &regs);

    return dst_proc->ID;
}

ProcessErrors
GetProcessInformation(UID           pid,
                      ProcessInformation    *procInfo) {
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

        if(List_Length(processes) > 3)__asm__ ("cli\n\thlt");

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

char*
GetCurrentProcessWorkingDirectory(void) {
    ProcessInformation pinfo;
    GetProcessInformation(GetCurrentProcessUID(), &pinfo);

    return pinfo.WorkingDirectory;
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
                    const struct sigaction *sig) {
    if(sig_no >= SUPPORTED_SIGNAL_COUNT)
        return ProcessErrors_Unknown;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            LockSpinlock(pInf->lock);
            memcpy(&pInf->SignalHandlers[sig_no], sig, sizeof(struct sigaction));
            UnlockSpinlock(pInf->lock);
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
GetProcessSigaction(UID pid,
                    int sig_no,
                    struct sigaction *sig) {
    if(sig_no >= SUPPORTED_SIGNAL_COUNT)
        return ProcessErrors_Unknown;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            if(sig != NULL)
                memcpy(sig, &pInf->SignalHandlers[sig_no], sizeof(struct sigaction));
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


bool
PostMessage(Message *msg) {
    ProcessInformation *pInfo;
    GetProcessReference(msg->DestinationPID, &pInfo);

    if(List_Length(pInfo->PendingMessages) > MAX_PENDING_MESSAGE_CNT)return FALSE;

    Message *m = kmalloc(sizeof(Message));
    if(m == NULL)return FALSE;

    LockSpinlock(pInfo->MessageLock);
    memcpy(m, msg, sizeof(Message));
    List_AddEntry(pInfo->PendingMessages, m);
    UnlockSpinlock(pInfo->MessageLock);

    return TRUE;
}

bool
GetMessage(Message *msg) {
    ProcessInformation *pInfo;
    GetProcessReference(GetCurrentProcessUID(), &pInfo);

    if(List_Length(pInfo->PendingMessages) == 0)return FALSE;

    LockSpinlock(pInfo->MessageLock);
    Message *tmp = (Message*)List_EntryAt(pInfo->PendingMessages, 0);
    List_Remove(pInfo->PendingMessages, 0);

    if(msg != NULL)memcpy(msg, tmp, sizeof(Message));

    kfree(tmp);
    UnlockSpinlock(pInfo->MessageLock);

    __asm__("cli\n\thlt");
    return TRUE;
}

bool
GetMessageFrom(Message *msg,
               UID SourcePID) {
    ProcessInformation *pInfo;
    GetProcessReference(GetCurrentProcessUID(), &pInfo);

    if(List_Length(pInfo->PendingMessages) == 0)return FALSE;

    LockSpinlock(pInfo->MessageLock);
    Message *tmp = NULL;

    for(uint64_t i = 0; i < List_Length(pInfo->PendingMessages); i++) {
        tmp = (Message*)List_EntryAt(pInfo->PendingMessages, 0);

        if(tmp->SourcePID == SourcePID) {
            List_Remove(pInfo->PendingMessages, 0);
            if(msg != NULL)memcpy(msg, tmp, sizeof(Message));
            kfree(tmp);

            UnlockSpinlock(pInfo->MessageLock);
            return TRUE;
        }
    }

    UnlockSpinlock(pInfo->MessageLock);
    return FALSE;
}

ProcessErrors
GetDescriptor(UID pid,
              int d_num,
              Descriptors *desc) {

    if(d_num < 0)return ProcessErrors_Unknown;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {

            LockSpinlock(pInf->lock);
            if(List_Length(pInf->Descriptors) > (uint64_t)d_num) {

                if(desc != NULL)
                    memcpy(desc, List_EntryAt(pInf->Descriptors, d_num), sizeof(Descriptor));

                UnlockSpinlock(pInf->lock);
                return ProcessErrors_None;
            }

            UnlockSpinlock(pInf->lock);
            return ProcessErrors_DescriptorNotFound;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
CreateDescriptor(UID pid,
                 Descriptors *desc,
                 int *d_num) {
    if(desc == NULL)return ProcessErrors_Unknown;
    if(d_num == NULL)return ProcessErrors_Unknown;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {

            LockSpinlock(pInf->lock);
            bool desc_found = FALSE;
            for(uint64_t j = 0; j < List_Length(pInf->Descriptors); j++) {
                Descriptor *d = (Descriptor*)List_EntryAt(pInf->Descriptors, j);
                if(d->Flags == DescriptorFlags_Free) {
                    memcpy(d, desc, sizeof(Descriptor));
                    *d_num = (int)j;
                    desc_found = TRUE;
                    break;
                }
            }
            if(!desc_found) {
                Descriptor *d = kmalloc(sizeof(Descriptor));
                memcpy(d, desc, sizeof(Descriptor));
                *d_num = (int)List_Length(pInf->Descriptors);
                List_AddEntry(pInf->Descriptors, d);
            }
            UnlockSpinlock(pInf->lock);
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
CloseDescriptor(UID pid,
                int d_num) {

    if(d_num < 0)return ProcessErrors_Unknown;

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {

            LockSpinlock(pInf->lock);
            if(List_Length(pInf->Descriptors) > (uint64_t)d_num) {

                Descriptor *d = (Descriptor*)List_EntryAt(pInf->Descriptors, d_num);
                d->Flags = DescriptorFlags_Free;
                d->AdditionalData = 0;
                d->TargetPID = 0;

                UnlockSpinlock(pInf->lock);
                return ProcessErrors_None;
            }

            UnlockSpinlock(pInf->lock);
            return ProcessErrors_DescriptorNotFound;

        }
    }
    return ProcessErrors_UIDNotFound;

}