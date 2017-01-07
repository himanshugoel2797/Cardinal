#ifndef _CARDINAL_REF_COUNTER_H_
#define _CARDINAL_REF_COUNTER_H_

#include "types.h"

typedef void (*ReferenceFreeHandler)(const void*);

typedef struct {
    _Atomic int ref_count;
    int32_t	top_offset;
    ReferenceFreeHandler free;
} Ref;

static __inline
void RefInit(Ref *ref, ReferenceFreeHandler free, int top_off) {

    ref->free = free;
    ref->top_offset = top_off;
    ref->ref_count = 1;
    //TODO fix this to use the offset to the ref object
}

static __inline
void RefInc(const Ref *obj) {
    Ref* ref = (Ref*)obj;
    ref->ref_count++;
}

static __inline
void* RefDec(const void *obj) {
    Ref* ref = (Ref*)obj;
    ref->ref_count--;
    if(ref->ref_count == 0) {
        ref->free((const void*)((uintptr_t)obj - ref->top_offset));
        return NULL;
    }

    return (void*)((uintptr_t)obj - ref->top_offset);
}

#endif