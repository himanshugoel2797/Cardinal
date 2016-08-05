#include "process.h"
#include "kmalloc.h"
#include "memory.h"
#include "common.h"

static List *processes;
static ProcessInformation *root = NULL;
static volatile UID baseID = 0;
static UID specialDestinationPIDs[CARDINAL_IPCDEST_NUM];
static Spinlock specialPIDLock = NULL;

static UID
new_proc_uid(void) {
    register UID dummy = 1;
    __asm__ volatile("lock xadd %[dummy], (%[bs])" : [dummy]"+r"(dummy) : [bs]"r"(&baseID));
    return (UID)(uint32_t)dummy;
}

void
ProcessSys_Initialize(void) {

    for(int i = 0; i < CARDINAL_IPCDEST_NUM; i++)
        specialDestinationPIDs[i] = 0;

    specialPIDLock = CreateSpinlock();

    root = kmalloc(sizeof(ProcessInformation));
    root->ID = new_proc_uid();  //Root process ID is 0
    strcpy(root->Name, u8"Root Process");
    root->Status = ProcessStatus_Executing;
    root->Permissions = ProcessPermissions_None;
    root->PageTable = GetActiveVirtualMemoryInstance();
    root->SyscallFlags = ProcessSyscallFlags_PermissionsLocked;
    root->HeapBreak = 0;
    root->Children = List_Create(CreateSpinlock());
    root->Parent = NULL;

    root->PendingMessages = List_Create(CreateSpinlock());
    root->MessageLock = CreateSpinlock();

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

    dst->PendingMessages = List_Create(CreateSpinlock());
    dst->MessageLock = CreateSpinlock();

    dst->Children = List_Create(CreateSpinlock());
    List_AddEntry(src->Children, (void*)(uint64_t)dst->ID);
    dst->Parent = src;

    ForkTable(src->PageTable, dst->PageTable);

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
            if(procInfo != NULL)*procInfo = pInf;
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

void
RaiseSignal(UID pid,
            int sig_no) {
    pid = 0;
    sig_no = 0;
}


uint64_t
PostMessages(Message **msg, uint64_t cnt) {

    if(msg == NULL)
        return -1;

    for(uint64_t i = 0; i < cnt; i++){
        
        ProcessInformation *pInfo;


        UID DestinationPID = msg[i]->DestinationPID;

        int index = (int)DestinationPID;
        if((uint64_t)index != DestinationPID && index < CARDINAL_IPCDEST_NUM)
            DestinationPID = specialDestinationPIDs[index];

        GetProcessReference(DestinationPID, &pInfo);
        if(i == 0 || (msg[i]->DestinationPID != msg[i - 1]->DestinationPID))
            LockSpinlock(pInfo->MessageLock);
        
        if(msg[i] == NULL)
            return i;

        if(msg[i]->Size < sizeof(Message))
            return i;

        if(List_Length(pInfo->PendingMessages) > MAX_PENDING_MESSAGE_CNT)return i;

        Message *m = kmalloc(msg[i]->Size);
        m->SourcePID = GetCurrentProcessUID();
        if(m == NULL)return i;

        memcpy(m, msg[i], msg[i]->Size);
        List_AddEntry(pInfo->PendingMessages, m);
        
        if(i == cnt - 1 || msg[i]->DestinationPID != msg[i + 1]->DestinationPID)UnlockSpinlock(pInfo->MessageLock);
    }

    return TRUE;
}

bool
GetMessageFrom(Message *msg,
               UID SourcePID,
               uint64_t msg_id) {

    int index = (int)SourcePID;
    if((uint64_t)index != SourcePID && index < CARDINAL_IPCDEST_NUM)
        SourcePID = specialDestinationPIDs[index];

    ProcessInformation *pInfo;
    GetProcessReference(GetCurrentProcessUID(), &pInfo);

    if(List_Length(pInfo->PendingMessages) == 0)return FALSE;

    LockSpinlock(pInfo->MessageLock);
    Message *tmp = NULL;

    for(uint64_t i = 0; i < List_Length(pInfo->PendingMessages); i++) {
        tmp = (Message*)List_EntryAt(pInfo->PendingMessages, 0);

        if(tmp->SourcePID == SourcePID) {

            if((tmp->MsgID == msg_id) | (msg_id == 0)){
                List_Remove(pInfo->PendingMessages, 0);
                if(msg != NULL)memcpy(msg, tmp, tmp->Size);
                kfree(tmp);

                UnlockSpinlock(pInfo->MessageLock);
            }
            return TRUE;
        }
    }

    UnlockSpinlock(pInfo->MessageLock);
    return FALSE;
}


bool
SetSpecialDestinationPID(UID specialID) {
    int index = (int)specialID;

    if(index >= CARDINAL_IPCDEST_NUM)
        return FALSE;

    LockSpinlock(specialPIDLock);

    if(specialDestinationPIDs[index] != 0 && GetProcessReference(specialDestinationPIDs[index], NULL) == ProcessErrors_None)
            return UnlockSpinlock(specialPIDLock), FALSE;

    specialDestinationPIDs[index] = GetCurrentProcessUID();
    UnlockSpinlock(specialPIDLock);
    return TRUE;
}