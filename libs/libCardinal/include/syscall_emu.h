#ifndef _LIB_CARDINAL_SYSCALL_EMU_H_
#define _LIB_CARDINAL_SYSCALL_EMU_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "cardinal_types.h"
#include "syscall_property.h"
#include "syscall_list.h"
#include "syscall.h"
#include "file_server.h"
#include "ipc.h"

#include <memserver/server.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FILE_DESCRIPTORS 1024

typedef enum {
    Cardinal_EmulatedSyscalls_Read = 0,
    Cardinal_EmulatedSyscalls_Write = 1,
    Cardinal_EmulatedSyscalls_Open = 2,
    Cardinal_EmulatedSyscalls_Close = 3,
    Cardinal_EmulatedSyscalls_MMap = 9,
    Cardinal_EmulatedSyscalls_Brk = 12,
    Cardinal_EmulatedSyscalls_Ioctl = 16, 	//TODO: Implement a temporary wrapper for this, we don't want to actually pass ioctls to file servers
    Cardinal_EmulatedSyscalls_Readv = 19,
    Cardinal_EmulatedSyscalls_Writev = 20,
    Cardinal_EmulatedSyscalls_Nanosleep = 35,
    Cardinal_EmulatedSyscalls_GetPID = 39,
    Cardinal_EmulatedSyscalls_Fork = 57,
    Cardinal_EmulatedSyscalls_Execve = 59,
    Cardinal_EmulatedSyscalls_Exit = 60,
    Cardinal_EmulatedSyscalls_Fcntl = 72,
    Cardinal_EmulatedSyscalls_ArchPrctl = 158,
    Cardinal_EmulatedSyscalls_GetTID = 186,
    Cardinal_EmulatedSyscalls_Time = 201,
    Cardinal_EmulatedSyscalls_SetTidAddress = 218,
} Cardinal_EmulatedSyscalls;

static __inline uint64_t
SyscallEmu0(uint32_t syscall_num) {
    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    case Cardinal_EmulatedSyscalls_GetPID:
        ret_error = Syscall2(Syscall_GetProperty, CardinalProperty_PID, 0);
        break;
    case Cardinal_EmulatedSyscalls_Fork:
        //TODO Implement the IPC path

        break;
    case Cardinal_EmulatedSyscalls_GetTID:
        ret_error = Syscall2(Syscall_GetProperty, CardinalProperty_TID, 0);
        break;
    default:
        __asm__("hlt" :: "a"(syscall_num));
        break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu1(uint32_t syscall_num,
            uint64_t p0) {

    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    case Cardinal_EmulatedSyscalls_Brk:
        ret_error = Syscall1(Syscall_Brk, p0);
        break;
    case Cardinal_EmulatedSyscalls_Time:
        ret_error = Syscall2(Syscall_GetProperty, CardinalProperty_Time, 0);
        if(p0 != 0)*(long*)p0 = (long)ret_error;
        break;
    case Cardinal_EmulatedSyscalls_SetTidAddress:
        ret_error = Syscall3(Syscall_SetProperty, CardinalProperty_SetTidAddress, 0, p0);
        break;
    case Cardinal_EmulatedSyscalls_Exit:
        ret_error = -1;//Syscall3(Syscall_SetProperty, CardinalProperty_Exit, 0, p0);
        break;
    default:
        __asm__("hlt" :: "a"(syscall_num));
        break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu2(uint32_t syscall_num,
            uint64_t p0,
            uint64_t p1) {

    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    case Cardinal_EmulatedSyscalls_ArchPrctl:
        ret_error = Syscall3(Syscall_SetProperty, CardinalProperty_ArchPrctl, p0, p1);
        break;
    case Cardinal_EmulatedSyscalls_Nanosleep: {
        struct timespec *tmp = (struct timespec*)p0;
        ret_error = Syscall1(Syscall_Nanosleep, tmp->tv_sec * 1000000000ULL + tmp->tv_nsec);
    }
    break;
    case Cardinal_EmulatedSyscalls_Ioctl:
        return -1;
        break;
    default:
        __asm__("hlt" :: "a"(syscall_num));
        break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu3(uint32_t syscall_num,
            uint64_t p0,
            uint64_t p1,
            uint64_t p2) {

    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    default:
        __asm__("hlt" :: "a"(syscall_num));
        break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu4(uint32_t syscall_num,
            uint64_t p0,
            uint64_t p1,
            uint64_t p2,
            uint64_t p3) {

    uint64_t volatile parameters[4];
    parameters[0] = p0;
    parameters[1] = p1;
    parameters[2] = p2;
    parameters[3] = p3;

    __asm__("hlt" :: "a"(syscall_num));
    uint64_t ret_error = -ENOSYS;

    return ret_error;
}

static __inline uint64_t
SyscallEmu5(uint32_t syscall_num,
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

    __asm__("hlt" :: "a"(syscall_num));
    uint64_t ret_error = -ENOSYS;

    return ret_error;
}

static __inline uint64_t
SyscallEmu6(uint32_t syscall_num,
            uint64_t p0,
            uint64_t p1,
            uint64_t p2,
            uint64_t p3,
            uint64_t p4,
            uint64_t p5) {

    uint64_t ret_error = -ENOSYS;

    switch(syscall_num) {
    default:
        __asm__("hlt" :: "a"(syscall_num));
        break;
    }

    return ret_error;
}

#ifdef __cplusplus
}
#endif

#endif