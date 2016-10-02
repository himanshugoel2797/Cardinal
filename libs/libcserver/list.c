#include "list.h"

#include <stdlib.h>

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
} List;

List*
List_Create(void) {
    List* t = malloc(sizeof(List));

    t->nodes = NULL;
    t->last = NULL;
    t->last_accessed_node = NULL;
    t->last_accessed_index = 0;
    t->entry_count = 0;

    return t;
}

ListError
List_AddEntry(List *a,
              void *value) {
    ListNode *l = malloc(sizeof(ListNode));
    if(l == NULL)return ListError_AllocationFailed;


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

    return ListError_None;
}

uint64_t
List_Length(List *a) {
    uint64_t val = a->entry_count;
    return val;
}


uint64_t
List_Find(List *a,
          bool (*finder)(void *val, void *s_val),
          void *s_val) {
    if(finder == NULL)
        return -1;

    uint64_t checked_vals = 0;
    uint64_t n = a->last_accessed_index;

    while(checked_vals != a->entry_count) {

        if(finder(List_EntryAt(a, n), s_val)) {
            return n;
        }

        n = (n + 1) % a->entry_count;
        checked_vals++;
    }

    return -1;
}

void
List_Remove(List *a,
            uint64_t index) {
    if(a->entry_count == 0 | index >= a->entry_count) {
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
    free(tmp);

    a->entry_count--;
}

void
List_Free(List *a) {
    while(List_Length(a) > 0) {
        List_Remove(a, 0);
    }
    free(a);
}

void*
List_EntryAt(List *a,
             uint64_t index) {

    if(a->last_accessed_index >= a->entry_count) {
        a->last_accessed_index = 0;
        a->last_accessed_node = a->nodes;
    }

    if(index >= a->entry_count) {
        return NULL;
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
    return val;
}

void*
List_Next(List *a) {
    if(a->last_accessed_index < a->entry_count - 1) {
        a->last_accessed_index++;
        a->last_accessed_node = a->last_accessed_node->next;
    }
    void *val = a->last_accessed_node->value;
    return val;
}

void*
List_Prev(List *a) {
    if(a->last_accessed_index > 0) {
        a->last_accessed_index--;
        a->last_accessed_node = a->last_accessed_node->prev;
    }
    void *val = a->last_accessed_node->value;
    return val;
}
