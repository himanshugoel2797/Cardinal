	.text
	.file	"smp/smp.c"
	.globl	SMP_IncrementCoreCount
	.align	16, 0x90
	.type	SMP_IncrementCoreCount,@function
SMP_IncrementCoreCount:                 # @SMP_IncrementCoreCount
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
	movl	smp_coreCount, %eax
	incl	%eax
	movl	%eax, smp_coreCount
	popq	%rbp
	retq
.Lfunc_end0:
	.size	SMP_IncrementCoreCount, .Lfunc_end0-SMP_IncrementCoreCount
	.cfi_endproc

	.globl	SMP_GetCoreCount
	.align	16, 0x90
	.type	SMP_GetCoreCount,@function
SMP_GetCoreCount:                       # @SMP_GetCoreCount
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
	movl	smp_coreCount, %eax
	popq	%rbp
	retq
.Lfunc_end1:
	.size	SMP_GetCoreCount, .Lfunc_end1-SMP_GetCoreCount
	.cfi_endproc

	.globl	SMP_WaitForCoreCountIncrement
	.align	16, 0x90
	.type	SMP_WaitForCoreCountIncrement,@function
SMP_WaitForCoreCountIncrement:          # @SMP_WaitForCoreCountIncrement
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
	callq	SMP_GetCoreCount
	movl	%eax, -4(%rbp)
.LBB2_1:                                # =>This Inner Loop Header: Depth=1
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)          # 4-byte Spill
	callq	SMP_GetCoreCount
	movl	-8(%rbp), %ecx          # 4-byte Reload
	cmpl	%eax, %ecx
	jne	.LBB2_3
# BB#2:                                 #   in Loop: Header=BB2_1 Depth=1
	jmp	.LBB2_1
.LBB2_3:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	SMP_WaitForCoreCountIncrement, .Lfunc_end2-SMP_WaitForCoreCountIncrement
	.cfi_endproc

	.globl	SMP_LockTrampoline
	.align	16, 0x90
	.type	SMP_LockTrampoline,@function
SMP_LockTrampoline:                     # @SMP_LockTrampoline
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
	movq	$smp_lock, %rax
	#APP
.spin:
	pause
	testl	$1, (%rax)
	jne	.spin
	lock
	btsl	$0, (%rax)
	jb	.spin
	#NO_APP
	popq	%rbp
	retq
.Lfunc_end3:
	.size	SMP_LockTrampoline, .Lfunc_end3-SMP_LockTrampoline
	.cfi_endproc

	.globl	SMP_UnlockTrampoline
	.align	16, 0x90
	.type	SMP_UnlockTrampoline,@function
SMP_UnlockTrampoline:                   # @SMP_UnlockTrampoline
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
	movq	$smp_lock, %rax
	#APP
	movl	$0, (%rax)
	#NO_APP
	popq	%rbp
	retq
.Lfunc_end4:
	.size	SMP_UnlockTrampoline, .Lfunc_end4-SMP_UnlockTrampoline
	.cfi_endproc

	.type	smp_coreCount,@object   # @smp_coreCount
	.local	smp_coreCount
	.comm	smp_coreCount,4,4
	.type	smp_lock,@object        # @smp_lock
	.local	smp_lock
	.comm	smp_lock,4,4

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
