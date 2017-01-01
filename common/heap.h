#ifndef HEAP_H
#define HEAP_H

#include "types.h"

typedef struct HeapInt Heap;

Heap*
Heap_Create(void);

void
Heap_Insert(Heap *h,
            uint64_t val,
            void *obj);

void*
Heap_Peek(Heap *h, 
		  uint64_t *val);

void*
Heap_Pop(Heap *h, 
		 uint64_t *val);

void
Heap_Delete(Heap *h);

#endif /* HEAP_H */

