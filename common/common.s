	.text
	.file	"common/common.c"
	.globl	memcpy
	.align	16, 0x90
	.type	memcpy,@function
memcpy:                                 # @memcpy
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
	subq	$56, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-24(%rbp), %rdx
	shrq	$2, %rdx
	movq	-24(%rbp), %rsi
	andq	$3, %rsi
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rax
	movq	%rsi, -56(%rbp)
	movq	%rdx, %rcx
	movq	%rax, %rsi
	#APP
	rep
	movsl	(%rsi), %es:(%rdi)
	movl	-56(%rbp), %ecx
	rep
	movsb	(%rsi), %es:(%rdi)

	#NO_APP
	movq	%rcx, -32(%rbp)
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	-8(%rbp), %rax
	addq	$56, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	memcpy, .Lfunc_end0-memcpy
	.cfi_endproc

	.globl	memset
	.align	16, 0x90
	.type	memset,@function
memset:                                 # @memset
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
	subq	$24, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	%rdx, -24(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	addq	$-1, %rcx
	movq	%rcx, -24(%rbp)
	cmpq	$0, %rax
	je	.LBB1_3
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	movl	-12(%rbp), %eax
	movb	%al, %cl
	movq	-8(%rbp), %rdx
	movq	%rdx, %rsi
	addq	$1, %rsi
	movq	%rsi, -8(%rbp)
	movb	%cl, (%rdx)
	jmp	.LBB1_1
.LBB1_3:
	movq	-8(%rbp), %rax
	addq	$24, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	memset, .Lfunc_end1-memset
	.cfi_endproc

	.globl	memmove
	.align	16, 0x90
	.type	memmove,@function
memmove:                                # @memmove
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
	subq	$40, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rdx
	movq	%rdx, -32(%rbp)
	movq	-16(%rbp), %rdx
	movq	%rdx, -40(%rbp)
	movq	-16(%rbp), %rdx
	cmpq	-8(%rbp), %rdx
	je	.LBB2_11
# BB#1:
	movq	-16(%rbp), %rax
	cmpq	-8(%rbp), %rax
	jbe	.LBB2_6
# BB#2:
	jmp	.LBB2_3
.LBB2_3:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	addq	$-1, %rcx
	movq	%rcx, -24(%rbp)
	cmpq	$0, %rax
	je	.LBB2_5
# BB#4:                                 #   in Loop: Header=BB2_3 Depth=1
	movq	-40(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -40(%rbp)
	movb	(%rax), %dl
	movq	-32(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -32(%rbp)
	movb	%dl, (%rax)
	jmp	.LBB2_3
.LBB2_5:
	jmp	.LBB2_10
.LBB2_6:
	movq	-24(%rbp), %rax
	subq	$1, %rax
	addq	-32(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-24(%rbp), %rax
	subq	$1, %rax
	addq	-40(%rbp), %rax
	movq	%rax, -40(%rbp)
.LBB2_7:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	addq	$-1, %rcx
	movq	%rcx, -24(%rbp)
	cmpq	$0, %rax
	je	.LBB2_9
# BB#8:                                 #   in Loop: Header=BB2_7 Depth=1
	movq	-40(%rbp), %rax
	movq	%rax, %rcx
	addq	$-1, %rcx
	movq	%rcx, -40(%rbp)
	movb	(%rax), %dl
	movq	-32(%rbp), %rax
	movq	%rax, %rcx
	addq	$-1, %rcx
	movq	%rcx, -32(%rbp)
	movb	%dl, (%rax)
	jmp	.LBB2_7
.LBB2_9:
	jmp	.LBB2_10
.LBB2_10:
	jmp	.LBB2_11
.LBB2_11:
	movq	-8(%rbp), %rax
	addq	$40, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	memmove, .Lfunc_end2-memmove
	.cfi_endproc

	.globl	strrev
	.align	16, 0x90
	.type	strrev,@function
strrev:                                 # @strrev
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
	cmpq	$0, -8(%rbp)
	je	.LBB3_2
# BB#1:
	movq	-8(%rbp), %rax
	cmpb	$0, (%rax)
	jne	.LBB3_3
.LBB3_2:
	jmp	.LBB3_6
.LBB3_3:
	movq	-8(%rbp), %rax
	movq	-8(%rbp), %rdi
	movq	%rax, -32(%rbp)         # 8-byte Spill
	callq	strlen
	movq	-32(%rbp), %rdi         # 8-byte Reload
	addq	%rax, %rdi
	addq	$-1, %rdi
	movq	%rdi, -24(%rbp)
.LBB3_4:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	cmpq	-8(%rbp), %rax
	jbe	.LBB3_6
# BB#5:                                 #   in Loop: Header=BB3_4 Depth=1
	movq	-8(%rbp), %rax
	movb	(%rax), %cl
	movb	%cl, -9(%rbp)
	movq	-24(%rbp), %rax
	movb	(%rax), %cl
	movq	-8(%rbp), %rax
	movb	%cl, (%rax)
	movb	-9(%rbp), %cl
	movq	-24(%rbp), %rax
	movb	%cl, (%rax)
	movq	-8(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	addq	$-1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB3_4
.LBB3_6:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	strrev, .Lfunc_end3-strrev
	.cfi_endproc

	.globl	strlen
	.align	16, 0x90
	.type	strlen,@function
strlen:                                 # @strlen
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
	movq	%rdi, -8(%rbp)
	movq	$0, -16(%rbp)
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	movq	-8(%rbp), %rcx
	movsbl	(%rcx,%rax), %edx
	cmpl	$0, %edx
	je	.LBB4_3
# BB#2:                                 #   in Loop: Header=BB4_1 Depth=1
	movq	-16(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -16(%rbp)
	jmp	.LBB4_1
.LBB4_3:
	movq	-16(%rbp), %rax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	strlen, .Lfunc_end4-strlen
	.cfi_endproc

	.globl	strncmp
	.align	16, 0x90
	.type	strncmp,@function
strncmp:                                # @strncmp
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
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	%rdx, -32(%rbp)
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	movq	-32(%rbp), %rax
	movq	%rax, %rcx
	addq	$-1, %rcx
	movq	%rcx, -32(%rbp)
	cmpq	$0, %rax
	je	.LBB5_5
# BB#2:                                 #   in Loop: Header=BB5_1 Depth=1
	movq	-16(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -16(%rbp)
	movsbl	(%rax), %edx
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -24(%rbp)
	movsbl	(%rax), %esi
	cmpl	%esi, %edx
	je	.LBB5_4
# BB#3:
	movq	-16(%rbp), %rax
	movzbl	-1(%rax), %ecx
	movq	-24(%rbp), %rax
	movzbl	-1(%rax), %edx
	subl	%edx, %ecx
	movl	%ecx, -4(%rbp)
	jmp	.LBB5_6
.LBB5_4:                                #   in Loop: Header=BB5_1 Depth=1
	jmp	.LBB5_1
.LBB5_5:
	movl	$0, -4(%rbp)
.LBB5_6:
	movl	-4(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	strncmp, .Lfunc_end5-strncmp
	.cfi_endproc

	.globl	strcpy
	.align	16, 0x90
	.type	strcpy,@function
strcpy:                                 # @strcpy
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-16(%rbp), %rax
	movq	%rdi, -24(%rbp)         # 8-byte Spill
	movq	%rax, %rdi
	movq	%rsi, -32(%rbp)         # 8-byte Spill
	callq	strlen
	movq	-24(%rbp), %rdi         # 8-byte Reload
	movq	-32(%rbp), %rsi         # 8-byte Reload
	movq	%rax, %rdx
	callq	memcpy
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	strcpy, .Lfunc_end6-strcpy
	.cfi_endproc

	.globl	strchr
	.align	16, 0x90
	.type	strchr,@function
strchr:                                 # @strchr
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
	subq	$24, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
.LBB7_1:                                # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	movsbl	(%rax), %ecx
	movl	-20(%rbp), %edx
	movb	%dl, %sil
	movsbl	%sil, %edx
	cmpl	%edx, %ecx
	je	.LBB7_5
# BB#2:                                 #   in Loop: Header=BB7_1 Depth=1
	movq	-16(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -16(%rbp)
	cmpb	$0, (%rax)
	jne	.LBB7_4
# BB#3:
	movq	$0, -8(%rbp)
	jmp	.LBB7_6
.LBB7_4:                                #   in Loop: Header=BB7_1 Depth=1
	jmp	.LBB7_1
.LBB7_5:
	movq	-16(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB7_6:
	movq	-8(%rbp), %rax
	addq	$24, %rsp
	popq	%rbp
	retq
.Lfunc_end7:
	.size	strchr, .Lfunc_end7-strchr
	.cfi_endproc

	.globl	strrchr
	.align	16, 0x90
	.type	strrchr,@function
strrchr:                                # @strrchr
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
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movq	-16(%rbp), %rdi
	movq	-16(%rbp), %rax
	movq	%rdi, -40(%rbp)         # 8-byte Spill
	movq	%rax, %rdi
	callq	strlen
	movq	-40(%rbp), %rdi         # 8-byte Reload
	addq	%rax, %rdi
	movq	%rdi, -32(%rbp)
.LBB8_1:                                # =>This Inner Loop Header: Depth=1
	movq	-32(%rbp), %rax
	movsbl	(%rax), %ecx
	movl	-20(%rbp), %edx
	movb	%dl, %sil
	movsbl	%sil, %edx
	cmpl	%edx, %ecx
	je	.LBB8_5
# BB#2:                                 #   in Loop: Header=BB8_1 Depth=1
	movq	-32(%rbp), %rax
	movq	%rax, %rcx
	addq	$-1, %rcx
	movq	%rcx, -32(%rbp)
	cmpq	-16(%rbp), %rax
	jne	.LBB8_4
# BB#3:
	movq	$0, -8(%rbp)
	jmp	.LBB8_6
.LBB8_4:                                #   in Loop: Header=BB8_1 Depth=1
	jmp	.LBB8_1
.LBB8_5:
	movq	-32(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB8_6:
	movq	-8(%rbp), %rax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end8:
	.size	strrchr, .Lfunc_end8-strrchr
	.cfi_endproc

	.globl	find_first_zero
	.align	16, 0x90
	.type	find_first_zero,@function
find_first_zero:                        # @find_first_zero
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
	subq	$12, %rsp
	movl	%edi, -8(%rbp)
	movl	$0, -12(%rbp)
	cmpl	$0, -8(%rbp)
	jne	.LBB9_2
# BB#1:
	movl	$0, -4(%rbp)
	jmp	.LBB9_3
.LBB9_2:
	movl	-8(%rbp), %eax
	xorl	$-1, %eax
	#APP
	bsfl	%eax, %eax
	jne	.Ltmp30
	movl	$32, %eax
.Ltmp30:
	#NO_APP
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movw	%ax, %cx
	movzwl	%cx, %eax
	movl	%eax, -4(%rbp)
.LBB9_3:
	movl	-4(%rbp), %eax
	addq	$12, %rsp
	popq	%rbp
	retq
.Lfunc_end9:
	.size	find_first_zero, .Lfunc_end9-find_first_zero
	.cfi_endproc

	.globl	set_bit_cnt
	.align	16, 0x90
	.type	set_bit_cnt,@function
set_bit_cnt:                            # @set_bit_cnt
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp31:
	.cfi_def_cfa_offset 16
.Ltmp32:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp33:
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	subq	$12, %rsp
.Ltmp34:
	.cfi_offset %rbx, -24
	movl	%edi, -16(%rbp)
	movl	$0, -20(%rbp)
	cmpl	$0, -16(%rbp)
	jne	.LBB10_2
# BB#1:
	movl	$0, -12(%rbp)
	jmp	.LBB10_3
.LBB10_2:
	movl	-16(%rbp), %eax
	#APP
	popcntl	%eax, %ebx
	#NO_APP
	movl	%ebx, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -12(%rbp)
.LBB10_3:
	movl	-12(%rbp), %eax
	addq	$12, %rsp
	popq	%rbx
	popq	%rbp
	retq
.Lfunc_end10:
	.size	set_bit_cnt, .Lfunc_end10-set_bit_cnt
	.cfi_endproc

	.globl	new_uid
	.align	16, 0x90
	.type	new_uid,@function
new_uid:                                # @new_uid
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp35:
	.cfi_def_cfa_offset 16
.Ltmp36:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp37:
	.cfi_def_cfa_register %rbp
	movq	uids_base, %rax
	incq	%rax
	movq	%rax, uids_base
	movabsq	$4294967295, %rcx       # imm = 0xFFFFFFFF
	andq	%rcx, %rax
	popq	%rbp
	retq
.Lfunc_end11:
	.size	new_uid, .Lfunc_end11-new_uid
	.cfi_endproc

	.globl	rand
	.align	16, 0x90
	.type	rand,@function
rand:                                   # @rand
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp38:
	.cfi_def_cfa_offset 16
.Ltmp39:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp40:
	.cfi_def_cfa_register %rbp
	subq	$12, %rsp
	movl	%edi, -4(%rbp)
	movl	$16807, -8(%rbp)        # imm = 0x41A7
	movl	$2147483647, -12(%rbp)  # imm = 0x7FFFFFFF
	movl	-8(%rbp), %edi
	imull	-4(%rbp), %edi
	movl	%edi, %eax
	xorl	%edx, %edx
	divl	-12(%rbp)
	movl	%edx, -4(%rbp)
	movl	-4(%rbp), %edx
	movl	%edx, %eax
	xorl	%edx, %edx
	divl	-12(%rbp)
	addq	$12, %rsp
	popq	%rbp
	retq
.Lfunc_end12:
	.size	rand, .Lfunc_end12-rand
	.cfi_endproc

	.type	uids_base,@object       # @uids_base
	.local	uids_base
	.comm	uids_base,8,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
