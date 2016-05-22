	.text
	.file	"GDT/gdt.c"
	.globl	GDT_Initialize
	.align	16, 0x90
	.type	GDT_Initialize,@function
GDT_Initialize:                         # @GDT_Initialize
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp0:
	.cfi_def_cfa_offset 16
.Ltmp1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp2:
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	subq	$96, %rsp
.Ltmp3:
	.cfi_offset %rbx, -32
.Ltmp4:
	.cfi_offset %r14, -24
	xorl	%eax, %eax
	#APP
	cli
	#NO_APP
	movq	$sys_tss, %rdi
	movl	$104, %ecx
	movl	%ecx, %edx
	xorl	%esi, %esi
	movl	%eax, -20(%rbp)         # 4-byte Spill
	callq	memset
	movw	$104, sys_tss+102
	movw	$47, gdt_table
	movq	$gdt_entries, gdt_table+2
	xorl	%ecx, %ecx
	movl	%ecx, %edi
	movl	%ecx, %esi
	movl	%ecx, %edx
	movl	%ecx, -24(%rbp)         # 4-byte Spill
	movl	-24(%rbp), %r8d         # 4-byte Reload
	movq	%rax, -32(%rbp)         # 8-byte Spill
	callq	GDT_SetEntry
	movl	$1, %ecx
	xorl	%edx, %edx
	movl	$4294967295, %esi       # imm = 0xFFFFFFFF
	movl	$154, %edi
	movl	$32, %r8d
	movl	$1, %r9d
	xorl	%r10d, %r10d
	movl	$-1, %r11d
	movl	$154, %ebx
	movl	$32, %r14d
	movl	%edi, -36(%rbp)         # 4-byte Spill
	movl	%r9d, %edi
	movl	%esi, -40(%rbp)         # 4-byte Spill
	movl	%r10d, %esi
	movl	%edx, -44(%rbp)         # 4-byte Spill
	movl	%r11d, %edx
	movl	%ecx, -48(%rbp)         # 4-byte Spill
	movl	%ebx, %ecx
	movl	%r8d, -52(%rbp)         # 4-byte Spill
	movl	%r14d, %r8d
	callq	GDT_SetEntry
	movl	$2, %ecx
	xorl	%edx, %edx
	movl	$4294967295, %esi       # imm = 0xFFFFFFFF
	movl	$146, %edi
	movl	$2, %r8d
	movl	$-1, %r9d
	movl	$146, %r10d
	xorl	%r11d, %r11d
	movl	%edi, -56(%rbp)         # 4-byte Spill
	movl	%r8d, %edi
	movl	%esi, -60(%rbp)         # 4-byte Spill
	movl	%r11d, %esi
	movl	%edx, -64(%rbp)         # 4-byte Spill
	movl	%r9d, %edx
	movl	%ecx, -68(%rbp)         # 4-byte Spill
	movl	%r10d, %ecx
	movl	%r11d, %r8d
	callq	GDT_SetEntry
	movl	$3, %ecx
	xorl	%edx, %edx
	movl	$4294967295, %esi       # imm = 0xFFFFFFFF
	movl	$250, %edi
	movl	$32, %r8d
	movl	$3, %r9d
	xorl	%r10d, %r10d
	movl	$-1, %r11d
	movl	$250, %ebx
	movl	$32, %r14d
	movl	%edi, -72(%rbp)         # 4-byte Spill
	movl	%r9d, %edi
	movl	%esi, -76(%rbp)         # 4-byte Spill
	movl	%r10d, %esi
	movl	%edx, -80(%rbp)         # 4-byte Spill
	movl	%r11d, %edx
	movl	%ecx, -84(%rbp)         # 4-byte Spill
	movl	%ebx, %ecx
	movl	%r8d, -88(%rbp)         # 4-byte Spill
	movl	%r14d, %r8d
	callq	GDT_SetEntry
	movl	$4, %ecx
	xorl	%edx, %edx
	movl	$4294967295, %esi       # imm = 0xFFFFFFFF
	movl	$242, %edi
	movl	$4, %r8d
	movl	$-1, %r9d
	movl	$242, %r10d
	xorl	%r11d, %r11d
	movl	%edi, -92(%rbp)         # 4-byte Spill
	movl	%r8d, %edi
	movl	%esi, -96(%rbp)         # 4-byte Spill
	movl	%r11d, %esi
	movl	%edx, -100(%rbp)        # 4-byte Spill
	movl	%r9d, %edx
	movl	%ecx, -104(%rbp)        # 4-byte Spill
	movl	%r10d, %ecx
	movl	%r11d, %r8d
	callq	GDT_SetEntry
	movl	$5, %ecx
	movq	$sys_tss, %rax
	movl	%eax, %edx
	movl	$5, %edi
	movl	$103, %esi
	movl	$233, %r8d
	xorl	%r9d, %r9d
	movl	%esi, -108(%rbp)        # 4-byte Spill
	movl	%edx, %esi
	movl	-108(%rbp), %edx        # 4-byte Reload
	movl	%ecx, -112(%rbp)        # 4-byte Spill
	movl	%r8d, %ecx
	movl	%r9d, %r8d
	callq	GDT_SetEntry
	movq	$gdt_table, %rax
	#APP
	lgdtq	(%rax)
	#NO_APP
	#APP
	movq	$flush, %rax
	ljmpl	$8, $.flush
