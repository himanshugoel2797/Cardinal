/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef OS_TYPES_H_
#define OS_TYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t UID;
typedef uint64_t SysID;

typedef void (*Callback)(void*);
typedef uint32_t (*Initializer)();
typedef int(*ProcessEntryPoint)(int, char**);

typedef uint64_t off_t;

#define TRUE true
#define FALSE false

#define ENABLE true
#define DISABLE false

#define ENABLED true
#define DISABLED false


#define KiB(x) (x * 1024ull)
#define MiB(x) (KiB(1) * 1024ull * x)
#define GiB(x) (uint64_t)(MiB(1) * 1024ull * x)

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#define NONNULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define PURE __attribute__((pure))
#define IS_NULL(x) if(!x)__builtin_trap()
#define PACKED __attribute__((packed))
#define NORETURN __attribute__((noreturn))
#define NONNULL_RETURN __attribute__((returns_nonnull))

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#define PANIC(msg) set_trap_str( __FILE__"," S__LINE__ ":" msg), __builtin_trap()

#if defined(DEBUG)
//First set the trap message, then raise the trap
void set_trap_str(const char *str);
#define ASSERT(x, msg) if(!(x)) PANIC(msg)
#else
#define ASSERT(x)
#endif /* end of include guard: _OS_TYPES_H_ */

#endif