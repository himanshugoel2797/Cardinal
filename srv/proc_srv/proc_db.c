/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
ProcDB_SetCreateSubsFlag(UID pid, char val) {

    uint64_t index = List_Find(processes, List_EntryFinder, (void*)pid);

    //This shouldn't really trigger
    if(index == (uint64_t)-1)
        return;

    ProcessEntry *proc_entry = (ProcessEntry*)List_EntryAt(processes, index);
    proc_entry->create_subbed = val;
}

void
ProcDB_SetExitSubsFlag(UID pid, char val) {

    uint64_t index = List_Find(processes, List_EntryFinder, (void*)pid);

    //This shouldn't really trigger
    if(index == (uint64_t)-1)
        return;

    ProcessEntry *proc_entry = (ProcessEntry*)List_EntryAt(processes, index);
    proc_entry->exit_subbed = val;
}

int
ProcDB_GetCreateSubsFlag(UID pid) {
    uint64_t index = List_Find(processes, List_EntryFinder, (void*)pid);

    //This shouldn't really trigger
    if(index == (uint64_t)-1)
        return -1;

    ProcessEntry *proc_entry = (ProcessEntry*)List_EntryAt(processes, index);
    return proc_entry->exit_subbed;
}

int
ProcDB_GetExitSubsFlag(UID pid) {

    uint64_t index = List_Find(processes, List_EntryFinder, (void*)pid);

    //This shouldn't really trigger
    if(index == (uint64_t)-1)
        return -1;

    ProcessEntry *proc_entry = (ProcessEntry*)List_EntryAt(processes, index);
    return proc_entry->exit_subbed;
}