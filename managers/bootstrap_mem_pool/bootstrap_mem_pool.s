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
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdi
	andq	$3, %rdi
	cmpq	$0, %rdi
	je	.LBB0_2
# BB#1:
	movq	-16(%rbp), %rax
	shrq	$2, %rax
	addq	$1, %rax
	shlq	$2, %rax
	movq	%rax, -16(%rbp)
.LBB0_2:
	movl	pos, %eax
	movl	%eax, %eax
	movl	%eax, %ecx
	addq	-16(%rbp), %rcx
	cmpq	$8388608, %rcx          # imm = 0x800000
	jbe	.LBB0_4
# BB#3:
	movq	$0, -8(%rbp)
	jmp	.LBB0_5
.LBB0_4:
	xorl	%eax, %eax
	movl	pos, %ecx
	movl	%ecx, %edx
	leaq	mem_pool(%rdx), %rdx
	movq	%rdx, -24(%rbp)
	movl	-16(%rbp), %ecx
	movl	pos, %esi
	addl	%ecx, %esi
	movl	%esi, pos
	movq	-24(%rbp), %rdi
	movq	-16(%rbp), %rdx
	xorl	%esi, %esi
	movl	%eax, -28(%rbp)         # 4-byte Spill
	callq	memset
	movq	-24(%rbp), %rdx
	movq	%rdx, -8(%rbp)
	movq	%rax, -40(%rbp)         # 8-byte Spill
.LBB0_5:
	movq	-8(%rbp), %rax
	addq	$48, %rsp
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
	.comm	mem_pool,8388608,16

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
