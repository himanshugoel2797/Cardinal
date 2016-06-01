	.text
	.file	"pit/pit.c"
	.globl	PIT_SetFrequency
	.align	16, 0x90
	.type	PIT_SetFrequency,@function
PIT_SetFrequency:                       # @PIT_SetFrequency
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
	movb	%cl, %al
	movb	%dl, %r9b
	movb	%sil, %r10b
	movb	%dil, %r11b
	movl	$67, %ecx
	movb	%r11b, -1(%rbp)
	movb	%r10b, -2(%rbp)
	movb	%r9b, -3(%rbp)
	movb	%al, -4(%rbp)
	movl	%r8d, -8(%rbp)
	xorl	%edx, %edx
	movl	$1193180, %esi          # imm = 0x1234DC
	movl	%esi, %eax
	movl	%edx, -20(%rbp)         # 4-byte Spill
	divl	%r8d
	movl	%eax, -12(%rbp)
	movl	%eax, -24(%rbp)         # 4-byte Spill
	movl	%esi, %eax
	movl	-20(%rbp), %edx         # 4-byte Reload
	movl	-24(%rbp), %esi         # 4-byte Reload
	divl	%esi
	movl	%eax, curFrequency
	movzbl	-1(%rbp), %eax
	andl	$3, %eax
	movb	%al, %r9b
	movb	%r9b, -1(%rbp)
	movzbl	-2(%rbp), %eax
	andl	$3, %eax
	movb	%al, %r9b
	movb	%r9b, -2(%rbp)
	movzbl	-3(%rbp), %eax
	andl	$7, %eax
	movb	%al, %r9b
	movb	%r9b, -3(%rbp)
	movzbl	-4(%rbp), %eax
	andl	$1, %eax
	movb	%al, %r9b
	movb	%r9b, -4(%rbp)
	movb	$0, -13(%rbp)
	movzbl	-1(%rbp), %eax
	shll	$6, %eax
	movzbl	-2(%rbp), %edx
	shll	$4, %edx
	orl	%edx, %eax
	movzbl	-3(%rbp), %edx
	shll	$1, %edx
	orl	%edx, %eax
	movzbl	-4(%rbp), %edx
	orl	%edx, %eax
	movb	%al, %r9b
	movb	%r9b, -13(%rbp)
	movb	-13(%rbp), %r9b
	movzbl	%r9b, %esi
	movl	$67, %edi
	movl	%ecx, -28(%rbp)         # 4-byte Spill
	callq	outb
	movzbl	-2(%rbp), %eax
	andl	$1, %eax
	cmpl	$1, %eax
	jne	.LBB0_2
# BB#1:
	movl	-12(%rbp), %eax
	andl	$255, %eax
	movb	%al, %cl
	movb	%cl, -14(%rbp)
	movzbl	-1(%rbp), %eax
	addl	$64, %eax
	movw	%ax, %dx
	movb	-14(%rbp), %cl
	movzwl	%dx, %edi
	movzbl	%cl, %esi
	callq	outb
.LBB0_2:
	movzbl	-2(%rbp), %eax
	andl	$2, %eax
	cmpl	$2, %eax
	jne	.LBB0_4
# BB#3:
	movl	-12(%rbp), %eax
	shrl	$8, %eax
	andl	$255, %eax
	movb	%al, %cl
	movb	%cl, -15(%rbp)
	movzbl	-1(%rbp), %eax
	addl	$64, %eax
	movw	%ax, %dx
	movb	-15(%rbp), %cl
	movzwl	%dx, %edi
	movzbl	%cl, %esi
	callq	outb
.LBB0_4:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	PIT_SetFrequency, .Lfunc_end0-PIT_SetFrequency
	.cfi_endproc

	.globl	PIT_Sleep
	.align	16, 0x90
	.type	PIT_Sleep,@function
PIT_Sleep:                              # @PIT_Sleep
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
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	movl	$1, pit_handlerMode
	xorl	%edi, %edi
	callq	PIT_SetEnableMode
	movq	$0, -16(%rbp)
	movl	$32, %edi
	leaq	-16(%rbp), %rsi
	movl	%edi, -28(%rbp)         # 4-byte Spill
	callq	GetInterruptHandler
	movq	$PIT_TempIntHandler, %rsi
	movl	-28(%rbp), %edi         # 4-byte Reload
	movl	%eax, -32(%rbp)         # 4-byte Spill
	callq	RegisterInterruptHandler
	movl	-4(%rbp), %edi
	movl	curFrequency, %ecx
	imull	%ecx, %edi
	movl	%edi, %esi
	imulq	$274877907, %rsi, %rsi  # imm = 0x10624DD3
	shrq	$38, %rsi
	movl	%esi, %ecx
	movl	%ecx, pit_ticksToWait
	movl	$1, %edi
	movl	%eax, -36(%rbp)         # 4-byte Spill
	callq	PIT_SetEnableMode
	movl	$0, pit_handlerMode
	#APP
	sti
	#NO_APP
	movb	$1, -17(%rbp)
