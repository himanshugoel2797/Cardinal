	.text
	.file	"managers/bootstrap_mem_pool/bootstrap_mem_pool.c"
	.globl	bootstrap_malloc
	.align	16, 0x90
	.type	bootstrap_malloc,@function
bootstrap_malloc:                       # @bootstrap_malloc
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
	subq	$24, %rsp
	movq	%rdi, -16(%rbp)
	movl	%edi, %eax
	andl	$3, %eax
	movl	%eax, %ecx
	addq	%rcx, %rdi
	movq	%rdi, -16(%rbp)
	movl	pos, %eax
	movl	%eax, %eax
	movl	%eax, %ecx
	addq	-16(%rbp), %rcx
	cmpq	$16777216, %rcx         # imm = 0x1000000
	jbe	.LBB0_2
# BB#1:
	movq	$0, -8(%rbp)
	jmp	.LBB0_3
.LBB0_2:
	movl	pos, %eax
	movl	%eax, %ecx
	leaq	mem_pool(%rcx), %rcx
	movq	%rcx, -24(%rbp)
	movl	-16(%rbp), %eax
	movl	pos, %edx
	addl	%eax, %edx
	movl	%edx, pos
	movq	-24(%rbp), %rcx
	movq	%rcx, -8(%rbp)
.LBB0_3:
	movq	-8(%rbp), %rax
	addq	$24, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	bootstrap_malloc, .Lfunc_end0-bootstrap_malloc
	.cfi_endproc

	.type	pos,@object             # @pos
	.local	pos
	.comm	pos,4,4
	.type	mem_pool,@object        # @mem_pool
	.local	mem_pool
	.comm	mem_pool,16777216,16

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
