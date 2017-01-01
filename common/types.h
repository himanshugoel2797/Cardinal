/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _OS_TYPES_H_
#define _OS_TYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t UID;
typedef uint64_t SysID;

typedef void (*Callback)(void*);
typedef uint32_t (*Initializer)();
typedef int(*ProcessEntryPoint)(int, char**);

typedef uint64_t off_t;

#define TRUE 1
#define FALSE 0

#define ENABLE 1
#define DISABLE 0

#define ENABLED 1
#define DISABLED 0


#define KiB(x) (x * 1024ull)
#define MiB(x) (KiB(1) * 1024ull * x)
#define GiB(x) (uint64_t)(MiB(1) * 1024ull * x)

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))

#if defined(DEBUG)

#if defined(x86_64)
#define ASSERT(x) if(!(x))__asm__ volatile("cli\n\thlt")
#endif


#else

#define ASSET(x)
#endif

#endif /* end of include guard: _OS_TYPES_H_ */
