	.text
	.file	"page_manager/phys_mem_manager.c"
	.globl	MemMan_Initialize
	.align	16, 0x90
	.type	MemMan_Initialize,@function
MemMan_Initialize:                      # @MemMan_Initialize
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
	callq	GetBootInfo
	movq	%rax, -8(%rbp)
	movq	(%rax), %rax
	movq	%rax, memory_size
	movl	%eax, %ecx
	andl	$4095, %ecx             # imm = 0xFFF
	movl	%ecx, %edx
	subq	%rdx, %rax
	movq	%rax, memory_size
	shrq	$12, %rax
	movq	%rax, page_count
	movq	%rax, freePageCount
	movq	page_count, %rax
	cmpq	$524288, %rax           # imm = 0x80000
	ja	.LBB0_2
# BB#1:
	movq	page_count, %rax
	shrq	$6, %rax
	decq	%rax
	movq	%rax, lastNonFullPage
	jmp	.LBB0_3
.LBB0_2:
	movq	$0, lastNonFullPage
.LBB0_3:
	movl	$4294967295, %eax       # imm = 0xFFFFFFFF
	movq	memory_size, %rcx
	shrq	$17, %rcx
	movl	%ecx, %edx
	movl	%edx, KB4_Blocks_Count
	movl	KB4_Blocks_Count, %edx
	movl	%edx, %ecx
	shlq	$2, %rcx
	movq	%rcx, %rdi
	movl	%eax, -36(%rbp)         # 4-byte Spill
	callq	bootstrap_malloc
	movq	%rax, KB4_Blocks_Bitmap
	movl	KB4_Blocks_Count, %edx
	movl	%edx, %edx
	movl	%edx, %ecx
	shlq	$2, %rcx
	movl	$-1, %esi
	movq	%rax, %rdi
	movq	%rcx, %rdx
	callq	memset
	movl	$0, -12(%rbp)
	movq	%rax, -48(%rbp)         # 8-byte Spill
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movl	$24, %eax
	movl	%eax, %ecx
	movl	-12(%rbp), %eax
	movl	%eax, %edx
	movq	-8(%rbp), %rsi
	movl	28(%rsi), %eax
                                        # kill: RAX<def> EAX<kill>
	xorl	%edi, %edi
	movq	%rdx, -56(%rbp)         # 8-byte Spill
	movl	%edi, %edx
	divq	%rcx
	movq	-56(%rbp), %rcx         # 8-byte Reload
	cmpq	%rax, %rcx
	jae	.LBB0_11
