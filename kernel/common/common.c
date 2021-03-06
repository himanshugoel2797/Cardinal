/*
The MIT License (MIT)

Copyright (c) 2016-2017 Himanshu Goel

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "common.h"

void*
memcpy(void *dest,
       const void *src,
       size_t size) {

    while(size--)
        *(uint8_t*)dest++ = *(uint8_t*)src++;

    return dest;
}

void*
memset(void *dstpp,
       int c,
       size_t len) {
    while(len--)*(uint8_t*)dstpp++ = (uint8_t)c;
    return dstpp;
}

void*
memmove(void *dst,
        const void *src,
        size_t count) {
    char *a = dst;
    const char *b = src;
    if (src!=dst) {
        if (src>dst) {
            while (count--) *a++ = *b++;
        } else {
            a+=count-1;
            b+=count-1;
            while (count--) *a-- = *b--;
        }
    }
    return dst;
}

void
strrev(char *str) {
    char temp, *end_ptr;

    /* If str is NULL or empty, do nothing */
    if( str == NULL || !(*str) )
        return;

    end_ptr = str + strlen(str) - 1;

    /* Swap the chars */
    while( end_ptr > str ) {
        temp = *str;
        *str = *end_ptr;
        *end_ptr = temp;
        str++;
        end_ptr--;
    }
}

size_t
strlen(const char *str) {
    size_t size = 0;
    while(str[size] != 0) {
        size++;
    }
    return size;
}

size_t
strnlen_s(const char *str,
          size_t strsz) {

    if(str == NULL)
        return 0;

    size_t size = 0;
    for(; size < strsz; size++)
        if(str[size] == 0)break;

    return size;
}

int
strncmp(const char* s1,
        const char* s2,
        size_t n) {
    while(n--)
        if(*s1++!=*s2++)
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
    return 0;
}

char*
strcpy(char* destination,
       const char* source) {
    return (char*)memcpy(destination, (void*)source, strlen(source));
}

wchar_t*
wcscpy( wchar_t* destination,
        const wchar_t *source) {
    wchar_t *a = destination;
    while((*destination++ = *source++));
    return a;
}

size_t
wcslen(const wchar_t *str) {
    size_t size = 0;
    while(str[size] != 0) {
        size++;
    }
    return size;
}


char*
strcpy_s(char *dst,
         size_t d_len,
         const char *src,
         size_t s_len) {
    return (char*)memcpy(dst, (void*)src, d_len < s_len?d_len:s_len );
}

char*
strchr(const char *s,
       int c) {
    while (*s != (char)c)
        if (!*s++)
            return 0;
    return (char *)s;
}

char*
strrchr(const char *s,
        int c) {
    const char *e = s + strlen(s);
    while (*e != (char)c)
        if (e-- == s)
            return 0;
    return (char *)e;
}

uint32_t
find_first_zero(uint32_t bit_array) {
    uint32_t pos = 0;
    if(bit_array == 0) return 0;

    __asm__("bsfl %1,%0\n\t"
            "jne 1f\n\t"
            "movl $32, %0\n"
            "1:"
            : "=r" (pos)
            : "r" (~(bit_array)));

    return (unsigned short) pos;
}

uint32_t
set_bit_cnt(uint32_t bit_array) {
    uint32_t set_bit = 0;
    if(bit_array == 0)return 0;
    for(int i = 0; i < 32; i++) {
        if((bit_array >> i) & 1)set_bit++;
    }
    return set_bit;
}

static volatile _Atomic UID uids_base = 0;
UID
new_uid(void) {
    return (++uids_base & 0xFFFFFFFF);
}


static uint64_t rng_val = 0;

void
seed(uint64_t seed) {
    rng_val = seed;
}

uint32_t
rand(void) {
    const uint64_t a = 6364136223846793005;
    const uint64_t c = 1;
    rng_val = (a * rng_val + c);
    uint32_t v = (uint32_t)(rng_val >> 32);
    if(v == 0)
        return v+1;
    return v;
}

int
ffs(uint32_t val) {
    int cnt = 0;
    for(; ((val >> cnt) & 1) == 1; cnt++);
    return cnt;
}