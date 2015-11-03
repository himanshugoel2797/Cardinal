	.text
	.file	"utils/native.c"
	.globl	outb
	.align	16, 0x90
	.type	outb,@function
outb:                                   # @outb
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
	subq	$4, %rsp
	movb	%sil, %al
	movw	%di, %cx
	movw	%cx, -2(%rbp)
	movb	%al, -3(%rbp)
	movw	-2(%rbp), %dx
	movb	-3(%rbp), %al
	#APP
	outb	%al, %dx
	#NO_APP
	addq	$4, %rsp
	popq	%rbp
	retq
.Ltmp3:
	.size	outb, .Ltmp3-outb
	.cfi_endproc

	.globl	inb
	.align	16, 0x90
	.type	inb,@function
inb:                                    # @inb
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
	subq	$4, %rsp
	movw	%di, %ax
	movw	%ax, -2(%rbp)
	movw	-2(%rbp), %dx
	#APP
	inb	%dx, %al
	#NO_APP
	movb	%al, -3(%rbp)
	movzbl	-3(%rbp), %eax
	addq	$4, %rsp
	popq	%rbp
	retq
.Ltmp7:
	.size	inb, .Ltmp7-inb
	.cfi_endproc

	.globl	outw
	.align	16, 0x90
	.type	outw,@function
outw:                                   # @outw
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
	subq	$4, %rsp
	movw	%si, %ax
	movw	%di, %cx
	movw	%cx, -2(%rbp)
	movw	%ax, -4(%rbp)
	movw	-2(%rbp), %dx
	movw	-4(%rbp), %ax
	#APP
	outw	%ax, %dx
	#NO_APP
	addq	$4, %rsp
	popq	%rbp
	retq
.Ltmp11:
	.size	outw, .Ltmp11-outw
	.cfi_endproc

	.globl	inw
	.align	16, 0x90
	.type	inw,@function
inw:                                    # @inw
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
	movw	%di, %ax
	movw	%ax, -2(%rbp)
	movw	-2(%rbp), %dx
	#APP
	inw	%dx, %ax
	#NO_APP
	movw	%ax, -4(%rbp)
	movzwl	-4(%rbp), %eax
	addq	$4, %rsp
	popq	%rbp
	retq
.Ltmp15:
	.size	inw, .Ltmp15-inw
	.cfi_endproc

	.globl	outl
	.align	16, 0x90
	.type	outl,@function
outl:                                   # @outl
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
	pushq	%rax
	movw	%di, %ax
	movw	%ax, -2(%rbp)
	movl	%esi, -8(%rbp)
	movw	-2(%rbp), %dx
	movl	-8(%rbp), %eax
	#APP
	outl	%eax, %dx
	#NO_APP
	addq	$8, %rsp
	popq	%rbp
	retq
.Ltmp19:
	.size	outl, .Ltmp19-outl
	.cfi_endproc

	.globl	inl
	.align	16, 0x90
	.type	inl,@function
inl:                                    # @inl
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp20:
	.cfi_def_cfa_offset 16
.Ltmp21:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp22:
	.cfi_def_cfa_register %rbp
	pushq	%rax
	movw	%di, %ax
	movw	%ax, -2(%rbp)
	movw	-2(%rbp), %dx
	#APP
	inl	%dx, %eax
	#NO_APP
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addq	$8, %rsp
	popq	%rbp
	retq
.Ltmp23:
	.size	inl, .Ltmp23-inl
	.cfi_endproc

	.globl	wrmsr
	.align	16, 0x90
	.type	wrmsr,@function
wrmsr:                                  # @wrmsr
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
	subq	$24, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rsi
	movl	%esi, %edi
	movl	%edi, -20(%rbp)
	movq	-16(%rbp), %rsi
	shrq	$32, %rsi
	movl	%esi, %edi
	movl	%edi, -24(%rbp)
	movl	-20(%rbp), %eax
	movl	-24(%rbp), %edx
	movl	-4(%rbp), %ecx
	#APP
	wrmsr
	#NO_APP
	addq	$24, %rsp
	popq	%rbp
	retq
.Ltmp27:
	.size	wrmsr, .Ltmp27-wrmsr
	.cfi_endproc

	.globl	rdmsr
	.align	16, 0x90
	.type	rdmsr,@function
rdmsr:                                  # @rdmsr
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp28:
	.cfi_def_cfa_offset 16
.Ltmp29:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp30:
	.cfi_def_cfa_register %rbp
	subq	$12, %rsp
	movl	%edi, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	movl	-4(%rbp), %ecx
	#APP
	rdmsr
	#NO_APP
	movl	%eax, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	shlq	$32, %rsi
	movl	-8(%rbp), %eax
	movl	%eax, %r8d
	orq	%r8, %rsi
	movq	%rsi, %rax
	addq	$12, %rsp
	popq	%rbp
	retq
.Ltmp31:
	.size	rdmsr, .Ltmp31-rdmsr
	.cfi_endproc


	.ident	"clang version 3.5.0 (tags/RELEASE_350/final)"
	.section	".note.GNU-stack","",@progbits
