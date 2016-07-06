#include "process.h"
#include "kmalloc.h"
#include "memory.h"
#include "common.h"

static List *processes;
static ProcessInformation *root = NULL;

void
ProcessSys_Initialize(MemoryAllocationsMap *allocMap) {
    root = kmalloc(sizeof(ProcessInformation));
    root->ID = 0;	//Root process ID is 0
    strcpy(root->Name, "Root Process");
    root->Status = ProcessStatus_Executing;
    root->Priority = ProcessPriority_High;
    root->Permissions = ProcessPermissions_None;
    root->PageTable = GetActiveVirtualMemoryInstance();
    root->AllocationMap = allocMap;
    root->ThreadIDs = List_Create(CreateSpinlock());
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
    dst->lock = CreateSpinlock();
    dst->Priority = src->Priority;
    dst->Permissions = src->Permissions;
    for(int i = 0; i < ProcessSignals_MAX; i++)
        dst->SignalHandlers[i] = src->SignalHandlers[i];

    ForkTable(src->PageTable, src->AllocationMap, &dst->PageTable, &dst->AllocationMap);

    //Add dst to src's children
    dst->ThreadIDs = List_Create(CreateSpinlock());

    UnlockSpinlock(src->lock);
    List_AddEntry(processes, dst);
    *dest = dst;

    return ProcessErrors_None;
}

ProcessErrors
GetProcessInformation(UID 			pid,
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
KillProcess(UID pid) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {

            LockSpinlock(pInf->lock);

            if(List_Length(pInf->ThreadIDs) != 0) {
                pInf->Status = ProcessStatus_Terminating;

                for(uint64_t j = 0; j < List_Length(pInf->ThreadIDs); j++) {
                    SetThreadState((UID)List_EntryAt(pInf->ThreadIDs, j), ThreadState_Exiting);
                }

                UnlockSpinlock(pInf->lock);
            } else {

                LockSpinlock(pInf->lock);
                List_Remove(processes, i);
                //TODO Delete the process data, free up any application memory
                MemoryAllocationsMap *c = pInf->AllocationMap;
                while(c != NULL) {
                    MemoryAllocationType allocType = c->AllocationType;

                    if((allocType & MemoryAllocationType_Fork) == 0) {
                        allocType = allocType & ~MemoryAllocationType_Fork;

                        if(allocType != MemoryAllocationType_Global &&
                                allocType != MemoryAllocationType_Shared &&
                                allocType != MemoryAllocationType_Paged)
                            FreePhysicalPageCont(c->PhysicalAddress, c->Length / PAGE_SIZE);
                    }
                    MemoryAllocationsMap *tmp_c = c;
                    c = c->next;
                    kfree(tmp_c);
                }

                FreeVirtualMemoryInstance(pInf->PageTable);
                List_Free(root->ThreadIDs);

                FreeSpinlock(pInf->lock);
                kfree(pInf);
            }

            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
SleepProcess(UID pid) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            LockSpinlock(pInf->lock);
            pInf->Status = ProcessStatus_Sleeping;
            UnlockSpinlock(pInf->lock);
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
RegisterSignalHandler(UID 		pid,
                      ProcessSignals 	signals,
                      ProcessSignalHandler sigHandler) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        LockSpinlock(pInf->lock);
        UID pInfID = pInf->ID;
        UnlockSpinlock(pInf->lock);

        if(pInfID == pid) {
            for(int i = 0; i < ProcessSignals_MAX; i++) {
                if((signals >> i) & 1) {
                    LockSpinlock(pInf->lock);
                    pInf->SignalHandlers[i] = sigHandler;
                    UnlockSpinlock(pInf->lock);
                }
            }
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}