/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "btree.h"
#include "kmalloc.h"
#include "memory.h"
#include "synchronization.h"

#include "common/common.h"

#define BITS_PER_LEVEL 7
#define BTREE_NODE_COUNT (1 << BITS_PER_LEVEL)
#define LEVEL_INDEX_MASK ((1 << BITS_PER_LEVEL) - 1)
#define INDEX_MAX ((1 << BITS_PER_LEVEL) - 3)

typedef struct Node {
    union{
        uint64_t vals[BTREE_NODE_COUNT - 1];
        struct Node *children[BTREE_NODE_COUNT - 1];
    };
    uint8_t cnt;
    Spinlock lock;
} Node;

struct BTreeInt {
    Node *nodes;
    int level_count;
    int max_levels;
    _Atomic uint64_t key;
    Spinlock key_lock;
    uint64_t item_count;
};


static int
insert(Node *nodes,
       int level,
       uint64_t key,
       void *obj) {

    int pos = (key >> (level * BITS_PER_LEVEL)) & LEVEL_INDEX_MASK;

    if(pos > INDEX_MAX)
        return -1;  //Invalid index

    if(level == 0) {
        LockSpinlock(nodes->lock);
        if(nodes->vals[pos] == 0)
            nodes->cnt++;

        nodes->vals[pos] = (uint64_t)obj;
        UnlockSpinlock(nodes->lock);

    }else{
        LockSpinlock(nodes->lock);
        if(nodes->children[pos] == NULL){
            nodes->children[pos] = kmalloc(sizeof(Node));
            memset(nodes->children[pos], 0, sizeof(Node));
            nodes->children[pos]->cnt = 0;
            nodes->children[pos]->lock = CreateSpinlock();

            nodes->cnt++;
        }

        int retVal = insert(nodes->children[pos], level - 1, key, obj);
        UnlockSpinlock(nodes->lock);
        return retVal;
    }

    return 0;
}

static void*
get(Node *nodes,
       int level,
       uint64_t key) {

    int pos = (key >> (level * BITS_PER_LEVEL)) & LEVEL_INDEX_MASK;

    if(pos > INDEX_MAX)
        return NULL;  //Invalid index

    if(level == 0){
        LockSpinlock(nodes->lock);
        void* retVal = (void*)nodes->vals[pos];
        UnlockSpinlock(nodes->lock);
        return retVal;
    }else{
        LockSpinlock(nodes->lock);
        if(nodes->children[pos] == NULL){
            UnlockSpinlock(nodes->lock);
            return NULL;
        }

        void *retVal = get(nodes->children[pos], level - 1, key);
        UnlockSpinlock(nodes->lock);
        return retVal;
    }
}

static void*
remove(Node *nodes,
       int level,
       uint64_t key) {

    int pos = (key >> (level * BITS_PER_LEVEL)) & LEVEL_INDEX_MASK;

    if(pos > INDEX_MAX)
        return NULL;  //Invalid index

    if(level == 0) {
        LockSpinlock(nodes->lock);

        if(nodes->vals[pos] != 0)
            nodes->cnt--;

        void *retVal = (void*)nodes->vals[pos];
        nodes->vals[pos] = 0;

        UnlockSpinlock(nodes->lock);
        return retVal;

    }else{
        LockSpinlock(nodes->lock);

        if(nodes->children[pos] == NULL){
            UnlockSpinlock(nodes->lock);
            return NULL;
        }

        void *retVal = remove(nodes->children[pos], level - 1, key);

        if(nodes->children[pos]->cnt == 0){
            kfree(nodes->children[pos]);
            nodes->children[pos] = NULL;
            nodes->cnt--;
        }

        UnlockSpinlock(nodes->lock);
        return retVal;
    }

    return 0;
}


static void
delete(Node *nodes,
       int level) {

    for(int pos = 0; pos <= INDEX_MAX; pos++){
        if(nodes->children[pos] != NULL){
            
            if(level > 0)
                delete(nodes->children[pos], level - 1);

            FreeSpinlock(nodes->children[pos]->lock);
            kfree(nodes->children[pos]);
        }
    }
}

BTree*
BTree_Create(int max_levels) {

    struct BTreeInt *h = kmalloc(sizeof (struct BTreeInt));

    h->nodes = NULL;
    h->level_count = 0;
    h->max_levels = max_levels;
    h->key_lock = CreateSpinlock();
    h->item_count = 0;

    return h;
}

int
BTree_Insert(BTree *h,
        uint64_t key,
        void *obj) {

    if(h->nodes == NULL){
        h->nodes = kmalloc(sizeof(Node));
        memset(h->nodes, 0, sizeof(Node));
        h->nodes->cnt = 0;
        h->nodes->lock = CreateSpinlock();
    }

    int retVal = insert(h->nodes, h->max_levels - 1, key, obj);

    if(retVal == 0)
        h->item_count++;

    return retVal;
}

void*
BTree_GetValue(BTree *h,
               uint64_t key) {
    if(h->nodes == NULL)
        return NULL;

    return get(h->nodes, h->max_levels - 1, key);
}

void*
BTree_RemoveEntry(BTree *h, uint64_t key) {
    
    if(h->nodes == NULL)
        return NULL;

    void *retVal = remove(h->nodes, h->max_levels - 1, key);

    if(retVal != NULL)
        h->item_count--;

    return retVal;
}

void
BTree_Delete(BTree *h) {

    if(h->nodes != NULL){
        delete(h->nodes, h->max_levels - 1);
    }

    FreeSpinlock(h->key_lock);
    kfree(h);
}

uint64_t
BTree_GetKey(BTree *h) {
    LockSpinlock(h->key_lock);
    uint64_t cur_key = h->key++;
    //TODO this routine is bugged, fix it
    for(int i = 0; i < h->max_levels; i++){

        int pos = (cur_key >> (i * BITS_PER_LEVEL)) & LEVEL_INDEX_MASK;

        if(pos > INDEX_MAX){
            cur_key += ((1 << BITS_PER_LEVEL) - pos) << (i * BITS_PER_LEVEL);
        }
    }
    UnlockSpinlock(h->key_lock);
    return cur_key;
}

uint64_t
BTree_GetCount(BTree *h){
    return h->item_count;
}