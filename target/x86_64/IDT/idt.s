	.text
	.file	"IDT/idt.c"
	.globl	IDT_Initialize
	.align	16, 0x90
	.type	IDT_Initialize,@function
IDT_Initialize:                         # @IDT_Initialize
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
	movw	$4095, idt_table        # imm = 0xFFF
	movq	$idt_entries, idt_table+2
	movq	$idt_table, -8(%rbp)
	#APP
	cli
	#NO_APP
	movq	-8(%rbp), %rax
	#APP
	lidtq	(%rax)
	#NO_APP
	movl	$0, -12(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$256, -12(%rbp)         # imm = 0x100
	jge	.LBB0_4
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	xorl	%eax, %eax
	movl	%eax, %esi
	movl	%eax, %edi
	movl	%eax, %edx
	movl	%eax, %ecx
	callq	IDT_SetEntry
	movslq	-12(%rbp), %rsi
	leaq	idt_handler_calls(,%rsi,8), %rsi
	movq	$0, (%rsi)
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.LBB0_1
.LBB0_4:
	movl	$1, -16(%rbp)
	movl	$0, -20(%rbp)
.LBB0_5:                                # =>This Inner Loop Header: Depth=1
	cmpl	$256, -20(%rbp)         # imm = 0x100
	jge	.LBB0_12
# BB#6:                                 #   in Loop: Header=BB0_5 Depth=1
	cmpl	$8, -20(%rbp)
	je	.LBB0_9
# BB#7:                                 #   in Loop: Header=BB0_5 Depth=1
	cmpl	$10, -20(%rbp)
	jl	.LBB0_10
# BB#8:                                 #   in Loop: Header=BB0_5 Depth=1
	cmpl	$14, -20(%rbp)
	jg	.LBB0_10
.LBB0_9:                                #   in Loop: Header=BB0_5 Depth=1
	movl	$0, -16(%rbp)
.LBB0_10:                               #   in Loop: Header=BB0_5 Depth=1
	movl	$8, %eax
	movl	$142, %ecx
	movslq	-20(%rbp), %rdx
	movl	%edx, %esi
	shlq	$5, %rdx
	leaq	idt_handlers(%rdx), %rdi
	movzbl	-16(%rbp), %edx
	movb	%sil, %r8b
	movzbl	%r8b, %esi
	movl	%eax, -24(%rbp)         # 4-byte Spill
	movl	%ecx, -28(%rbp)         # 4-byte Spill
	callq	IDT_FillSWInterruptHandler
	movslq	-20(%rbp), %rdi
	movb	%dil, %r8b
	shlq	$5, %rdi
	leaq	idt_handlers(%rdi), %rsi
	movzbl	%r8b, %edi
	movl	$8, %edx
	movl	$142, %ecx
	callq	IDT_SetEntry
	movl	$1, -16(%rbp)
# BB#11:                                #   in Loop: Header=BB0_5 Depth=1
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -20(%rbp)
	jmp	.LBB0_5
.LBB0_12:
	addq	$32, %rsp
	popq	%rbp
	retq
.Ltmp3:
	.size	IDT_Initialize, .Ltmp3-IDT_Initialize
	.cfi_endproc

	.globl	IDT_SetEntry
	.align	16, 0x90
	.type	IDT_SetEntry,@function
IDT_SetEntry:                           # @IDT_SetEntry
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
	subq	$24, %rsp
	movb	%cl, %al
	movw	%dx, %r8w
	movb	%dil, %r9b
	movb	%r9b, -1(%rbp)
	movq	%rsi, -16(%rbp)
	movw	%r8w, -18(%rbp)
	movb	%al, -19(%rbp)
	movw	-16(%rbp), %r8w
	movzbl	-1(%rbp), %ecx
	movl	%ecx, %esi
	shlq	$4, %rsi
	movw	%r8w, idt_entries(%rsi)
	movw	-14(%rbp), %r8w
	movzbl	-1(%rbp), %ecx
	movl	%ecx, %esi
	shlq	$4, %rsi
	movw	%r8w, idt_entries+6(%rsi)
	movl	-12(%rbp), %ecx
	movzbl	-1(%rbp), %edx
	movl	%edx, %esi
	shlq	$4, %rsi
	movl	%ecx, idt_entries+8(%rsi)
	movb	-19(%rbp), %al
	movzbl	-1(%rbp), %ecx
	movl	%ecx, %esi
	shlq	$4, %rsi
	movb	%al, idt_entries+5(%rsi)
	movzbl	-1(%rbp), %ecx
	movl	%ecx, %esi
	shlq	$4, %rsi
	movb	$0, idt_entries+4(%rsi)
	movzwl	-18(%rbp), %ecx
	movw	%cx, %r8w
	movzbl	-1(%rbp), %ecx
	movl	%ecx, %esi
	shlq	$4, %rsi
	leaq	idt_entries(%rsi), %rsi
	movw	%r8w, 2(%rsi)
	addq	$24, %rsp
	popq	%rbp
	retq
.Ltmp7:
	.size	IDT_SetEntry, .Ltmp7-IDT_SetEntry
	.cfi_endproc

	.globl	IDT_FillSWInterruptHandler
	.align	16, 0x90
	.type	IDT_FillSWInterruptHandler,@function
IDT_FillSWInterruptHandler:             # @IDT_FillSWInterruptHandler
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
	pushq	%rbx
	subq	$16, %rsp
.Ltmp11:
	.cfi_offset %rbx, -24
	movb	%dl, %al
	movb	%sil, %cl
	movq	%rdi, -16(%rbp)
	movb	%cl, -17(%rbp)
	movb	%al, -18(%rbp)
	movl	$0, -24(%rbp)
	cmpb	$0, -18(%rbp)
	je	.LBB2_2
# BB#1:
	movl	-24(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	movl	%ecx, -24(%rbp)
	movslq	%eax, %rdx
	movq	-16(%rbp), %rsi
	movb	$106, (%rsi,%rdx)
	movb	-18(%rbp), %dil
	movl	-24(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	movl	%ecx, -24(%rbp)
	movslq	%eax, %rdx
	movq	-16(%rbp), %rsi
	movb	%dil, (%rsi,%rdx)
.LBB2_2:
	movslq	-24(%rbp), %rax
	movl	%eax, %ecx
	incl	%ecx
	movl	%ecx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movb	$106, (%rdx,%rax)
	movb	-17(%rbp), %sil
	movslq	-24(%rbp), %rax
	movl	%eax, %ecx
	incl	%ecx
	movl	%ecx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movb	%sil, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %ecx
	incl	%ecx
	movl	%ecx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movb	$80, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %ecx
	incl	%ecx
	movl	%ecx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movb	$72, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %ecx
	incl	%ecx
	movl	%ecx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movb	$-72, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %ecx
	incl	%ecx
	movl	%ecx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movq	$IDT_DefaultHandler, %rdi
	movb	%dil, %sil
	movb	%sil, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %ecx
	incl	%ecx
	movl	%ecx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movq	%rdi, %rbx
	movb	%bh, %cl  # NOREX
	movb	%cl, (%rdx,%rax)  # NOREX
	movslq	-24(%rbp), %rax
	movl	%eax, %r8d
	incl	%r8d
	movl	%r8d, -24(%rbp)
	movq	-16(%rbp), %rdx
	movq	%rdi, %rbx
	shrq	$16, %rbx
	movb	%bl, %cl
	movb	%cl, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %r8d
	incl	%r8d
	movl	%r8d, -24(%rbp)
	movq	-16(%rbp), %rdx
	movq	%rdi, %rbx
	shrq	$24, %rbx
	movb	%bl, %cl
	movb	%cl, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %r8d
	incl	%r8d
	movl	%r8d, -24(%rbp)
	movq	-16(%rbp), %rdx
	movq	%rdi, %rbx
	shrq	$32, %rbx
	movb	%bl, %cl
	movb	%cl, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %r8d
	incl	%r8d
	movl	%r8d, -24(%rbp)
	movq	-16(%rbp), %rdx
	movq	%rdi, %rbx
	shrq	$40, %rbx
	movb	%bl, %cl
	movb	%cl, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %r8d
	incl	%r8d
	movl	%r8d, -24(%rbp)
	movq	-16(%rbp), %rdx
	movq	%rdi, %rbx
	shrq	$48, %rbx
	movb	%bl, %cl
	movb	%cl, (%rdx,%rax)
	movslq	-24(%rbp), %rax
	movl	%eax, %r8d
	incl	%r8d
	movl	%r8d, -24(%rbp)
	movq	-16(%rbp), %rdx
	shrq	$56, %rdi
	movb	%dil, %cl
	movb	%cl, (%rdx,%rax)
	movl	-24(%rbp), %r8d
	movl	%r8d, %r9d
	addl	$1, %r9d
	movl	%r9d, -24(%rbp)
	movslq	%r8d, %rax
	movq	-16(%rbp), %rdx
	movb	$80, (%rdx,%rax)
	movl	-24(%rbp), %r8d
	movl	%r8d, %r9d
	addl	$1, %r9d
	movl	%r9d, -24(%rbp)
	movslq	%r8d, %rax
	movq	-16(%rbp), %rdx
	movb	$-61, (%rdx,%rax)
	addq	$16, %rsp
	popq	%rbx
	popq	%rbp
	retq
.Ltmp12:
	.size	IDT_FillSWInterruptHandler, .Ltmp12-IDT_FillSWInterruptHandler
	.cfi_endproc

	.globl	IDT_DefaultHandler
	.align	16, 0x90
	.type	IDT_DefaultHandler,@function
IDT_DefaultHandler:                     # @IDT_DefaultHandler
	.cfi_startproc
# BB#0:
	#APP
	pushq	%rbx
	pushq	%rcx
	pushq	%rdx
	pushq	%rbp
	pushq	%rsi
	pushq	%rdi
	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	movq	%rsp, %rdi
	callq	IDT_MainHandler
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8
	popq	%rdi
	popq	%rsi
	popq	%rbp
	popq	%rdx
	popq	%rcx
	popq	%rbx
	popq	%rax
	addq	$24, %rsp
	iretq

	#NO_APP
	retq
.Ltmp13:
	.size	IDT_DefaultHandler, .Ltmp13-IDT_DefaultHandler
	.cfi_endproc

	.globl	IDT_MainHandler
	.align	16, 0x90
	.type	IDT_MainHandler,@function
IDT_MainHandler:                        # @IDT_MainHandler
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp14:
	.cfi_def_cfa_offset 16
.Ltmp15:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp16:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	128(%rdi), %rax
	#APP
	hlt
	#NO_APP
	movq	-8(%rbp), %rax
	movq	128(%rax), %rax
	leaq	idt_handler_calls(,%rax,8), %rax
	cmpq	$0, (%rax)
	je	.LBB4_2
# BB#1:
	movq	-8(%rbp), %rax
	movq	128(%rax), %rax
	leaq	idt_handler_calls(,%rax,8), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rdi
	callq	*%rax
.LBB4_2:
	addq	$16, %rsp
	popq	%rbp
	retq
.Ltmp17:
	.size	IDT_MainHandler, .Ltmp17-IDT_MainHandler
	.cfi_endproc

	.globl	IDT_RegisterHandler
	.align	16, 0x90
	.type	IDT_RegisterHandler,@function
IDT_RegisterHandler:                    # @IDT_RegisterHandler
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
	movb	%dil, %al
	movb	%al, -1(%rbp)
	movq	%rsi, -16(%rbp)
	movzbl	-1(%rbp), %edi
	movl	%edi, %ecx
	leaq	idt_handler_calls(,%rcx,8), %rcx
	movq	%rsi, (%rcx)
	addq	$16, %rsp
	popq	%rbp
	retq
.Ltmp21:
	.size	IDT_RegisterHandler, .Ltmp21-IDT_RegisterHandler
	.cfi_endproc

	.type	idt_table,@object       # @idt_table
	.local	idt_table
	.comm	idt_table,10,1
	.type	idt_entries,@object     # @idt_entries
	.local	idt_entries
	.comm	idt_entries,4096,16
	.type	idt_handler_calls,@object # @idt_handler_calls
	.local	idt_handler_calls
	.comm	idt_handler_calls,2048,16
	.type	idt_handlers,@object    # @idt_handlers
	.local	idt_handlers
	.comm	idt_handlers,8192,16

	.ident	"clang version 3.5.0 (tags/RELEASE_350/final)"
	.section	".note.GNU-stack","",@progbits
