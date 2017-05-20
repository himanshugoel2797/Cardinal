// Copyright (c) 2017 Himanshu Goel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef _GDT_H_
#define _GDT_H_

#include "types.h"

#define GDT_ENTRY_COUNT 8

void GDT_Bootstrap(void);

void GDT_InitializeMP(void);

void GDT_Initialize();

void GDT_SetIST(uint8_t ist, uint64_t stack);

typedef volatile struct tss_struct {
    uint32_t rsv0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t rsv1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t rsv2;
    uint16_t rsv3;
    uint16_t iomap;
} __attribute__((packed)) tss_struct;

#endif /* end of include guard: _GDT_H_ */
