	.text
	.file	"page_manager/phys_mem_manager.c"
	.globl	pmem_Initialize
	.align	16, 0x90
	.type	pmem_Initialize,@function
pmem_Initialize:                        # @pmem_Initialize
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
	subq	$64, %rsp
	xorl	%eax, %eax
	movl	%eax, -32(%rbp)         # 4-byte Spill
	callq	GetBootInfo
	movq	%rax, -8(%rbp)
	movq	(%rax), %rax
	shlq	$10, %rax
	movq	%rax, memory_size
	shrq	$12, %rax
	movq	%rax, page_count
	movq	%rax, totalPageCount
	movq	%rax, freePageCount
	movq	$0, lastNonFullPage
	movq	memory_size, %rax
	shrq	$17, %rax
	movq	%rax, KB4_Blocks_Count
	shlq	$2, %rax
	movq	%rax, %rdi
	callq	bootstrap_malloc
	movq	%rax, KB4_Blocks_Bitmap
	movq	KB4_Blocks_Count, %rax
	movabsq	$-3689348814741910323, %rdi # imm = 0xCCCCCCCCCCCCCCCD
	mulq	%rdi
	shrq	$2, %rdx
	movq	%rdx, KB4_Blocks_FreeBitCount_EntryNum
	shlq	$2, %rdx
	movq	%rdx, %rdi
	callq	bootstrap_malloc
	movq	%rax, KB4_Blocks_FreeBitCount
	movq	KB4_Blocks_Bitmap, %rdi
	movq	KB4_Blocks_Count, %rax
	shlq	$2, %rax
	xorl	%esi, %esi
	movq	%rax, %rdx
	callq	memset
	movq	$0, -16(%rbp)
	movq	%rax, -40(%rbp)         # 8-byte Spill
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	movq	KB4_Blocks_Count, %rcx
	cmpq	%rcx, %rax
	jae	.LBB0_4
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movq	-16(%rbp), %rax
	movabsq	$-3689348814741910323, %rcx # imm = 0xCCCCCCCCCCCCCCCD
	movq	%rax, -48(%rbp)         # 8-byte Spill
	mulq	%rcx
	shrq	$2, %rdx
	movq	KB4_Blocks_FreeBitCount, %rax
	movl	(%rax,%rdx,4), %esi
	movl	%edx, %edi
	movl	%edi, %ecx
	leal	(%rcx,%rcx,4), %edi
	movq	-48(%rbp), %rcx         # 8-byte Reload
	movl	%ecx, %r8d
	subl	%edi, %r8d
	addl	%r8d, %r8d
	movl	%r8d, %r9d
	leal	(%r9,%r9,2), %edi
	movb	%dil, %r10b
	movl	$63, %edi
	movb	%r10b, %cl
	shll	%cl, %edi
	notl	%edi
	andl	%edi, %esi
	movl	$32, %edi
	movb	%r10b, %cl
	shll	%cl, %edi
	orl	%edi, %esi
	movl	%esi, (%rax,%rdx,4)
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movq	-16(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -16(%rbp)
	jmp	.LBB0_1
.LBB0_4:
	movq	-8(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, -24(%rbp)
	movl	$0, -28(%rbp)
.LBB0_5:                                # =>This Inner Loop Header: Depth=1
	movl	-28(%rbp), %eax
	movq	-8(%rbp), %rcx
	cmpl	16(%rcx), %eax
	jae	.LBB0_10
# BB#6:                                 #   in Loop: Header=BB0_5 Depth=1
	movq	-24(%rbp), %rax
	cmpl	$0, 20(%rax)
	je	.LBB0_8
# BB#7:                                 #   in Loop: Header=BB0_5 Depth=1
	movq	-24(%rbp), %rax
	movl	12(%rax), %ecx
	movl	%ecx, %edi
	movq	-24(%rbp), %rax
	movl	20(%rax), %ecx
	movl	%ecx, %esi
	callq	physMemMan_MarkUsed
.LBB0_8:                                #   in Loop: Header=BB0_5 Depth=1
	movq	-24(%rbp), %rax
	addq	$40, %rax
	movq	%rax, -24(%rbp)
# BB#9:                                 #   in Loop: Header=BB0_5 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB0_5
.LBB0_10:
	xorl	%eax, %eax
	movl	%eax, %ecx
	movl	$1048576, %eax          # imm = 0x100000
	movl	%eax, %edx
	xorl	%eax, %eax
	movl	%eax, %edi
	movl	$1048576, %eax          # imm = 0x100000
	movl	%eax, %esi
	movq	%rcx, -56(%rbp)         # 8-byte Spill
	movq	%rdx, -64(%rbp)         # 8-byte Spill
	callq	physMemMan_MarkUsed
	xorl	%eax, %eax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	pmem_Initialize, .Lfunc_end0-pmem_Initialize
	.cfi_endproc

	.globl	physMemMan_MarkUsed
	.align	16, 0x90
	.type	physMemMan_MarkUsed,@function
physMemMan_MarkUsed:                    # @physMemMan_MarkUsed
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
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	callq	MemMan_MarkKB4Used
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	physMemMan_MarkUsed, .Lfunc_end1-physMemMan_MarkUsed
	.cfi_endproc

	.globl	MemMan_MarkKB4Used
	.align	16, 0x90
	.type	MemMan_MarkKB4Used,@function
MemMan_MarkKB4Used:                     # @MemMan_MarkKB4Used
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
	subq	$88, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.LBB2_2
# BB#1:
	jmp	.LBB2_10
.LBB2_2:
	movq	-8(%rbp), %rax
	shrq	$12, %rax
	shlq	$12, %rax
	movq	%rax, -24(%rbp)
	movq	-8(%rbp), %rax
	subq	-24(%rbp), %rax
	addq	-16(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	-24(%rbp), %rax
	shrq	$12, %rax
	movq	%rax, -32(%rbp)
	movq	-16(%rbp), %rax
	shrq	$12, %rax
	movq	%rax, -40(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, -48(%rbp)
.LBB2_3:                                # =>This Inner Loop Header: Depth=1
	movq	-48(%rbp), %rax
	movq	-32(%rbp), %rcx
	addq	-40(%rbp), %rcx
	cmpq	%rcx, %rax
	jae	.LBB2_10
# BB#4:                                 #   in Loop: Header=BB2_3 Depth=1
	movq	-48(%rbp), %rax
	andq	$31, %rax
	cmpq	$0, %rax
	jne	.LBB2_7
# BB#5:                                 #   in Loop: Header=BB2_3 Depth=1
	movq	-48(%rbp), %rax
	addq	$32, %rax
	movq	-32(%rbp), %rcx
	addq	-40(%rbp), %rcx
	cmpq	%rcx, %rax
	jae	.LBB2_7
# BB#6:                                 #   in Loop: Header=BB2_3 Depth=1
	movq	-48(%rbp), %rax
	shrq	$5, %rax
	movq	KB4_Blocks_Bitmap, %rcx
	movl	$-1, (%rcx,%rax,4)
	movq	-48(%rbp), %rax
	shrq	$5, %rax
	movabsq	$-3689348814741910323, %rcx # imm = 0xCCCCCCCCCCCCCCCD
	movq	%rax, -56(%rbp)         # 8-byte Spill
	mulq	%rcx
	movq	%rdx, %rax
	andq	$-4, %rax
	movq	KB4_Blocks_FreeBitCount, %rcx
	movl	(%rcx,%rax), %esi
	movl	%edx, %edi
	shrl	$2, %edi
	movl	%edi, %edx
	leal	(%rdx,%rdx,4), %edi
	movq	-56(%rbp), %rdx         # 8-byte Reload
	movl	%edx, %r8d
	subl	%edi, %r8d
	addl	%r8d, %r8d
	movl	%r8d, %r9d
	leal	(%r9,%r9,2), %edi
	movb	%dil, %r10b
	movl	$63, %edi
	movq	%rcx, -64(%rbp)         # 8-byte Spill
	movb	%r10b, %cl
	shll	%cl, %edi
	notl	%edi
	andl	%edi, %esi
	movq	-64(%rbp), %r9          # 8-byte Reload
	movl	%esi, (%r9,%rax)
	movq	freePageCount, %rax
	addq	$-32, %rax
	movq	%rax, freePageCount
	movq	-48(%rbp), %rax
	addq	$31, %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB2_8
.LBB2_7:                                #   in Loop: Header=BB2_3 Depth=1
	movq	-48(%rbp), %rax
	movq	%rax, %rcx
	shrq	$5, %rcx
	movq	KB4_Blocks_Bitmap, %rdx
	movl	(%rdx,%rcx,4), %esi
	movb	%al, %dil
	movl	$1, %r8d
	movq	%rcx, -72(%rbp)         # 8-byte Spill
	movb	%dil, %cl
	shll	%cl, %r8d
	orl	%r8d, %esi
	movq	-72(%rbp), %rax         # 8-byte Reload
	movl	%esi, (%rdx,%rax,4)
	movq	-48(%rbp), %rdx
	shrq	$5, %rdx
	movabsq	$-3689348814741910323, %r9 # imm = 0xCCCCCCCCCCCCCCCD
	movq	%rdx, %rax
	movq	%rdx, -80(%rbp)         # 8-byte Spill
	mulq	%r9
	movq	%rdx, %rax
	andq	$-4, %rax
	movq	KB4_Blocks_FreeBitCount, %r9
	movl	(%r9,%rax), %esi
	movl	%edx, %r8d
	shrl	$2, %r8d
	movl	%r8d, %edx
	leal	(%rdx,%rdx,4), %r8d
	movq	-80(%rbp), %rdx         # 8-byte Reload
	movl	%edx, %r10d
	subl	%r8d, %r10d
	addl	%r10d, %r10d
	movl	%r10d, %r11d
	leal	(%r11,%r11,2), %r8d
	movb	%r8b, %cl
	movl	$63, %r8d
	movb	%cl, -81(%rbp)          # 1-byte Spill
	shll	%cl, %r8d
	notl	%r8d
	movl	%esi, %r10d
	andl	%r8d, %r10d
	movb	-81(%rbp), %cl          # 1-byte Reload
	shrl	%cl, %esi
	decl	%esi
	andl	$63, %esi
	movb	-81(%rbp), %cl          # 1-byte Reload
	shll	%cl, %esi
	orl	%esi, %r10d
	movl	%r10d, (%r9,%rax)
	movq	freePageCount, %rax
	decq	%rax
	movq	%rax, freePageCount
	movq	-24(%rbp), %rax
	addq	$4096, %rax             # imm = 0x1000
	movq	%rax, -24(%rbp)
.LBB2_8:                                #   in Loop: Header=BB2_3 Depth=1
	jmp	.LBB2_9
.LBB2_9:                                #   in Loop: Header=BB2_3 Depth=1
	movq	-48(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB2_3
.LBB2_10:
	addq	$88, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	MemMan_MarkKB4Used, .Lfunc_end2-MemMan_MarkKB4Used
	.cfi_endproc

	.globl	physMemMan_Alloc
	.align	16, 0x90
	.type	physMemMan_Alloc,@function
physMemMan_Alloc:                       # @physMemMan_Alloc
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
	subq	$64, %rsp
	movl	$0, -12(%rbp)
	movq	lastNonFullPage, %rax
	movq	%rax, -24(%rbp)
.LBB3_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	KB4_Blocks_Count, %rcx
	cmpq	%rcx, %rax
	jae	.LBB3_6
# BB#2:                                 #   in Loop: Header=BB3_1 Depth=1
	movq	-24(%rbp), %rax
	movq	KB4_Blocks_Bitmap, %rcx
	cmpl	$-1, (%rcx,%rax,4)
	jae	.LBB3_4
# BB#3:
	movq	-24(%rbp), %rax
	movq	KB4_Blocks_Bitmap, %rcx
	movl	(%rcx,%rax,4), %edi
	callq	find_first_zero
	movl	%eax, -12(%rbp)
	movq	-24(%rbp), %rcx
	movq	KB4_Blocks_Bitmap, %rdx
	movl	(%rdx,%rcx,4), %eax
	movb	-12(%rbp), %sil
	movl	$1, %edi
	movq	%rcx, -40(%rbp)         # 8-byte Spill
	movb	%sil, %cl
	shll	%cl, %edi
	orl	%edi, %eax
	movq	-40(%rbp), %r8          # 8-byte Reload
	movl	%eax, (%rdx,%r8,4)
	jmp	.LBB3_6
.LBB3_4:                                #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_5
.LBB3_5:                                #   in Loop: Header=BB3_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB3_1
.LBB3_6:
	movq	-24(%rbp), %rax
	movq	KB4_Blocks_Count, %rcx
	cmpq	%rcx, %rax
	jb	.LBB3_8
# BB#7:
	movq	$0, -8(%rbp)
	jmp	.LBB3_12
.LBB3_8:
	movq	-24(%rbp), %rax
	movabsq	$-3689348814741910323, %rcx # imm = 0xCCCCCCCCCCCCCCCD
	movq	%rax, -48(%rbp)         # 8-byte Spill
	mulq	%rcx
	movq	%rdx, %rax
	andq	$-4, %rax
	movq	KB4_Blocks_FreeBitCount, %rcx
	movl	(%rcx,%rax), %esi
	movl	%edx, %edi
	shrl	$2, %edi
	movl	%edi, %edx
	leal	(%rdx,%rdx,4), %edi
	movq	-48(%rbp), %rdx         # 8-byte Reload
	movl	%edx, %r8d
	subl	%edi, %r8d
	addl	%r8d, %r8d
	movl	%r8d, %r9d
	leal	(%r9,%r9,2), %edi
	movb	%dil, %r10b
	movl	$63, %edi
	movq	%rcx, -56(%rbp)         # 8-byte Spill
	movb	%r10b, %cl
	shll	%cl, %edi
	notl	%edi
	movl	%esi, %r8d
	andl	%edi, %r8d
	movb	%r10b, %cl
	shrl	%cl, %esi
	decl	%esi
	andl	$63, %esi
	movb	%r10b, %cl
	shll	%cl, %esi
	orl	%esi, %r8d
	movq	-56(%rbp), %r9          # 8-byte Reload
	movl	%r8d, (%r9,%rax)
	movq	freePageCount, %rax
	decq	%rax
	movq	%rax, freePageCount
	movq	-24(%rbp), %rax
	shlq	$5, %rax
	movslq	-12(%rbp), %r11
	addq	%r11, %rax
	shlq	$12, %rax
	movq	%rax, -32(%rbp)
.LBB3_9:                                # =>This Inner Loop Header: Depth=1
	movl	$5, %eax
	movl	%eax, %ecx
	movq	lastNonFullPage, %rdx
	movabsq	$-3689348814741910323, %rsi # imm = 0xCCCCCCCCCCCCCCCD
	movq	%rdx, %rax
	movq	%rdx, -64(%rbp)         # 8-byte Spill
	mulq	%rsi
	andq	$-4, %rdx
	movq	KB4_Blocks_FreeBitCount, %rax
	movl	(%rax,%rdx), %edi
	movq	-64(%rbp), %rax         # 8-byte Reload
	xorl	%r8d, %r8d
	movl	%r8d, %edx
	divq	%rcx
	imulq	$6, %rdx, %rcx
	movl	%ecx, %r8d
	movl	%r8d, %ecx
                                        # kill: CL<def> ECX<kill>
	shrl	%cl, %edi
	andl	$63, %edi
	cmpl	$0, %edi
	jne	.LBB3_11
# BB#10:                                #   in Loop: Header=BB3_9 Depth=1
	movq	lastNonFullPage, %rax
	incq	%rax
	movq	KB4_Blocks_Count, %rcx
	xorl	%edx, %edx
                                        # kill: RDX<def> EDX<kill>
	divq	%rcx
	movq	%rdx, lastNonFullPage
	jmp	.LBB3_9
.LBB3_11:
	movq	-32(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB3_12:
	movq	-8(%rbp), %rax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	physMemMan_Alloc, .Lfunc_end3-physMemMan_Alloc
	.cfi_endproc

	.globl	physMemMan_Free
	.align	16, 0x90
	.type	physMemMan_Free,@function
physMemMan_Free:                        # @physMemMan_Free
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
	subq	$72, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdi
	shrq	$12, %rdi
	shlq	$12, %rdi
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rdi
	shrq	$12, %rdi
	movq	%rdi, -32(%rbp)
	movq	-32(%rbp), %rdi
	movq	%rdi, -40(%rbp)
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movq	-40(%rbp), %rax
	movq	-32(%rbp), %rcx
	addq	$1, %rcx
	cmpq	%rcx, %rax
	jae	.LBB4_6
# BB#2:                                 #   in Loop: Header=BB4_1 Depth=1
	movq	-40(%rbp), %rax
	movq	%rax, %rcx
	shrq	$5, %rcx
	movq	KB4_Blocks_Bitmap, %rdx
	movl	(%rdx,%rcx,4), %esi
	movb	%al, %dil
	movl	$-2, %r8d
	movq	%rcx, -56(%rbp)         # 8-byte Spill
	movb	%dil, %cl
	roll	%cl, %r8d
	andl	%r8d, %esi
	movq	-56(%rbp), %rax         # 8-byte Reload
	movl	%esi, (%rdx,%rax,4)
	movq	-40(%rbp), %rdx
	shrq	$5, %rdx
	movabsq	$-3689348814741910323, %r9 # imm = 0xCCCCCCCCCCCCCCCD
	movq	%rdx, %rax
	movq	%rdx, -64(%rbp)         # 8-byte Spill
	mulq	%r9
	movq	%rdx, %rax
	andq	$-4, %rax
	movq	KB4_Blocks_FreeBitCount, %r9
	movl	(%r9,%rax), %esi
	movl	%edx, %r8d
	shrl	$2, %r8d
	movl	%r8d, %edx
	leal	(%rdx,%rdx,4), %r8d
	movq	-64(%rbp), %rdx         # 8-byte Reload
	movl	%edx, %r10d
	subl	%r8d, %r10d
	addl	%r10d, %r10d
	movl	%r10d, %r11d
	leal	(%r11,%r11,2), %r8d
	movb	%r8b, %cl
	movl	$63, %r8d
	movb	%cl, -65(%rbp)          # 1-byte Spill
	shll	%cl, %r8d
	notl	%r8d
	movl	%esi, %r10d
	andl	%r8d, %r10d
	movb	-65(%rbp), %cl          # 1-byte Reload
	shrl	%cl, %esi
	incl	%esi
	andl	$63, %esi
	movb	-65(%rbp), %cl          # 1-byte Reload
	shll	%cl, %esi
	orl	%esi, %r10d
	movl	%r10d, (%r9,%rax)
	movq	freePageCount, %rax
	incq	%rax
	movq	%rax, freePageCount
	movq	-40(%rbp), %rax
	shrq	$5, %rax
	movq	%rax, -48(%rbp)
	movq	lastNonFullPage, %rax
	cmpq	-48(%rbp), %rax
	jbe	.LBB4_4
# BB#3:                                 #   in Loop: Header=BB4_1 Depth=1
	movq	-48(%rbp), %rax
	movq	%rax, lastNonFullPage
.LBB4_4:                                #   in Loop: Header=BB4_1 Depth=1
	jmp	.LBB4_5
.LBB4_5:                                #   in Loop: Header=BB4_1 Depth=1
	movq	-40(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -40(%rbp)
	jmp	.LBB4_1
.LBB4_6:
	addq	$72, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	physMemMan_Free, .Lfunc_end4-physMemMan_Free
	.cfi_endproc

	.type	memory_size,@object     # @memory_size
	.local	memory_size
	.comm	memory_size,8,8
	.type	page_count,@object      # @page_count
	.local	page_count
	.comm	page_count,8,8
	.type	totalPageCount,@object  # @totalPageCount
	.comm	totalPageCount,8,8
	.type	freePageCount,@object   # @freePageCount
	.comm	freePageCount,8,8
	.type	lastNonFullPage,@object # @lastNonFullPage
	.comm	lastNonFullPage,8,8
	.type	KB4_Blocks_Count,@object # @KB4_Blocks_Count
	.comm	KB4_Blocks_Count,8,8
	.type	KB4_Blocks_Bitmap,@object # @KB4_Blocks_Bitmap
	.comm	KB4_Blocks_Bitmap,8,8
	.type	KB4_Blocks_FreeBitCount_EntryNum,@object # @KB4_Blocks_FreeBitCount_EntryNum
	.comm	KB4_Blocks_FreeBitCount_EntryNum,8,8
	.type	KB4_Blocks_FreeBitCount,@object # @KB4_Blocks_FreeBitCount
	.comm	KB4_Blocks_FreeBitCount,8,8
	.type	MB2_Blocks_Bitmap,@object # @MB2_Blocks_Bitmap
	.comm	MB2_Blocks_Bitmap,8,8
	.type	MB2_Blocks_Count,@object # @MB2_Blocks_Count
	.comm	MB2_Blocks_Count,4,4

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
