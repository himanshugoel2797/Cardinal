	.text
	.file	"kmalloc/kmalloc.c"
	.globl	kmalloc_init
	.align	16, 0x90
	.type	kmalloc_init,@function
kmalloc_init:                           # @kmalloc_init
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
	subq	$80, %rsp
	movq	%rdi, -8(%rbp)
	callq	CreateBootstrapSpinlock
	movq	%rax, alloc_sync
	movq	$0, -16(%rbp)
	callq	GetActiveVirtualMemoryInstance
	movl	$134217728, %ecx        # imm = 0x8000000
	movl	%ecx, %edi
	leaq	-16(%rbp), %rsi
	xorl	%ecx, %ecx
	movl	$4, %r8d
	movq	%rdi, -32(%rbp)         # 8-byte Spill
	movq	%rax, %rdi
	movq	-32(%rbp), %rdx         # 8-byte Reload
	movl	%ecx, -36(%rbp)         # 4-byte Spill
	callq	FindFreeVirtualAddress
	movl	$32768, %edi            # imm = 0x8000
	movl	%eax, -40(%rbp)         # 4-byte Spill
	callq	AllocatePhysicalPageCont
	movq	%rax, -24(%rbp)
	callq	GetActiveVirtualMemoryInstance
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rcx
	movq	%rsp, %rsi
	movl	$4, 8(%rsi)
	movl	$0, (%rsi)
	movl	-36(%rbp), %edi         # 4-byte Reload
	movl	%edi, %esi
	movq	%rax, %rdi
	movq	-32(%rbp), %r8          # 8-byte Reload
	movl	-36(%rbp), %r9d         # 4-byte Reload
	callq	MapPage
	movq	-8(%rbp), %rcx
	movl	$0, (%rcx)
	movq	-16(%rbp), %rcx
	movq	-8(%rbp), %rdx
	movq	%rcx, 8(%rdx)
	movq	-8(%rbp), %rcx
	movq	$134217728, 16(%rcx)    # imm = 0x8000000
	movq	-8(%rbp), %rcx
	movl	$0, 28(%rcx)
	movq	-8(%rbp), %rcx
	movq	$0, 32(%rcx)
	movq	-8(%rbp), %rcx
	movq	$0, 40(%rcx)
	movq	-16(%rbp), %rcx
	movq	%rcx, allocation_info
	movq	%rcx, next_free_block
	movq	-16(%rbp), %rcx
	addq	$1048576, %rcx          # imm = 0x100000
	movq	%rcx, k_pages_base_addr
	movl	$43690, max_allocs      # imm = 0xAAAA
	movl	$133169152, free_space  # imm = 0x7F00000
	movq	allocation_info, %rdi
	movl	$1048576, %r9d          # imm = 0x100000
	movl	%r9d, %edx
	movl	-36(%rbp), %esi         # 4-byte Reload
	movl	%eax, -44(%rbp)         # 4-byte Spill
	callq	memset
	movq	k_pages_base_addr, %rcx
	movq	allocation_info, %rdx
	movq	%rcx, (%rdx)
	movl	free_space, %esi
	movl	%esi, %ecx
	movq	allocation_info, %rdx
	movq	%rcx, 8(%rdx)
	movq	allocation_info, %rcx
	movq	$0, 16(%rcx)
	movq	allocation_info, %rcx
	movq	(%rcx), %rdx
	andq	$-2, %rdx
	movq	%rdx, (%rcx)
	movq	next_free_block, %rcx
	addq	$24, %rcx
	movq	%rcx, next_free_block
	movq	%rax, -56(%rbp)         # 8-byte Spill
	callq	Balloc_Initialize
	movl	%eax, -60(%rbp)         # 4-byte Spill
	addq	$80, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	kmalloc_init, .Lfunc_end0-kmalloc_init
	.cfi_endproc

	.globl	kcompact
	.align	16, 0x90
	.type	kcompact,@function
kcompact:                               # @kcompact
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
	pushq	%rax
	movq	allocation_info, %rax
	movq	%rax, -8(%rbp)
.LBB1_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB1_3 Depth 2
	movq	-8(%rbp), %rax
	cmpq	$0, 16(%rax)
	je	.LBB1_10
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	jmp	.LBB1_3
.LBB1_3:                                #   Parent Loop BB1_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movq	-8(%rbp), %rax
	cmpq	$0, 16(%rax)
	je	.LBB1_7