# BB#5:                                 #   in Loop: Header=BB0_4 Depth=1
	movl	-12(%rbp), %eax
	movl	%eax, %ecx
	movq	-8(%rbp), %rdx
	imulq	$24, %rcx, %rcx
	addq	48(%rdx), %rcx
	movq	(%rcx), %rcx
	movq	%rcx, -24(%rbp)
	movq	-24(%rbp), %rcx
	shrq	$12, %rcx
	shlq	$12, %rcx
	movq	%rcx, -24(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, %ecx
	movq	-8(%rbp), %rdx
	imulq	$24, %rcx, %rcx
	addq	48(%rdx), %rcx
	movq	8(%rcx), %rcx
	movq	%rcx, -32(%rbp)
	movq	-32(%rbp), %rcx
	andq	$4095, %rcx             # imm = 0xFFF
	cmpq	$0, %rcx
	je	.LBB0_7
# BB#6:                                 #   in Loop: Header=BB0_4 Depth=1
	movq	-32(%rbp), %rax
	shrq	$12, %rax
	addq	$1, %rax
	shlq	$12, %rax
	movq	%rax, -32(%rbp)
.LBB0_7:                                #   in Loop: Header=BB0_4 Depth=1
	movl	-12(%rbp), %eax
	movl	%eax, %ecx
	movq	-8(%rbp), %rdx
	imulq	$24, %rcx, %rcx
	addq	48(%rdx), %rcx
	movzbl	16(%rcx), %eax
	cmpl	$1, %eax
	jne	.LBB0_9
# BB#8:                                 #   in Loop: Header=BB0_4 Depth=1
	movq	-24(%rbp), %rdi
	movq	-32(%rbp), %rsi
	callq	MemMan_MarkFree
.LBB0_9:                                #   in Loop: Header=BB0_4 Depth=1
	jmp	.LBB0_10
.LBB0_10:                               #   in Loop: Header=BB0_4 Depth=1
	movl	-12(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -12(%rbp)
	jmp	.LBB0_4
.LBB0_11:
	movq	-8(%rbp), %rax
	movq	56(%rax), %rdi
	callq	GetPhysicalAddress
	movq	-8(%rbp), %rdi
	movl	64(%rdi), %ecx
	movq	-8(%rbp), %rdi
	imull	72(%rdi), %ecx
	movl	%ecx, %ecx
	movl	%ecx, %esi
	movq	%rax, %rdi
	callq	MemMan_MarkUsed
	movq	$_region_kernel_start_, %rax
	movq	$_region_kernel_end_, %rsi
	subq	%rax, %rsi
	addq	$4096, %rsi             # imm = 0x1000
	movq	%rax, %rdi
	callq	MemMan_MarkUsed
	xorl	%ecx, %ecx
	movl	%ecx, %eax
	movl	$2097152, %ecx          # imm = 0x200000
	movl	%ecx, %esi
	xorl	%ecx, %ecx
	movl	%ecx, %edi
	movl	$2097152, %ecx          # imm = 0x200000
	movl	%ecx, %edx
	movq	%rsi, -64(%rbp)         # 8-byte Spill
	movq	%rdx, %rsi
	movq	%rax, -72(%rbp)         # 8-byte Spill
	callq	MemMan_MarkUsed
	xorl	%eax, %eax
	addq	$80, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	MemMan_Initialize, .Lfunc_end0-MemMan_Initialize
	.cfi_endproc

	.globl	MemMan_MarkFree
	.align	16, 0x90
	.type	MemMan_MarkFree,@function
MemMan_MarkFree:                        # @MemMan_MarkFree
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.LBB1_2
# BB#1:
	jmp	.LBB1_8
.LBB1_2:
	movq	-16(%rbp), %rax
	andq	$4095, %rax             # imm = 0xFFF
	cmpq	$0, %rax
	je	.LBB1_4
# BB#3:
	movq	-16(%rbp), %rax
	shrq	$12, %rax
	addq	$1, %rax
	shlq	$12, %rax
	movq	%rax, -16(%rbp)
.LBB1_4:
	movq	-8(%rbp), %rax
	shrq	$12, %rax
	shlq	$12, %rax
	movq	%rax, -8(%rbp)
	movq	$0, -24(%rbp)
.LBB1_5:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	-16(%rbp), %rcx
	shrq	$12, %rcx
	cmpq	%rcx, %rax
	jae	.LBB1_8
# BB#6:                                 #   in Loop: Header=BB1_5 Depth=1
	movq	-8(%rbp), %rdi
	callq	MemMan_SetPageFree
	movq	-8(%rbp), %rdi
	addq	$4096, %rdi             # imm = 0x1000
	movq	%rdi, -8(%rbp)
# BB#7:                                 #   in Loop: Header=BB1_5 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB1_5
.LBB1_8:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	MemMan_MarkFree, .Lfunc_end1-MemMan_MarkFree
	.cfi_endproc

	.globl	MemMan_MarkUsed
	.align	16, 0x90
	.type	MemMan_MarkUsed,@function
MemMan_MarkUsed:                        # @MemMan_MarkUsed
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.LBB2_2
# BB#1:
	jmp	.LBB2_8
.LBB2_2:
	movq	-16(%rbp), %rax
	andq	$4095, %rax             # imm = 0xFFF
	cmpq	$0, %rax
	je	.LBB2_4
# BB#3:
	movq	-16(%rbp), %rax
	shrq	$12, %rax
	addq	$1, %rax
	shlq	$12, %rax
	movq	%rax, -16(%rbp)
.LBB2_4:
	movq	-8(%rbp), %rax
	shrq	$12, %rax
	shlq	$12, %rax
	movq	%rax, -8(%rbp)
	movq	$0, -24(%rbp)
.LBB2_5:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	-16(%rbp), %rcx
	shrq	$12, %rcx
	cmpq	%rcx, %rax
	jae	.LBB2_8
# BB#6:                                 #   in Loop: Header=BB2_5 Depth=1
	movq	-8(%rbp), %rdi
	callq	MemMan_SetPageUsed
	movq	-8(%rbp), %rdi
	addq	$4096, %rdi             # imm = 0x1000
	movq	%rdi, -8(%rbp)
# BB#7:                                 #   in Loop: Header=BB2_5 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB2_5
.LBB2_8:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	MemMan_MarkUsed, .Lfunc_end2-MemMan_MarkUsed
	.cfi_endproc

	.align	16, 0x90
	.type	MemMan_SetPageUsed,@function
MemMan_SetPageUsed:                     # @MemMan_SetPageUsed
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
	movq	%rdi, -8(%rbp)
	movq	%rdi, %rax
	shrq	$12, %rax
	movb	%al, %cl
	movl	$1, %edx
	shll	%cl, %edx
	shrq	$17, %rdi
	movq	KB4_Blocks_Bitmap, %rax
	movl	(%rax,%rdi,4), %esi
	orl	%edx, %esi
	movl	%esi, (%rax,%rdi,4)
	movq	freePageCount, %rax
	decq	%rax
	movq	%rax, freePageCount
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	MemMan_SetPageUsed, .Lfunc_end3-MemMan_SetPageUsed
	.cfi_endproc

	.align	16, 0x90
	.type	MemMan_SetPageFree,@function
MemMan_SetPageFree:                     # @MemMan_SetPageFree
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
	pushq	%rax
	movq	%rdi, -8(%rbp)
	movq	%rdi, %rax
	shrq	$12, %rax
	movb	%al, %cl
	movl	$-2, %edx
	roll	%cl, %edx
	shrq	$17, %rdi
	movq	KB4_Blocks_Bitmap, %rax
	movl	(%rax,%rdi,4), %esi
	andl	%edx, %esi
	movl	%esi, (%rax,%rdi,4)
	movq	freePageCount, %rax
	incq	%rax
	movq	%rax, freePageCount
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	MemMan_SetPageFree, .Lfunc_end4-MemMan_SetPageFree
	.cfi_endproc

	.globl	MemMan_Alloc
	.align	16, 0x90
	.type	MemMan_Alloc,@function
MemMan_Alloc:                           # @MemMan_Alloc
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
	movq	freePageCount, %rax
	cmpq	$0, %rax
	jne	.LBB5_2
# BB#1:
	movq	$0, -8(%rbp)
	jmp	.LBB5_12
.LBB5_2:
	jmp	.LBB5_3
.LBB5_3:                                # =>This Inner Loop Header: Depth=1
	movq	lastNonFullPage, %rax
	movq	KB4_Blocks_Bitmap, %rcx
	cmpl	$-1, (%rcx,%rax,4)
	jne	.LBB5_5
# BB#4:                                 #   in Loop: Header=BB5_3 Depth=1
	movq	lastNonFullPage, %rax
	incq	%rax
	movq	page_count, %rcx
	xorl	%edx, %edx
                                        # kill: RDX<def> EDX<kill>
	divq	%rcx
	movq	%rdx, lastNonFullPage
	jmp	.LBB5_3
.LBB5_5:
	movq	lastNonFullPage, %rax
	movq	KB4_Blocks_Bitmap, %rcx
	movl	(%rcx,%rax,4), %edx
	xorl	$-1, %edx
	movl	%edx, -12(%rbp)
	movl	$0, -16(%rbp)
.LBB5_6:                                # =>This Inner Loop Header: Depth=1
	cmpl	$32, -16(%rbp)
	jge	.LBB5_11
# BB#7:                                 #   in Loop: Header=BB5_6 Depth=1
	movl	-12(%rbp), %eax
	movl	-16(%rbp), %ecx
                                        # kill: CL<def> ECX<kill>
	shrl	%cl, %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB5_9
# BB#8:
	movq	lastNonFullPage, %rax
	shlq	$17, %rax
	movslq	-16(%rbp), %rcx
	shlq	$12, %rcx
	addq	%rcx, %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rdi
	callq	MemMan_SetPageUsed
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB5_12
.LBB5_9:                                #   in Loop: Header=BB5_6 Depth=1
	jmp	.LBB5_10
.LBB5_10:                               #   in Loop: Header=BB5_6 Depth=1
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	jmp	.LBB5_6
.LBB5_11:
	movq	$-1, -8(%rbp)
.LBB5_12:
	movq	-8(%rbp), %rax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	MemMan_Alloc, .Lfunc_end5-MemMan_Alloc
	.cfi_endproc

	.globl	MemMan_Alloc2MiBPage
	.align	16, 0x90
	.type	MemMan_Alloc2MiBPage,@function
MemMan_Alloc2MiBPage:                   # @MemMan_Alloc2MiBPage
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
	movl	$512, %eax              # imm = 0x200
	movl	$512, %edi              # imm = 0x200
	movl	%eax, -4(%rbp)          # 4-byte Spill
	callq	MemMan_Alloc4KiBPageCont
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	MemMan_Alloc2MiBPage, .Lfunc_end6-MemMan_Alloc2MiBPage
	.cfi_endproc

	.globl	MemMan_Alloc4KiBPageCont
	.align	16, 0x90
	.type	MemMan_Alloc4KiBPageCont,@function
MemMan_Alloc4KiBPageCont:               # @MemMan_Alloc4KiBPageCont
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
	subq	$48, %rsp
	movl	%edi, -12(%rbp)
	movq	freePageCount, %rax
	cmpq	$0, %rax
	jne	.LBB7_2
# BB#1:
	movq	$0, -8(%rbp)
	jmp	.LBB7_20
.LBB7_2:
	movl	$0, -16(%rbp)
	movq	$0, -24(%rbp)
	movl	$0, -28(%rbp)
	movl	$0, -32(%rbp)
.LBB7_3:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB7_5 Depth 2
	movl	-32(%rbp), %eax
	movl	KB4_Blocks_Count, %ecx
	cmpl	%ecx, %eax
	jae	.LBB7_17
# BB#4:                                 #   in Loop: Header=BB7_3 Depth=1
	movl	-32(%rbp), %eax
	movl	%eax, %ecx
	movq	KB4_Blocks_Bitmap, %rdx
	movl	(%rdx,%rcx,4), %eax
	xorl	$-1, %eax
	movl	%eax, -36(%rbp)
	movl	$0, -40(%rbp)
.LBB7_5:                                #   Parent Loop BB7_3 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	cmpl	$32, -40(%rbp)
	jge	.LBB7_15
# BB#6:                                 #   in Loop: Header=BB7_5 Depth=2
	movl	-16(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jne	.LBB7_8
# BB#7:                                 #   in Loop: Header=BB7_3 Depth=1
	jmp	.LBB7_15
.LBB7_8:                                #   in Loop: Header=BB7_5 Depth=2
	movl	-36(%rbp), %eax
	movl	-40(%rbp), %ecx
                                        # kill: CL<def> ECX<kill>
	shrl	%cl, %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB7_12
# BB#9:                                 #   in Loop: Header=BB7_5 Depth=2
	cmpl	$0, -16(%rbp)
	jne	.LBB7_11
# BB#10:                                #   in Loop: Header=BB7_5 Depth=2
	movl	-32(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-32(%rbp), %eax
	movl	%eax, %ecx
	shlq	$17, %rcx
	movslq	-40(%rbp), %rdx
	shlq	$12, %rdx
	addq	%rdx, %rcx
	movq	%rcx, -24(%rbp)
.LBB7_11:                               #   in Loop: Header=BB7_5 Depth=2
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	jmp	.LBB7_13
.LBB7_12:                               #   in Loop: Header=BB7_5 Depth=2
	movl	$0, -16(%rbp)
.LBB7_13:                               #   in Loop: Header=BB7_5 Depth=2
	jmp	.LBB7_14
.LBB7_14:                               #   in Loop: Header=BB7_5 Depth=2
	movl	-40(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -40(%rbp)
	jmp	.LBB7_5
.LBB7_15:                               #   in Loop: Header=BB7_3 Depth=1
	jmp	.LBB7_16
.LBB7_16:                               #   in Loop: Header=BB7_3 Depth=1
	movl	-32(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -32(%rbp)
	jmp	.LBB7_3
.LBB7_17:
	movl	-16(%rbp), %eax
	cmpl	-12(%rbp), %eax
	je	.LBB7_19
# BB#18:
	movq	$0, -8(%rbp)
	jmp	.LBB7_20
.LBB7_19:
	movq	-24(%rbp), %rdi
	movslq	-12(%rbp), %rax
	shlq	$12, %rax
	movq	%rax, %rsi
	callq	MemMan_MarkUsed
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB7_20:
	movq	-8(%rbp), %rax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end7:
	.size	MemMan_Alloc4KiBPageCont, .Lfunc_end7-MemMan_Alloc4KiBPageCont
	.cfi_endproc

	.globl	MemMan_Alloc2MiBPageCont
	.align	16, 0x90
	.type	MemMan_Alloc2MiBPageCont,@function
MemMan_Alloc2MiBPageCont:               # @MemMan_Alloc2MiBPageCont
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
	movl	%edi, -4(%rbp)
	movslq	-4(%rbp), %rax
	shlq	$9, %rax
	movl	%eax, %edi
	callq	MemMan_Alloc4KiBPageCont
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end8:
	.size	MemMan_Alloc2MiBPageCont, .Lfunc_end8-MemMan_Alloc2MiBPageCont
	.cfi_endproc

	.globl	MemMan_Free
	.align	16, 0x90
	.type	MemMan_Free,@function
MemMan_Free:                            # @MemMan_Free
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
	movq	%rdi, -8(%rbp)
	andq	$-4096, %rdi            # imm = 0xFFFFFFFFFFFFF000
	movq	%rdi, -8(%rbp)
	callq	MemMan_SetPageFree
	movq	-8(%rbp), %rdi
	shrq	$17, %rdi
	movq	%rdi, lastNonFullPage
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end9:
	.size	MemMan_Free, .Lfunc_end9-MemMan_Free
	.cfi_endproc

	.globl	MemMan_FreeCont
	.align	16, 0x90
	.type	MemMan_FreeCont,@function
MemMan_FreeCont:                        # @MemMan_FreeCont
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
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdi
	movslq	-12(%rbp), %rax
	shlq	$12, %rax
	movq	%rax, %rsi
	callq	MemMan_MarkFree
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end10:
	.size	MemMan_FreeCont, .Lfunc_end10-MemMan_FreeCont
	.cfi_endproc

	.type	memory_size,@object     # @memory_size
	.local	memory_size
	.comm	memory_size,8,8
	.type	page_count,@object      # @page_count
	.local	page_count
	.comm	page_count,8,8
	.type	freePageCount,@object   # @freePageCount
	.local	freePageCount
	.comm	freePageCount,8,8
	.type	lastNonFullPage,@object # @lastNonFullPage
	.local	lastNonFullPage
	.comm	lastNonFullPage,8,8
	.type	KB4_Blocks_Count,@object # @KB4_Blocks_Count
	.local	KB4_Blocks_Count
	.comm	KB4_Blocks_Count,4,4
	.type	KB4_Blocks_Bitmap,@object # @KB4_Blocks_Bitmap
	.local	KB4_Blocks_Bitmap
	.comm	KB4_Blocks_Bitmap,8,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