.LBB1_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB1_5 Depth 2
	cmpb	$0, -17(%rbp)
	je	.LBB1_9
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	movl	$1, pit_handlerMode
	movl	pit_ticksToWait, %eax
	cmpl	$0, %eax
	jne	.LBB1_4
# BB#3:                                 #   in Loop: Header=BB1_1 Depth=1
	movb	$0, -17(%rbp)
.LBB1_4:                                #   in Loop: Header=BB1_1 Depth=1
	movl	$0, pit_handlerMode
	movl	$0, -24(%rbp)
.LBB1_5:                                #   Parent Loop BB1_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	cmpl	$50, -24(%rbp)
	jge	.LBB1_8
# BB#6:                                 #   in Loop: Header=BB1_5 Depth=2
	#APP
	nop
	#NO_APP
	#APP
	nop
	#NO_APP
	#APP
	nop
	#NO_APP
	#APP
	nop
	#NO_APP
	#APP
	nop
	#NO_APP
# BB#7:                                 #   in Loop: Header=BB1_5 Depth=2
	movl	-24(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -24(%rbp)
	jmp	.LBB1_5
.LBB1_8:                                #   in Loop: Header=BB1_1 Depth=1
	jmp	.LBB1_1
.LBB1_9:
	movl	$1, pit_handlerMode
	xorl	%edi, %edi
	callq	PIT_SetEnableMode
	movq	-16(%rbp), %rsi
	movl	$32, %edi
	callq	RegisterInterruptHandler
	movl	$1, stop
	movl	%eax, -40(%rbp)         # 4-byte Spill
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	PIT_Sleep, .Lfunc_end1-PIT_Sleep
	.cfi_endproc

	.globl	PIT_SetEnableMode
	.align	16, 0x90
	.type	PIT_SetEnableMode,@function
PIT_SetEnableMode:                      # @PIT_SetEnableMode
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
	movb	%dil, %al
	movl	$32, %edi
	movb	%al, -1(%rbp)
	movb	-1(%rbp), %al
	movzbl	%al, %esi
	movl	$32, %ecx
	movl	%edi, -8(%rbp)          # 4-byte Spill
	movl	%ecx, %edi
	callq	SetInterruptEnableMode
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	PIT_SetEnableMode, .Lfunc_end2-PIT_SetEnableMode
	.cfi_endproc

	.align	16, 0x90
	.type	PIT_TempIntHandler,@function
PIT_TempIntHandler:                     # @PIT_TempIntHandler
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
	pushq	%rax
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$32, -4(%rbp)
	jne	.LBB3_6
# BB#1:
	movl	pit_handlerMode, %eax
	cmpl	$0, %eax
	jne	.LBB3_5
# BB#2:
	movl	pit_ticksToWait, %eax
	cmpl	$0, %eax
	je	.LBB3_4
# BB#3:
	movl	pit_ticksToWait, %eax
	decl	%eax
	movl	%eax, pit_ticksToWait
.LBB3_4:
	jmp	.LBB3_5
.LBB3_5:
	jmp	.LBB3_6
.LBB3_6:
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	PIT_TempIntHandler, .Lfunc_end3-PIT_TempIntHandler
	.cfi_endproc

	.globl	PIT_Initialize
	.align	16, 0x90
	.type	PIT_Initialize,@function
PIT_Initialize:                         # @PIT_Initialize
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
	subq	$32, %rsp
	xorl	%eax, %eax
	movl	$3, %ecx
	movl	$2, %edx
	movl	$19886, %esi            # imm = 0x4DAE
	movl	$3, %edi
	movl	$2, %r8d
	xorl	%r9d, %r9d
	movl	$19886, %r10d           # imm = 0x4DAE
	movl	%edi, -4(%rbp)          # 4-byte Spill
	movl	%r9d, %edi
	movl	-4(%rbp), %r11d         # 4-byte Reload
	movl	%esi, -8(%rbp)          # 4-byte Spill
	movl	%r11d, %esi
	movl	%edx, -12(%rbp)         # 4-byte Spill
	movl	%r8d, %edx
	movl	%ecx, -16(%rbp)         # 4-byte Spill
	movl	%r9d, %ecx
	movl	%r10d, %r8d
	movl	%eax, -20(%rbp)         # 4-byte Spill
	callq	PIT_SetFrequency
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	PIT_Initialize, .Lfunc_end4-PIT_Initialize
	.cfi_endproc

	.type	curFrequency,@object    # @curFrequency
	.bss
	.globl	curFrequency
	.align	4
curFrequency:
	.long	0                       # 0x0
	.size	curFrequency, 4

	.type	pit_ticksToWait,@object # @pit_ticksToWait
	.globl	pit_ticksToWait
	.align	4
pit_ticksToWait:
	.long	0                       # 0x0
	.size	pit_ticksToWait, 4

	.type	pit_handlerMode,@object # @pit_handlerMode
	.globl	pit_handlerMode
	.align	4
pit_handlerMode:
	.long	0                       # 0x0
	.size	pit_handlerMode, 4

	.type	stop,@object            # @stop
	.globl	stop
	.align	4
stop:
	.long	0                       # 0x0
	.size	stop, 4


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
