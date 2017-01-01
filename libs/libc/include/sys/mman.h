/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_MMAN_H_
#define _CARDINAL_MMAN_H_

#define MAP_FAILED ((void *) -1)

#define	PROT_NONE      0
#define	PROT_READ      1
#define	PROT_WRITE     2
#define	PROT_EXEC      4
#define PROT_GROWSDOWN  0x01000000
#define PROT_GROWSUP    0x02000000

#define	MAP_SHARED     0x01
#define	MAP_PRIVATE    0x02
#define	MAP_FIXED      0x10

#define MAP_TYPE       0x0f
#define MAP_FILE       0x00
#define MAP_ANON       0x20
#define MAP_ANONYMOUS  MAP_ANON
#define MAP_32BIT      0x40
#define MAP_NORESERVE  0x4000
#define MAP_GROWSDOWN  0x0100
#define MAP_DENYWRITE  0x0800
#define MAP_EXECUTABLE 0x1000
#define MAP_LOCKED     0x2000
#define MAP_POPULATE   0x8000
#define MAP_NONBLOCK   0x10000
#define MAP_STACK      0x20000
#define MAP_HUGETLB    0x40000


void*
mmap(void* addr,
     size_t target_len,
     int prot,
     int flags,
     int fd,
     off_t offset);

uint64_t
brk(void* addr);

#endif