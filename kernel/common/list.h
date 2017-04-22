/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_LIST_H_
#define _CARDINAL_LIST_H_

#include "types.h"
#include "synchronization.h"

typedef struct ListNode ListNode;
typedef struct List List;


typedef enum ListError {
    ListError_None = 0,
    ListError_AllocationFailed = (1 << 0),
    ListError_EndOfList = (1 << 1),
    ListError_Deleting = (1 << 2),
} ListError;

List*
List_Create(Spinlock spin);

ListError
List_AddEntry(List *a, void *value);

uint64_t
List_Length(List *a);

void
List_Remove(List *a,
            uint64_t index);

void
List_Free(List *a);

void*
List_EntryAt(List *a,
             uint64_t index);

void*
List_RotNext(List *a);

void*
List_RotPrev(List *a);

uint64_t
List_GetLastIndex(List *a);

#endif