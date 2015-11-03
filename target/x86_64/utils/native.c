#include "native.h"

void
outb(const uint16_t port,
     const uint8_t val)
{
    __asm__ volatile ("outb %1, %0" : : "dN" (port), "a" (val));
}

uint8_t
inb(const uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}


void
outw(const uint16_t port,
     const uint16_t val)
{
    __asm__ volatile ("outw %1, %0" : : "dN" (port), "a" (val));
}

uint16_t
inw(const uint16_t port)
{
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}


void
outl(const uint16_t port,
     const uint32_t val)
{
    __asm__ volatile ("outl %1, %0" : : "dN" (port), "a" (val));
}

uint32_t
inl(const uint16_t port)
{
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void
wrmsr(uint32_t msr,
      uint64_t val)
{
    uint32_t lo = val;
    uint32_t hi = (val >> 32);
    __asm__ volatile ("wrmsr" :: "a" (lo), "d" (hi), "c" (msr));

}

uint64_t
rdmsr(uint32_t msr)
{
    uint32_t lo = 0, hi = 0;
    __asm__ volatile ("rdmsr" : "=a" (lo), "=d" (hi) : "c" (msr));
    return ((uint64_t)hi << 32) | lo;
}
