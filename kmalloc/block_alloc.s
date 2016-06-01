	.text
	.file	"kmalloc/block_alloc.c"
	.globl	Balloc_Initialize
	.align	16, 0x90
	.type	Balloc_Initialize,@function
Balloc_Initialize:                      # @Balloc_Initialize
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
	subq	$48, %rsp
	movb	initialized, %al
	cmpb	$0, %al
	je	.LBB0_2
# BB#1:
	movl	$0, -4(%rbp)
	jmp	.LBB0_3
.LBB0_2:
	movl	$33554432, %eax         # imm = 0x2000000
	movl	%eax, %edi
	callq	kmalloc
	movq	%rax, -16(%rbp)
	movq	%rax, _smallH_Loc
	movq	$33554432, _smallH_FreeSize # imm = 0x2000000
	movq	-16(%rbp), %rax
	addq	$33554432, %rax         # imm = 0x2000000
	movq	%rax, _largeH_Loc
	movq	$0, _largeH_FreeSize
	movq	$_smallH_Blocks, %rdi
	movl	$1048576, %ecx          # imm = 0x100000
	movl	%ecx, %edx
	xorl	%ecx, %ecx
	movl	%ecx, %esi
	movl	%ecx, -20(%rbp)         # 4-byte Spill
	callq	memset
	movq	$_largeH_Blocks, %rdi
	movl	-20(%rbp), %ecx         # 4-byte Reload
	movl	%ecx, %edx
	movl	%ecx, %esi
	movq	%rax, -32(%rbp)         # 8-byte Spill
	callq	memset
	movb	$1, initialized
	movl	$0, -4(%rbp)
	movq	%rax, -40(%rbp)         # 8-byte Spill
.LBB0_3:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	Balloc_Initialize, .Lfunc_end0-Balloc_Initialize
	.cfi_endproc

	.globl	_allocBlockSet
	.align	16, 0x90
	.type	_allocBlockSet,@function