# BB#4:                                 #   in Loop: Header=BB1_3 Depth=2
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	andq	$1, %rax
	cmpq	$0, %rax
	jne	.LBB1_6
# BB#5:                                 #   in Loop: Header=BB1_1 Depth=1
	jmp	.LBB1_7
.LBB1_6:                                #   in Loop: Header=BB1_3 Depth=2
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB1_3
.LBB1_7:                                #   in Loop: Header=BB1_1 Depth=1
	movq	-8(%rbp), %rax
	cmpq	$0, 16(%rax)
	je	.LBB1_9
# BB#8:                                 #   in Loop: Header=BB1_1 Depth=1
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	8(%rax), %rax
	movq	-8(%rbp), %rcx
	addq	8(%rcx), %rax
	movq	%rax, 8(%rcx)
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	16(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	%rax, 16(%rcx)
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -8(%rbp)
.LBB1_9:                                #   in Loop: Header=BB1_1 Depth=1
	jmp	.LBB1_1
.LBB1_10:
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	kcompact, .Lfunc_end1-kcompact
	.cfi_endproc

	.globl	kmalloc
	.align	16, 0x90
	.type	kmalloc,@function
kmalloc:                                # @kmalloc
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
	movq	%rdi, -16(%rbp)
	movq	alloc_sync, %rdi
	callq	LockSpinlock
	cmpq	$4096, -16(%rbp)        # imm = 0x1000
	movb	%al, -57(%rbp)          # 1-byte Spill
	jae	.LBB2_4
# BB#1:
	movq	-16(%rbp), %rdi
	callq	Balloc_Alloc
	movq	%rax, -24(%rbp)
	cmpq	$-1, -24(%rbp)
	je	.LBB2_3
# BB#2:
	movq	alloc_sync, %rdi
	callq	UnlockSpinlock
	movq	-24(%rbp), %rdi
	movb	%al, -58(%rbp)          # 1-byte Spill
	callq	Balloc_GetBaseAddress
	movq	%rax, -8(%rbp)
	jmp	.LBB2_19
.LBB2_3:
	jmp	.LBB2_4
.LBB2_4:
	movq	allocation_info, %rax
	movq	%rax, -32(%rbp)
.LBB2_5:                                # =>This Inner Loop Header: Depth=1
	xorl	%eax, %eax
	movb	%al, %cl
	cmpq	$0, -32(%rbp)
	movb	%cl, -59(%rbp)          # 1-byte Spill
	je	.LBB2_7
# BB#6:                                 #   in Loop: Header=BB2_5 Depth=1
	movq	-32(%rbp), %rax
	cmpq	$0, 16(%rax)
	setne	%cl
	movb	%cl, -59(%rbp)          # 1-byte Spill
.LBB2_7:                                #   in Loop: Header=BB2_5 Depth=1
	movb	-59(%rbp), %al          # 1-byte Reload
	testb	$1, %al
	jne	.LBB2_8
	jmp	.LBB2_12
.LBB2_8:                                #   in Loop: Header=BB2_5 Depth=1
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	andq	$1, %rax
	cmpq	$0, %rax
	jne	.LBB2_11
# BB#9:                                 #   in Loop: Header=BB2_5 Depth=1
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-16(%rbp), %rax
	jb	.LBB2_11
# BB#10:
	jmp	.LBB2_12
.LBB2_11:                               #   in Loop: Header=BB2_5 Depth=1
	movq	-32(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -32(%rbp)
	jmp	.LBB2_5
.LBB2_12:
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	andq	$1, %rax
	cmpq	$1, %rax
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %edx
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-16(%rbp), %rax
	setb	%cl
	andb	$1, %cl
	movzbl	%cl, %esi
	orl	%esi, %edx
	cmpl	$0, %edx
	je	.LBB2_16
# BB#13:
	movq	alloc_sync, %rdi
	callq	UnlockSpinlock
	movb	retry, %cl
	cmpb	$0, %cl
	movb	%al, -60(%rbp)          # 1-byte Spill
	jne	.LBB2_15
# BB#14:
	movb	$1, retry
	movq	-16(%rbp), %rdi
	callq	kmalloc
	movq	%rax, -40(%rbp)
	movb	$0, retry
	movq	-40(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_19
.LBB2_15:
	movq	$0, -8(%rbp)
	jmp	.LBB2_19
.LBB2_16:
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	andq	$-2, %rax
	movq	%rax, -48(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	subq	-16(%rbp), %rax
	movq	%rax, -56(%rbp)
	cmpq	$0, -56(%rbp)
	je	.LBB2_18
# BB#17:
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rcx
	addq	%rcx, %rax
	movq	next_free_block, %rcx
	movq	%rax, (%rcx)
	movq	-56(%rbp), %rax
	movq	next_free_block, %rcx
	movq	%rax, 8(%rcx)
	movq	-32(%rbp), %rax
	movq	16(%rax), %rax
	movq	next_free_block, %rcx
	movq	%rax, 16(%rcx)
	movq	next_free_block, %rax
	movq	(%rax), %rcx
	andq	$-2, %rcx
	movq	%rcx, (%rax)
	movq	next_free_block, %rax
	movq	-32(%rbp), %rcx
	movq	%rax, 16(%rcx)
	movq	next_free_block, %rax
	addq	$24, %rax
	movq	%rax, next_free_block
.LBB2_18:
	movq	-32(%rbp), %rax
	movq	(%rax), %rcx
	orq	$1, %rcx
	movq	%rcx, (%rax)
	movq	-16(%rbp), %rax
	movq	-32(%rbp), %rcx
	movq	%rax, 8(%rcx)
	movq	alloc_sync, %rdi
	callq	UnlockSpinlock
	movq	-48(%rbp), %rcx
	movq	%rcx, -8(%rbp)
	movb	%al, -61(%rbp)          # 1-byte Spill
.LBB2_19:
	movq	-8(%rbp), %rax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	kmalloc, .Lfunc_end2-kmalloc
	.cfi_endproc

	.globl	kfree
	.align	16, 0x90
	.type	kfree,@function
kfree:                                  # @kfree
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
	movq	alloc_sync, %rdi
	callq	LockSpinlock
	movq	-8(%rbp), %rdi
	movb	%al, -25(%rbp)          # 1-byte Spill
	callq	Balloc_GetUID
	movq	%rax, -16(%rbp)
	cmpq	$-1, -16(%rbp)
	je	.LBB3_2
# BB#1:
	movq	-16(%rbp), %rdi
	callq	Balloc_Free
	movq	alloc_sync, %rdi
	callq	UnlockSpinlock
	movb	%al, -26(%rbp)          # 1-byte Spill
	jmp	.LBB3_9
.LBB3_2:
	movq	allocation_info, %rax
	movq	%rax, -24(%rbp)
.LBB3_3:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	cmpq	$0, 16(%rax)
	je	.LBB3_8
# BB#4:                                 #   in Loop: Header=BB3_3 Depth=1
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	andq	$1, %rax
	cmpq	$1, %rax
	jne	.LBB3_7
# BB#5:                                 #   in Loop: Header=BB3_3 Depth=1
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	%rcx, %rax
	jne	.LBB3_7
# BB#6:
	jmp	.LBB3_8
.LBB3_7:                                #   in Loop: Header=BB3_3 Depth=1
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB3_3
.LBB3_8:
	movq	-24(%rbp), %rax
	movq	(%rax), %rcx
	andq	$-2, %rcx
	movq	%rcx, (%rax)
	movq	alloc_sync, %rdi
	callq	UnlockSpinlock
	movb	%al, -27(%rbp)          # 1-byte Spill
.LBB3_9:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	kfree, .Lfunc_end3-kfree
	.cfi_endproc

	.type	max_allocs,@object      # @max_allocs
	.bss
	.globl	max_allocs
	.align	4
max_allocs:
	.long	0                       # 0x0
	.size	max_allocs, 4

	.type	free_space,@object      # @free_space
	.globl	free_space
	.align	4
free_space:
	.long	0                       # 0x0
	.size	free_space, 4

	.type	allocation_info,@object # @allocation_info
	.globl	allocation_info
	.align	8
allocation_info:
	.quad	0
	.size	allocation_info, 8

	.type	next_free_block,@object # @next_free_block
	.globl	next_free_block
	.align	8
next_free_block:
	.quad	0
	.size	next_free_block, 8

	.type	alloc_sync,@object      # @alloc_sync
	.comm	alloc_sync,8,8
	.type	k_pages_base_addr,@object # @k_pages_base_addr
	.comm	k_pages_base_addr,8,8
	.type	retry,@object           # @retry
	.globl	retry
retry:
	.byte	0                       # 0x0
	.size	retry, 1


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
