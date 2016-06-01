	.text
	.file	"hal/syscall.c"
	.globl	Syscall_Handler
	.align	16, 0x90
	.type	Syscall_Handler,@function
Syscall_Handler:                        # @Syscall_Handler
	.cfi_startproc
# BB#0:
	#APP
	movq	%rsp, %rax
	movq	$k_stack, %rsp
	pushq	%rax
	pushq	%rcx
	callq	SyscallReceived
	popq	%rcx
	popq	%rax
	movq	%rax, %rsp
	sysretl

	#NO_APP
.Lfunc_end0:
	.size	Syscall_Handler, .Lfunc_end0-Syscall_Handler
	.cfi_endproc

	.globl	Syscall_Initialize
	.align	16, 0x90
	.type	Syscall_Initialize,@function
Syscall_Initialize:                     # @Syscall_Initialize
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
	subq	$64, %rsp
	movq	$0, -8(%rbp)
	movq	$Syscall_Handler, -16(%rbp)
	movq	$0, -24(%rbp)
	movq	$0, -32(%rbp)
	movq	$k_stack, %rdi
	movl	$16384, %eax            # imm = 0x4000
	movl	%eax, %edx
	xorl	%esi, %esi
	callq	memset
	movl	$3221225600, %esi       # imm = 0xC0000080
	movl	$-1073741696, %edi      # imm = 0xFFFFFFFFC0000080
	movq	%rax, -40(%rbp)         # 8-byte Spill
	movl	%esi, -44(%rbp)         # 4-byte Spill
	callq	rdmsr
	movl	$3221225600, %esi       # imm = 0xC0000080
	orq	$1, %rax
	movl	$-1073741696, %edi      # imm = 0xFFFFFFFFC0000080
	movl	%esi, -48(%rbp)         # 4-byte Spill
	movq	%rax, %rsi
	callq	wrmsr
	movl	$3221225601, %edi       # imm = 0xC0000081
	movq	-8(%rbp), %rsi
	movl	$-1073741695, %ecx      # imm = 0xFFFFFFFFC0000081
	movl	%edi, -52(%rbp)         # 4-byte Spill
	movl	%ecx, %edi
	callq	wrmsr
	movl	$3221225602, %ecx       # imm = 0xC0000082
	movq	-16(%rbp), %rsi
	movl	$-1073741694, %edi      # imm = 0xFFFFFFFFC0000082
	movl	%ecx, -56(%rbp)         # 4-byte Spill
	callq	wrmsr
	movl	$3221225603, %ecx       # imm = 0xC0000083
	movq	-24(%rbp), %rsi
	movl	$-1073741693, %edi      # imm = 0xFFFFFFFFC0000083
	movl	%ecx, -60(%rbp)         # 4-byte Spill
	callq	wrmsr
	movl	$3221225604, %ecx       # imm = 0xC0000084
	movq	-32(%rbp), %rsi
	movl	$-1073741692, %edi      # imm = 0xFFFFFFFFC0000084
	movl	%ecx, -64(%rbp)         # 4-byte Spill
	callq	wrmsr
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	Syscall_Initialize, .Lfunc_end1-Syscall_Initialize
	.cfi_endproc

	.type	k_stack,@object         # @k_stack
	.local	k_stack
	.comm	k_stack,16384,16

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
