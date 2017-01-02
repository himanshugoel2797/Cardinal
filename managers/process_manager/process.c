/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "process.h"
#include "kmalloc.h"
#include "memory.h"
#include "common.h"

#include "common/btree.h"


static BTree *processes;
static ProcessInformation *root = NULL;
static volatile _Atomic UID baseID = ROOT_PID;


static UID
new_proc_uid(void) {
    //register UID dummy = 1;
    //__asm__ volatile("lock xadd %[dummy], (%[bs])" : [dummy]"+r"(dummy) : [bs]"r"(&baseID));
    return (UID)BTree_GetKey(processes);
}

void
ProcessSys_Initialize(void) {
    processes = BTree_Create(3);

    //Get an ID to force the btree id to be equal to the ROOT_PID
    uint64_t pid = new_proc_uid();
    while(pid != ROOT_PID) {
        pid = new_proc_uid();
    }

    root = kmalloc(sizeof(ProcessInformation));
    root->ID = pid;  //Root process ID is ROOT_PID
    root->Status = ProcessStatus_Executing;
    root->PageTable = GetActiveVirtualMemoryInstance();
    root->HeapBreak = 0;
    root->Children = List_Create(CreateSpinlock());
    root->Parent = NULL;

    root->PendingMessages = List_Create(CreateSpinlock());
    root->ThreadInfos = List_Create(CreateSpinlock());
    root->MessageLock = CreateSpinlock();

    root->reference_count = 0;
    root->lock = CreateSpinlock();

    BTree_Insert(processes, root->ID, root);
}

bool
ProcessSys_IsInitialized(void) {
    return !(root == NULL);
}

ProcessErrors
CreateProcess(UID parent, UID group_id, UID *pid) {

    if(pid == NULL)
        return ProcessErrors_InvalidParameters;

    ProcessInformation *src = NULL;
    ProcessErrors err = GetProcessReference(parent, &src);
    if(err != ProcessErrors_None)
        return err;

    ProcessInformation *dst = kmalloc(sizeof(ProcessInformation));
    *pid = dst->ID = new_proc_uid();
    dst->GroupID = group_id;           //All processes inherit their parent's group.
    dst->Status = ProcessStatus_Stopped;

    dst->PageTable = kmalloc(sizeof(ManagedPageTable));
    if(CreateVirtualMemoryInstance(dst->PageTable) != MemoryAllocationErrors_None)
        return ProcessErrors_Unknown;

    dst->HeapBreak = 0;

    dst->ThreadInfos = List_Create(CreateSpinlock());
    dst->PendingMessages = List_Create(CreateSpinlock());

    dst->Keys = kmalloc(MAX_KEYS_PER_PROCESS * sizeof(uint64_t));
    for(int i = 0; i < MAX_KEYS_PER_PROCESS; i++)
        dst->Keys[i] = 0;

    dst->MessageLock = CreateSpinlock();

    dst->Children = List_Create(CreateSpinlock());
    List_AddEntry(src->Children, (void*)dst);

    dst->Parent = src;
    dst->reference_count = 1;
    dst->lock = CreateSpinlock();

    AtomicIncrement32(&src->reference_count);

    BTree_Insert(processes, dst->ID, dst);
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

    while(GetCurrentProcessUID() == pid)
        YieldThread();

    ProcessInformation *pinfo = NULL;
    ProcessErrors err = GetProcessReference(pid, &pinfo);
    if(err != ProcessErrors_None)
        return err;

    LockSpinlock(pinfo->lock);

    //Stop this process
    pinfo->Status = ProcessStatus_Terminating;

    //Free keys
    for(int i = 0; i < MAX_KEYS_PER_PROCESS; i++) {
        if(pinfo->Keys[i] != 0)
            KeyMan_DecrementRefCount(pinfo->Keys[i]);
    }
    kfree(pinfo->Keys);

    //Remove this process from the list of processes
    BTree_RemoveEntry(processes, pid);

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
            exit_msg->m.MsgType = CardinalMsgType_Signal;
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

        //The root process is now the parent of the children
        ProcessInformation *child = (ProcessInformation*)List_EntryAt(pinfo->Children, i);
        child->Parent = root_p;
    }

    List_Free(pinfo->Children);

    //Now free all the memory related to this process
    LockSpinlock(pinfo->MessageLock);
    for(uint64_t i = 0; i < List_Length(pinfo->PendingMessages); i++) {
        void *message = List_EntryAt(pinfo->PendingMessages, i);
        List_Remove(pinfo->PendingMessages, i);

        kfree(message);
    }
    List_Free(pinfo->PendingMessages);
    List_Free(pinfo->ThreadInfos);

    UnlockSpinlock(pinfo->MessageLock);
    FreeSpinlock(pinfo->MessageLock);

    if(pinfo->InterruptsUsed)
        InterruptMan_UnregisterProcess(pid);

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

    ProcessInformation *pInf = BTree_GetValue(processes, pid);

    if(pInf == NULL)
        return ProcessErrors_UIDNotFound;

    LockSpinlock(pInf->lock);
    memcpy(procInfo, pInf, sizeof(ProcessInformation));
    UnlockSpinlock(pInf->lock);
    return ProcessErrors_None;
}

