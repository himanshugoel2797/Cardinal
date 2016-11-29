#include "process.h"
#include "kmalloc.h"
#include "memory.h"
#include "common.h"


static List *processes;
static ProcessInformation *root = NULL;
static volatile _Atomic UID baseID = ROOT_PID;
static UID specialDestinationPIDs[CARDINAL_IPCDEST_NUM];
static Spinlock specialPIDLock = NULL;


static UID
new_proc_uid(void) {
    //register UID dummy = 1;
    //__asm__ volatile("lock xadd %[dummy], (%[bs])" : [dummy]"+r"(dummy) : [bs]"r"(&baseID));
    return (UID)(uint32_t)baseID++;
}

void
ProcessSys_Initialize(void) {

    for(int i = 0; i < CARDINAL_IPCDEST_NUM; i++)
        specialDestinationPIDs[i] = 0;

    specialPIDLock = CreateSpinlock();

    root = kmalloc(sizeof(ProcessInformation));
    root->ID = new_proc_uid();  //Root process ID is ROOT_PID
    root->Status = ProcessStatus_Executing;
    root->PageTable = GetActiveVirtualMemoryInstance();
    root->HeapBreak = 0;
    root->Children = List_Create(CreateSpinlock());
    root->Parent = NULL;

    root->PendingMessages = List_Create(CreateSpinlock());
    root->ThreadIDs = List_Create(CreateSpinlock());
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
CreateProcess(UID parent, UID userID, UID *pid) {

    if(pid == NULL)
        return ProcessErrors_InvalidParameters;

    ProcessInformation *src = NULL;
    ProcessErrors err = GetProcessReference(parent, &src);
    if(err != ProcessErrors_None)
        return err;

    ProcessInformation *dst = kmalloc(sizeof(ProcessInformation));
    *pid = dst->ID = new_proc_uid();
    dst->UserID = userID;
    dst->GroupID = 0;           //All processes start as group 0, the program loader lowers their permissions.
    dst->Status = ProcessStatus_Stopped;

    dst->PageTable = kmalloc(sizeof(ManagedPageTable));
    if(CreateVirtualMemoryInstance(dst->PageTable) != MemoryAllocationErrors_None)
        return ProcessErrors_Unknown;

    dst->HeapBreak = 0;

    dst->ResponseBuffer = 0;

    FindFreeVirtualAddress(
        dst->PageTable,
        &dst->ResponseBuffer,
        MAX_RESPONSE_BUFFER_LEN,
        MemoryAllocationType_Application,
        MemoryAllocationFlags_Write | MemoryAllocationFlags_User);

    MapPage(dst->PageTable,
            0,
            dst->ResponseBuffer,
            MAX_RESPONSE_BUFFER_LEN,
            CachingModeWriteBack,
            MemoryAllocationType_Application | MemoryAllocationType_ReservedAllocation,
            MemoryAllocationFlags_Write | MemoryAllocationFlags_Present | MemoryAllocationFlags_User
           );

    dst->ThreadIDs = List_Create(CreateSpinlock());
    dst->PendingMessages = List_Create(CreateSpinlock());
    dst->Keys = List_Create(CreateSpinlock());

    dst->MessageLock = CreateSpinlock();

    dst->Children = List_Create(CreateSpinlock());
    List_AddEntry(src->Children, (void*)dst);

    dst->Parent = src;
    dst->reference_count = 1;
    dst->lock = CreateSpinlock();

    AtomicIncrement32(&src->reference_count);

    List_AddEntry(processes, dst);
    return ProcessErrors_None;
}

ProcessErrors
StartProcess(UID pid) {

    ProcessInformation *pinfo = NULL;
    ProcessErrors err = GetProcessReference(pid, &pinfo);
    if(err != ProcessErrors_None)
        return err;

    LockSpinlock(pinfo->lock);
    pinfo->Status = ProcessStatus_Executing;
    UnlockSpinlock(pinfo->lock);

    return ProcessErrors_None;
}

ProcessErrors
TerminateProcess(UID pid) {

    UID curPID = GetCurrentProcessUID();

    ProcessInformation *pinfo = NULL;
    ProcessErrors err = GetProcessReference(pid, &pinfo);
    if(err != ProcessErrors_None)
        return err;

    LockSpinlock(pinfo->lock);

    //Stop this process
    pinfo->Status = ProcessStatus_Terminating;

    //Remove this process from the list of processes
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *tmp_procInfo = List_EntryAt(processes, i);

        if(tmp_procInfo->ID == pid) {
            List_Remove(processes, i);
            break;
        }
    }

    //Remove this process from its parent's list of children
    if(pinfo->Parent != NULL) {

        for(uint64_t i = 0; i < List_Length(pinfo->Parent->Children); i++) {
            ProcessInformation *inf = List_EntryAt(pinfo->Parent->Children, i);

            if(inf->ID == pid) {
                List_Remove(pinfo->Parent->Children, i);
                break;
            }
        }

        //This process no longer holds a reference to its parent, reduce reference count
        AtomicDecrement32(&pinfo->Parent->reference_count);

        //Post a message to the parent process with the exit code
        struct SigChild *exit_msg = kmalloc(MESSAGE_SIZE);
        if(exit_msg != NULL) {
            memset(exit_msg, 0, sizeof(struct SigChild));
            exit_msg->m.SourcePID = pid;
            exit_msg->m.MsgID = 0;
            exit_msg->m.MsgType = CARDINAL_MSG_TYPE_SIGNAL;
            exit_msg->signal_type = CARDINAL_SIGNAL_TYPE_SIGCHILD;
            exit_msg->exit_code = pinfo->ExitStatus;

            PostMessages(pinfo->Parent->ID, (Message**)&exit_msg, 1);
        }
    }

    //Move children to root process
    ProcessInformation *root_p = NULL;
    GetProcessReference(ROOT_PID, &root_p);
    for(uint64_t i = 0; i < List_Length(pinfo->Children); i++) {
        List_AddEntry(root_p->Children, List_EntryAt(pinfo->Children, i));
    }

    List_Free(pinfo->Children);

    //Now free all the memory related to this process
    LockSpinlock(pinfo->lock);
    LockSpinlock(pinfo->MessageLock);
    for(uint64_t i = 0; i < List_Length(pinfo->PendingMessages); i++) {
        void *message = List_EntryAt(pinfo->PendingMessages, i);
        List_Remove(pinfo->PendingMessages, i);

        kfree(message);
    }
    List_Free(pinfo->PendingMessages);
    List_Free(pinfo->ThreadIDs);
    List_Free(pinfo->Keys);
    
    UnlockSpinlock(pinfo->MessageLock);
    FreeSpinlock(pinfo->MessageLock);

    FreeVirtualMemoryInstance(pinfo->PageTable);


    //TODO Inspect this to make sure the entire process information data is being freed
    UnlockSpinlock(pinfo->lock);
    FreeSpinlock(pinfo->lock);
    kfree(pinfo);

    return ProcessErrors_None;
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

uint64_t
PostMessages(UID dstPID, Message **msg, uint64_t cnt) {

    if(msg == NULL)
        return -2;

    UID DestinationPID = dstPID;

    int index = (int)DestinationPID;
    if((uint64_t)index != DestinationPID && index < CARDINAL_IPCDEST_NUM)
        DestinationPID = specialDestinationPIDs[index];

    ProcessInformation *pInfo = NULL;
    if(GetProcessReference(DestinationPID, &pInfo) != ProcessErrors_None)
        return -3;

    ProcessInformation *cur_procInfo = NULL;
    GetProcessReference(GetCurrentProcessUID(), &cur_procInfo);

    //Check to ensure that the destination ID is of the same user ID, and if not, if it belongs to the lowest group ID, if not, fail
    if(cur_procInfo->UserID != pInfo->UserID && (cur_procInfo->GroupID != 0 | pInfo->GroupID != 0))
        return -1;

    for(uint64_t i = 0; i < cnt; i++) {

        Message *m = NULL;

        LockSpinlock(pInfo->MessageLock);

        {
            if(msg[i] == NULL | List_Length(pInfo->PendingMessages) > MAX_PENDING_MESSAGE_CNT) {
                UnlockSpinlock(pInfo->MessageLock);
                return i;
            }

            m = kmalloc(MESSAGE_SIZE);

            if(m == NULL) {
                UnlockSpinlock(pInfo->MessageLock);
                return i;
            }

            memcpy(m, msg[i], MESSAGE_SIZE);
        }

        UnlockSpinlock(pInfo->MessageLock);

        m->SourcePID = GetCurrentProcessUID();
        List_AddEntry(pInfo->PendingMessages, m);
    }

    return TRUE;
}

bool
GetMessageFrom(Message *msg,
               UID SourcePID,
               uint32_t msg_id) {

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

        if((tmp->SourcePID == SourcePID) | (SourcePID == 0)) {

            if((tmp->MsgID == msg_id) | (msg_id == 0)) {
                List_Remove(pInfo->PendingMessages, 0);
                if(msg != NULL)memcpy(msg, tmp, MESSAGE_SIZE);
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

uint64_t
GetProcessGroupID(UID pid) {
    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return -1;

    uint64_t res = 0;
    LockSpinlock(info->lock);
    res = info->GroupID;
    UnlockSpinlock(info->lock);

    return res;
}

uint64_t
SetProcessGroupID(UID pid, uint64_t id) {
    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return -1;

    LockSpinlock(info->lock);

    if(id < info->GroupID) {
        UnlockSpinlock(info->lock);
        return -2;
    }

    info->GroupID = id;

    UnlockSpinlock(info->lock);
    return info->GroupID;
}

uint64_t
ScheduleProcessForTermination(UID pid, uint32_t exit_code) {

    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return ProcessErrors_UIDNotFound;

    LockSpinlock(info->lock);
    info->Status = ProcessStatus_Terminating;
    info->ExitStatus = exit_code;
    UnlockSpinlock(info->lock);
    return ProcessErrors_None;
}

uint64_t
CreateResponseBufferKey(UID pid,
                        uint32_t offset,
                        uint32_t length) {
    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return ProcessErrors_UIDNotFound;

    uint64_t key = 0;
    uint64_t id = (uint64_t)offset << 32;
    id |= length;

    if(KeyMan_AllocateKey(pid, id, KeyFlags_SingleTransfer, &key) != KeyManagerErrors_None)
        return 0;

    LockSpinlock(info->lock);
    List_AddEntry(info->Keys, (void*)key);
    UnlockSpinlock(info->lock);

    return key;
}

ProcessErrors
SubmitToResponseBuffer(uint64_t key,
                       void *buffer,
                       uint32_t buf_len) {
    UID pid = 0;
    uint64_t id = 0;

    if(KeyMan_ReadKey(key, &pid, &id, NULL) != KeyManagerErrors_None)
        return ProcessErrors_InvalidParameters;


    uint32_t offset = (uint32_t)(id >> 32);
    uint32_t length = (uint32_t)id;

    length = (length > buf_len) ? buf_len : length;

    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return ProcessErrors_UIDNotFound;

    LockSpinlock(info->lock);

    for(uint64_t i = 0; i < List_Length(info->Keys); i++) {
        if((uint64_t)List_EntryAt(info->Keys, i) == key) {
            List_Remove(info->Keys, i);
            KeyMan_FreeKey(key);

            uint32_t write_off = offset;
            uint32_t write_len = length;

            //Round offset down to nearest page size
            if(write_off % PAGE_SIZE)
                write_off -= PAGE_SIZE;

            //Round length up to nearest page size
            if(write_len % PAGE_SIZE)
                write_len += PAGE_SIZE - (write_len % PAGE_SIZE);

            //Setup a temporary mapping of the response buffers and copy the response
            uint8_t *vaddr = (uint8_t*)SetupTemporaryWriteMap(info->PageTable, info->ResponseBuffer + write_off, write_len);

            //Copy from bottom to top to account for header
            for(; write_len > 0; write_len--)
                vaddr[offset + write_len - 1] = ((uint8_t*)buffer)[write_len - 1];

            UninstallTemporaryWriteMap((uint64_t)vaddr, write_len);

            UnlockSpinlock(info->lock);
            return ProcessErrors_None;
        }
    }

    UnlockSpinlock(info->lock);
    return ProcessErrors_InvalidParameters;
}

ProcessErrors
QueryResponseKeyLength(uint64_t key,
                       uint32_t *length) {

    UID pid = 0;
    uint64_t id = 0;

    if(KeyMan_ReadKey(key, &pid, &id, NULL) != KeyManagerErrors_None)
        return ProcessErrors_InvalidParameters;


    if(length != NULL)
        *length = (uint32_t)id;

    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return ProcessErrors_UIDNotFound;

    LockSpinlock(info->lock);

    for(uint64_t i = 0; i < List_Length(info->Keys); i++) {
        if((uint64_t)List_EntryAt(info->Keys, i) == key) {
            UnlockSpinlock(info->lock);
            return ProcessErrors_None;
        }
    }

    UnlockSpinlock(info->lock);
    return ProcessErrors_InvalidParameters;
}