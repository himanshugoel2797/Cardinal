/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CARDINAL_SIGNAL_H_
#define _CARDINAL_SIGNAL_H_

#include "types.h"
#include "time.h"

#define SUPPORTED_SIGNAL_COUNT 512

typedef struct {
    uint64_t val[SUPPORTED_SIGNAL_COUNT/(8 * sizeof(uint64_t))];
} sigset_t;

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

typedef union {
    int int_val;
    void* ptr_val;
} sigval_t;

typedef struct {
    int si_signo;
    int si_errno;
    int si_code;
    int si_trapno;

    UID si_pid;
    UID si_uid;
    int si_status;
    clock_t si_utime;
    clock_t si_stime;
    sigval_t si_value;
    int si_int;
    void *si_ptr;
    int si_overrun;

    int si_timerid;
    void *si_addr;
    long si_band;

    int si_fd;
    short si_addr_lsb;
    void *si_lower;
    void *si_upper;
    void *si_call_addr;
    int si_syscall;
} siginfo_t;

#define SA_NOCLDSTOP  1
#define SA_NOCLDWAIT  2
#define SA_SIGINFO    4
#define SA_ONSTACK    0x08000000
#define SA_RESTART    0x10000000
#define SA_NODEFER    0x40000000
#define SA_RESETHAND  0x80000000
#define SA_RESTORER   0x04000000

struct sigaction {
    union {
        void (*sa_handler)(int);
        void (*sa_sigaction)(int, siginfo_t *, void *);
    };
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};

uint64_t
rt_sigprocmask(int how,
               const sigset_t* set,
               sigset_t* oldset,
               size_t len);

#endif