ProcessErrors
GetProcessReference(UID           pid,
                    ProcessInformation    **procInfo) {
    ProcessInformation *pInf = BTree_GetValue(processes, pid);

    if(pInf == NULL)
        return ProcessErrors_UIDNotFound;

    *procInfo = pInf;
    return ProcessErrors_None;
}

uint64_t
PostMessages(UID dstPID, Message **msg, uint64_t cnt) {

    if(msg == NULL)
        return -2;

    UID DestinationPID = dstPID;

    int index = (int)DestinationPID;

    ProcessInformation *pInfo = NULL;
    if(GetProcessReference(DestinationPID, &pInfo) != ProcessErrors_None)
        return -3;

    ProcessInformation *cur_procInfo = NULL;
    GetProcessReference(GetCurrentProcessUID(), &cur_procInfo);

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

    ProcessCheckWakeThreads(dstPID);

    return TRUE;
}

bool
GetMessageFrom(Message *msg,
               UID SourcePID,
               uint32_t msg_id) {

    int index = (int)SourcePID;

    ProcessInformation *pInfo;
    GetProcessReference(GetCurrentProcessUID(), &pInfo);

    if(List_Length(pInfo->PendingMessages) == 0) {
        YieldThread();

        if(List_Length(pInfo->PendingMessages) == 0)
            return FALSE;
    }

    LockSpinlock(pInfo->MessageLock);
    Message *tmp = NULL;

    for(uint64_t i = 0; i < List_Length(pInfo->PendingMessages); i++) {
        tmp = (Message*)List_EntryAt(pInfo->PendingMessages, i);

        if((tmp->SourcePID == SourcePID) | (SourcePID == 0)) {

            if((tmp->MsgID == msg_id) | (msg_id == 0)) {
                List_Remove(pInfo->PendingMessages, i);
                if(msg != NULL)memcpy(msg, tmp, MESSAGE_SIZE);
                kfree(tmp);

                //Copy the mapping over to the other process.
                //Add the sharing info to the source's data.


                UnlockSpinlock(pInfo->MessageLock);
            }
            return TRUE;
        }
    }

    UnlockSpinlock(pInfo->MessageLock);
    return FALSE;
}

