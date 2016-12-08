#ifndef _CARDINAL_LIB_SYSCALL_H_
#define _CARDINAL_LIB_SYSCALL_H_

#include <stdint.h>
#include <stddef.h>
#include "syscall_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PARAM_COUNT 6

#define EPERM            1
#define EINVAL           2
#define ENOSYS           3
#define EUNKNWN          4
#define ERDONLY          5
#define ENOMEM           6
#define ENOMSG           7
#define EINUSE           8
#define EEOF             9
#define ENAVAIL          10

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