/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_LIB_SYSCALL_H_
#define _CARDINAL_LIB_SYSCALL_H_

#include <stdint.h>
#include <stddef.h>
#include "syscall_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PARAM_COUNT 6

#undef EPERM
#define EPERM            1

#undef EINVAL
#define EINVAL           2

#undef ENOSYS
#define ENOSYS           3

#undef EUNKNWN
#define EUNKNWN          4

#undef ERDONLY
#define ERDONLY          5

#undef ENOMEM
#define ENOMEM           6

#undef ENOMSG
#define ENOMSG           7

#undef EINUSE
#define EINUSE           8

#undef EEOF
#define EEOF             9

#undef ENAVAIL
#define ENAVAIL          10

#undef EISDIR
#define EISDIR           11

#undef EISFILE
#define EISFILE          12

static __inline uint64_t
Syscall0(uint32_t syscall_num) {

    uint64_t volatile parameters[0];
    uint64_t ret_error = -ENOSYS;

#if defined(__x86_64__)
    __asm__ volatile
    (
        "syscall"
        : "=S"(ret_error)
        : "S"(syscall_num), "d"(parameters), "D"(0)
        : "rcx","r11", "memory"
    );
#endif

    return ret_error;
}

static __inline uint64_t
Syscall1(uint32_t syscall_num,
         uint64_t p0) {

    uint64_t volatile parameters[1];
    parameters[0] = p0;

    uint64_t ret_error = -ENOSYS;

#if defined(__x86_64__)
    __asm__ volatile
    (
        "syscall"
        : "=S"(ret_error)
        : "S"(syscall_num), "d"(parameters), "D"(1)
        : "rcx","r11", "memory"
    );
#endif

    return ret_error;
}

static __inline uint64_t
Syscall2(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1) {

    uint64_t volatile parameters[2];
    parameters[0] = p0;
    parameters[1] = p1;

    uint64_t ret_error = -ENOSYS;

#if defined(__x86_64__)
    __asm__ volatile
    (
        "syscall"
        : "=S"(ret_error)
        : "S"(syscall_num), "d"(parameters), "D"(2)
        : "rcx","r11", "memory"
    );
#endif

    return ret_error;
}

static __inline uint64_t
Syscall3(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2) {

    uint64_t volatile parameters[3];
    parameters[0] = p0;
    parameters[1] = p1;
    parameters[2] = p2;

    uint64_t ret_error = -ENOSYS;

#if defined(__x86_64__)
    __asm__ volatile
    (
        "syscall"
        : "=S"(ret_error)
        : "S"(syscall_num), "d"(parameters), "D"(3)
        : "rcx","r11", "memory"
    );
#endif

    return ret_error;
}

static __inline uint64_t
Syscall4(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2,
         uint64_t p3) {

    uint64_t volatile parameters[4];
    parameters[0] = p0;
    parameters[1] = p1;
    parameters[2] = p2;
    parameters[3] = p3;

    uint64_t ret_error = -ENOSYS;

#if defined(__x86_64__)
    __asm__ volatile
    (
        "syscall"
        : "=S"(ret_error)
        : "S"(syscall_num), "d"(parameters), "D"(4)
        : "rcx","r11", "memory"
    );
#endif

    return ret_error;
}

static __inline uint64_t
Syscall5(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2,
         uint64_t p3,
         uint64_t p4) {

    uint64_t volatile parameters[5];
    parameters[0] = p0;
    parameters[1] = p1;
    parameters[2] = p2;
    parameters[3] = p3;
    parameters[4] = p4;

    uint64_t ret_error = -ENOSYS;

#if defined(__x86_64__)
    __asm__ volatile
    (
        "syscall"
        : "=S"(ret_error)
        : "S"(syscall_num), "d"(parameters), "D"(5)
        : "rcx","r11", "memory"
    );
#endif

    return ret_error;
}

static __inline uint64_t
Syscall6(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2,
         uint64_t p3,
         uint64_t p4,
         uint64_t p5) {

    uint64_t volatile parameters[6];
    parameters[0] = p0;
    parameters[1] = p1;
    parameters[2] = p2;
    parameters[3] = p3;
    parameters[4] = p4;
    parameters[5] = p5;

    uint64_t ret_error = -ENOSYS;

#if defined(__x86_64__)
    __asm__ volatile
    (
        "syscall"
        : "=S"(ret_error)
        : "S"(syscall_num), "d"(parameters), "D"(6)
        : "rcx","r11", "memory"
    );
#endif

    return ret_error;
}

/**
 * @brief      Gets the errno.
 *
 * @return     The errno.
 */
static __inline uint64_t
GetErrno(void) {
    return Syscall0(Syscall_GetErrno);
}

#ifdef __cplusplus
}
#endif

#endif