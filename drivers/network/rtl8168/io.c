/**
 * Copyright (c) 2017 Himanshu Goel
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "io.h"

static uint64_t PCI_BAR;
static bool isPCI_BAR_IO;

void
SetPCIBAR(uint64_t bar, bool isIO) {
    PCI_BAR = bar;
    isPCI_BAR_IO = isIO;
}

void
Write8(uint32_t off, uint8_t val) {
    if(isPCI_BAR_IO) {
        outb(PCI_BAR + off, val);
    } else {
        *(uint8_t*)(PCI_BAR + off) = val;
    }
}

uint8_t
Read8(uint32_t off) {
    if(isPCI_BAR_IO) {
        return inb(PCI_BAR + off);
    } else {
        return *(uint8_t*)(PCI_BAR + off);
    }
}

void
Write16(uint32_t off, uint16_t val) {
    if(isPCI_BAR_IO) {
        outw(PCI_BAR + off, val);
    } else {
        *(uint16_t*)(PCI_BAR + off) = val;
    }
}

uint16_t
Read16(uint32_t off) {
    if(isPCI_BAR_IO) {
        return inw(PCI_BAR + off);
    } else {
        return *(uint16_t*)(PCI_BAR + off);
    }
}

void
Write32(uint32_t off, uint32_t val) {
    if(isPCI_BAR_IO) {
        outl(PCI_BAR + off, val);
    } else {
        *(uint32_t*)(PCI_BAR + off) = val;
    }
}

uint32_t
Read32(uint32_t off) {
    if(isPCI_BAR_IO) {
        return inl(PCI_BAR + off);
    } else {
        return *(uint32_t*)(PCI_BAR + off);
    }
}