bool
GetMessageFromType(Message *msg,
                   CardinalMsgType msgType) {

    ProcessInformation *pInfo;
    GetProcessReference(GetCurrentProcessUID(), &pInfo);

    if(List_Length(pInfo->PendingMessages) == 0)return FALSE;

    LockSpinlock(pInfo->MessageLock);
    Message *tmp = NULL;

    for(uint64_t i = 0; i < List_Length(pInfo->PendingMessages); i++) {
        tmp = (Message*)List_EntryAt(pInfo->PendingMessages, i);

        if(tmp->MsgType == msgType) {
            List_Remove(pInfo->PendingMessages, i);
            if(msg != NULL)memcpy(msg, tmp, MESSAGE_SIZE);
            kfree(tmp);

            UnlockSpinlock(pInfo->MessageLock);
            return TRUE;
        }
    }

    UnlockSpinlock(pInfo->MessageLock);
    return FALSE;
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

ProcessErrors
AllocateDescriptor(UID pid,
                   uint64_t key,
                   uint32_t *index) {

    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return ProcessErrors_UIDNotFound;

    LockSpinlock(info->lock);

    if(info->LowestFreeKeyIndex >= MAX_KEYS_PER_PROCESS) {
        UnlockSpinlock(info->lock);
        return ProcessErrors_OutOfMemory;
    }

    info->Keys[info->LowestFreeKeyIndex] = key;
    KeyMan_IncrementRefCount(key);

    if(index != NULL)
        *index = info->LowestFreeKeyIndex;

    while(info->Keys[info->LowestFreeKeyIndex] != 0 && info->LowestFreeKeyIndex < MAX_KEYS_PER_PROCESS) {
        info->LowestFreeKeyIndex++;
    }

    UnlockSpinlock(info->lock);
    return ProcessErrors_None;
}

ProcessErrors
CopyDescriptor(UID src_pid,
               UID dst_pid,
               uint32_t index,
               uint32_t *new_index) {

    ProcessInformation *info;
    if(GetProcessReference(src_pid, &info) != ProcessErrors_None)
        return ProcessErrors_UIDNotFound;

    if(index >= MAX_KEYS_PER_PROCESS)
        return ProcessErrors_InvalidParameters;

    LockSpinlock(info->lock);

    if(info->LowestFreeKeyIndex >= MAX_KEYS_PER_PROCESS) {
        UnlockSpinlock(info->lock);
        return ProcessErrors_OutOfMemory;
    }

    uint64_t key = info->Keys[index];
    UnlockSpinlock(info->lock);

    return AllocateDescriptor(dst_pid, key, new_index);
}

ProcessErrors
GetIndexOfKey(UID pid,
              uint64_t key,
              uint32_t *index) {

    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return ProcessErrors_UIDNotFound;

    if(index == NULL)
        return ProcessErrors_InvalidParameters;

    LockSpinlock(info->lock);

    *index = (uint32_t)-1;

    for(int i = 0; i < MAX_KEYS_PER_PROCESS; i++) {
        if(info->Keys[i] == key && info->Keys[i] != 0) {
            *index = i;
            break;
        }
    }

    UnlockSpinlock(info->lock);

    if(*index == (uint32_t)-1)
        return ProcessErrors_InvalidParameters;

    return ProcessErrors_None;
}

ProcessErrors
DeleteDescriptor(UID pid,
                 uint32_t index) {

    ProcessInformation *info;
    if(GetProcessReference(pid, &info) != ProcessErrors_None)
        return ProcessErrors_UIDNotFound;

    LockSpinlock(info->lock);

    info->Keys[index] = 0;
    KeyMan_DecrementRefCount(info->Keys[index]);

    if(index < info->LowestFreeKeyIndex)
        info->LowestFreeKeyIndex = index;

    UnlockSpinlock(info->lock);
    return ProcessErrors_None;
}

void
ProcessCheckWakeThreads(UID pid) {

    ProcessInformation *pInfo = NULL;
    if(GetProcessReference(pid, &pInfo) != ProcessErrors_None)
        return;

    LockSpinlock(pInfo->lock);

    List_RotPrev(pInfo->ThreadInfos);
    for(uint64_t i = 0; i < List_Length(pInfo->ThreadInfos); i++) {
        ThreadInfo* tInfo = (ThreadInfo*)List_RotNext(pInfo->ThreadInfos);

        LockSpinlock(tInfo->lock);

        if(tInfo->State != ThreadState_Sleep) {
            UnlockSpinlock(tInfo->lock);
            continue;
        }

        ThreadWakeCondition condition = tInfo->WakeCondition;

        uint64_t wake_val = tInfo->TargetMsgSourcePID;

        switch(condition) {
        case ThreadWakeCondition_MatchMsgType: {
            LockSpinlock(pInfo->MessageLock);

            for(uint64_t i = 0; i < List_Length(pInfo->PendingMessages); i++) {

                Message *m = List_EntryAt(pInfo->PendingMessages, i);

                if(m->MsgType == wake_val) {
                    WakeThread(tInfo->ID);
                    break;
                }
            }

            UnlockSpinlock(pInfo->MessageLock);
        }
        break;
        case ThreadWakeCondition_MatchMsgSourcePID: {
            LockSpinlock(pInfo->MessageLock);

            for(uint64_t i = 0; i < List_Length(pInfo->PendingMessages); i++) {

                Message *m = List_EntryAt(pInfo->PendingMessages, i);

                if(m->SourcePID == wake_val) {
                    WakeThread(tInfo->ID);
                    break;
                }
            }

            UnlockSpinlock(pInfo->MessageLock);

            //If the process from which a message is expected no longer exists, wake the thread
            ProcessInformation *pInfo = NULL;
            if(GetProcessReference(wake_val, &pInfo) != ProcessErrors_None)
                WakeThread(tInfo->ID);
        }
        break;
        case ThreadWakeCondition_MatchMsgAny: {
            LockSpinlock(pInfo->MessageLock);

            if(List_Length(pInfo->PendingMessages) != 0)
                WakeThread(tInfo->ID);

            UnlockSpinlock(pInfo->MessageLock);
        }
        break;
        default:
            break;
        }

        UnlockSpinlock(tInfo->lock);
    }

    UnlockSpinlock(pInfo->lock);
}