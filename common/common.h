#ifndef _UTILS_COMMON_H_
#define _UTILS_COMMON_H_

#include "types.h"
#include <stdarg.h>

#define MAX_CORES 32

#define SET_BIT(x, index) (x | (1 << index))
#define CLEAR_BIT(x, index) (x & ~(1 << index))
#define TOGGLE_BIT(x, index) (x ^ (1 << index))
#define CHECK_BIT(x, index) ((x & (1 << index)) >> index)
#define SET_VAL_BIT(number, n, x) ( number ^ ((-x ^ number) & (1 << n)))

#define MAX(x, y)  ((x > y)? x : y)
#define MIN(x, y)  ((x < y)? x : y)

void*
memcpy(void *dest,
       void *src,
       size_t size);

void*
memset(void *ptr,
       int val,
       size_t num);

void*
memmove(void *dst,
        const void *src,
        size_t count);

void
strrev(char *str);

size_t
strlen(const char *str);

char*
strcpy ( char * destination,
         const char * source );

int
strncmp(const char * s1,
        const char * s2,
        size_t n);

int
vsnprintf ( char * str,
            const char * format,
            va_list vl );

int
sprintf ( char * str,
          const char * format,
          ... );

char*
utoa(uint64_t val,
     char *ostr,
     int base);

char*
itoa(int64_t val,
     char *ostr,
     int base);

char*
strchr(const char *s,
       int c);

char*
strrchr(const char *s,
        int c);

uint32_t
find_first_zero(uint32_t bit_array);

uint32_t
set_bit_cnt(uint32_t bit_array);

UID
new_uid(void);

uint32_t
rand(int seed);

int
ffs(uint32_t val);

#endif /* end of include guard: _UTILS_COMMON_H_ */
