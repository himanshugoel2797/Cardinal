#ifndef _LIB_CARDINAL_SYSCALL_EMU_H_
#define _LIB_CARDINAL_SYSCALL_EMU_H_

#include <stdint.h>
#include <stddef.h>
#include "syscall_property.h"
#include "syscall_list.h"
#include "syscall.h"

typedef enum {
	Cardinal_EmulatedSyscalls_MMap = 9,
	Cardinal_EmulatedSyscalls_Brk = 12,
	Cardinal_EmulatedSyscalls_ArchPrctl = 158,
	Cardinal_EmulatedSyscalls_SetTidAddress = 218,
} Cardinal_EmulatedSyscalls;

static __inline uint64_t
SyscallEmu0(uint32_t syscall_num) {
    uint64_t ret_error = ENOSYS;



    return ret_error;
}

static __inline uint64_t
SyscallEmu1(uint32_t syscall_num,
         uint64_t p0) {

    uint64_t ret_error = ENOSYS;

    switch(syscall_num) {
    	case Cardinal_EmulatedSyscalls_SetTidAddress:
    		ret_error = Syscall3(Syscall_SetProperty, CardinalProperty_SetTidAddress, 0, p0);
    	break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu2(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1) {

    uint64_t ret_error = ENOSYS;

    switch(syscall_num) {
    	case Cardinal_EmulatedSyscalls_ArchPrctl:
    		ret_error = Syscall3(Syscall_SetProperty, Cardinal_EmulatedSyscalls_ArchPrctl, p0, p1);
    	break;
    }

    return ret_error;
}

static __inline uint64_t
SyscallEmu3(uint32_t syscall_num,
         uint64_t p0,
         uint64_t p1,
         uint64_t p2) {

    uint64_t volatile parameters[3];
    parameters[0] = p0;
    parameters[1] = p1;
    parameters[2] = p2;

    uint64_t ret_error = ENOSYS;

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

    uint64_t ret_error = ENOSYS;

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

    uint64_t ret_error = ENOSYS;

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

    uint64_t ret_error = ENOSYS;

    switch(syscall_num) {
    	case Cardinal_EmulatedSyscalls_MMap:
    		ret_error = Syscall6(Syscall_MMap, p0, p1, p2, p3, p4, p5);
    	break;
    }

    return ret_error;
}


#endif