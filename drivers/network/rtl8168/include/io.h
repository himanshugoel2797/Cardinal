// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef RTL8168_DRIVER_IO_H_
#define RTL8168_DRIVER_IO_H_

#include <stdint.h>
#include <stdbool.h>

void
SetPCIBAR(uint64_t bar, bool isIO);

void
Write8(uint32_t off, uint8_t val);

uint8_t
Read8(uint32_t off);

void
Write16(uint32_t off, uint16_t val);

uint16_t
Read16(uint32_t off);

void
Write32(uint32_t off, uint32_t val);

uint32_t
Read32(uint32_t off);


#endif