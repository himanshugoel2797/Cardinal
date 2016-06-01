	.text
	.file	"drivers/uhci/uhci.c"
	.globl	UHCI_DetectController
	.align	16, 0x90
	.type	UHCI_DetectController,@function
UHCI_DetectController:                  # @UHCI_DetectController
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
	subq	$16, %rsp
	xorl	%eax, %eax
	movb	%al, %cl
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movzbl	(%rdi), %eax
	cmpl	$12, %eax
	movb	%cl, -9(%rbp)           # 1-byte Spill
	jne	.LBB0_3
# BB#1:
	xorl	%eax, %eax
	movb	%al, %cl
	movq	-8(%rbp), %rdx
	movzbl	1(%rdx), %eax
	cmpl	$3, %eax
	movb	%cl, -9(%rbp)           # 1-byte Spill
	jne	.LBB0_3
# BB#2:
	movq	-8(%rbp), %rax
	movzbl	2(%rax), %ecx
	cmpl	$0, %ecx
	sete	%dl
	movb	%dl, -9(%rbp)           # 1-byte Spill
.LBB0_3:
	movb	-9(%rbp), %al           # 1-byte Reload
	andb	$1, %al
	movzbl	%al, %ecx
	movb	%cl, %al
	movzbl	%al, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	UHCI_DetectController, .Lfunc_end0-UHCI_DetectController
	.cfi_endproc

	.globl	UHCI_InitializeController
	.align	16, 0x90
	.type	UHCI_InitializeController,@function
UHCI_InitializeController:              # @UHCI_InitializeController
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
	movq	%rdi, -16(%rbp)
	movq	$0, -24(%rbp)
	movl	$0, -28(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movl	-28(%rbp), %eax
	movq	-16(%rbp), %rcx
	movzbl	20(%rcx), %edx
	cmpl	%edx, %eax
	jge	.LBB1_6
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	movslq	-28(%rbp), %rax
	movq	-16(%rbp), %rcx
	addq	$24, %rcx
	shlq	$4, %rax
	addq	%rax, %rcx
	cmpl	$32, 8(%rcx)
	jne	.LBB1_4
# BB#3:
	movslq	-28(%rbp), %rax
	movq	-16(%rbp), %rcx
	addq	$24, %rcx
	shlq	$4, %rax
	addq	%rax, %rcx
	movq	(%rcx), %rax
	movq	%rax, -24(%rbp)
	movslq	-28(%rbp), %rax
	movq	-16(%rbp), %rcx
	addq	$24, %rcx
	shlq	$4, %rax
	addq	%rax, %rcx
	movzbl	12(%rcx), %edx
	andl	$1, %edx
	movslq	%edx, %rax
	orq	-24(%rbp), %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB1_6
.LBB1_4:                                #   in Loop: Header=BB1_1 Depth=1
	jmp	.LBB1_5
.LBB1_5:                                #   in Loop: Header=BB1_1 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB1_1
.LBB1_6:
	cmpq	$0, -24(%rbp)
	jne	.LBB1_8
# BB#7:
	movb	$0, -1(%rbp)
	jmp	.LBB1_9
.LBB1_8:
	xorl	%eax, %eax
	movq	-24(%rbp), %rsi
	movq	$UHCI_Read, %rdx
	movq	$UHCI_Write, %rcx
	xorl	%edi, %edi
	movl	%eax, -32(%rbp)         # 4-byte Spill
	callq	USB_AddController
	movb	$1, -1(%rbp)
.LBB1_9:
	movzbl	-1(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	UHCI_InitializeController, .Lfunc_end1-UHCI_InitializeController
	.cfi_endproc

	.align	16, 0x90
	.type	UHCI_Read,@function
UHCI_Read:                              # @UHCI_Read
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
	subq	$32, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movq	-16(%rbp), %rdi
	andq	$1, %rdi
	cmpq	$0, %rdi
	je	.LBB2_2
# BB#1:
	movq	-16(%rbp), %rax
	andq	$65534, %rax            # imm = 0xFFFE
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	addq	%rdx, %rax
	movw	%ax, %si
	movzwl	%si, %edi
	callq	inl
	movl	%eax, -4(%rbp)
	jmp	.LBB2_3
.LBB2_2:
	movq	-16(%rbp), %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	addq	%rdx, %rax
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movl	(%rax), %ecx
	movl	%ecx, -4(%rbp)
.LBB2_3:
	movl	-4(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	UHCI_Read, .Lfunc_end2-UHCI_Read
	.cfi_endproc

	.align	16, 0x90
	.type	UHCI_Write,@function
UHCI_Write:                             # @UHCI_Write
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rdi
	andq	$1, %rdi
	cmpq	$0, %rdi
	je	.LBB3_2
# BB#1:
	movq	-8(%rbp), %rax
	andq	$65534, %rax            # imm = 0xFFFE
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movl	%ecx, %edx
	addq	%rdx, %rax
	movw	%ax, %si
	movl	-16(%rbp), %ecx
	movzwl	%si, %edi
	movl	%ecx, %esi
	callq	outl
	jmp	.LBB3_3
.LBB3_2:
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %ecx
	movl	%ecx, %edx
	addq	%rdx, %rax
	movq	%rax, -24(%rbp)
	movl	-16(%rbp), %ecx
	movq	-24(%rbp), %rax
	movl	%ecx, (%rax)
.LBB3_3:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	UHCI_Write, .Lfunc_end3-UHCI_Write
	.cfi_endproc

	.globl	UHCI_ActivationStateChange
	.align	16, 0x90
	.type	UHCI_ActivationStateChange,@function
UHCI_ActivationStateChange:             # @UHCI_ActivationStateChange
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
	subq	$4, %rsp
	movl	%edi, -4(%rbp)
	movl	$1, -4(%rbp)
	addq	$4, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	UHCI_ActivationStateChange, .Lfunc_end4-UHCI_ActivationStateChange
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
