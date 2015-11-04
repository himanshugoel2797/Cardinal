	.text
	.file	"hal/memory.c"
	.globl	GetVirtualAddress
	.align	16, 0x90
	.type	GetVirtualAddress,@function
GetVirtualAddress:                      # @GetVirtualAddress
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
	pushq	%rax
	movq	%rdi, -8(%rbp)
	movabsq	$-8589934592, %rdi      # imm = 0xFFFFFFFE00000000
	addq	-8(%rbp), %rdi
	movq	%rdi, %rax
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	GetVirtualAddress, .Lfunc_end0-GetVirtualAddress
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
