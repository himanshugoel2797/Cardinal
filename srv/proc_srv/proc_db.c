#include "common.h"
#include "server.h"
#include <liblist/list.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    UID pid;
    char name[MAX_PROCESS_NAME_LEN];
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