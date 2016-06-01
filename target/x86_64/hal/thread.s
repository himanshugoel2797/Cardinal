	.text
	.file	"hal/thread.c"
	.globl	SwitchAndInitializeThread
	.align	16, 0x90
	.type	SwitchAndInitializeThread,@function
SwitchAndInitializeThread:              # @SwitchAndInitializeThread
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
	pushq	%rbx
	pushq	%rax
.Ltmp3:
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdi
	movq	24(%rdi), %rax
	movq	-16(%rbp), %rdi
	movq	40(%rdi), %rbx
	#APP
	movq	%rbx, %rsp
	popq	%rbx
	popq	%rbx
	popq	%rbx
	popq	%rbx
	pushq	%rax
	retq
	#NO_APP
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	retq
.Lfunc_end0:
	.size	SwitchAndInitializeThread, .Lfunc_end0-SwitchAndInitializeThread
	.cfi_endproc

	.globl	SwapThreadOnInterrupt
	.align	16, 0x90
	.type	SwapThreadOnInterrupt,@function
SwapThreadOnInterrupt:                  # @SwapThreadOnInterrupt
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp4:
	.cfi_def_cfa_offset 16
.Ltmp5:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp6:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	subq	$24, %rsp
.Ltmp7:
	.cfi_offset %rbx, -24
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	$0, -32(%rbp)
	movq	-24(%rbp), %rsi
	movq	40(%rsi), %rbx
	#APP
	movq	%rsp, %rax
	movq	%rax, %rsp

	#NO_APP
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	-16(%rbp), %rsi
	movq	%rax, 40(%rsi)
	addq	$24, %rsp
	popq	%rbx
	popq	%rbp
	retq
.Lfunc_end1:
	.size	SwapThreadOnInterrupt, .Lfunc_end1-SwapThreadOnInterrupt
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
