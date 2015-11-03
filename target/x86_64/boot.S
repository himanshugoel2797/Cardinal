#ifdef MULTIBOOT1
.set ALIGN,    1 << 0
                 .set MEMINFO,  1 << 1
                 .set VIDINFO,  1 << 2

                 .set FLAGS,    ALIGN | MEMINFO | VIDINFO
                 .set MAGIC,    0x1BADB002
                 .set CHECKSUM, -(MAGIC + FLAGS)
                 .section .multiboot
                 .align 4
                 .long MAGIC
                 .long FLAGS
                 .long CHECKSUM
                 .long 0, 0, 0, 0, 0
                 .long 0 # 0 = set graphics mode
                               .long 1920, 1080, 32 # Width, height, depth

#elif defined(MULTIBOOT2)
.set MAGIC, 0xE85250D6
.set ARCH, 0  	#i386
.set SIZE, multiboot_end - multiboot_start
.set CHECKSUM, -(MAGIC + SIZE)

.section .multiboot
.align 4
multiboot_start:
.long MAGIC
.long ARCH
.long SIZE
.long CHECKSUM

.word 3
.word 0
.long 12
.long _start

.align 8

.word 1 	#type
.word 0 	#flags
.long 28 	#size
.long 6 	#mmap
.long 8 	#framebuffer
.long 9 	#elf map
.long 14    #acpi_old
.long 15 	#acpi_new

.align 8

.word 5
.word 0
.long 20
.long 1920
.long 1080
.long 32

.align 8

.word 0
.word 0
.long 8

multiboot_end:
#endif

                               .section .bootstrap_code, "ax", @progbits
                               .code32

                               temp_stor_info:
                               .long 0
                               .long 0
                               temp_stor_magic:
                               .long 0
                               .long 0
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

                               .global _start
                               .type _start, @function
                               _start:
                               mov $0x1000, %edi
                               mov %edi, %cr3

                               mov %eax, (temp_stor_magic)
                               mov %ebx, (temp_stor_info)	#Backup EBX

                               mov $0x80000001, %eax
                               cpuid
                               test $(1 << 29), %edx
                               jz .Crash

                               xor %eax, %eax
                               mov $4096, %ecx
                               rep stosl
                               mov $4096, %ecx
                               rep stosl
                               mov $4096, %ecx
                               rep stosl
                               mov %cr3, %edi
                               movl $0x2003, (%edi)
                               movl $0x5003, 0xFF8(%edi)
                               add $0x1000, %edi
                               movl $0x3003, (%edi)
                               add $0x1000, %edi
                               movl $0x4003, (%edi)
                               add $0x1000, %edi

#Map first 4GB RAM to the top of memory
                               movl $0x0, 0x1FE0(%edi)
                               movl $0x0, 0x1FE8(%edi)
                               movl $0x0, 0x1FF0(%edi)
                               movl $0x0, 0x1FF8(%edi)
                               movl $0x108B, 0x1FC0(%edi)
                               movl $0x4000108B, 0x1FC8(%edi)
                               movl $0x8000108B, 0x1FD0(%edi)
                               movl $0xC000108B, 0x1FD8(%edi)

                               movl $0x8000108B, 0x1FE0(%edi)
                               movl $0xC000108B, 0x1FE8(%edi)
                               movl $0x108B, 0x1FF0(%edi)
                               movl $0x4000108B, 0x1FF8(%edi)

                               mov $0x4000, %edi
                               mov $0x3, %ebx
                               mov $512, %ecx

                               .SetEntry:
                               movl %ebx, (%edi)
                               add $0x1000, %ebx
                               add $8, %edi
                               loop .SetEntry

#Enable PAE
                               mov %cr4, %eax
                               or $(1 << 5), %eax
                               mov %eax, %cr4

#Set LM bit
                               mov $0xC0000080, %ecx
                               rdmsr
                               or $256, %eax
                               wrmsr

#Enable paging
                               mov %cr0, %eax
                               or $0x80000000, %eax
                               mov %eax, %cr0

#We're in IA-32e mode now, restore the values of eax and ebx so they can be used
#ebx was restored earlier

#Now lets setup a temporary GDT with 64bit descriptors and jump to it
                               lgdt (.GDTPTR)
                               mov %edx, %ebx
                               jmp $0x8, $.Ljmp
                               .Ljmp:
                               .code64
                               mov $0x10, %ax
                               mov %ax, %ds
                               mov %ax, %es
                               mov %ax, %fs
                               mov %ax, %gs
                               mov %ax, %ss
                               mov $0xfffffffe00000000, %rsp
                               mov $0xfffffffe00000000, %rdi
                               add $stack_top, %rsp
                               mov (temp_stor_magic), %rsi
                               add (temp_stor_info), %rdi
                               mov $0xffffffff80200000, %rax
                               push %rax
                               ret
                               cli
                               hlt
                               .Lhang:
                               jmp .Lhang

                               .Crash:
                               sti
                               int $0x0
                               jmp .Crash

                               .size _start, . - _start

                               .section .bootstrap_stack
                               stack_bottom:
                               .skip 16384 # 4 KiB	(4 KiB is enough for initialization until threading is setup)
                               stack_top: