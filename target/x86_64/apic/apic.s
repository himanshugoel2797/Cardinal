	.text
	.file	"apic/apic.c"
	.globl	APIC_LocalInitialize
	.align	16, 0x90
	.type	APIC_LocalInitialize,@function
APIC_LocalInitialize:                   # @APIC_LocalInitialize
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
	subq	$144, %rsp
	movl	$33, %eax
	movl	$33, %edi
	movl	%eax, -28(%rbp)         # 4-byte Spill
	callq	inb
	movl	$161, %edi
	movb	%al, -1(%rbp)
	movl	$161, %ecx
	movl	%edi, -32(%rbp)         # 4-byte Spill
	movl	%ecx, %edi
	callq	inb
	movl	$32, %ecx
	movl	$17, %edi
	movb	%al, -2(%rbp)
	movl	$32, %edx
	movl	$17, %esi
	movl	%edi, -36(%rbp)         # 4-byte Spill
	movl	%edx, %edi
	movl	%ecx, -40(%rbp)         # 4-byte Spill
	callq	outb
	movl	$160, %ecx
	movl	$17, %edx
	movl	$160, %edi
	movl	$17, %esi
	movl	%ecx, -44(%rbp)         # 4-byte Spill
	movl	%edx, -48(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movl	$32, %edx
	movl	$33, %edi
	movl	$32, %esi
	movl	%ecx, -52(%rbp)         # 4-byte Spill
	movl	%edx, -56(%rbp)         # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movl	$40, %edx
	movl	$161, %edi
	movl	$40, %esi
	movl	%ecx, -60(%rbp)         # 4-byte Spill
	movl	%edx, -64(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movl	$4, %edx
	movl	$33, %edi
	movl	$4, %esi
	movl	%ecx, -68(%rbp)         # 4-byte Spill
	movl	%edx, -72(%rbp)         # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movl	$2, %edx
	movl	$161, %edi
	movl	$2, %esi
	movl	%ecx, -76(%rbp)         # 4-byte Spill
	movl	%edx, -80(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movl	$1, %edx
	movl	$33, %edi
	movl	$1, %esi
	movl	%ecx, -84(%rbp)         # 4-byte Spill
	movl	%edx, -88(%rbp)         # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movl	$1, %edx
	movl	$161, %edi
	movl	$1, %esi
	movl	%ecx, -92(%rbp)         # 4-byte Spill
	movl	%edx, -96(%rbp)         # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movb	-1(%rbp), %al
	movzbl	%al, %esi
	movl	$33, %edi
	movl	%ecx, -100(%rbp)        # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movb	-2(%rbp), %al
	movzbl	%al, %esi
	movl	$161, %edi
	movl	%ecx, -104(%rbp)        # 4-byte Spill
	callq	outb
	movl	$33, %ecx
	movl	$255, %edx
	movl	$33, %edi
	movl	$255, %esi
	movl	%ecx, -108(%rbp)        # 4-byte Spill
	movl	%edx, -112(%rbp)        # 4-byte Spill
	callq	outb
	movl	$161, %ecx
	movl	$255, %edx
	movl	$161, %edi
	movl	$255, %esi
	movl	%ecx, -116(%rbp)        # 4-byte Spill
	movl	%edx, -120(%rbp)        # 4-byte Spill
	callq	outb
	movl	$32, -8(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$256, -8(%rbp)          # imm = 0x100
	jge	.LBB0_4
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-8(%rbp), %eax
	movb	%al, %cl
	movzbl	%cl, %edi
	movq	$APIC_MainHandler, %rsi
	callq	IDT_RegisterHandler
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-8(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.LBB0_1
.LBB0_4:
	movl	$27, %eax
	movl	$27, %edi
	movl	%eax, -124(%rbp)        # 4-byte Spill
	callq	rdmsr
	movq	%rax, -16(%rbp)
	movl	%eax, %edi
	andl	$-4096, %edi            # imm = 0xFFFFFFFFFFFFF000
                                        # kill: RDI<def> EDI<kill>
	callq	GetVirtualAddress
	movq	%rax, apic_base_addr
	movq	-16(%rbp), %rax
	orq	$2048, %rax             # imm = 0x800
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rsi
	movl	$27, %edi
	callq	wrmsr
	movl	$128, %edi
	movl	$128, %ecx
	movl	%edi, -128(%rbp)        # 4-byte Spill
	movl	%ecx, %edi
	callq	APIC_Read
	movl	$128, %ecx
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	andl	$-256, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %esi
	movl	$128, %edi
	movl	%ecx, -132(%rbp)        # 4-byte Spill
	callq	APIC_Write
	movl	$240, %eax
	movl	$240, %edi
	movl	%eax, -136(%rbp)        # 4-byte Spill
	callq	APIC_Read
	movl	$240, %ecx
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	orl	$255, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %esi
	movl	$240, %edi
	movl	%ecx, -140(%rbp)        # 4-byte Spill
	callq	APIC_Write
	movl	$1, %eax
	movl	$1, %edi
	movl	%eax, -144(%rbp)        # 4-byte Spill
	callq	APIC_SetEnableMode
	xorl	%eax, %eax
	movb	%al, %dl
	movzbl	%dl, %eax
	addq	$144, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	APIC_LocalInitialize, .Lfunc_end0-APIC_LocalInitialize
	.cfi_endproc

	.align	16, 0x90
	.type	APIC_MainHandler,@function
APIC_MainHandler:                       # @APIC_MainHandler
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
	movq	-8(%rbp), %rdi
	movq	128(%rdi), %rdi
	movb	%dil, %al
	movzbl	%al, %edi
	callq	APIC_SendEOI
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	APIC_MainHandler, .Lfunc_end1-APIC_MainHandler
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
	movq	apic_base_addr, %rcx
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
	movq	apic_base_addr, %rcx
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

	.globl	APIC_SetEnableInterrupt
	.align	16, 0x90
	.type	APIC_SetEnableInterrupt,@function
APIC_SetEnableInterrupt:                # @APIC_SetEnableInterrupt
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
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	cmpl	$800, -4(%rbp)          # imm = 0x320
	jb	.LBB5_2
# BB#1:
	cmpl	$880, -4(%rbp)          # imm = 0x370
	jbe	.LBB5_3
.LBB5_2:
	jmp	.LBB5_4
.LBB5_3:
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
.LBB5_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	APIC_SetEnableInterrupt, .Lfunc_end5-APIC_SetEnableInterrupt
	.cfi_endproc

	.globl	APIC_SetVector
	.align	16, 0x90
	.type	APIC_SetVector,@function
APIC_SetVector:                         # @APIC_SetVector
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
	cmpl	$800, -4(%rbp)          # imm = 0x320
	jb	.LBB6_2
# BB#1:
	cmpl	$880, -4(%rbp)          # imm = 0x370
	jbe	.LBB6_3
.LBB6_2:
	jmp	.LBB6_4
.LBB6_3:
	movl	-4(%rbp), %edi
	callq	APIC_Read
	movl	%eax, -12(%rbp)
	movzbl	-5(%rbp), %eax
	orl	-12(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %edi
	movl	-12(%rbp), %esi
	callq	APIC_Write
.LBB6_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	APIC_SetVector, .Lfunc_end6-APIC_SetVector
	.cfi_endproc

	.globl	APIC_SetDeliveryMode
	.align	16, 0x90
	.type	APIC_SetDeliveryMode,@function
APIC_SetDeliveryMode:                   # @APIC_SetDeliveryMode
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
	cmpl	$864, -4(%rbp)          # imm = 0x360
	jbe	.LBB7_3
.LBB7_2:
	jmp	.LBB7_4
.LBB7_3:
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
.LBB7_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end7:
	.size	APIC_SetDeliveryMode, .Lfunc_end7-APIC_SetDeliveryMode
	.cfi_endproc

	.globl	APIC_SetTriggerMode
	.align	16, 0x90
	.type	APIC_SetTriggerMode,@function
APIC_SetTriggerMode:                    # @APIC_SetTriggerMode
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
	movb	%sil, %al
	movl	%edi, -4(%rbp)
	movb	%al, -5(%rbp)
	cmpl	$848, -4(%rbp)          # imm = 0x350
	jb	.LBB8_2
# BB#1:
	cmpl	$864, -4(%rbp)          # imm = 0x360
	jbe	.LBB8_3
.LBB8_2:
	jmp	.LBB8_4
.LBB8_3:
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
.LBB8_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end8:
	.size	APIC_SetTriggerMode, .Lfunc_end8-APIC_SetTriggerMode
	.cfi_endproc

	.globl	APIC_SetPolarity
	.align	16, 0x90
	.type	APIC_SetPolarity,@function
APIC_SetPolarity:                       # @APIC_SetPolarity
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
	movb	%sil, %al
	movl	%edi, -4(%rbp)
	movb	%al, -5(%rbp)
	cmpl	$848, -4(%rbp)          # imm = 0x350
	jb	.LBB9_2
# BB#1:
	cmpl	$864, -4(%rbp)          # imm = 0x360
	jbe	.LBB9_3
.LBB9_2:
	jmp	.LBB9_4
.LBB9_3:
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
.LBB9_4:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end9:
	.size	APIC_SetPolarity, .Lfunc_end9-APIC_SetPolarity
	.cfi_endproc

	.globl	APIC_SetTimerMode
	.align	16, 0x90
	.type	APIC_SetTimerMode,@function
APIC_SetTimerMode:                      # @APIC_SetTimerMode
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
.Lfunc_end10:
	.size	APIC_SetTimerMode, .Lfunc_end10-APIC_SetTimerMode
	.cfi_endproc

	.globl	APIC_SetTimerDivisor
	.align	16, 0x90
	.type	APIC_SetTimerDivisor,@function
APIC_SetTimerDivisor:                   # @APIC_SetTimerDivisor
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
	movb	%al, -1(%rbp)
	movb	$0, -2(%rbp)
	movzbl	-1(%rbp), %edi
	cmpl	$16, %edi
	jle	.LBB11_2
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
.LBB11_2:
	movzbl	-1(%rbp), %eax
	cmpl	$16, %eax
	jne	.LBB11_4
# BB#3:
	movzbl	-2(%rbp), %eax
	orl	$3, %eax
	movb	%al, %cl
	movb	%cl, -2(%rbp)
	jmp	.LBB11_8
.LBB11_4:
	movzbl	-1(%rbp), %eax
	cmpl	$1, %eax
	jne	.LBB11_6
# BB#5:
	movb	$11, -2(%rbp)
	jmp	.LBB11_7
.LBB11_6:
	movzbl	-1(%rbp), %eax
	sarl	$2, %eax
	andl	$3, %eax
	movzbl	-2(%rbp), %ecx
	orl	%eax, %ecx
	movb	%cl, %dl
	movb	%dl, -2(%rbp)
.LBB11_7:
	jmp	.LBB11_8
.LBB11_8:
	movl	$992, %eax              # imm = 0x3E0
	movzbl	-2(%rbp), %esi
	movl	$992, %edi              # imm = 0x3E0
	movl	%eax, -12(%rbp)         # 4-byte Spill
	callq	APIC_Write
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end11:
	.size	APIC_SetTimerDivisor, .Lfunc_end11-APIC_SetTimerDivisor
	.cfi_endproc

	.globl	APIC_SetTimerValue
	.align	16, 0x90
	.type	APIC_SetTimerValue,@function
APIC_SetTimerValue:                     # @APIC_SetTimerValue
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
	movl	$896, %eax              # imm = 0x380
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %esi
	movl	$896, %edi              # imm = 0x380
	movl	%eax, -8(%rbp)          # 4-byte Spill
	callq	APIC_Write
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end12:
	.size	APIC_SetTimerValue, .Lfunc_end12-APIC_SetTimerValue
	.cfi_endproc

	.globl	APIC_GetTimerValue
	.align	16, 0x90
	.type	APIC_GetTimerValue,@function
APIC_GetTimerValue:                     # @APIC_GetTimerValue
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
	subq	$16, %rsp
	movl	$896, %eax              # imm = 0x380
	movl	$896, %edi              # imm = 0x380
	movl	%eax, -4(%rbp)          # 4-byte Spill
	callq	APIC_Read
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end13:
	.size	APIC_GetTimerValue, .Lfunc_end13-APIC_GetTimerValue
	.cfi_endproc

	.globl	APIC_GetID
	.align	16, 0x90
	.type	APIC_GetID,@function
APIC_GetID:                             # @APIC_GetID
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
	movl	$32, %eax
	movl	$32, %edi
	movl	%eax, -4(%rbp)          # 4-byte Spill
	callq	APIC_Read
	movb	%al, %cl
	movzbl	%cl, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end14:
	.size	APIC_GetID, .Lfunc_end14-APIC_GetID
	.cfi_endproc

	.globl	APIC_SendEOI
	.align	16, 0x90
	.type	APIC_SendEOI,@function
APIC_SendEOI:                           # @APIC_SendEOI
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
	je	.LBB15_2
# BB#1:
	movl	$176, %eax
	movl	$3735928559, %ecx       # imm = 0xDEADBEEF
	movl	$176, %edi
	movl	$-559038737, %esi       # imm = 0xFFFFFFFFDEADBEEF
	movl	%eax, -24(%rbp)         # 4-byte Spill
	movl	%ecx, -28(%rbp)         # 4-byte Spill
	callq	APIC_Write
.LBB15_2:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end15:
	.size	APIC_SendEOI, .Lfunc_end15-APIC_SendEOI
	.cfi_endproc

	.globl	APIC_SendIPI
	.align	16, 0x90
	.type	APIC_SendIPI,@function
APIC_SendIPI:                           # @APIC_SendIPI
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
.Lfunc_end16:
	.size	APIC_SendIPI, .Lfunc_end16-APIC_SendIPI
	.cfi_endproc

	.type	apic_base_addr,@object  # @apic_base_addr
	.bss
	.globl	apic_base_addr
	.align	8
apic_base_addr:
	.quad	0
	.size	apic_base_addr, 8


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