_allocBlockSet:                         # @_allocBlockSet
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
	subq	$96, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	%rcx, -40(%rbp)
	movq	%r8, -48(%rbp)
	movq	%r9, -56(%rbp)
	movq	-16(%rbp), %rcx
	movq	%rcx, -64(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	xorl	%eax, %eax
	movb	%al, %cl
	movq	-64(%rbp), %rdx
	cmpq	-40(%rbp), %rdx
	movb	%cl, -73(%rbp)          # 1-byte Spill
	jae	.LBB1_3
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	movq	-64(%rbp), %rax
	movq	-32(%rbp), %rcx
	movzbl	(%rcx,%rax), %edx
	cmpl	$0, %edx
	setne	%sil
	movb	%sil, -73(%rbp)         # 1-byte Spill
.LBB1_3:                                #   in Loop: Header=BB1_1 Depth=1
	movb	-73(%rbp), %al          # 1-byte Reload
	testb	$1, %al
	jne	.LBB1_4
	jmp	.LBB1_6
.LBB1_4:                                #   in Loop: Header=BB1_1 Depth=1
	jmp	.LBB1_5
.LBB1_5:                                #   in Loop: Header=BB1_1 Depth=1
	movq	-64(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -64(%rbp)
	jmp	.LBB1_1
.LBB1_6:
	movq	-64(%rbp), %rax
	cmpq	-40(%rbp), %rax
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %edx
	movq	-64(%rbp), %rax
	addq	-24(%rbp), %rax
	cmpq	-40(%rbp), %rax
	seta	%cl
	andb	$1, %cl
	movzbl	%cl, %esi
	orl	%esi, %edx
	cmpl	$0, %edx
	je	.LBB1_8
# BB#7:
	movq	$-1, -8(%rbp)
	jmp	.LBB1_20
.LBB1_8:
	movq	$0, -72(%rbp)
.LBB1_9:                                # =>This Inner Loop Header: Depth=1
	movq	-72(%rbp), %rax
	cmpq	-24(%rbp), %rax
	jae	.LBB1_14
# BB#10:                                #   in Loop: Header=BB1_9 Depth=1
	movq	-64(%rbp), %rax
	addq	-72(%rbp), %rax
	movq	-32(%rbp), %rcx
	movzbl	(%rcx,%rax), %edx
	cmpl	$0, %edx
	je	.LBB1_12
# BB#11:
	movq	-64(%rbp), %rax
	addq	-72(%rbp), %rax
	addq	$1, %rax
	movq	-24(%rbp), %rsi
	movq	-32(%rbp), %rdx
	movq	-40(%rbp), %rcx
	movq	-48(%rbp), %r8
	movq	-56(%rbp), %r9
	movq	%rax, %rdi
	callq	_allocBlockSet
	movq	%rax, -8(%rbp)
	jmp	.LBB1_20
.LBB1_12:                               #   in Loop: Header=BB1_9 Depth=1
	jmp	.LBB1_13
.LBB1_13:                               #   in Loop: Header=BB1_9 Depth=1
	movq	-72(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -72(%rbp)
	jmp	.LBB1_9
.LBB1_14:
	cmpq	$2, -24(%rbp)
	jbe	.LBB1_16
# BB#15:
	movl	$1, %eax
	movq	-64(%rbp), %rcx
	addq	$1, %rcx
	addq	-32(%rbp), %rcx
	movq	-24(%rbp), %rdx
	subq	$2, %rdx
	shlq	$2, %rdx
	movl	$1, %esi
	movq	%rcx, %rdi
	movl	%eax, -80(%rbp)         # 4-byte Spill
	callq	memset
	movq	%rax, -88(%rbp)         # 8-byte Spill
.LBB1_16:
	movq	-64(%rbp), %rax
	movq	-32(%rbp), %rcx
	movb	$2, (%rcx,%rax)
	cmpq	$2, -24(%rbp)
	jb	.LBB1_18
# BB#17:
	movq	-64(%rbp), %rax
	addq	-24(%rbp), %rax
	subq	$1, %rax
	movq	-32(%rbp), %rcx
	movb	$4, (%rcx,%rax)
	jmp	.LBB1_19
.LBB1_18:
	movq	-64(%rbp), %rax
	movq	-32(%rbp), %rcx
	movb	$6, (%rcx,%rax)
.LBB1_19:
	movq	-64(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB1_20:
	movq	-8(%rbp), %rax
	addq	$96, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	_allocBlockSet, .Lfunc_end1-_allocBlockSet
	.cfi_endproc

	.globl	Balloc_Alloc
	.align	16, 0x90
	.type	Balloc_Alloc,@function
Balloc_Alloc:                           # @Balloc_Alloc
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
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, -16(%rbp)
	movb	initialized, %al
	cmpb	$0, %al
	jne	.LBB2_2
# BB#1:
	callq	Balloc_Initialize
	movl	%eax, -36(%rbp)         # 4-byte Spill
.LBB2_2:
	cmpq	$4096, -8(%rbp)         # imm = 0x1000
	jb	.LBB2_6
# BB#3:
	movq	-8(%rbp), %rax
	shrq	$12, %rax
	movq	%rax, -24(%rbp)
	movq	-8(%rbp), %rax
	movq	-24(%rbp), %rcx
	shlq	$12, %rcx
	cmpq	%rcx, %rax
	jbe	.LBB2_5
# BB#4:
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
.LBB2_5:
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	-24(%rbp), %rsi
	movq	_largeH_Loc, %r8
	movq	$_largeH_Blocks, %rdx
	xorl	%eax, %eax
	movl	%eax, %edi
	movl	$4096, %eax             # imm = 0x1000
	movl	%eax, %r9d
	movq	%rdi, -48(%rbp)         # 8-byte Spill
	movq	-48(%rbp), %r10         # 8-byte Reload
	movq	%rcx, -56(%rbp)         # 8-byte Spill
	movq	%r10, %rcx
	callq	_allocBlockSet
	movq	%rax, -16(%rbp)
	jmp	.LBB2_11
.LBB2_6:
	movq	-8(%rbp), %rax
	shrq	$5, %rax
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	-32(%rbp), %rcx
	shlq	$5, %rcx
	cmpq	%rcx, %rax
	jbe	.LBB2_8
# BB#7:
	movq	-32(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -32(%rbp)
.LBB2_8:
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	-32(%rbp), %rsi
	movq	_smallH_Loc, %r8
	movq	$_smallH_Blocks, %rdx
	xorl	%eax, %eax
	movl	%eax, %edi
	movl	$1048576, %eax          # imm = 0x100000
	movl	%eax, %r9d
	movl	$32, %eax
	movl	%eax, %r10d
	movq	%rcx, -64(%rbp)         # 8-byte Spill
	movq	%r9, %rcx
	movq	%r10, %r9
	callq	_allocBlockSet
	movq	%rax, -16(%rbp)
	cmpq	$-1, -16(%rbp)
	je	.LBB2_10
# BB#9:
	movq	-16(%rbp), %rax
	addq	$0, %rax
	movq	%rax, -16(%rbp)
.LBB2_10:
	jmp	.LBB2_11
.LBB2_11:
	movq	-16(%rbp), %rax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	Balloc_Alloc, .Lfunc_end2-Balloc_Alloc
	.cfi_endproc

	.globl	Balloc_GetBaseAddress
	.align	16, 0x90
	.type	Balloc_GetBaseAddress,@function
Balloc_GetBaseAddress:                  # @Balloc_GetBaseAddress
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
	movq	%rdi, -16(%rbp)
	movq	_largeH_Loc, %rdi
	movq	%rdi, -24(%rbp)
	movq	$4096, -32(%rbp)        # imm = 0x1000
	cmpq	$0, -16(%rbp)
	jb	.LBB3_2
# BB#1:
	movq	_smallH_Loc, %rax
	movq	%rax, -24(%rbp)
	movq	$32, -32(%rbp)
.LBB3_2:
	movq	-24(%rbp), %rax
	movq	-32(%rbp), %rcx
	imulq	-16(%rbp), %rcx
	addq	%rcx, %rax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	Balloc_GetBaseAddress, .Lfunc_end3-Balloc_GetBaseAddress
	.cfi_endproc

	.globl	Balloc_GetUID
	.align	16, 0x90
	.type	Balloc_GetUID,@function
Balloc_GetUID:                          # @Balloc_GetUID
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
	movq	%rdi, -16(%rbp)
	movq	%rdi, -24(%rbp)
	movq	$0, -32(%rbp)
	movq	-24(%rbp), %rdi
	movq	_largeH_Loc, %rax
	cmpq	%rax, %rdi
	jb	.LBB4_3
# BB#1:
	movq	-24(%rbp), %rax
	movq	_largeH_Loc, %rcx
	movq	-32(%rbp), %rdx
	addq	$4096, %rdx             # imm = 0x1000
	imulq	$0, %rdx, %rdx
	addq	%rdx, %rcx
	cmpq	%rcx, %rax
	jae	.LBB4_3
# BB#2:
	movq	-32(%rbp), %rax
	addq	$4096, %rax             # imm = 0x1000
	movq	%rax, -32(%rbp)
	movq	-24(%rbp), %rax
	movq	_largeH_Loc, %rcx
	subq	%rcx, %rax
	xorl	%edx, %edx
                                        # kill: RDX<def> EDX<kill>
	divq	-32(%rbp)
	movq	%rax, -8(%rbp)
	jmp	.LBB4_7
.LBB4_3:
	movq	-24(%rbp), %rax
	movq	_smallH_Loc, %rcx
	cmpq	%rcx, %rax
	jb	.LBB4_6
# BB#4:
	movq	-24(%rbp), %rax
	movq	_smallH_Loc, %rcx
	movq	-32(%rbp), %rdx
	addq	$32, %rdx
	shlq	$20, %rdx
	addq	%rdx, %rcx
	cmpq	%rcx, %rax
	jae	.LBB4_6
# BB#5:
	movq	-32(%rbp), %rax
	addq	$32, %rax
	movq	%rax, -32(%rbp)
	movq	-24(%rbp), %rax
	movq	_smallH_Loc, %rcx
	subq	%rcx, %rax
	xorl	%edx, %edx
                                        # kill: RDX<def> EDX<kill>
	divq	-32(%rbp)
	addq	$0, %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB4_7
.LBB4_6:
	movq	$-1, -8(%rbp)
.LBB4_7:
	movq	-8(%rbp), %rax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	Balloc_GetUID, .Lfunc_end4-Balloc_GetUID
	.cfi_endproc

	.globl	Balloc_Free
	.align	16, 0x90
	.type	Balloc_Free,@function
Balloc_Free:                            # @Balloc_Free
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
	subq	$40, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rdi, -16(%rbp)
	movq	$_largeH_Blocks, -24(%rbp)
	movq	_largeH_Loc, %rdi
	movq	%rdi, -32(%rbp)
	movq	$4096, -40(%rbp)        # imm = 0x1000
	cmpq	$0, -16(%rbp)
	jb	.LBB5_2
# BB#1:
	movq	$_smallH_Blocks, -24(%rbp)
	movq	_smallH_Loc, %rax
	movq	%rax, -32(%rbp)
	movq	$32, -40(%rbp)
.LBB5_2:
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rcx
	movzbl	(%rcx,%rax), %edx
	andl	$2, %edx
	cmpl	$0, %edx
	je	.LBB5_7
# BB#3:
	jmp	.LBB5_4
.LBB5_4:                                # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rcx
	movzbl	(%rcx,%rax), %edx
	andl	$4, %edx
	cmpl	$0, %edx
	setne	%sil
	xorb	$-1, %sil
	testb	$1, %sil
	jne	.LBB5_5
	jmp	.LBB5_6
.LBB5_5:                                #   in Loop: Header=BB5_4 Depth=1
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rcx
	movb	$0, (%rcx,%rax)
	movq	-16(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -16(%rbp)
	jmp	.LBB5_4
.LBB5_6:
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rcx
	movb	$0, (%rcx,%rax)
.LBB5_7:
	addq	$40, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	Balloc_Free, .Lfunc_end5-Balloc_Free
	.cfi_endproc

	.type	initialized,@object     # @initialized
	.local	initialized
	.comm	initialized,1,1
	.type	_smallH_Loc,@object     # @_smallH_Loc
	.local	_smallH_Loc
	.comm	_smallH_Loc,8,8
	.type	_smallH_FreeSize,@object # @_smallH_FreeSize
	.local	_smallH_FreeSize
	.comm	_smallH_FreeSize,8,8
	.type	_largeH_Loc,@object     # @_largeH_Loc
	.local	_largeH_Loc
	.comm	_largeH_Loc,8,8
	.type	_largeH_FreeSize,@object # @_largeH_FreeSize
	.local	_largeH_FreeSize
	.comm	_largeH_FreeSize,8,8
	.type	_smallH_Blocks,@object  # @_smallH_Blocks
	.local	_smallH_Blocks
	.comm	_smallH_Blocks,1048576,16
	.type	_largeH_Blocks,@object  # @_largeH_Blocks
	.local	_largeH_Blocks
	.comm	_largeH_Blocks,1,1

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
