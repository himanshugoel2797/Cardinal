	.text
	.file	"fpu/fpu.c"
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
	subq	$80, %rsp
	movl	$1, %eax
	xorl	%ecx, %ecx
	movl	$1, %edi
	xorl	%esi, %esi
	movl	%eax, -12(%rbp)         # 4-byte Spill
	movl	%ecx, -16(%rbp)         # 4-byte Spill
	callq	CPUID_RequestInfo
	movl	$3, %eax
	movl	$1, %ecx
	movl	$3, %edi
	movl	$1, %esi
	movl	%eax, -20(%rbp)         # 4-byte Spill
	movl	%ecx, -24(%rbp)         # 4-byte Spill
	callq	CPUID_FeatureIsAvailable
	cmpb	$0, %al
	jne	.LBB0_2
# BB#1:
	movl	$255, %eax
	movl	$255, %edi
	movl	%eax, -28(%rbp)         # 4-byte Spill
	callq	bootstrap_kernel_panic
.LBB0_2:
	movl	$3, %eax
	movl	$33554432, %ecx         # imm = 0x2000000
	movl	$3, %edi
	movl	$33554432, %esi         # imm = 0x2000000
	movl	%eax, -32(%rbp)         # 4-byte Spill
	movl	%ecx, -36(%rbp)         # 4-byte Spill
	callq	CPUID_FeatureIsAvailable
	cmpb	$0, %al
	jne	.LBB0_4
# BB#3:
	movl	$255, %eax
	movl	$255, %edi
	movl	%eax, -40(%rbp)         # 4-byte Spill
	callq	bootstrap_kernel_panic
.LBB0_4:
	movl	$3, %eax
	movl	$16777216, %ecx         # imm = 0x1000000
	movl	$3, %edi
	movl	$16777216, %esi         # imm = 0x1000000
	movl	%eax, -44(%rbp)         # 4-byte Spill
	movl	%ecx, -48(%rbp)         # 4-byte Spill
	callq	CPUID_FeatureIsAvailable
	cmpb	$0, %al
	jne	.LBB0_6
# BB#5:
	movl	$255, %eax
	movl	$255, %edi
	movl	%eax, -52(%rbp)         # 4-byte Spill
	callq	bootstrap_kernel_panic
.LBB0_6:
	movq	$0, -8(%rbp)
	#APP
	movq	%cr0, %rax
	#NO_APP
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	andq	$-5, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	orq	$32, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	orq	$2, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	#APP
	movq	%rax, %cr0
	#NO_APP
	#APP
	movq	%cr4, %rax
	#NO_APP
	movl	$16, %ecx
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	orq	$512, %rax              # imm = 0x200
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	orq	$1024, %rax             # imm = 0x400
	movq	%rax, -8(%rbp)
	movq	$FPU_X87Exception, %rsi
	movl	$16, %edi
	movl	%ecx, -56(%rbp)         # 4-byte Spill
	callq	RegisterInterruptHandler
	movl	$19, %ecx
	movq	$FPU_SIMDException, %rsi
	movl	$19, %edi
	movl	%eax, -60(%rbp)         # 4-byte Spill
	movl	%ecx, -64(%rbp)         # 4-byte Spill
	callq	RegisterInterruptHandler
	movq	-8(%rbp), %rsi
	#APP
	movq	%rsi, %cr4
	#NO_APP
	#APP
	fninit
	#NO_APP
	movl	%eax, -68(%rbp)         # 4-byte Spill
	addq	$80, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	FPU_Initialize, .Lfunc_end0-FPU_Initialize
	.cfi_endproc

	.align	16, 0x90
	.type	FPU_X87Exception,@function
FPU_X87Exception:                       # @FPU_X87Exception
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
	pushq	%rax
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$16, -4(%rbp)
	je	.LBB1_2
# BB#1:
	jmp	.LBB1_3
.LBB1_2:
	movl	$0, -8(%rbp)
	movl	$721077, %eax           # imm = 0xB00B5
	movl	%eax, %ecx
	#APP
	cli
	movq	%rcx, %rax
	hlt
	#NO_APP
.LBB1_3:
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	FPU_X87Exception, .Lfunc_end1-FPU_X87Exception
	.cfi_endproc

	.align	16, 0x90
	.type	FPU_SIMDException,@function
FPU_SIMDException:                      # @FPU_SIMDException
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
	pushq	%rax
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$19, -4(%rbp)
	je	.LBB2_2
# BB#1:
	jmp	.LBB2_3
.LBB2_2:
	movl	$0, -8(%rbp)
	movl	$184594917, %eax        # imm = 0xB00B1E5
	movl	%eax, %ecx
	#APP
	cli
	movq	%rcx, %rax
	hlt
	#NO_APP
.LBB2_3:
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	FPU_SIMDException, .Lfunc_end2-FPU_SIMDException
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
