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
	subq	$144, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movq	%rdi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movl	(%rdi), %eax
	movl	%eax, -20(%rbp)
	movq	$bootInfo, %rdi
	movl	$80, %eax
	movl	%eax, %edx
	xorl	%esi, %esi
	callq	memset
	movl	$0, -24(%rbp)
	movl	$8, -28(%rbp)
	movq	%rax, -104(%rbp)        # 8-byte Spill
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_4 Depth 2
	movl	-28(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jae	.LBB0_15
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movl	(%rdx,%rcx), %eax
	movl	%eax, -32(%rbp)
	testl	%eax, %eax
	movl	%eax, -108(%rbp)        # 4-byte Spill
	je	.LBB0_11
	jmp	.LBB0_16
.LBB0_16:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-108(%rbp), %eax        # 4-byte Reload
	subl	$6, %eax
	movl	%eax, -112(%rbp)        # 4-byte Spill
	je	.LBB0_3
	jmp	.LBB0_17
.LBB0_17:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-108(%rbp), %eax        # 4-byte Reload
	subl	$8, %eax
	movl	%eax, -116(%rbp)        # 4-byte Spill
	je	.LBB0_8
	jmp	.LBB0_18
.LBB0_18:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-108(%rbp), %eax        # 4-byte Reload
	subl	$9, %eax
	movl	%eax, -120(%rbp)        # 4-byte Spill
	je	.LBB0_9
	jmp	.LBB0_19
.LBB0_19:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-108(%rbp), %eax        # 4-byte Reload
	addl	$-14, %eax
	subl	$2, %eax
	movl	%eax, -124(%rbp)        # 4-byte Spill
	jb	.LBB0_10
	jmp	.LBB0_12
.LBB0_3:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	addq	-16(%rbp), %rcx
	movq	%rcx, -40(%rbp)
	movq	-40(%rbp), %rcx
	movl	4(%rcx), %eax
	subl	$16, %eax
	movq	-40(%rbp), %rcx
	xorl	%edx, %edx
	divl	8(%rcx)
	movl	%eax, -44(%rbp)
	movslq	-44(%rbp), %rcx
	imulq	$24, %rcx, %rdi
	callq	bootstrap_malloc
	movq	%rax, -56(%rbp)
	movl	$0, -60(%rbp)
.LBB0_4:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-60(%rbp), %eax
	movq	-40(%rbp), %rcx
	movl	4(%rcx), %edx
	subl	$16, %edx
	cmpl	%edx, %eax
	jae	.LBB0_7
# BB#5:                                 #   in Loop: Header=BB0_4 Depth=2
	movq	-40(%rbp), %rax
	movl	-60(%rbp), %ecx
	movl	%ecx, %edx
	leaq	16(%rax,%rdx), %rsi
	movq	%rsi, -72(%rbp)
	movq	16(%rax,%rdx), %rsi
	movq	24(%rax,%rdx), %rax
	addq	%rax, %rsi
	movq	%rsi, bootInfo
	movq	-72(%rbp), %rax
	movq	(%rax), %rax
	movl	-60(%rbp), %ecx
	movq	-40(%rbp), %rdx
	movq	%rax, -136(%rbp)        # 8-byte Spill
	movl	%ecx, %eax
	xorl	%ecx, %ecx
	movq	%rdx, -144(%rbp)        # 8-byte Spill
	movl	%ecx, %edx
	movq	-144(%rbp), %rsi        # 8-byte Reload
	divl	8(%rsi)
	movl	%eax, %eax
	movl	%eax, %edi
	imulq	$24, %rdi, %rdi
	addq	-56(%rbp), %rdi
	movq	-136(%rbp), %r8         # 8-byte Reload
	movq	%r8, (%rdi)
	movq	-72(%rbp), %rdi
	movq	8(%rdi), %rdi
	movl	-60(%rbp), %eax
	movq	-40(%rbp), %r8
	xorl	%ecx, %ecx
	movl	%ecx, %edx
	divl	8(%r8)
	movl	%eax, %eax
	movl	%eax, %r8d
	imulq	$24, %r8, %r8
	addq	-56(%rbp), %r8
	movq	%rdi, 8(%r8)
	movq	-72(%rbp), %rdi
	movl	16(%rdi), %eax
	movb	%al, %r9b
	movl	-60(%rbp), %eax
	movq	-40(%rbp), %rdi
	xorl	%ecx, %ecx
	movl	%ecx, %edx
	divl	8(%rdi)
	movl	%eax, %eax
	movl	%eax, %edi
	imulq	$24, %rdi, %rdi
	addq	-56(%rbp), %rdi
	movb	%r9b, 16(%rdi)
# BB#6:                                 #   in Loop: Header=BB0_4 Depth=2
	movq	-40(%rbp), %rax
	movl	8(%rax), %ecx
	addl	-60(%rbp), %ecx
	movl	%ecx, -60(%rbp)
	jmp	.LBB0_4
.LBB0_7:                                #   in Loop: Header=BB0_1 Depth=1
	movq	-56(%rbp), %rax
	movq	%rax, bootInfo+48
	movl	-44(%rbp), %ecx
	shll	$3, %ecx
	movl	%ecx, %eax
	leal	(%rax,%rax,2), %ecx
	movl	%ecx, bootInfo+28
	jmp	.LBB0_12
.LBB0_8:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movq	%rdx, %rsi
	addq	%rcx, %rsi
	movq	%rsi, -80(%rbp)
	movq	8(%rdx,%rcx), %rcx
	movq	%rcx, bootInfo+56
	movq	-80(%rbp), %rcx
	movl	16(%rcx), %eax
	movl	%eax, bootInfo+64
	movq	-80(%rbp), %rcx
	movl	20(%rcx), %eax
	movl	%eax, bootInfo+68
	movq	-80(%rbp), %rcx
	movl	24(%rcx), %eax
	movl	%eax, bootInfo+72
	movq	-80(%rbp), %rcx
	movb	28(%rcx), %dil
	movb	%dil, bootInfo+76
	jmp	.LBB0_12
.LBB0_9:                                #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movq	%rdx, %rsi
	addq	%rcx, %rsi
	movq	%rsi, -88(%rbp)
	movl	(%rdx,%rcx), %eax
	movl	%eax, bootInfo+8
	movq	-88(%rbp), %rcx
	movl	4(%rcx), %eax
	movl	%eax, bootInfo+12
	movq	-88(%rbp), %rcx
	movl	8(%rcx), %eax
	movl	%eax, bootInfo+16
	movq	-88(%rbp), %rcx
	movl	12(%rcx), %eax
	movl	%eax, bootInfo+20
	movq	-88(%rbp), %rcx
	movl	16(%rcx), %eax
	movl	%eax, bootInfo+24
	movq	-88(%rbp), %rcx
	addq	$20, %rcx
	movq	%rcx, bootInfo+32
	jmp	.LBB0_12
.LBB0_10:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movq	%rdx, %rsi
	addq	%rcx, %rsi
	movq	%rsi, -96(%rbp)
	leaq	8(%rdx,%rcx), %rcx
	movq	%rcx, bootInfo+40
	jmp	.LBB0_12
.LBB0_11:                               #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_12
.LBB0_12:                               #   in Loop: Header=BB0_1 Depth=1
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
	je	.LBB0_14
# BB#13:                                #   in Loop: Header=BB0_1 Depth=1
	movl	$8, %eax
	movl	-24(%rbp), %ecx
	andl	$7, %ecx
	subl	%ecx, %eax
	addl	-24(%rbp), %eax
	movl	%eax, -24(%rbp)
.LBB0_14:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-24(%rbp), %eax
	addl	-28(%rbp), %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB0_1
.LBB0_15:
	addq	$144, %rsp
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
	.comm	bootInfo,80,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
