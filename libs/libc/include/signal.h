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

typedef struct {
    union {
        void (*sa_handler)(int);
        void (*sa_sigaction)(int, siginfo_t *, void *);
    };
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
} sigaction;

uint64_t
rt_sigprocmask(int how,
               const sigset_t* set,
               sigset_t* oldset,
               size_t len);

#endif