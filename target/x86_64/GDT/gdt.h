#ifndef _GDT_H_
#define _GDT_H_

#define GDT_ENTRY_COUNT 7

void GDT_Initialize();

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