flush:
	movw	$16, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	movw	%ax, %ss
	movw	$43, %ax
	ltrw	%ax
	#NO_APP
	movl	$104, %ecx
	movl	%ecx, %eax
	#APP
	movq	%rax, %rax
	hlt
	#NO_APP
	addq	$96, %rsp
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
.Lfunc_end0:
	.size	GDT_Initialize, .Lfunc_end0-GDT_Initialize
	.cfi_endproc

	.globl	GDT_SetEntry
	.align	16, 0x90
	.type	GDT_SetEntry,@function
GDT_SetEntry:                           # @GDT_SetEntry
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp5:
	.cfi_def_cfa_offset 16
.Ltmp6:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp7:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%r8b, %al
	movb	%cl, %r9b
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movb	%r9b, -13(%rbp)
	movb	%al, -14(%rbp)
	movw	-8(%rbp), %r10w
	movslq	-4(%rbp), %r11
	movw	%r10w, gdt_entries+2(,%r11,8)
	movb	-6(%rbp), %al
	movslq	-4(%rbp), %r11
	movb	%al, gdt_entries+4(,%r11,8)
	movb	-5(%rbp), %al
	movslq	-4(%rbp), %r11
	movb	%al, gdt_entries+7(,%r11,8)
	movw	-12(%rbp), %r10w
	movslq	-4(%rbp), %r11
	movw	%r10w, gdt_entries(,%r11,8)
	movzwl	-10(%rbp), %ecx
	andl	$15, %ecx
	movb	%cl, %al
	movslq	-4(%rbp), %r11
	movb	%al, gdt_entries+6(,%r11,8)
	movzbl	-14(%rbp), %ecx
	andl	$240, %ecx
	movslq	-4(%rbp), %r11
	movzbl	gdt_entries+6(,%r11,8), %edx
	orl	%ecx, %edx
	movb	%dl, %al
	movb	%al, gdt_entries+6(,%r11,8)
	movb	-13(%rbp), %al
	movslq	-4(%rbp), %r11
	leaq	gdt_entries(,%r11,8), %r11
	movb	%al, 5(%r11)
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	GDT_SetEntry, .Lfunc_end1-GDT_SetEntry
	.cfi_endproc

	.globl	SetKernelStack
	.align	16, 0x90
	.type	SetKernelStack,@function
SetKernelStack:                         # @SetKernelStack
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp8:
	.cfi_def_cfa_offset 16
.Ltmp9:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp10:
	.cfi_def_cfa_register %rbp
	pushq	%rax
	movq	%rdi, -8(%rbp)
	movq	%rdi, sys_tss+4
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	SetKernelStack, .Lfunc_end2-SetKernelStack
	.cfi_endproc

	.globl	SetUserStack
	.align	16, 0x90
	.type	SetUserStack,@function
SetUserStack:                           # @SetUserStack
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp11:
	.cfi_def_cfa_offset 16
.Ltmp12:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp13:
	.cfi_def_cfa_register %rbp
	pushq	%rax
	movq	%rdi, -8(%rbp)
	movq	%rdi, sys_tss+20
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	SetUserStack, .Lfunc_end3-SetUserStack
	.cfi_endproc

	.type	sys_tss,@object         # @sys_tss
	.comm	sys_tss,104,1
	.type	gdt_table,@object       # @gdt_table
	.comm	gdt_table,10,1
	.type	gdt_entries,@object     # @gdt_entries
	.comm	gdt_entries,48,16

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
