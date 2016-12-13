#ifndef _CARDINAL_DRIVER_UTILS_H_
#define _CARDINAL_DRIVER_UTILS_H_

#include "cardinal_types.h"
#include "syscall.h"
#include "syscall_list.h"

#ifdef __x86_64__
static __inline void
outb(const uint16_t port,
     const uint8_t val) {
    __asm__ volatile ("outb %1, %0" : : "dN" (port), "a" (val));
}

static __inline uint8_t
inb(const uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}


static __inline void
outw(const uint16_t port,
     const uint16_t val) {
    __asm__ volatile ("outw %1, %0" : : "dN" (port), "a" (val));
}

static __inline uint16_t
inw(const uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}


static __inline void
outl(const uint16_t port,
     const uint32_t val) {
    __asm__ volatile ("outl %1, %0" : : "dN" (port), "a" (val));
}

static __inline uint32_t
inl(const uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}
#endif

static __inline void
R0_EnterCriticalSection(void) {
#ifdef __x86_64__
    __asm__("cli");
#endif
}

static __inline void
R0_ExitCriticalSection(void) {
#ifdef __x86_64__
    __asm__("sti");
#endif
}

static __inline uint64_t
R01_AllocateInterrupts(uint32_t cnt,
                       uint32_t *val) {
    if(val == NULL)
        return -EINVAL;

    *val = Syscall1(Syscall_R01_AllocateInterrupts, cnt);
    return GetErrno();
}

static __inline uint64_t
R01_RegisterForInterrupts(uint32_t irq,
                          uint32_t cnt) {
    Syscall2(Syscall_R01_RegisterForInterrupts, irq, cnt);
    return GetErrno();
}

#endif