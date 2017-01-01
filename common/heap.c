#include "heap.h"
#include "kmalloc.h"
#include "memory.h"

#define NODE_TABLE_SIZE KiB(256)

#define GET_LEFT_CHILD(n) ( 2 * n + 1)
#define GET_RIGHT_CHILD(n) (2 * n + 2)
#define GET_PARENT(n) ((n - 1) >> 1)
#define SWAP(a, b) a ^= b; b ^= a; a ^= b

typedef struct {
    uint64_t val;
    uint64_t obj;
} Node;

struct HeapInt {
    Node *nodes;
    int node_count;
};

Heap
Heap_Create(void) {

    struct HeapInt *h = kmalloc(sizeof (struct HeapInt));

    uint64_t keyTable_addr = 0;

    if(FindFreeVirtualAddress(GetActiveVirtualMemoryInstance(),
                              (uint64_t*)&h->nodes,
                              NODE_TABLE_SIZE,
                              MemoryAllocationType_Heap,
                              MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Kernel
                             ) != MemoryAllocationErrors_None) {
        //TODO go back and make the kernel panic on any initialization failures.
    }

    if(MapPage(GetActiveVirtualMemoryInstance(),
               0,//AllocatePhysicalPageCont(KEY_TABLE_SIZE / PAGE_SIZE),
               (uint64_t)h->nodes,
               NODE_TABLE_SIZE,
               CachingModeWriteBack,
               MemoryAllocationType_Heap | MemoryAllocationType_ReservedAllocation,
               MemoryAllocationFlags_Write | MemoryAllocationFlags_NoExec | MemoryAllocationFlags_Kernel | MemoryAllocationFlags_Present
              ) != MemoryAllocationErrors_None) {
        //TODO Panic
    }

    h->node_count = 0;

    return h;
}

void
Heap_Insert(Heap h,
        uint64_t val,
        void *obj) {

    h->nodes[h->node_count].val = val;
    h->nodes[h->node_count].obj = (uint64_t) obj;

    int node_index = h->node_count++;
    int cur_index = node_index;
    while (GET_PARENT(cur_index) >= 0) {
        int parent = GET_PARENT(cur_index);
        if (h->nodes[cur_index].val > h->nodes[parent].val) {

            SWAP(h->nodes[cur_index].val, h->nodes[parent].val);
            SWAP(h->nodes[cur_index].obj, h->nodes[parent].obj);

            cur_index = parent;
        } else
            break;
    }

}

void*
Heap_Peek(Heap h, uint64_t *val) {
    if(val != NULL)
        *val = h->nodes[0].val;
    return (void*) h->nodes[0].obj;
}

void siftDown(Heap h, int nodeIndex) {
    
    int leftChildIndex, rightChildIndex, maxIndex, tmp;
    leftChildIndex = GET_LEFT_CHILD(nodeIndex);
    rightChildIndex = GET_RIGHT_CHILD(nodeIndex);
    if (rightChildIndex >= h->node_count) {
        if (leftChildIndex >= h->node_count)
            return;
        else
            maxIndex = leftChildIndex;
    } else {
        if (h->nodes[leftChildIndex].val > h->nodes[rightChildIndex].val)
            maxIndex = leftChildIndex;
        else
            maxIndex = rightChildIndex;
    }
    
    if (h->nodes[nodeIndex].val <= h->nodes[maxIndex].val) {
        SWAP(h->nodes[nodeIndex].val, h->nodes[maxIndex].val);
        SWAP(h->nodes[nodeIndex].obj, h->nodes[maxIndex].obj);
        siftDown(h, maxIndex);
    }
}

void*
Heap_Pop(Heap h, uint64_t *val) {
    void *retVal = Heap_Peek(h, val);
    
    h->node_count--;
    if(h->node_count > 0)
        siftDown(h, 0);
    
    return retVal;
}

void
Heap_Delete(Heap h) {
    kfree(h->nodes);
    kfree(h);
}