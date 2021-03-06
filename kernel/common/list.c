/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "list.h"
#include "kmalloc.h"

typedef struct ListNode {
    void *value;
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

typedef struct List {
    ListNode *nodes;
    ListNode *last;
    ListNode *last_accessed_node;
    uint64_t last_accessed_index;
    uint64_t entry_count;
    Spinlock spin;
} List;

List*
List_Create(Spinlock spin) {
    List* t = kmalloc(sizeof(List));
    if(LockSpinlock(spin) == NULL)
        return NULL;

    t->nodes = NULL;
    t->last = NULL;
    t->last_accessed_node = NULL;
    t->last_accessed_index = 0;
    t->entry_count = 0;
    t->spin = spin;
    UnlockSpinlock(spin);
    return t;
}

ListError
List_AddEntry(List *a,
              void *value) {
    ListNode *l = kmalloc(sizeof(ListNode));
    if(l == NULL)return ListError_AllocationFailed;

    if(LockSpinlock(a->spin) == NULL)
        return ListError_Deleting;

    l->prev = a->last;
    l->value = value;
    l->next = NULL;
    if(a->last != NULL)a->last->next = l;
    a->last = l;
    if(a->nodes == NULL)a->nodes = a->last;
    if(a->last_accessed_node == NULL) {
        a->last_accessed_node = a->nodes;
        a->last_accessed_index = 0;
    }
    a->entry_count++;
    UnlockSpinlock(a->spin);
    return ListError_None;
}

uint64_t
List_Length(List *a) {
    if(LockSpinlock(a->spin) == NULL)
        return 0;

    uint64_t val = a->entry_count;
    UnlockSpinlock(a->spin);
    return val;
}

void
List_Remove(List *a,
            uint64_t index) {
    if(LockSpinlock(a->spin) == NULL)
        return;

    if(a->entry_count == 0 | index >= a->entry_count) {
        UnlockSpinlock(a->spin);
        return;
    }

    if(a->last_accessed_index >= a->entry_count) {
        a->last_accessed_index = 0;
        a->last_accessed_node = a->nodes;
    }

    while(a->last_accessed_index > index) {
        a->last_accessed_node = a->last_accessed_node->prev;
        a->last_accessed_index--;
    }

    while(a->last_accessed_index < index) {
        a->last_accessed_node = a->last_accessed_node->next;
        a->last_accessed_index++;
    }

    void *tmp = a->last_accessed_node;

    if(a->last_accessed_node->prev != NULL)
        a->last_accessed_node->prev->next = a->last_accessed_node->next;

    if(a->last_accessed_node->next != NULL)
        a->last_accessed_node->next->prev = a->last_accessed_node->prev;

    if(a->nodes == a->last_accessed_node)
        a->nodes = a->last_accessed_node->next;

    if(a->last == a->last_accessed_node)
        a->last = a->last_accessed_node->prev;

    a->last_accessed_node = a->nodes;
    a->last_accessed_index = 0;
    kfree(tmp);

    a->entry_count--;
    UnlockSpinlock(a->spin);
}

void
List_Free(List *a) {
    FinalLockSpinlock(a->spin);
    while(List_Length(a) > 0) {
        List_Remove(a, 0);
    }
    Spinlock p = a->spin;
    kfree(a);

    UnlockSpinlock(p);
    FreeSpinlock(p);
}

void*
List_EntryAt(List *a,
             uint64_t index) {

    if(LockSpinlock(a->spin) == NULL)
        return NULL;

    if(a->last_accessed_index >= a->entry_count) {
        a->last_accessed_index = 0;
        a->last_accessed_node = a->nodes;
    }

    if(index >= a->entry_count) {
        UnlockSpinlock(a->spin);
        return NULL;
    }

    //Accelarate first and last element accesses
    if(index == 0) {
        a->last_accessed_index = 0;
        a->last_accessed_node = a->nodes;
    }

    //Accelerate first and last element accesses
    if(index == a->entry_count - 1) {
        a->last_accessed_index = a->entry_count - 1;
        a->last_accessed_node = a->last;
    }

    while(a->last_accessed_index > index) {
        a->last_accessed_node = a->last_accessed_node->prev;
        a->last_accessed_index--;
    }

    while(a->last_accessed_index < index) {
        a->last_accessed_node = a->last_accessed_node->next;
        a->last_accessed_index++;
    }

    void *val = a->last_accessed_node->value;
    UnlockSpinlock(a->spin);
    return val;
}

void*
List_RotNext(List *a) {
    if(LockSpinlock(a->spin) == NULL)
        return NULL;

    void *val = NULL;
    if(a->entry_count != 0)
        val = List_EntryAt(a, (a->last_accessed_index + 1) % a->entry_count);
    UnlockSpinlock(a->spin);
    return val;
}

void*
List_RotPrev(List *a) {
    if(LockSpinlock(a->spin) == NULL)
        return NULL;

    void *val = NULL;
    if(a->entry_count != 0)
        val = List_EntryAt(a, (a->last_accessed_index - 1) % a->entry_count);
    UnlockSpinlock(a->spin);
    return val;
}

uint64_t
List_GetLastIndex(List *a) {
    if(LockSpinlock(a->spin) == NULL)
        return -1;

    uint64_t i = a->last_accessed_index;
    UnlockSpinlock(a->spin);
    return i;
}