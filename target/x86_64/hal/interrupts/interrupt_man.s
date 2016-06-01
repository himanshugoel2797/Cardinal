	.text
	.file	"hal/interrupts/interrupt_man.c"
	.globl	RequestInterruptVectorBlock
	.align	16, 0x90
	.type	RequestInterruptVectorBlock,@function
RequestInterruptVectorBlock:            # @RequestInterruptVectorBlock
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
	movl	%edi, -8(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -16(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$256, -16(%rbp)         # imm = 0x100
	jge	.LBB0_11
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	cmpl	$0, -16(%rbp)
	jle	.LBB0_5
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-16(%rbp), %eax
	decl	%eax
	movslq	%eax, %rcx
	leaq	intHandlers(,%rcx,8), %rcx
	cmpq	$0, (%rcx)
	je	.LBB0_5
# BB#4:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	$0, -12(%rbp)
.LBB0_5:                                #   in Loop: Header=BB0_1 Depth=1
	movslq	-16(%rbp), %rax
	leaq	intHandlers(,%rax,8), %rax
	cmpq	$0, (%rax)
	jne	.LBB0_7
# BB#6:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
.LBB0_7:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-12(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jne	.LBB0_9
# BB#8:
	jmp	.LBB0_11
.LBB0_9:                                #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_10
.LBB0_10:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	jmp	.LBB0_1
.LBB0_11:
	movl	-16(%rbp), %eax
	addl	-8(%rbp), %eax
	cmpl	$256, %eax              # imm = 0x100
	jb	.LBB0_13
# BB#12:
	movl	$-1, -4(%rbp)
	jmp	.LBB0_14
.LBB0_13:
	movl	-16(%rbp), %eax
	movl	%eax, -4(%rbp)
.LBB0_14:
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	RequestInterruptVectorBlock, .Lfunc_end0-RequestInterruptVectorBlock
	.cfi_endproc

	.globl	ShadowInterruptHandler
	.align	16, 0x90
	.type	ShadowInterruptHandler,@function
ShadowInterruptHandler:                 # @ShadowInterruptHandler
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
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	120(%rdi), %rdi
	leaq	intHandlers(,%rdi,8), %rdi
	cmpq	$0, (%rdi)
	je	.LBB1_2
# BB#1:
	movq	-8(%rbp), %rax
	movq	120(%rax), %rax
	leaq	intHandlers(,%rax,8), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	120(%rcx), %rcx
	movl	%ecx, %edx
	movq	-8(%rbp), %rcx
	movq	128(%rcx), %rcx
	movl	%ecx, %esi
	movl	%edx, %edi
	callq	*%rax
.LBB1_2:
	movq	-8(%rbp), %rax
	cmpq	$31, 120(%rax)
	jbe	.LBB1_4
# BB#3:
	movq	-8(%rbp), %rax
	movq	120(%rax), %rax
	movb	%al, %cl
	movzbl	%cl, %edi
	callq	APIC_SendEOI
.LBB1_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	ShadowInterruptHandler, .Lfunc_end1-ShadowInterruptHandler
	.cfi_endproc

	.globl	HandleInterruptNoReturn
	.align	16, 0x90
	.type	HandleInterruptNoReturn,@function
HandleInterruptNoReturn:                # @HandleInterruptNoReturn
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
	movl	%edi, -4(%rbp)
	cmpl	$31, -4(%rbp)
	jbe	.LBB2_2
# BB#1:
	movl	-4(%rbp), %eax
	movb	%al, %cl
	movzbl	%cl, %edi
	callq	APIC_SendEOI
.LBB2_2:
	#APP
	sti
	#NO_APP
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	HandleInterruptNoReturn, .Lfunc_end2-HandleInterruptNoReturn
	.cfi_endproc

	.globl	RegisterInterruptHandler
	.align	16, 0x90
	.type	RegisterInterruptHandler,@function
RegisterInterruptHandler:               # @RegisterInterruptHandler
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
	movl	%edi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	cmpl	$256, -8(%rbp)          # imm = 0x100
	jae	.LBB3_2
# BB#1:
	cmpl	$31, -8(%rbp)
	ja	.LBB3_3
.LBB3_2:
	movl	$-1, -4(%rbp)
	jmp	.LBB3_4
.LBB3_3:
	movq	-16(%rbp), %rax
	movl	-8(%rbp), %ecx
	movl	%ecx, %edx
	leaq	intHandlers(,%rdx,8), %rdx
	movq	%rax, (%rdx)
	movl	-8(%rbp), %ecx
	movb	%cl, %sil
	movzbl	%sil, %edi
	movq	$ShadowInterruptHandler, %rsi
	callq	IDT_RegisterHandler
	movl	$0, -4(%rbp)
.LBB3_4:
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	RegisterInterruptHandler, .Lfunc_end3-RegisterInterruptHandler
	.cfi_endproc

	.globl	GetInterruptHandler
	.align	16, 0x90
	.type	GetInterruptHandler,@function
GetInterruptHandler:                    # @GetInterruptHandler
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
	movl	%edi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	cmpl	$256, -8(%rbp)          # imm = 0x100
	jb	.LBB4_2
# BB#1:
	movl	$-1, -4(%rbp)
	jmp	.LBB4_5
.LBB4_2:
	cmpq	$0, -16(%rbp)
	je	.LBB4_4
# BB#3:
	movl	-8(%rbp), %eax
	movl	%eax, %ecx
	leaq	intHandlers(,%rcx,8), %rcx
	movq	(%rcx), %rcx
	movq	-16(%rbp), %rdx
	movq	%rcx, (%rdx)
.LBB4_4:
	movl	$0, -4(%rbp)
.LBB4_5:
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	GetInterruptHandler, .Lfunc_end4-GetInterruptHandler
	.cfi_endproc

	.globl	RemoveInterruptHandler
	.align	16, 0x90
	.type	RemoveInterruptHandler,@function
RemoveInterruptHandler:                 # @RemoveInterruptHandler
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp15:
	.cfi_def_cfa_offset 16
.Ltmp16:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp17:
	.cfi_def_cfa_register %rbp
	subq	$4, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$256, -4(%rbp)          # imm = 0x100
	jae	.LBB5_3
# BB#1:
	cmpl	$31, -4(%rbp)
	jbe	.LBB5_3
# BB#2:
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	leaq	intHandlers(,%rcx,8), %rcx
	movq	$0, (%rcx)
.LBB5_3:
	addq	$4, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	RemoveInterruptHandler, .Lfunc_end5-RemoveInterruptHandler
	.cfi_endproc

	.globl	SetInterruptEnableMode
	.align	16, 0x90
	.type	SetInterruptEnableMode,@function
SetInterruptEnableMode:                 # @SetInterruptEnableMode
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp18:
	.cfi_def_cfa_offset 16
.Ltmp19:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp20:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%sil, %al
	movl	%edi, -4(%rbp)
	movb	%al, -5(%rbp)
	movl	-4(%rbp), %esi
	movb	%sil, %al
	movb	-5(%rbp), %cl
	movzbl	%al, %edi
	movzbl	%cl, %esi
	callq	IOAPIC_SetEnableMode
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	SetInterruptEnableMode, .Lfunc_end6-SetInterruptEnableMode
	.cfi_endproc

	.type	intHandlers,@object     # @intHandlers
	.local	intHandlers
	.comm	intHandlers,2048,16

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
