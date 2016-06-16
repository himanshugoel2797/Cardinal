#include "process.h"
#include "kmalloc.h"
#include "memory.h"
#include "common.h"

static List *processes;
static ProcessInformation *root;

void
ProcessSys_Initialize(MemoryAllocationsMap *allocMap) {
    root = kmalloc(sizeof(ProcessInformation));
    root->ID = 0;	//Root process ID is 0
    strcpy(root->Name, "Root Process");
    root->Status = ProcessStatus_Executing;
    root->Priority = ProcessPriority_High;
    root->Permissions = ProcessPermissions_Kernel;
    root->PageTable = GetActiveVirtualMemoryInstance();
    root->AllocationMap = allocMap;
    root->children = NULL;
    root->next = NULL;
    root->TLSSize = 0;
    root->ThreadIDs = List_Create();

    processes = List_Create();
    List_AddEntry(processes, root);
}

ProcessErrors
ForkProcess(ProcessInformation *src,
            ProcessInformation **dest) {
    ProcessInformation *dst = kmalloc(sizeof(ProcessInformation));
    dst->ID = new_uid();
    dst->Status = src->Status;
    dst->Priority = src->Priority;
    dst->Permissions = src->Permissions;
    for(int i = 0; i < ProcessSignals_MAX; i++)
        dst->SignalHandlers[i] = src->SignalHandlers[i];

    ForkTable(src->PageTable, src->AllocationMap, &dst->PageTable, &dst->AllocationMap);

    //Add dst to src's children
    dst->next = src->children;
    src->children = dst;
    dst->ThreadIDs = List_Create();

    List_AddEntry(processes, dst);
    *dest = dst;

    return ProcessErrors_None;
}

ProcessErrors
GetProcessInformation(UID 			pid,
                      ProcessInformation	*procInfo) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        if(pInf->ID == pid) {
            memcpy(procInfo, pInf, sizeof(ProcessInformation));
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

        if(pInf->ID == pid) {
            *procInfo = pInf;
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
KillProcess(UID pid) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        if(pInf->ID == pid) {
            pInf->Status = ProcessStatus_Terminating;

            for(uint64_t j = 0; j < List_Length(pInf->ThreadIDs); j++) {
                SetThreadState((UID)List_EntryAt(pInf->ThreadIDs, j), ThreadState_Exiting);
            }

            //TODO Delete the process data, free up any application memory
            MemoryAllocationsMap *c = pInf->AllocationMap;
            while(c != NULL) {
                if(c->AllocationType != MemoryAllocationType_Global &&
                        c->AllocationType != MemoryAllocationType_Shared &&
                        c->AllocationType != MemoryAllocationType_Paged &&
                        c->AllocationType != MemoryAllocationType_Fork)
                    FreePhysicalPageCont(c->PhysicalAddress, c->Length / PAGE_SIZE);
                c = c->next;
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

        if(pInf->ID == pid) {
            pInf->Status = ProcessStatus_Sleeping;
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}

ProcessErrors
SetTLSSize(UID pid,
           uint64_t tls_size) {
    //Round up tls_size to nearest page boundary
    if(tls_size % PAGE_SIZE != 0)tls_size += PAGE_SIZE - (tls_size % PAGE_SIZE);

    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessInformation *pInf = List_EntryAt(processes, i);

        if(pInf->ID == pid) {
            if(tls_size == 0) {
                pInf->TLSSize = tls_size;
                return ProcessErrors_None;
            } else
                return ProcessErrors_TLSAlreadySetup;
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

        if(pInf->ID == pid) {
            for(int i = 0; i < ProcessSignals_MAX; i++) {
                if((signals >> i) & 1)
                    pInf->SignalHandlers[i] = sigHandler;
            }
            return ProcessErrors_None;
        }
    }
    return ProcessErrors_UIDNotFound;
}