	.text
	.file	"fpu.c"
	.globl	FPU_Initialize
	.align	16, 0x90
	.type	FPU_Initialize,@function
FPU_Initialize:                         # @FPU_Initialize
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
	movl	$1, %eax
	xorl	%ecx, %ecx
	movl	$1, %edi
	xorl	%esi, %esi
	movl	%eax, -8(%rbp)          # 4-byte Spill
	movl	%ecx, -12(%rbp)         # 4-byte Spill
	callq	CPUID_RequestInfo
	movl	$3, %eax
	movl	$1, %ecx
	movl	$3, %edi
	movl	$1, %esi
	movl	%eax, -16(%rbp)         # 4-byte Spill
	movl	%ecx, -20(%rbp)         # 4-byte Spill
	callq	CPUID_FeatureIsAvailable
	cmpb	$0, %al
	jne	.LBB0_2
# BB#1:
	movl	$255, %eax
	movl	$255, %edi
	movl	%eax, -24(%rbp)         # 4-byte Spill
	callq	bootstrap_kernel_panic
.LBB0_2:
	movl	$3, %eax
	movl	$33554432, %ecx         # imm = 0x2000000
	movl	$3, %edi
	movl	$33554432, %esi         # imm = 0x2000000
	movl	%eax, -28(%rbp)         # 4-byte Spill
	movl	%ecx, -32(%rbp)         # 4-byte Spill
	callq	CPUID_FeatureIsAvailable
	cmpb	$0, %al
	jne	.LBB0_4
# BB#3:
	movl	$255, %eax
	movl	$255, %edi
	movl	%eax, -36(%rbp)         # 4-byte Spill
	callq	bootstrap_kernel_panic
.LBB0_4:
	movl	$3, %eax
	movl	$16777216, %ecx         # imm = 0x1000000
	movl	$3, %edi
	movl	$16777216, %esi         # imm = 0x1000000
	movl	%eax, -40(%rbp)         # 4-byte Spill
	movl	%ecx, -44(%rbp)         # 4-byte Spill
	callq	CPUID_FeatureIsAvailable
	cmpb	$0, %al
	jne	.LBB0_6
# BB#5:
	movl	$255, %eax
	movl	$255, %edi
	movl	%eax, -48(%rbp)         # 4-byte Spill
	callq	bootstrap_kernel_panic
.LBB0_6:
	movl	$0, -4(%rbp)
	#APP
	movq	%cr0, %rax
	#NO_APP
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	$-5, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	orl	$32, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	orl	$2, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	#APP
	movq	%rax, %cr0
	#NO_APP
	#APP
	movq	%cr4, %rax
	#NO_APP
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	orl	$512, %eax              # imm = 0x200
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	orl	$1024, %eax             # imm = 0x400
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	orl	$262144, %eax           # imm = 0x40000
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	#APP
	movq	%rax, %cr4
	#NO_APP
	#APP
	fninit
	#NO_APP
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	FPU_Initialize, .Lfunc_end0-FPU_Initialize
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
