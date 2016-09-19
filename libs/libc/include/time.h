#ifndef _CARDINAL_LIBC_TIME_H_
#define _CARDINAL_LIBC_TIME_H_

#include "types.h"

typedef long clock_t;
typedef long time_t;

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

#endif