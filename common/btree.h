#ifndef _CARDINAL_BTREE_H_
#define _CARDINAL_BTREE_H_

#include "types.h"

typedef struct BTreeInt BTree;

BTree*
BTree_Create(int max_levels);

int
BTree_Insert(BTree *h,
	         uint64_t key,
    	     void *obj);

void*
BTree_GetValue(BTree *h,
               uint64_t key);

void*
BTree_RemoveEntry(BTree *h, 
				  uint64_t key);

void
BTree_Delete(BTree *h);

uint64_t
BTree_GetKey(BTree *h);

#endif