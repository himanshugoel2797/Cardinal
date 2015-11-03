	.text
	.file	"apic/io_apic/io_apic.c"
	.globl	IOAPIC_Initialize
	.align	16, 0x90
	.type	IOAPIC_Initialize,@function
IOAPIC_Initialize:                      # @IOAPIC_Initialize
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
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movl	curIOAPIC_index, %esi
	cmpl	$128, %esi
	jb	.LBB0_2
# BB#1:
	movb	$-1, -1(%rbp)
	jmp	.LBB0_3
.LBB0_2:
	movq	-16(%rbp), %rax
	movl	curIOAPIC_index, %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	movq	%rax, ioapics(,%rdx,8)
	movl	-20(%rbp), %ecx
	movl	curIOAPIC_index, %esi
	movl	%esi, %eax
	leaq	(%rax,%rax,2), %rax
	movl	%ecx, ioapics+8(,%rax,8)
	movq	-16(%rbp), %rdi
	xorl	%esi, %esi
	callq	IOAPIC_Read
	shrl	$24, %eax
	andl	$15, %eax
	movl	curIOAPIC_index, %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	movl	%eax, ioapics+12(,%rdx,8)
	movq	-16(%rbp), %rdi
	movl	$1, %esi
	callq	IOAPIC_Read
	shrl	$16, %eax
	movb	%al, %r8b
	movzbl	%r8b, %eax
	movl	curIOAPIC_index, %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	movl	%eax, ioapics+16(,%rdx,8)
	movl	curIOAPIC_index, %eax
	incl	%eax
	movl	%eax, curIOAPIC_index
	movb	$0, -1(%rbp)
.LBB0_3:
	movzbl	-1(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	retq
.Ltmp3:
	.size	IOAPIC_Initialize, .Ltmp3-IOAPIC_Initialize
	.cfi_endproc

	.globl	IOAPIC_Read
	.align	16, 0x90
	.type	IOAPIC_Read,@function
IOAPIC_Read:                            # @IOAPIC_Read
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
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	-12(%rbp), %esi
	movq	-8(%rbp), %rdi
	movl	%esi, (%rdi)
	movq	-8(%rbp), %rdi
	movl	16(%rdi), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Ltmp7:
	.size	IOAPIC_Read, .Ltmp7-IOAPIC_Read
	.cfi_endproc

	.globl	IOAPIC_Write
	.align	16, 0x90
	.type	IOAPIC_Write,@function
IOAPIC_Write:                           # @IOAPIC_Write
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp8:
	.cfi_def_cfa_offset 16
.Ltmp9:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp10:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	-12(%rbp), %edx
	movq	-8(%rbp), %rdi
	movl	%edx, (%rdi)
	movl	-16(%rbp), %edx
	movq	-8(%rbp), %rdi
	movl	%edx, 16(%rdi)
	addq	$16, %rsp
	popq	%rbp
	retq
.Ltmp11:
	.size	IOAPIC_Write, .Ltmp11-IOAPIC_Write
	.cfi_endproc

	.globl	IOAPIC_GetVector
	.align	16, 0x90
	.type	IOAPIC_GetVector,@function
IOAPIC_GetVector:                       # @IOAPIC_GetVector
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
	subq	$48, %rsp
	movb	%dil, %al
	movb	%al, -2(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$0, -20(%rbp)
	movq	$0, -32(%rbp)
	movl	$0, -36(%rbp)
.LBB3_1:                                # =>This Inner Loop Header: Depth=1
	movl	-36(%rbp), %eax
	movl	curIOAPIC_index, %ecx
	cmpl	%ecx, %eax
	jae	.LBB3_7
# BB#2:                                 #   in Loop: Header=BB3_1 Depth=1
	movzbl	-2(%rbp), %eax
	movl	-36(%rbp), %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	leaq	ioapics(,%rdx,8), %rdx
	cmpl	8(%rdx), %eax
	jb	.LBB3_5
# BB#3:                                 #   in Loop: Header=BB3_1 Depth=1
	movzbl	-2(%rbp), %eax
	movl	-36(%rbp), %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	leaq	ioapics(,%rdx,8), %rsi
	movl	ioapics+8(,%rdx,8), %ecx
	addl	16(%rsi), %ecx
	cmpl	%ecx, %eax
	jae	.LBB3_5
# BB#4:                                 #   in Loop: Header=BB3_1 Depth=1
	movzbl	-2(%rbp), %eax
	movl	-36(%rbp), %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	movl	ioapics+8(,%rdx,8), %ecx
	subl	%ecx, %eax
	movl	%eax, -20(%rbp)
	movl	-36(%rbp), %eax
	movl	%eax, %edx
	leaq	(%rdx,%rdx,2), %rdx
	leaq	ioapics(,%rdx,8), %rdx
	movq	(%rdx), %rdx
	movq	%rdx, -32(%rbp)
.LBB3_5:                                #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_6
.LBB3_6:                                #   in Loop: Header=BB3_1 Depth=1
	movl	-36(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -36(%rbp)
	jmp	.LBB3_1
.LBB3_7:
	cmpq	$0, -32(%rbp)
	jne	.LBB3_9
# BB#8:
	movb	$-1, -1(%rbp)
	jmp	.LBB3_10
.LBB3_9:
	movl	-20(%rbp), %eax
	shll	$1, %eax
	addl	$16, %eax
	movl	%eax, -40(%rbp)
	movq	-32(%rbp), %rdi
	movl	-40(%rbp), %esi
	callq	IOAPIC_Read
	movl	%eax, -44(%rbp)
	movl	-44(%rbp), %eax
	andl	$255, %eax
	movb	%al, %cl
	movq	-16(%rbp), %rdi
	movb	%cl, (%rdi)
	movl	-44(%rbp), %eax
	andl	$255, %eax
	movb	%al, %cl
	movb	%cl, -1(%rbp)
.LBB3_10:
	movzbl	-1(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Ltmp15:
	.size	IOAPIC_GetVector, .Ltmp15-IOAPIC_GetVector
	.cfi_endproc

	.globl	IOAPIC_MapIRQ
	.align	16, 0x90
	.type	IOAPIC_MapIRQ,@function
IOAPIC_MapIRQ:                          # @IOAPIC_MapIRQ
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp16:
	.cfi_def_cfa_offset 16
.Ltmp17:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp18:
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	subq	$64, %rsp
.Ltmp19:
	.cfi_offset %rbx, -32
.Ltmp20:
	.cfi_offset %r14, -24
	movb	%r9b, %al
	movb	%r8b, %r10b
	movb	%cl, %r11b
	movb	%sil, %bl
	movb	%dil, %r14b
	movb	%r14b, -17(%rbp)
	movb	%bl, -18(%rbp)
	movq	%rdx, -32(%rbp)
	movb	%r11b, -33(%rbp)
	movb	%r10b, -34(%rbp)
	movb	%al, -35(%rbp)
	movl	$0, -40(%rbp)
	movq	$0, -48(%rbp)
	movl	$0, -52(%rbp)
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movl	-52(%rbp), %eax
	movl	curIOAPIC_index, %ecx
	cmpl	%ecx, %eax
	jae	.LBB4_7
# BB#2:                                 #   in Loop: Header=BB4_1 Depth=1
	movzbl	-17(%rbp), %eax
	movl	-52(%rbp), %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	leaq	ioapics(,%rdx,8), %rdx
	cmpl	8(%rdx), %eax
	jb	.LBB4_5
# BB#3:                                 #   in Loop: Header=BB4_1 Depth=1
	movzbl	-17(%rbp), %eax
	movl	-52(%rbp), %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	leaq	ioapics(,%rdx,8), %rsi
	movl	ioapics+8(,%rdx,8), %ecx
	addl	16(%rsi), %ecx
	cmpl	%ecx, %eax
	jae	.LBB4_5
# BB#4:                                 #   in Loop: Header=BB4_1 Depth=1
	movzbl	-17(%rbp), %eax
	movl	-52(%rbp), %ecx
	movl	%ecx, %edx
	leaq	(%rdx,%rdx,2), %rdx
	movl	ioapics+8(,%rdx,8), %ecx
	subl	%ecx, %eax
	movl	%eax, -40(%rbp)
	movl	-52(%rbp), %eax
	movl	%eax, %edx
	leaq	(%rdx,%rdx,2), %rdx
	movq	ioapics(,%rdx,8), %rdx
	movq	%rdx, -48(%rbp)
	movl	-52(%rbp), %eax
	movzbl	-18(%rbp), %ecx
	movl	%ecx, %edx
	movl	%eax, ioapic_interruptMap(,%rdx,8)
	movl	-40(%rbp), %eax
	movzbl	-18(%rbp), %ecx
	movl	%ecx, %edx
	leaq	ioapic_interruptMap(,%rdx,8), %rdx
	movl	%eax, 4(%rdx)
.LBB4_5:                                #   in Loop: Header=BB4_1 Depth=1
	jmp	.LBB4_6
.LBB4_6:                                #   in Loop: Header=BB4_1 Depth=1
	movl	-52(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -52(%rbp)
	jmp	.LBB4_1
.LBB4_7:
	cmpq	$0, -48(%rbp)
	jne	.LBB4_9
# BB#8:
	jmp	.LBB4_10
.LBB4_9:
	movl	-40(%rbp), %eax
	shll	$1, %eax
	addl	$16, %eax
	movl	%eax, -56(%rbp)
	movl	-40(%rbp), %eax
	shll	$1, %eax
	addl	$16, %eax
	addl	$1, %eax
	movl	%eax, -60(%rbp)
	movq	-48(%rbp), %rdi
	movl	-60(%rbp), %esi
	callq	IOAPIC_Read
	movl	%eax, -64(%rbp)
	movl	-64(%rbp), %eax
	andl	$16777215, %eax         # imm = 0xFFFFFF
	movl	%eax, -64(%rbp)
	movq	-32(%rbp), %rdi
	shlq	$24, %rdi
	movl	-64(%rbp), %eax
	movl	%eax, %ecx
	orq	%rdi, %rcx
	movl	%ecx, %eax
	movl	%eax, -64(%rbp)
	movq	-48(%rbp), %rdi
	movl	-60(%rbp), %esi
	movl	-64(%rbp), %edx
	callq	IOAPIC_Write
	movq	-48(%rbp), %rdi
	movl	-56(%rbp), %esi
	callq	IOAPIC_Read
	movl	%eax, -68(%rbp)
	movl	-68(%rbp), %eax
	andl	$4294959103, %eax       # imm = 0xFFFFDFFF
	movl	%eax, -68(%rbp)
	movzbl	-34(%rbp), %eax
	andl	$1, %eax
	shll	$13, %eax
	movl	-68(%rbp), %edx
	orl	%eax, %edx
	movl	%edx, -68(%rbp)
	movl	-68(%rbp), %eax
	andl	$4294934527, %eax       # imm = 0xFFFF7FFF
	movl	%eax, -68(%rbp)
	movzbl	-33(%rbp), %eax
	andl	$1, %eax
	shll	$15, %eax
	movl	-68(%rbp), %edx
	orl	%eax, %edx
	movl	%edx, -68(%rbp)
	movl	-68(%rbp), %eax
	andl	$4294965247, %eax       # imm = 0xFFFFF7FF
	movl	%eax, -68(%rbp)
	movl	-68(%rbp), %eax
	andl	$4294965503, %eax       # imm = 0xFFFFF8FF
	movl	%eax, -68(%rbp)
	movl	-68(%rbp), %eax
	andl	$4294967040, %eax       # imm = 0xFFFFFF00
	movl	%eax, -68(%rbp)
	movzbl	-18(%rbp), %eax
	movl	-68(%rbp), %edx
	orl	%eax, %edx
	movl	%edx, -68(%rbp)
	movzbl	-35(%rbp), %eax
	andl	$7, %eax
	shll	$8, %eax
	movl	-68(%rbp), %edx
	orl	%eax, %edx
	movl	%edx, -68(%rbp)
	movq	-48(%rbp), %rdi
	movl	-56(%rbp), %esi
	movl	-68(%rbp), %edx
	callq	IOAPIC_Write
.LBB4_10:
	addq	$64, %rsp
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
.Ltmp21:
	.size	IOAPIC_MapIRQ, .Ltmp21-IOAPIC_MapIRQ
	.cfi_endproc

	.globl	IOAPIC_SetEnableMode
	.align	16, 0x90
	.type	IOAPIC_SetEnableMode,@function
IOAPIC_SetEnableMode:                   # @IOAPIC_SetEnableMode
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp22:
	.cfi_def_cfa_offset 16
.Ltmp23:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp24:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movb	%sil, %al
	movb	%dil, %cl
	movb	%cl, -1(%rbp)
	movb	%al, -2(%rbp)
	movzbl	-1(%rbp), %esi
	movl	%esi, %edx
	leaq	ioapic_interruptMap(,%rdx,8), %rdx
	cmpl	$-1, (%rdx)
	je	.LBB5_2
# BB#1:
	movzbl	-1(%rbp), %eax
	movl	%eax, %ecx
	movl	ioapic_interruptMap+4(,%rcx,8), %eax
	movl	%eax, %ecx
	leal	16(%rcx,%rcx), %eax
	movl	%eax, -8(%rbp)
	movzbl	-1(%rbp), %eax
	movl	%eax, %ecx
	movl	ioapic_interruptMap(,%rcx,8), %eax
	movl	%eax, %ecx
	leaq	(%rcx,%rcx,2), %rcx
	leaq	ioapics(,%rcx,8), %rcx
	movq	(%rcx), %rcx
	movq	%rcx, -16(%rbp)
	movq	-16(%rbp), %rdi
	movl	-8(%rbp), %esi
	callq	IOAPIC_Read
	movl	$0, %esi
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	movzbl	-2(%rbp), %edx
	xorl	$4294967295, %edx       # imm = 0xFFFFFFFF
	andl	$1, %edx
	subl	%edx, %esi
	xorl	-20(%rbp), %esi
	andl	$65536, %esi            # imm = 0x10000
	xorl	%esi, %eax
	movl	%eax, -20(%rbp)
	movq	-16(%rbp), %rdi
	movl	-8(%rbp), %esi
	movl	-20(%rbp), %edx
	callq	IOAPIC_Write
.LBB5_2:
	addq	$32, %rsp
	popq	%rbp
	retq
.Ltmp25:
	.size	IOAPIC_SetEnableMode, .Ltmp25-IOAPIC_SetEnableMode
	.cfi_endproc

	.type	curIOAPIC_index,@object # @curIOAPIC_index
	.bss
	.globl	curIOAPIC_index
	.align	4
curIOAPIC_index:
	.long	0                       # 0x0
	.size	curIOAPIC_index, 4

	.type	ioapics,@object         # @ioapics
	.comm	ioapics,3072,16
	.type	ioapic_interruptMap,@object # @ioapic_interruptMap
	.comm	ioapic_interruptMap,2048,16

	.ident	"clang version 3.5.0 (tags/RELEASE_350/final)"
	.section	".note.GNU-stack","",@progbits
