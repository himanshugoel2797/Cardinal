#include "common.h"
#include <cardinal/proc/server.h>
#include <list/list.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    UID pid;
    char name[MAX_PROCESS_NAME_LEN];
    char create_subbed;
    char exit_subbed;
} ProcessEntry;

static List *processes;

void
ProcDB_Initialize(void) {
    processes = List_Create();
}

void
ProcDB_AddProcess(UID pid, char *name) {
    ProcessEntry *entry = malloc(sizeof(ProcessEntry));
    entry->pid = pid;
    if(name != NULL)
        strncpy(entry->name, name, MAX_PROCESS_NAME_LEN);
    else
        entry->name[0] = '\0';

    entry->create_subbed = 0;
    entry->exit_subbed = 0;

    List_AddEntry(processes, entry);
}

void
ProcDB_RemoveProcess(UID pid) {
    for(uint64_t i = 0; i < List_Length(processes); i++) {
        ProcessEntry *entry = List_EntryAt(processes, i);

        if(entry->pid == pid) {
            List_Remove(processes, i);
            free(entry);
            return;
        }
    }
}

static bool
List_EntryFinder(void *val, void *s_val) {
    ProcessEntry *f = (ProcessEntry*)val;

    return (f->pid == (UID)s_val);
}

void
ProcDB_SetCreateSubsFlag(UID pid, char val){

    uint64_t index = List_Find(processes, List_EntryFinder, (void*)pid);

    //This shouldn't really trigger
    if(index == (uint64_t)-1)
        return;

    ProcessEntry *proc_entry = (ProcessEntry*)List_EntryAt(processes, index);
    proc_entry->create_subbed = val;
}

void
ProcDB_SetExitSubsFlag(UID pid, char val){

    uint64_t index = List_Find(processes, List_EntryFinder, (void*)pid);

    //This shouldn't really trigger
    if(index == (uint64_t)-1)
        return;

    ProcessEntry *proc_entry = (ProcessEntry*)List_EntryAt(processes, index);
    proc_entry->exit_subbed = val;    
}

int
ProcDB_GetCreateSubsFlag(UID pid){
    uint64_t index = List_Find(processes, List_EntryFinder, (void*)pid);

    //This shouldn't really trigger
    if(index == (uint64_t)-1)
        return -1;

    ProcessEntry *proc_entry = (ProcessEntry*)List_EntryAt(processes, index);
    return proc_entry->exit_subbed;    
}

int
ProcDB_GetExitSubsFlag(UID pid){

    uint64_t index = List_Find(processes, List_EntryFinder, (void*)pid);

    //This shouldn't really trigger
    if(index == (uint64_t)-1)
        return -1;

    ProcessEntry *proc_entry = (ProcessEntry*)List_EntryAt(processes, index);
    return proc_entry->exit_subbed;    
}