	.text
	.file	"boot_information/multiboot2/parseMultiboot2.c"
	.globl	ParseAndSaveBootInformation
	.align	16, 0x90
	.type	ParseAndSaveBootInformation,@function
ParseAndSaveBootInformation:            # @ParseAndSaveBootInformation
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
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movq	%rdi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movl	(%rdi), %eax
	movl	%eax, -20(%rbp)
	movq	$bootInfo, %rdi
	movl	$72, %eax
	movl	%eax, %edx
	xorl	%esi, %esi
	callq	memset
	movl	$0, -24(%rbp)
	movl	$8, -28(%rbp)
	movq	%rax, -88(%rbp)         # 8-byte Spill
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_4 Depth 2
	movl	-28(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jae	.LBB0_17
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movl	(%rdx,%rcx), %eax
	movl	%eax, -32(%rbp)
	testl	%eax, %eax
	movl	%eax, -92(%rbp)         # 4-byte Spill
	je	.LBB0_13
	jmp	.LBB0_18
.LBB0_18:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-92(%rbp), %eax         # 4-byte Reload
	subl	$6, %eax
	movl	%eax, -96(%rbp)         # 4-byte Spill
	je	.LBB0_3
	jmp	.LBB0_19
.LBB0_19:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-92(%rbp), %eax         # 4-byte Reload
	subl	$8, %eax
	movl	%eax, -100(%rbp)        # 4-byte Spill
	je	.LBB0_10
	jmp	.LBB0_20
.LBB0_20:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-92(%rbp), %eax         # 4-byte Reload
	subl	$9, %eax
	movl	%eax, -104(%rbp)        # 4-byte Spill
	je	.LBB0_11
	jmp	.LBB0_21
.LBB0_21:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-92(%rbp), %eax         # 4-byte Reload
	addl	$-14, %eax
	subl	$2, %eax
	movl	%eax, -108(%rbp)        # 4-byte Spill
	jb	.LBB0_12
	jmp	.LBB0_14
.LBB0_3:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	addq	-16(%rbp), %rcx
	movq	%rcx, -40(%rbp)
	movl	$0, -44(%rbp)
.LBB0_4:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-44(%rbp), %eax
	movq	-40(%rbp), %rcx
	movl	4(%rcx), %edx
	subl	$16, %edx
	cmpl	%edx, %eax
	jae	.LBB0_9
# BB#5:                                 #   in Loop: Header=BB0_4 Depth=2
	movq	-40(%rbp), %rax
	addq	$16, %rax
	movl	-44(%rbp), %ecx
	movl	%ecx, %edx
	addq	%rdx, %rax
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rax
	cmpl	$1, 16(%rax)
	jne	.LBB0_7
# BB#6:                                 #   in Loop: Header=BB0_4 Depth=2
	movq	-56(%rbp), %rax
	movq	8(%rax), %rax
	movq	bootInfo, %rcx
	addq	%rax, %rcx
	movq	%rcx, bootInfo
.LBB0_7:                                #   in Loop: Header=BB0_4 Depth=2
	jmp	.LBB0_8
.LBB0_8:                                #   in Loop: Header=BB0_4 Depth=2
	movq	-40(%rbp), %rax
	movl	8(%rax), %ecx
	addl	-44(%rbp), %ecx
	movl	%ecx, -44(%rbp)
	jmp	.LBB0_4
.LBB0_9:                                #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_14
.LBB0_10:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movq	%rdx, %rsi
	addq	%rcx, %rsi
	movq	%rsi, -64(%rbp)
	movq	8(%rdx,%rcx), %rcx
	movq	%rcx, bootInfo+48
	movq	-64(%rbp), %rcx
	movl	16(%rcx), %eax
	movl	%eax, bootInfo+56
	movq	-64(%rbp), %rcx
	movl	20(%rcx), %eax
	movl	%eax, bootInfo+60
	movq	-64(%rbp), %rcx
	movl	24(%rcx), %eax
	movl	%eax, bootInfo+64
	movq	-64(%rbp), %rcx
	movb	28(%rcx), %dil
	movb	%dil, bootInfo+68
	jmp	.LBB0_14
.LBB0_11:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movq	%rdx, %rsi
	addq	%rcx, %rsi
	movq	%rsi, -72(%rbp)
	movl	(%rdx,%rcx), %eax
	movl	%eax, bootInfo+8
	movq	-72(%rbp), %rcx
	movl	4(%rcx), %eax
	movl	%eax, bootInfo+12
	movq	-72(%rbp), %rcx
	movl	8(%rcx), %eax
	movl	%eax, bootInfo+16
	movq	-72(%rbp), %rcx
	movl	12(%rcx), %eax
	movl	%eax, bootInfo+20
	movq	-72(%rbp), %rcx
	movl	16(%rcx), %eax
	movl	%eax, bootInfo+24
	movq	-72(%rbp), %rcx
	addq	$20, %rcx
	movq	%rcx, bootInfo+32
	jmp	.LBB0_14
.LBB0_12:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movq	%rdx, %rsi
	addq	%rcx, %rsi
	movq	%rsi, -80(%rbp)
	leaq	8(%rdx,%rcx), %rcx
	movq	%rcx, bootInfo+40
	jmp	.LBB0_14
.LBB0_13:                               #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_14
.LBB0_14:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	addl	$4, %eax
	movl	%eax, %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movl	(%rdx,%rcx), %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	andl	$7, %eax
	cmpl	$0, %eax
	je	.LBB0_16
# BB#15:                                #   in Loop: Header=BB0_1 Depth=1
	movl	$8, %eax
	movl	-24(%rbp), %ecx
	andl	$7, %ecx
	subl	%ecx, %eax
	addl	-24(%rbp), %eax
	movl	%eax, -24(%rbp)
.LBB0_16:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-24(%rbp), %eax
	addl	-28(%rbp), %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB0_1
.LBB0_17:
	addq	$112, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	ParseAndSaveBootInformation, .Lfunc_end0-ParseAndSaveBootInformation
	.cfi_endproc

	.globl	GetBootInfo
	.align	16, 0x90
	.type	GetBootInfo,@function
GetBootInfo:                            # @GetBootInfo
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
	movq	$bootInfo, %rax
	popq	%rbp
	retq
.Lfunc_end1:
	.size	GetBootInfo, .Lfunc_end1-GetBootInfo
	.cfi_endproc

	.type	bootInfo,@object        # @bootInfo
	.local	bootInfo
	.comm	bootInfo,72,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
