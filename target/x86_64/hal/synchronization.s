	.text
	.file	"hal/synchronization.c"
	.globl	CreateSpinlock
	.align	16, 0x90
	.type	CreateSpinlock,@function
CreateSpinlock:                         # @CreateSpinlock
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
	subq	$32, %rsp
	callq	CPUID_GetCacheLineSize
	movl	%eax, %eax
	movl	%eax, %edi
	callq	kmalloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)         # 8-byte Spill
	callq	CPUID_GetCacheLineSize
	xorl	%ecx, %ecx
	movl	%eax, %eax
	movl	%eax, %edx
	xorl	%esi, %esi
	movq	-16(%rbp), %rdi         # 8-byte Reload
	movl	%ecx, -20(%rbp)         # 4-byte Spill
	callq	memset
	movq	-8(%rbp), %rdx
	movq	%rax, -32(%rbp)         # 8-byte Spill
	movq	%rdx, %rax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	CreateSpinlock, .Lfunc_end0-CreateSpinlock
	.cfi_endproc

	.globl	CreateBootstrapSpinlock
	.align	16, 0x90
	.type	CreateBootstrapSpinlock,@function
CreateBootstrapSpinlock:                # @CreateBootstrapSpinlock
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp3:
	.cfi_def_cfa_offset 16
.Ltmp4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp5:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	callq	CPUID_GetCacheLineSize
	movl	%eax, %eax
	movl	%eax, %edi
	callq	bootstrap_malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -16(%rbp)         # 8-byte Spill
	callq	CPUID_GetCacheLineSize
	xorl	%ecx, %ecx
	movl	%eax, %eax
	movl	%eax, %edx
	xorl	%esi, %esi
	movq	-16(%rbp), %rdi         # 8-byte Reload
	movl	%ecx, -20(%rbp)         # 4-byte Spill
	callq	memset
	movq	-8(%rbp), %rdx
	movq	%rax, -32(%rbp)         # 8-byte Spill
	movq	%rdx, %rax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	CreateBootstrapSpinlock, .Lfunc_end1-CreateBootstrapSpinlock
	.cfi_endproc

	.globl	LockSpinlock
	.align	16, 0x90
	.type	LockSpinlock,@function
LockSpinlock:                           # @LockSpinlock
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp6:
	.cfi_def_cfa_offset 16
.Ltmp7:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp8:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.LBB2_2
# BB#1:
	movb	$0, -1(%rbp)
	jmp	.LBB2_3
.LBB2_2:
	movq	-16(%rbp), %rax
	#APP
.spin:
	pause
	testl	$1, (%rax)
	jne	.spin
	lock
	btsl	$0, (%rax)
	jb	.spin
	#NO_APP
	movb	$1, -1(%rbp)
.LBB2_3:
	movzbl	-1(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	LockSpinlock, .Lfunc_end2-LockSpinlock
	.cfi_endproc

	.globl	UnlockSpinlock
	.align	16, 0x90
	.type	UnlockSpinlock,@function
UnlockSpinlock:                         # @UnlockSpinlock
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp9:
	.cfi_def_cfa_offset 16
.Ltmp10:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp11:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.LBB3_2
# BB#1:
	movb	$0, -1(%rbp)
	jmp	.LBB3_3
.LBB3_2:
	movq	-16(%rbp), %rax
	#APP
	movl	$0, (%rax)
	#NO_APP
	movb	$1, -1(%rbp)
.LBB3_3:
	movzbl	-1(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	UnlockSpinlock, .Lfunc_end3-UnlockSpinlock
	.cfi_endproc

	.globl	FreeSpinlock
	.align	16, 0x90
	.type	FreeSpinlock,@function
FreeSpinlock:                           # @FreeSpinlock
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp12:
	.cfi_def_cfa_offset 16
.Ltmp13:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp14:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	cmpq	$0, -8(%rbp)
	je	.LBB4_2
# BB#1:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	kfree
.LBB4_2:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	FreeSpinlock, .Lfunc_end4-FreeSpinlock
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
