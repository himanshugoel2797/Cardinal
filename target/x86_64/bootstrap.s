	.text
	.file	"bootstrap.c"
	.globl	bootstrap_render
	.align	16, 0x90
	.type	bootstrap_render,@function
bootstrap_render:                       # @bootstrap_render
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
	movl	%edi, -4(%rbp)
	callq	GetBootInfo
	movq	%rax, -16(%rbp)
	movl	$0, -20(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movl	-20(%rbp), %eax
	movq	-16(%rbp), %rcx
	movl	64(%rcx), %edx
	movq	-16(%rbp), %rcx
	imull	56(%rcx), %edx
	cmpl	%edx, %eax
	jae	.LBB0_4
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-4(%rbp), %eax
	movq	-16(%rbp), %rcx
	movq	48(%rcx), %rcx
	movl	-20(%rbp), %edx
	movl	%edx, %esi
	addq	%rsi, %rcx
	movl	%eax, (%rcx)
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-20(%rbp), %eax
	addl	$4, %eax
	movl	%eax, -20(%rbp)
	jmp	.LBB0_1
.LBB0_4:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	bootstrap_render, .Lfunc_end0-bootstrap_render
	.cfi_endproc

	.globl	bootstrap_kernel_panic
	.align	16, 0x90
	.type	bootstrap_kernel_panic,@function
bootstrap_kernel_panic:                 # @bootstrap_kernel_panic
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
	movb	%dil, %al
	movb	%al, -1(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movl	$255, %eax
	movzbl	-1(%rbp), %ecx
	subl	%ecx, %eax
	orl	$16711680, %eax         # imm = 0xFF0000
	movl	%eax, %edi
	callq	bootstrap_render
	jmp	.LBB1_1
.Lfunc_end1:
	.size	bootstrap_kernel_panic, .Lfunc_end1-bootstrap_kernel_panic
	.cfi_endproc

	.section	.entry_point,"ax",@progbits
	.globl	bootstrap_kernel
	.align	16, 0x90
	.type	bootstrap_kernel,@function
bootstrap_kernel:                       # @bootstrap_kernel
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
	subq	$80, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	$0, -24(%rbp)
	#APP
	movq	%cr3, %rax
	#NO_APP
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	callq	GetVirtualAddress
	movq	%rax, -24(%rbp)
	movq	-8(%rbp), %rdi
	callq	ParseAndSaveBootInformation
	callq	GetBootInfo
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movabsq	$-8589934592, %rsi      # imm = 0xFFFFFFFE00000000
	addq	48(%rax), %rsi
	movq	%rsi, 48(%rax)
	cmpq	$920085129, -16(%rbp)   # imm = 0x36D76289
	je	.LBB2_2
# BB#1:
	movl	$255, %eax
	movl	$255, %edi
	movl	%eax, -60(%rbp)         # 4-byte Spill
	callq	bootstrap_kernel_panic
	jmp	.LBB2_11
.LBB2_2:
	movl	$0, -36(%rbp)
.LBB2_3:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB2_5 Depth 2
	movl	-36(%rbp), %eax
	movq	-32(%rbp), %rcx
	cmpl	64(%rcx), %eax
	jae	.LBB2_10
# BB#4:                                 #   in Loop: Header=BB2_3 Depth=1
	movl	$0, -40(%rbp)
.LBB2_5:                                #   Parent Loop BB2_3 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-40(%rbp), %eax
	movq	-32(%rbp), %rcx
	cmpl	56(%rcx), %eax
	jae	.LBB2_8
# BB#6:                                 #   in Loop: Header=BB2_5 Depth=2
	movl	-36(%rbp), %eax
	andl	$15, %eax
	shrl	$2, %eax
	cmpl	$0, %eax
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %eax
	imull	$16763904, %eax, %eax   # imm = 0xFFCC00
	movl	-40(%rbp), %edx
	andl	$63, %edx
	shrl	$4, %edx
	cmpl	$0, %edx
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %edx
	imull	$16763904, %edx, %edx   # imm = 0xFFCC00
	orl	%edx, %eax
	movq	-32(%rbp), %rsi
	movq	48(%rsi), %rsi
	movl	-40(%rbp), %edx
	movl	%edx, %edi
	addq	%rdi, %rsi
	movl	-36(%rbp), %edx
	movq	-32(%rbp), %rdi
	imull	56(%rdi), %edx
	movl	%edx, %edx
	movl	%edx, %edi
	addq	%rdi, %rsi
	movl	%eax, (%rsi)
# BB#7:                                 #   in Loop: Header=BB2_5 Depth=2
	movl	-40(%rbp), %eax
	addl	$4, %eax
	movl	%eax, -40(%rbp)
	jmp	.LBB2_5
.LBB2_8:                                #   in Loop: Header=BB2_3 Depth=1
	jmp	.LBB2_9
.LBB2_9:                                #   in Loop: Header=BB2_3 Depth=1
	movl	-36(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -36(%rbp)
	jmp	.LBB2_3
.LBB2_10:
	jmp	.LBB2_11
.LBB2_11:
	xorl	%eax, %eax
	movb	%al, %cl
	movb	%cl, %al
	callq	GDT_Initialize
	callq	IDT_Initialize
	callq	FPU_Initialize
	callq	ACPITables_Initialize
	movb	%al, -61(%rbp)          # 1-byte Spill
	callq	APIC_Initialize
	movl	%eax, -68(%rbp)         # 4-byte Spill
	callq	kernel_main
	movl	$16711680, -44(%rbp)    # imm = 0xFF0000
	movl	$16776960, -48(%rbp)    # imm = 0xFFFF00
	movl	$0, -52(%rbp)
.LBB2_12:                               # =>This Loop Header: Depth=1
                                        #     Child Loop BB2_13 Depth 2
	movl	$0, -56(%rbp)
.LBB2_13:                               #   Parent Loop BB2_12 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-56(%rbp), %eax
	movq	-32(%rbp), %rcx
	movl	64(%rcx), %edx
	movq	-32(%rbp), %rcx
	imull	56(%rcx), %edx
	cmpl	%edx, %eax
	jae	.LBB2_16
# BB#14:                                #   in Loop: Header=BB2_13 Depth=2
	movl	-44(%rbp), %eax
	movq	-32(%rbp), %rcx
	movq	48(%rcx), %rcx
	movl	-56(%rbp), %edx
	movl	%edx, %esi
	addq	%rsi, %rcx
	movl	%eax, (%rcx)
# BB#15:                                #   in Loop: Header=BB2_13 Depth=2
	movl	-56(%rbp), %eax
	addl	$4, %eax
	movl	%eax, -56(%rbp)
	jmp	.LBB2_13
.LBB2_16:                               #   in Loop: Header=BB2_12 Depth=1
	movl	-52(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -52(%rbp)
	cmpl	$128, -52(%rbp)
	jne	.LBB2_18
# BB#17:                                #   in Loop: Header=BB2_12 Depth=1
	movl	-44(%rbp), %eax
	movl	%eax, -52(%rbp)
	movl	-48(%rbp), %eax
	movl	%eax, -44(%rbp)
	movl	-52(%rbp), %eax
	movl	%eax, -48(%rbp)
	movl	$0, -52(%rbp)
.LBB2_18:                               #   in Loop: Header=BB2_12 Depth=1
	jmp	.LBB2_12
.Lfunc_end2:
	.size	bootstrap_kernel, .Lfunc_end2-bootstrap_kernel
	.cfi_endproc

	.section	.tramp_handler,"ax",@progbits
	.globl	smp_bootstrap
	.align	16, 0x90
	.type	smp_bootstrap,@function
smp_bootstrap:                          # @smp_bootstrap
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
	subq	$16, %rsp
	movl	coreCount, %eax
	incl	%eax
	movl	%eax, coreCount
	#APP
	hlt
	#NO_APP
	movl	$16384, %eax            # imm = 0x4000
	movl	%eax, %ecx
	movl	$16384, %eax            # imm = 0x4000
	movl	%eax, %edi
	movq	%rcx, -16(%rbp)         # 8-byte Spill
	callq	bootstrap_malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$16384, %rax            # imm = 0x4000
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	#APP
	movq	%rax, %rsp
	#NO_APP
	xorl	%edx, %edx
	movb	%dl, %sil
	movb	%sil, %al
	callq	GDT_Initialize
	callq	IDT_Initialize
	callq	FPU_Initialize
.LBB3_1:                                # =>This Inner Loop Header: Depth=1
	jmp	.LBB3_1
.Lfunc_end3:
	.size	smp_bootstrap, .Lfunc_end3-smp_bootstrap
	.cfi_endproc

	.type	coreCount,@object       # @coreCount
	.data
	.globl	coreCount
	.align	4
coreCount:
	.long	1                       # 0x1
	.size	coreCount, 4

	.type	sys_tss,@object         # @sys_tss
	.comm	sys_tss,160,1

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
