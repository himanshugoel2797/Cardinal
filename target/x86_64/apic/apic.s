	.text
	.file	"apic/apic.c"
	.globl	APIC_LockPIC
	.align	16, 0x90
	.type	APIC_LockPIC,@function
APIC_LockPIC:                           # @APIC_LockPIC
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
	subq	$112, %rsp
	movl	$33, %eax
	movl	$33, %edi
	movl	%eax, -8(%rbp)          # 4-byte Spill
	callq	inb
	movl	$161, %edi
	movb	%al, -1(%rbp)
	movl	$161, %ecx
	movl	%edi, -12(%rbp)         # 4-byte Spill
	movl	%ecx, %edi
	callq	inb
	movl	$32, %ecx
	movl	$17, %edi
	movb	%al, -2(%rbp)
	movl	$32, %edx
	movl	$17, %esi
	movl	%edi, -16(%rbp)         # 4-byte Spill
	movl	%edx, %edi
	movl	%ecx, -20(%rbp)         # 4-byte Spill
	callq	outb
	movl	$160, %ecx
	movl	$17, %edx
	movl	$160, %edi
	movl	$17, %esi
	movl	%ecx, -24(%rbp)         # 4-byte Spill
	movl	%edx, -28(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movl	$32, %edx
	movl	$33, %edi
	movl	$32, %esi
	movl	%ecx, -32(%rbp)         # 4-byte Spill
	movl	%edx, -36(%rbp)         # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movl	$40, %edx
	movl	$161, %edi
	movl	$40, %esi
	movl	%ecx, -40(%rbp)         # 4-byte Spill
	movl	%edx, -44(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movl	$4, %edx
	movl	$33, %edi
	movl	$4, %esi
	movl	%ecx, -48(%rbp)         # 4-byte Spill
	movl	%edx, -52(%rbp)         # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movl	$2, %edx
	movl	$161, %edi
	movl	$2, %esi
	movl	%ecx, -56(%rbp)         # 4-byte Spill
	movl	%edx, -60(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movl	$1, %edx
	movl	$33, %edi
	movl	$1, %esi
	movl	%ecx, -64(%rbp)         # 4-byte Spill
	movl	%edx, -68(%rbp)         # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movl	$1, %edx
	movl	$161, %edi
	movl	$1, %esi
	movl	%ecx, -72(%rbp)         # 4-byte Spill
	movl	%edx, -76(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movb	-1(%rbp), %al
	movzbl	%al, %esi
	movl	$33, %edi
	movl	%ecx, -80(%rbp)         # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movb	-2(%rbp), %al
	movzbl	%al, %esi
	movl	$161, %edi
	movl	%ecx, -84(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movl	$255, %edx
	movl	$33, %edi
	movl	$255, %esi
	movl	%ecx, -88(%rbp)         # 4-byte Spill
	movl	%edx, -92(%rbp)         # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movl	$255, %edx
	movl	$161, %edi
	movl	$255, %esi
	movl	%ecx, -96(%rbp)         # 4-byte Spill
	movl	%edx, -100(%rbp)        # 4-byte Spill
	callq	outb
	addq	$112, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	APIC_LockPIC, .Lfunc_end0-APIC_LockPIC
	.cfi_endproc

	.globl	APIC_LocalInitialize
	.align	16, 0x90
	.type	APIC_LocalInitialize,@function
APIC_LocalInitialize:                   # @APIC_LocalInitialize
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
	subq	$80, %rsp
	movl	$32, -4(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$256, -4(%rbp)          # imm = 0x100
	jge	.LBB1_4
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	leaq	-16(%rbp), %rax
	movq	$0, -16(%rbp)
	movl	-4(%rbp), %edi
	leaq	-16(%rbp), %rsi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	GetInterruptHandler
	movl	-4(%rbp), %edi
	movq	-16(%rbp), %rsi
	movl	%eax, -44(%rbp)         # 4-byte Spill
	callq	RegisterInterruptHandler
	movl	%eax, -48(%rbp)         # 4-byte Spill
# BB#3:                                 #   in Loop: Header=BB1_1 Depth=1
	movl	-4(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.LBB1_1
.LBB1_4:
	movl	$27, %edi
	callq	rdmsr
	movq	%rax, -24(%rbp)
	movq	apic_data, %rax
	cmpq	$0, %rax
	jne	.LBB1_6
# BB#5:
	movl	$16, %eax
	movl	%eax, %edi
	callq	AllocateAPLSMemory
	movq	%rax, apic_data
.LBB1_6:
	movl	$27, %eax
	movq	-24(%rbp), %rcx
	movl	%ecx, %edx
	andl	$-4096, %edx            # imm = 0xFFFFFFFFFFFFF000
	movl	%edx, %esi
	movl	$1, %edi
	movl	%eax, -52(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	apic_data, %rcx
	movq	%rax, (%rcx)
	movq	-24(%rbp), %rax
	orq	$2048, %rax             # imm = 0x800
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rsi
	movl	$27, %edi
	callq	wrmsr
	movl	$128, %edx
	movl	$128, %edi
	movl	%edx, -56(%rbp)         # 4-byte Spill
	callq	APIC_Read
	movl	$128, %edx
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	andl	$-256, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %esi
	movl	$128, %edi
	movl	%edx, -60(%rbp)         # 4-byte Spill
	callq	APIC_Write
	movl	$240, %eax
	movl	$240, %edi
	movl	%eax, -64(%rbp)         # 4-byte Spill
	callq	APIC_Read
	movl	$240, %edx
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	orl	$255, %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %esi
	movl	$240, %edi
	movl	%edx, -68(%rbp)         # 4-byte Spill
	callq	APIC_Write
	movl	$1, %eax
	movl	$1, %edi
	movl	%eax, -72(%rbp)         # 4-byte Spill
	callq	APIC_SetEnableMode
	xorl	%eax, %eax
	movb	%al, %r8b
	movzbl	%r8b, %eax
	addq	$80, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	APIC_LocalInitialize, .Lfunc_end1-APIC_LocalInitialize
	.cfi_endproc

	.globl	APIC_Read
	.align	16, 0x90
	.type	APIC_Read,@function
APIC_Read:                              # @APIC_Read
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
	subq	$4, %rsp
	movl	%edi, -4(%rbp)
	shrl	$2, %edi
	movl	%edi, %eax
	movq	apic_data, %rcx
	movq	(%rcx), %rcx
	movl	(%rcx,%rax,4), %eax
	addq	$4, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	APIC_Read, .Lfunc_end2-APIC_Read
	.cfi_endproc

	.globl	APIC_Write
	.align	16, 0x90
	.type	APIC_Write,@function
APIC_Write:                             # @APIC_Write
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
	movl	-4(%rbp), %edi
	shrl	$2, %edi
	movl	%edi, %eax
	movq	apic_data, %rcx
	movq	(%rcx), %rcx
	movl	%esi, (%rcx,%rax,4)
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	APIC_Write, .Lfunc_end3-APIC_Write
	.cfi_endproc

	.globl	APIC_SetEnableMode
	.align	16, 0x90
	.type	APIC_SetEnableMode,@function
APIC_SetEnableMode:                     # @APIC_SetEnableMode
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
	movb	%dil, %al
	movl	$240, %edi
	movb	%al, -1(%rbp)
	movl	$240, %ecx
	movl	%edi, -12(%rbp)         # 4-byte Spill
	movl	%ecx, %edi
	callq	APIC_Read
	movl	$240, %ecx
	xorl	%edi, %edi
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	movzbl	-1(%rbp), %edx
	andl	$1, %edx
	subl	%edx, %edi
	xorl	-8(%rbp), %edi
	andl	$256, %edi              # imm = 0x100
	xorl	%edi, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %esi
	movl	$240, %edi
	movl	%ecx, -16(%rbp)         # 4-byte Spill
	callq	APIC_Write
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	APIC_SetEnableMode, .Lfunc_end4-APIC_SetEnableMode
	.cfi_endproc

	.globl	APIC_CallibrateTimer
	.align	16, 0x90
	.type	APIC_CallibrateTimer,@function
APIC_CallibrateTimer:                   # @APIC_CallibrateTimer
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
	subq	$32, %rsp
	movl	$0, rollover_cnt
	movl	$-1, pit_ticks
	movl	$0, apic_timer_value
	movq	$APIC_TimerCallibrateRollover, %rsi
	movl	$33, %eax
	movl	%eax, %edi
	movl	%eax, -12(%rbp)         # 4-byte Spill
	callq	RegisterInterruptHandler
	movl	$800, %edi              # imm = 0x320
	movl	%edi, -16(%rbp)         # 4-byte Spill
	movl	-12(%rbp), %esi         # 4-byte Reload
	movl	%eax, -20(%rbp)         # 4-byte Spill
	callq	APIC_SetVector
	xorl	%edi, %edi
	callq	APIC_SetTimerValue
	movl	$16, %edi
	callq	APIC_SetTimerDivisor
	movq	$APIC_TimerCallibrate, %rsi
	movl	$32, %edi
	callq	RegisterInterruptHandler
	movl	$-1, %edi
	movl	%eax, -24(%rbp)         # 4-byte Spill
	callq	APIC_SetTimerValue
	movl	$1, %eax
	movl	%eax, %edi
	movl	%eax, -28(%rbp)         # 4-byte Spill
	callq	APIC_SetTimerMode
	movl	-28(%rbp), %edi         # 4-byte Reload
	callq	PIT_SetEnableMode
	movl	-16(%rbp), %edi         # 4-byte Reload
	movl	-28(%rbp), %esi         # 4-byte Reload
	callq	APIC_SetEnableInterrupt
	movl	$0, pit_ticks
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	movl	pit_ticks, %eax
	cmpl	$198, %eax
	jge	.LBB5_3
# BB#2:                                 #   in Loop: Header=BB5_1 Depth=1
	jmp	.LBB5_1
.LBB5_3:
	xorl	%eax, %eax
	movl	%eax, %edi
	movl	%eax, -32(%rbp)         # 4-byte Spill
	callq	PIT_SetEnableMode
	movl	$800, %edi              # imm = 0x320
	movl	-32(%rbp), %esi         # 4-byte Reload
	callq	APIC_SetEnableInterrupt
	movl	rollover_cnt, %eax
	movl	apic_timer_value, %esi
	notl	%esi
	subl	%eax, %esi
	movl	%esi, %ecx
	movq	%rcx, -8(%rbp)
	imulq	$19886, %rcx, %rcx      # imm = 0x4DAE
	movq	%rcx, -8(%rbp)
	shrq	%rcx
	movabsq	$-6521576187675094005, %rdx # imm = 0xA57EB50295FAD40B
	movq	%rcx, %rax
	mulq	%rdx
	shrq	$6, %rdx
	movq	%rdx, -8(%rbp)
	movq	apic_data, %rax
	movq	%rdx, 8(%rax)
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	APIC_CallibrateTimer, .Lfunc_end5-APIC_CallibrateTimer
	.cfi_endproc

	.align	16, 0x90
	.type	APIC_TimerCallibrateRollover,@function
APIC_TimerCallibrateRollover:           # @APIC_TimerCallibrateRollover
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
	pushq	%rax
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$33, -4(%rbp)
	jne	.LBB6_2
# BB#1:
	movl	rollover_cnt, %eax
	incl	%eax
	movl	%eax, rollover_cnt
.LBB6_2:
	cmpl	$0, -8(%rbp)
	je	.LBB6_4
# BB#3:
	jmp	.LBB6_4
.LBB6_4:
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	APIC_TimerCallibrateRollover, .Lfunc_end6-APIC_TimerCallibrateRollover
	.cfi_endproc

	.globl	APIC_SetVector
	.align	16, 0x90
	.type	APIC_SetVector,@function
APIC_SetVector:                         # @APIC_SetVector
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp21:
	.cfi_def_cfa_offset 16
.Ltmp22:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp23:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%sil, %al
	movl	%edi, -4(%rbp)
	movb	%al, -5(%rbp)
	cmpl	$800, -4(%rbp)          # imm = 0x320
	jb	.LBB7_2
# BB#1:
	cmpl	$880, -4(%rbp)          # imm = 0x370
	jbe	.LBB7_3
.LBB7_2:
	jmp	.LBB7_4
.LBB7_3:
	movl	-4(%rbp), %edi
	callq	APIC_Read
	movl	%eax, -12(%rbp)
	movzbl	-5(%rbp), %eax
	orl	-12(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %edi
	movl	-12(%rbp), %esi
	callq	APIC_Write
.LBB7_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end7:
	.size	APIC_SetVector, .Lfunc_end7-APIC_SetVector
	.cfi_endproc

	.globl	APIC_SetTimerValue
	.align	16, 0x90
	.type	APIC_SetTimerValue,@function
APIC_SetTimerValue:                     # @APIC_SetTimerValue
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp24:
	.cfi_def_cfa_offset 16
.Ltmp25:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp26:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$896, %eax              # imm = 0x380
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %esi
	movl	$896, %edi              # imm = 0x380
	movl	%eax, -8(%rbp)          # 4-byte Spill
	callq	APIC_Write
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end8:
	.size	APIC_SetTimerValue, .Lfunc_end8-APIC_SetTimerValue
	.cfi_endproc

	.globl	APIC_SetTimerDivisor
	.align	16, 0x90
	.type	APIC_SetTimerDivisor,@function
APIC_SetTimerDivisor:                   # @APIC_SetTimerDivisor
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp27:
	.cfi_def_cfa_offset 16
.Ltmp28:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp29:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%dil, %al
	movb	%al, -1(%rbp)
	movb	$0, -2(%rbp)
	movzbl	-1(%rbp), %edi
	cmpl	$16, %edi
	jle	.LBB9_2
# BB#1:
	movl	$16, %eax
	movzbl	-2(%rbp), %ecx
	orl	$8, %ecx
	movb	%cl, %dl
	movb	%dl, -2(%rbp)
	movzbl	-1(%rbp), %ecx
	movl	%eax, -8(%rbp)          # 4-byte Spill
	movl	%ecx, %eax
	cltd
	movl	-8(%rbp), %ecx          # 4-byte Reload
	idivl	%ecx
	movb	%al, %sil
	movb	%sil, -1(%rbp)
.LBB9_2:
	movzbl	-1(%rbp), %eax
	cmpl	$16, %eax
	jne	.LBB9_4
# BB#3:
	movzbl	-2(%rbp), %eax
	orl	$3, %eax
	movb	%al, %cl
	movb	%cl, -2(%rbp)
	jmp	.LBB9_8
.LBB9_4:
	movzbl	-1(%rbp), %eax
	cmpl	$1, %eax
	jne	.LBB9_6
# BB#5:
	movb	$11, -2(%rbp)
	jmp	.LBB9_7
.LBB9_6:
	movzbl	-1(%rbp), %eax
	sarl	$2, %eax
	andl	$3, %eax
	movzbl	-2(%rbp), %ecx
	orl	%eax, %ecx
	movb	%cl, %dl
	movb	%dl, -2(%rbp)
.LBB9_7:
	jmp	.LBB9_8
.LBB9_8:
	movl	$992, %eax              # imm = 0x3E0
	movzbl	-2(%rbp), %esi
	movl	$992, %edi              # imm = 0x3E0
	movl	%eax, -12(%rbp)         # 4-byte Spill
	callq	APIC_Write
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end9:
	.size	APIC_SetTimerDivisor, .Lfunc_end9-APIC_SetTimerDivisor
	.cfi_endproc

	.align	16, 0x90
	.type	APIC_TimerCallibrate,@function
APIC_TimerCallibrate:                   # @APIC_TimerCallibrate
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp30:
	.cfi_def_cfa_offset 16
.Ltmp31:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp32:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$32, -4(%rbp)
	jne	.LBB10_5
# BB#1:
	movl	pit_ticks, %eax
	cmpl	$0, %eax
	jl	.LBB10_5
# BB#2:
	movl	pit_ticks, %eax
	incl	%eax
	movl	%eax, pit_ticks
	cmpl	$198, %eax
	jne	.LBB10_4
# BB#3:
	callq	APIC_GetTimerValue
	movl	%eax, apic_timer_value
.LBB10_4:
	jmp	.LBB10_5
.LBB10_5:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end10:
	.size	APIC_TimerCallibrate, .Lfunc_end10-APIC_TimerCallibrate
	.cfi_endproc

	.globl	APIC_SetTimerMode
	.align	16, 0x90
	.type	APIC_SetTimerMode,@function
APIC_SetTimerMode:                      # @APIC_SetTimerMode
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp33:
	.cfi_def_cfa_offset 16
.Ltmp34:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp35:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%dil, %al
	movl	$800, %edi              # imm = 0x320
	movb	%al, -1(%rbp)
	movl	$800, %ecx              # imm = 0x320
	movl	%edi, -12(%rbp)         # 4-byte Spill
	movl	%ecx, %edi
	callq	APIC_Read
	movl	$800, %ecx              # imm = 0x320
	movl	%eax, -8(%rbp)
	movzbl	-1(%rbp), %eax
	andl	$3, %eax
	shll	$17, %eax
	orl	-8(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %esi
	movl	$800, %edi              # imm = 0x320
	movl	%ecx, -16(%rbp)         # 4-byte Spill
	callq	APIC_Write
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end11:
	.size	APIC_SetTimerMode, .Lfunc_end11-APIC_SetTimerMode
	.cfi_endproc

	.globl	APIC_SetEnableInterrupt
	.align	16, 0x90
	.type	APIC_SetEnableInterrupt,@function
APIC_SetEnableInterrupt:                # @APIC_SetEnableInterrupt
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp36:
	.cfi_def_cfa_offset 16
.Ltmp37:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp38:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$800, -4(%rbp)          # imm = 0x320
	jb	.LBB12_2
# BB#1:
	cmpl	$880, -4(%rbp)          # imm = 0x370
	jbe	.LBB12_3
.LBB12_2:
	jmp	.LBB12_4
.LBB12_3:
	movl	-4(%rbp), %edi
	callq	APIC_Read
	xorl	%edi, %edi
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	-8(%rbp), %ecx
	xorl	$-1, %ecx
	andl	$1, %ecx
	subl	%ecx, %edi
	xorl	-12(%rbp), %edi
	andl	$65536, %edi            # imm = 0x10000
	xorl	%edi, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %edi
	movl	-12(%rbp), %esi
	callq	APIC_Write
.LBB12_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end12:
	.size	APIC_SetEnableInterrupt, .Lfunc_end12-APIC_SetEnableInterrupt
	.cfi_endproc

	.globl	APIC_GetTimerFrequency
	.align	16, 0x90
	.type	APIC_GetTimerFrequency,@function
APIC_GetTimerFrequency:                 # @APIC_GetTimerFrequency
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp39:
	.cfi_def_cfa_offset 16
.Ltmp40:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp41:
	.cfi_def_cfa_register %rbp
	movq	apic_data, %rax
	movq	8(%rax), %rax
	popq	%rbp
	retq
.Lfunc_end13:
	.size	APIC_GetTimerFrequency, .Lfunc_end13-APIC_GetTimerFrequency
	.cfi_endproc

	.globl	APIC_SetDeliveryMode
	.align	16, 0x90
	.type	APIC_SetDeliveryMode,@function
APIC_SetDeliveryMode:                   # @APIC_SetDeliveryMode
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp42:
	.cfi_def_cfa_offset 16
.Ltmp43:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp44:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%sil, %al
	movl	%edi, -4(%rbp)
	movb	%al, -5(%rbp)
	cmpl	$800, -4(%rbp)          # imm = 0x320
	jb	.LBB14_2
# BB#1:
	cmpl	$864, -4(%rbp)          # imm = 0x360
	jbe	.LBB14_3
.LBB14_2:
	jmp	.LBB14_4
.LBB14_3:
	movl	-4(%rbp), %edi
	callq	APIC_Read
	movl	%eax, -12(%rbp)
	movzbl	-5(%rbp), %eax
	andl	$7, %eax
	shll	$8, %eax
	orl	-12(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %edi
	movl	-12(%rbp), %esi
	callq	APIC_Write
.LBB14_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end14:
	.size	APIC_SetDeliveryMode, .Lfunc_end14-APIC_SetDeliveryMode
	.cfi_endproc

	.globl	APIC_SetTriggerMode
	.align	16, 0x90
	.type	APIC_SetTriggerMode,@function
APIC_SetTriggerMode:                    # @APIC_SetTriggerMode
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp45:
	.cfi_def_cfa_offset 16
.Ltmp46:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp47:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%sil, %al
	movl	%edi, -4(%rbp)
	movb	%al, -5(%rbp)
	cmpl	$848, -4(%rbp)          # imm = 0x350
	jb	.LBB15_2
# BB#1:
	cmpl	$864, -4(%rbp)          # imm = 0x360
	jbe	.LBB15_3
.LBB15_2:
	jmp	.LBB15_4
.LBB15_3:
	movl	-4(%rbp), %edi
	callq	APIC_Read
	movl	%eax, -12(%rbp)
	movzbl	-5(%rbp), %eax
	andl	$1, %eax
	shll	$15, %eax
	orl	-12(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %edi
	movl	-12(%rbp), %esi
	callq	APIC_Write
.LBB15_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end15:
	.size	APIC_SetTriggerMode, .Lfunc_end15-APIC_SetTriggerMode
	.cfi_endproc

	.globl	APIC_SetPolarity
	.align	16, 0x90
	.type	APIC_SetPolarity,@function
APIC_SetPolarity:                       # @APIC_SetPolarity
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp48:
	.cfi_def_cfa_offset 16
.Ltmp49:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp50:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%sil, %al
	movl	%edi, -4(%rbp)
	movb	%al, -5(%rbp)
	cmpl	$848, -4(%rbp)          # imm = 0x350
	jb	.LBB16_2
# BB#1:
	cmpl	$864, -4(%rbp)          # imm = 0x360
	jbe	.LBB16_3
.LBB16_2:
	jmp	.LBB16_4
.LBB16_3:
	movl	-4(%rbp), %edi
	callq	APIC_Read
	movl	%eax, -12(%rbp)
	movzbl	-5(%rbp), %eax
	andl	$1, %eax
	shll	$13, %eax
	orl	-12(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %edi
	movl	-12(%rbp), %esi
	callq	APIC_Write
.LBB16_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end16:
	.size	APIC_SetPolarity, .Lfunc_end16-APIC_SetPolarity
	.cfi_endproc

	.globl	APIC_GetTimerValue
	.align	16, 0x90
	.type	APIC_GetTimerValue,@function
APIC_GetTimerValue:                     # @APIC_GetTimerValue
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp51:
	.cfi_def_cfa_offset 16
.Ltmp52:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp53:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$912, %eax              # imm = 0x390
	movl	$912, %edi              # imm = 0x390
	movl	%eax, -4(%rbp)          # 4-byte Spill
	callq	APIC_Read
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end17:
	.size	APIC_GetTimerValue, .Lfunc_end17-APIC_GetTimerValue
	.cfi_endproc

	.globl	APIC_GetID
	.align	16, 0x90
	.type	APIC_GetID,@function
APIC_GetID:                             # @APIC_GetID
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp54:
	.cfi_def_cfa_offset 16
.Ltmp55:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp56:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	$32, %eax
	movl	$32, %edi
	movl	%eax, -4(%rbp)          # 4-byte Spill
	callq	APIC_Read
	movb	%al, %cl
	movzbl	%cl, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end18:
	.size	APIC_GetID, .Lfunc_end18-APIC_GetID
	.cfi_endproc

	.globl	APIC_SendEOI
	.align	16, 0x90
	.type	APIC_SendEOI,@function
APIC_SendEOI:                           # @APIC_SendEOI
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp57:
	.cfi_def_cfa_offset 16
.Ltmp58:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp59:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movb	%dil, %al
	movl	$32, %edi
	movb	%al, -1(%rbp)
	movzbl	-1(%rbp), %eax
	cltd
	idivl	%edi
	shll	$4, %eax
	addl	$256, %eax              # imm = 0x100
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %edi
	callq	APIC_Read
	movl	$32, %edi
	movl	$1, %ecx
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movzbl	-1(%rbp), %esi
	movl	%eax, -16(%rbp)         # 4-byte Spill
	movl	%esi, %eax
	cltd
	idivl	%edi
	movl	%ecx, -20(%rbp)         # 4-byte Spill
	movl	%edx, %ecx
                                        # kill: CL<def> ECX<kill>
	movl	-20(%rbp), %edx         # 4-byte Reload
	shll	%cl, %edx
	movl	-16(%rbp), %esi         # 4-byte Reload
	andl	%edx, %esi
	movzbl	-1(%rbp), %edx
	movl	%edx, %eax
	cltd
	idivl	%edi
	movl	%edx, %ecx
                                        # kill: CL<def> ECX<kill>
	shrl	%cl, %esi
	cmpl	$0, %esi
	je	.LBB19_2
# BB#1:
	movl	$176, %eax
	xorl	%ecx, %ecx
	movl	$176, %edi
	xorl	%esi, %esi
	movl	%eax, -24(%rbp)         # 4-byte Spill
	movl	%ecx, -28(%rbp)         # 4-byte Spill
	callq	APIC_Write
.LBB19_2:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end19:
	.size	APIC_SendEOI, .Lfunc_end19-APIC_SendEOI
	.cfi_endproc

	.globl	APIC_SendIPI
	.align	16, 0x90
	.type	APIC_SendIPI,@function
APIC_SendIPI:                           # @APIC_SendIPI
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp60:
	.cfi_def_cfa_offset 16
.Ltmp61:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp62:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movb	%dl, %al
	movb	%sil, %cl
	movl	$784, %edx              # imm = 0x310
	movl	%edi, -4(%rbp)
	movb	%cl, -5(%rbp)
	movb	%al, -6(%rbp)
	movl	-4(%rbp), %esi
	shll	$24, %esi
	movl	%esi, -12(%rbp)
	movzbl	-6(%rbp), %esi
	andl	$7, %esi
	shll	$8, %esi
	orl	$16384, %esi            # imm = 0x4000
	movzbl	-5(%rbp), %edi
	orl	%edi, %esi
	movl	%esi, -16(%rbp)
	movl	-12(%rbp), %esi
	movl	$784, %edi              # imm = 0x310
	movl	%edx, -20(%rbp)         # 4-byte Spill
	callq	APIC_Write
	movl	$768, %edx              # imm = 0x300
	movl	-16(%rbp), %esi
	movl	$768, %edi              # imm = 0x300
	movl	%edx, -24(%rbp)         # 4-byte Spill
	callq	APIC_Write
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end20:
	.size	APIC_SendIPI, .Lfunc_end20-APIC_SendIPI
	.cfi_endproc

	.type	apic_data,@object       # @apic_data
	.local	apic_data
	.comm	apic_data,8,8
	.type	rollover_cnt,@object    # @rollover_cnt
	.local	rollover_cnt
	.comm	rollover_cnt,4,4
	.type	pit_ticks,@object       # @pit_ticks
	.local	pit_ticks
	.comm	pit_ticks,4,4
	.type	apic_timer_value,@object # @apic_timer_value
	.local	apic_timer_value
	.comm	apic_timer_value,4,4

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
