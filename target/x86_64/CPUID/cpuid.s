	.text
	.file	"CPUID/cpuid.c"
	.globl	CPUID_RequestInfo
	.align	16, 0x90
	.type	CPUID_RequestInfo,@function
CPUID_RequestInfo:                      # @CPUID_RequestInfo
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
	movl	%edi, -12(%rbp)
	movl	%esi, -16(%rbp)
	movl	-12(%rbp), %eax
	movl	%esi, %ecx
	#APP
	cpuid

	#NO_APP
	movl	%eax, -12(%rbp)
	movl	%ebx, ebx
	movl	%ecx, -16(%rbp)
	movl	%edx, edx
	addq	$8, %rsp
	popq	%rbx
	popq	%rbp
	retq
.Ltmp4:
	.size	CPUID_RequestInfo, .Ltmp4-CPUID_RequestInfo
	.cfi_endproc

	.globl	CPUID_FeatureIsAvailable
	.align	16, 0x90
	.type	CPUID_FeatureIsAvailable,@function
CPUID_FeatureIsAvailable:               # @CPUID_FeatureIsAvailable
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
	subq	$32, %rsp
	movl	%edi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	-8(%rbp), %esi
	movl	%esi, %eax
	movq	%rax, %rcx
	subq	$3, %rcx
	movq	%rax, -24(%rbp)         # 8-byte Spill
	movq	%rcx, -32(%rbp)         # 8-byte Spill
	ja	.LBB1_5
# BB#7:
	movq	-24(%rbp), %rax         # 8-byte Reload
	movq	.LJTI1_0(,%rax,8), %rcx
	jmpq	*%rcx
.LBB1_1:
	movl	eax, %eax
	andl	-12(%rbp), %eax
	cmpl	-12(%rbp), %eax
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %eax
	movb	%al, %cl
	movb	%cl, -1(%rbp)
	jmp	.LBB1_6
.LBB1_2:
	movl	ebx, %eax
	andl	-12(%rbp), %eax
	cmpl	-12(%rbp), %eax
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %eax
	movb	%al, %cl
	movb	%cl, -1(%rbp)
	jmp	.LBB1_6
.LBB1_3:
	movl	ecx, %eax
	andl	-12(%rbp), %eax
	cmpl	-12(%rbp), %eax
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %eax
	movb	%al, %cl
	movb	%cl, -1(%rbp)
	jmp	.LBB1_6
.LBB1_4:
	movl	edx, %eax
	andl	-12(%rbp), %eax
	cmpl	-12(%rbp), %eax
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %eax
	movb	%al, %cl
	movb	%cl, -1(%rbp)
	jmp	.LBB1_6
.LBB1_5:
	movb	$0, -1(%rbp)
.LBB1_6:
	movzbl	-1(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	retq
.Ltmp8:
	.size	CPUID_FeatureIsAvailable, .Ltmp8-CPUID_FeatureIsAvailable
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI1_0:
	.quad	.LBB1_1
	.quad	.LBB1_2
	.quad	.LBB1_3
	.quad	.LBB1_4

	.text
	.globl	CPUID_GetValue
	.align	16, 0x90
	.type	CPUID_GetValue,@function
CPUID_GetValue:                         # @CPUID_GetValue
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
	subq	$24, %rsp
	movl	%edi, -8(%rbp)
	movl	-8(%rbp), %edi
	movl	%edi, %eax
	movq	%rax, %rcx
	subq	$3, %rcx
	movq	%rax, -16(%rbp)         # 8-byte Spill
	movq	%rcx, -24(%rbp)         # 8-byte Spill
	ja	.LBB2_5
# BB#7:
	movq	-16(%rbp), %rax         # 8-byte Reload
	movq	.LJTI2_0(,%rax,8), %rcx
	jmpq	*%rcx
.LBB2_1:
	movl	eax, %eax
	movl	%eax, -4(%rbp)
	jmp	.LBB2_6
.LBB2_2:
	movl	ebx, %eax
	movl	%eax, -4(%rbp)
	jmp	.LBB2_6
.LBB2_3:
	movl	ecx, %eax
	movl	%eax, -4(%rbp)
	jmp	.LBB2_6
.LBB2_4:
	movl	edx, %eax
	movl	%eax, -4(%rbp)
	jmp	.LBB2_6
.LBB2_5:
	movl	$0, -4(%rbp)
.LBB2_6:
	movl	-4(%rbp), %eax
	addq	$24, %rsp
	popq	%rbp
	retq
.Ltmp12:
	.size	CPUID_GetValue, .Ltmp12-CPUID_GetValue
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI2_0:
	.quad	.LBB2_1
	.quad	.LBB2_2
	.quad	.LBB2_3
	.quad	.LBB2_4

	.type	ebx,@object             # @ebx
	.local	ebx
	.comm	ebx,4,4
	.type	edx,@object             # @edx
	.local	edx
	.comm	edx,4,4
	.type	eax,@object             # @eax
	.local	eax
	.comm	eax,4,4
	.type	ecx,@object             # @ecx
	.local	ecx
	.comm	ecx,4,4

	.ident	"clang version 3.5.0 (tags/RELEASE_350/final)"
	.section	".note.GNU-stack","",@progbits
