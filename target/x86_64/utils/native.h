#ifndef _UTILS_NATIVE_H_
#define _UTILS_NATIVE_H_

#include "types.h"

void outb(const uint16_t port,const uint8_t val);
uint8_t inb(const uint16_t port);

void outw(const uint16_t port,const uint16_t val);
uint16_t inw(const uint16_t port);

void outl(const uint16_t port,const uint32_t val);
uint32_t inl(const uint16_t port);

void wrmsr(uint32_t msr, uint64_t val);
uint64_t rdmsr(uint32_t msr);

#endif /* end of include guard: _UTILS_NATIVE_H_ */
