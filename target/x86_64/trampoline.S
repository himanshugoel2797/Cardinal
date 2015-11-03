.section .trampoline_code, "ax", @progbits
.code32
                               gdt64:
                               .nulld:
                               .word 0
                               .word 0
                               .byte 0
                               .byte 0
                               .byte 0
                               .byte 0
                               .coded:
                               .word 0
                               .word 0
                               .byte 0
                               .byte 0b10011010
                               .byte 0b00100000
                               .byte 0
                               .datad:
                               .word 0
                               .word 0
                               .byte 0
                               .byte 0b10010010
                               .byte 0b00000000
                               .byte 0
                               .GDTPTR:
                               .word 23
                               .long gdt64
.code16
_trampoline:
cli
mov $0x1000, %eax
mov %eax, %cr3

mov %cr4, %eax
or $((1 << 5) | (1 << 7)), %eax
mov %eax, %cr4

mov $0xC0000080, %ecx
rdmsr
or $256, %eax
wrmsr

mov %cr0, %ebx
or $0x80000001, %ebx
mov %ebx, %cr0
lgdt (.GDTPTR)
jmp $0x8, $.tJmp
.tJmp:
.code64
mov $0x10, %ax
mov %ax, %ds
mov %ax, %es
mov %ax, %fs
mov %ax, %gs
mov %ax, %ss
mov $0xfffffffe00000000, %rsp
add $tramp_stack_top, %rsp
mov $0xffffffff80000000, %rax
add $smp_bootstrap, %rax
push %rax
retq
cli
hlt

.section .trampoline_stack
tramp_stack_bottom:
.skip 512
tramp_stack_top: