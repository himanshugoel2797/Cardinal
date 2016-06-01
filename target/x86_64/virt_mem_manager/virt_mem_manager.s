	.text
	.file	"virt_mem_manager/virt_mem_manager.c"
	.globl	VirtMemMan_InitializeBootstrap
	.align	16, 0x90
	.type	VirtMemMan_InitializeBootstrap,@function
VirtMemMan_InitializeBootstrap:         # @VirtMemMan_InitializeBootstrap
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
	movl	$24, %eax
	movl	%eax, %edi
	callq	bootstrap_malloc
	movq	%rax, virtMemData
	movq	$0, (%rax)
	movq	virtMemData, %rax
	movabsq	$-8589930496, %rdi      # imm = 0xFFFFFFFE00001000
	movq	%rdi, 8(%rax)
	movq	virtMemData, %rax
	movb	$0, 16(%rax)
	popq	%rbp
	retq
.Lfunc_end0:
	.size	VirtMemMan_InitializeBootstrap, .Lfunc_end0-VirtMemMan_InitializeBootstrap
	.cfi_endproc

	.globl	VirtMemMan_Initialize
	.align	16, 0x90
	.type	VirtMemMan_Initialize,@function
VirtMemMan_Initialize:                  # @VirtMemMan_Initialize
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
	subq	$144, %rsp
	movl	$-2147483647, %edi      # imm = 0xFFFFFFFF80000001
	xorl	%eax, %eax
	movl	%eax, %esi
	movl	%eax, -36(%rbp)         # 4-byte Spill
	callq	CPUID_RequestInfo
	movl	$3, %edi
	movl	$67108864, %esi         # imm = 0x4000000
	callq	CPUID_FeatureIsAvailable
	movq	virtMemData, %rcx
	movb	%al, 16(%rcx)
	movq	$0, -8(%rbp)
	movq	$6, -8(%rbp)
	movq	$1030, -8(%rbp)         # imm = 0x406
	movq	$16778246, -8(%rbp)     # imm = 0x1000406
	movl	$16778246, %esi         # imm = 0x1000406
                                        # kill: RSI<def> ESI<kill>
	movl	$631, %edi              # imm = 0x277
	callq	wrmsr
	callq	MemMan_Alloc
	movq	%rax, -16(%rbp)
	movl	-36(%rbp), %edi         # 4-byte Reload
	movq	%rax, %rsi
	callq	GetVirtualAddress
	movq	%rax, -16(%rbp)
	movl	$4096, %edi             # imm = 0x1000
	movl	%edi, %edx
	movq	%rax, %rdi
	movl	-36(%rbp), %esi         # 4-byte Reload
	callq	memset
	movq	%rax, -48(%rbp)         # 8-byte Spill
	callq	MemMan_Alloc
	movq	%rax, -24(%rbp)
	movl	-36(%rbp), %edi         # 4-byte Reload
	movq	%rax, %rsi
	callq	GetVirtualAddress
	movq	virtMemData, %rcx
	movq	%rax, (%rcx)
	movq	-24(%rbp), %rax
	orq	$3, %rax
	movq	-16(%rbp), %rcx
	movq	%rax, 4088(%rcx)
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movl	$1073741824, %r8d       # imm = 0x40000000
	movl	%r8d, %eax
	movq	$-1073741824, %rsi      # imm = 0xFFFFFFFFC0000000
	movl	$1, %r8d
	movl	$7, %r9d
	movq	%rax, %rdx
	movl	%r8d, %ecx
	movl	-36(%rbp), %r10d        # 4-byte Reload
	movl	%r8d, -52(%rbp)         # 4-byte Spill
	movl	%r10d, %r8d
	movl	%r9d, -56(%rbp)         # 4-byte Spill
	movq	%rax, -64(%rbp)         # 8-byte Spill
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movl	-36(%rbp), %ecx         # 4-byte Reload
	movl	%ecx, %eax
	movq	$-2147483648, %rsi      # imm = 0xFFFFFFFF80000000
	movq	%rax, %rdx
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	movq	%rax, -72(%rbp)         # 8-byte Spill
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movl	$3221225472, %ecx       # imm = 0xC0000000
	movl	%ecx, %eax
	movabsq	$-3221225472, %rsi      # imm = 0xFFFFFFFF40000000
	movq	%rax, %rdx
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	movq	%rax, -80(%rbp)         # 8-byte Spill
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movl	$2147483648, %ecx       # imm = 0x80000000
	movl	%ecx, %eax
	movabsq	$-4294967296, %rsi      # imm = 0xFFFFFFFF00000000
	movq	%rax, %rdx
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	movq	%rax, -88(%rbp)         # 8-byte Spill
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-9663676416, %rsi      # imm = 0xFFFFFFFDC0000000
	movabsq	$7516192768, %rdx       # imm = 0x1C0000000
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-10737418240, %rsi     # imm = 0xFFFFFFFD80000000
	movabsq	$6442450944, %rdx       # imm = 0x180000000
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-11811160064, %rsi     # imm = 0xFFFFFFFD40000000
	movabsq	$5368709120, %rdx       # imm = 0x140000000
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-12884901888, %rsi     # imm = 0xFFFFFFFD00000000
	movabsq	$4294967296, %rdx       # imm = 0x100000000
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-5368709120, %rsi      # imm = 0xFFFFFFFEC0000000
	movq	-80(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-6442450944, %rsi      # imm = 0xFFFFFFFE80000000
	movq	-88(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-7516192768, %rsi      # imm = 0xFFFFFFFE40000000
	movq	-64(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-8589934592, %rsi      # imm = 0xFFFFFFFE00000000
	movq	-72(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-36(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-21474836480, %rsi     # imm = 0xFFFFFFFB00000000
	movl	$2, %ecx
	movq	-72(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %r8d         # 4-byte Reload
	movl	%ecx, -92(%rbp)         # 4-byte Spill
	movl	%r8d, %ecx
	movl	-92(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-20401094656, %rsi     # imm = 0xFFFFFFFB40000000
	movq	-64(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-92(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-19327352832, %rsi     # imm = 0xFFFFFFFB80000000
	movq	-88(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-92(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-18253611008, %rsi     # imm = 0xFFFFFFFBC0000000
	movq	-80(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-92(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-17179869184, %rsi     # imm = 0xFFFFFFFC00000000
	movq	-72(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-52(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-16106127360, %rsi     # imm = 0xFFFFFFFC40000000
	movq	-64(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-52(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-15032385536, %rsi     # imm = 0xFFFFFFFC80000000
	movq	-88(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-52(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movq	%rsp, %rax
	movl	$0, (%rax)
	movabsq	$-13958643712, %rsi     # imm = 0xFFFFFFFCC0000000
	movq	-80(%rbp), %rdx         # 8-byte Reload
	movl	-52(%rbp), %ecx         # 4-byte Reload
	movl	-52(%rbp), %r8d         # 4-byte Reload
	movl	-56(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_MapHPage
	movq	-16(%rbp), %rdi
	movl	$4, %ecx
	movq	%rdi, -104(%rbp)        # 8-byte Spill
	movl	%ecx, %edi
	callq	MemMan_Alloc4KiBPageCont
	movq	%rsp, %rdx
	movl	$0, 8(%rdx)
	movl	$6, (%rdx)
	movl	$16384, %ecx            # imm = 0x4000
                                        # kill: RCX<def> ECX<kill>
	movabsq	$-25769803776, %rdx     # imm = 0xFFFFFFFA00000000
	movq	-104(%rbp), %rdi        # 8-byte Reload
	movq	%rdx, %rsi
	movq	%rdx, -112(%rbp)        # 8-byte Spill
	movq	%rax, %rdx
	movl	-52(%rbp), %r8d         # 4-byte Reload
	movl	-36(%rbp), %r9d         # 4-byte Reload
	callq	VirtMemMan_Map
	movl	$-1073741696, %r8d      # imm = 0xFFFFFFFFC0000080
	movl	%r8d, %edi
	movl	%r8d, -116(%rbp)        # 4-byte Spill
	callq	rdmsr
	orq	$2048, %rax             # imm = 0x800
	movl	-116(%rbp), %edi        # 4-byte Reload
	movq	%rax, %rsi
	callq	wrmsr
	movq	-16(%rbp), %rdi
	callq	VirtMemMan_SetCurrent
	movq	virtMemData, %rcx
	movq	%rcx, -32(%rbp)
	movq	-112(%rbp), %rcx        # 8-byte Reload
	movq	%rcx, virtMemData
	movq	-32(%rbp), %rdx
	movq	(%rdx), %rdx
	movq	%rdx, (%rcx)
	movq	-32(%rbp), %rdx
	movq	8(%rdx), %rdx
	movq	virtMemData, %rsi
	movq	%rdx, 8(%rsi)
	movq	-32(%rbp), %rdx
	movb	16(%rdx), %r11b
	movq	virtMemData, %rdx
	movb	%r11b, 16(%rdx)
	movq	$16360, coreLocalSpace  # imm = 0x3FE8
	movq	%rax, -128(%rbp)        # 8-byte Spill
	addq	$144, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	VirtMemMan_Initialize, .Lfunc_end1-VirtMemMan_Initialize
	.cfi_endproc

	.globl	VirtMemMan_MapHPage
	.align	16, 0x90
	.type	VirtMemMan_MapHPage,@function
VirtMemMan_MapHPage:                    # @VirtMemMan_MapHPage
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
	movb	%cl, %al
	movl	16(%rbp), %ecx
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -25(%rbp)
	movl	%r8d, -32(%rbp)
	movl	%r9d, -36(%rbp)
	movl	%ecx, -40(%rbp)
	movl	-24(%rbp), %ecx
	andl	$-1073741824, %ecx      # imm = 0xFFFFFFFFC0000000
	movl	%ecx, -24(%rbp)
	movq	virtMemData, %rdx
	cmpb	$0, 16(%rdx)
	jne	.LBB2_6
# BB#1:
	movq	$0, -48(%rbp)
.LBB2_2:                                # =>This Inner Loop Header: Depth=1
	cmpq	$512, -48(%rbp)         # imm = 0x200
	jae	.LBB2_5
# BB#3:                                 #   in Loop: Header=BB2_2 Depth=1
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rax
	movq	-48(%rbp), %rcx
	shlq	$21, %rcx
	addq	%rcx, %rax
	movq	-24(%rbp), %rcx
	movq	-48(%rbp), %rdx
	shlq	$21, %rdx
	addq	%rdx, %rcx
	movb	-25(%rbp), %sil
	movl	-32(%rbp), %r8d
	movl	-36(%rbp), %r9d
	movl	-40(%rbp), %r10d
	movq	%rsp, %rdx
	movl	%r10d, (%rdx)
	movzbl	%sil, %r10d
	movq	%rax, %rsi
	movq	%rcx, %rdx
	movl	%r10d, %ecx
	callq	VirtMemMan_MapLPage
# BB#4:                                 #   in Loop: Header=BB2_2 Depth=1
	movq	-48(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB2_2
.LBB2_5:
	jmp	.LBB2_17
.LBB2_6:
	movq	-16(%rbp), %rax
	shrq	$39, %rax
	andq	$511, %rax              # imm = 0x1FF
	movl	%eax, %ecx
	movl	%ecx, -52(%rbp)
	movq	-16(%rbp), %rax
	shrq	$30, %rax
	andq	$511, %rax              # imm = 0x1FF
	movl	%eax, %ecx
	movl	%ecx, -56(%rbp)
	movq	-8(%rbp), %rdi
	movl	-52(%rbp), %esi
	callq	VirtMemMan_SetupPDPTEntry
	xorl	%ecx, %ecx
	movl	-52(%rbp), %esi
	movl	%esi, %eax
	movq	-8(%rbp), %rdi
	movabsq	$1152921504606842880, %rdx # imm = 0xFFFFFFFFFFFF000
	andq	(%rdi,%rax,8), %rdx
	xorl	%edi, %edi
	movq	%rdx, %rsi
	movl	%ecx, -68(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -64(%rbp)
	movq	-24(%rbp), %rax
	movl	-56(%rbp), %ecx
	movl	%ecx, %edx
	movq	-64(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
	cmpb	$0, -25(%rbp)
	je	.LBB2_8
# BB#7:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$1, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-64(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB2_8:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$128, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-64(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	movl	-32(%rbp), %eax
	shll	$3, %eax
	movl	%eax, %eax
	movl	%eax, %edx
	orq	%rdx, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-64(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
	movl	-36(%rbp), %eax
	andl	$2, %eax
	cmpl	$0, %eax
	je	.LBB2_10
# BB#9:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$2, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-64(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB2_10:
	movl	-36(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB2_12
# BB#11:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movabsq	$9223372036854775807, %rsi # imm = 0x7FFFFFFFFFFFFFFF
	andq	(%rdx,%rcx,8), %rsi
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	%rsi, (%rdx,%rcx,8)
	jmp	.LBB2_13
.LBB2_12:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movabsq	$-9223372036854775808, %rsi # imm = 0x8000000000000000
	orq	(%rdx,%rcx,8), %rsi
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	%rsi, (%rdx,%rcx,8)
.LBB2_13:
	movl	-40(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB2_15
# BB#14:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$4, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-64(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB2_15:
	movq	-8(%rbp), %rax
	movq	virtMemData, %rcx
	cmpq	8(%rcx), %rax
	jne	.LBB2_17
# BB#16:
	movq	-16(%rbp), %rax
	#APP
	invlpg	(%rax)
	#NO_APP
.LBB2_17:
	addq	$80, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	VirtMemMan_MapHPage, .Lfunc_end2-VirtMemMan_MapHPage
	.cfi_endproc

	.globl	VirtMemMan_Map
	.align	16, 0x90
	.type	VirtMemMan_Map,@function
VirtMemMan_Map:                         # @VirtMemMan_Map
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
	subq	$96, %rsp
	movb	%r8b, %al
	movl	24(%rbp), %r8d
	movl	16(%rbp), %r10d
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movb	%al, -33(%rbp)
	movl	%r9d, -40(%rbp)
	movl	%r10d, -44(%rbp)
	movl	%r8d, -48(%rbp)
.LBB3_1:                                # =>This Inner Loop Header: Depth=1
	cmpq	$0, -32(%rbp)
	jbe	.LBB3_27
# BB#2:                                 #   in Loop: Header=BB3_1 Depth=1
	cmpq	$4096, -32(%rbp)        # imm = 0x1000
	jne	.LBB3_4
# BB#3:                                 #   in Loop: Header=BB3_1 Depth=1
	movq	-32(%rbp), %rax
	subq	$4096, %rax             # imm = 0x1000
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movb	-33(%rbp), %cl
	movl	-40(%rbp), %r8d
	movl	-44(%rbp), %r9d
	movl	-48(%rbp), %r10d
	movq	%rsp, %rax
	movl	%r10d, (%rax)
	movzbl	%cl, %ecx
	callq	VirtMemMan_MapSPage
	jmp	.LBB3_26
.LBB3_4:                                #   in Loop: Header=BB3_1 Depth=1
	cmpq	$2097152, -32(%rbp)     # imm = 0x200000
	jne	.LBB3_6
# BB#5:                                 #   in Loop: Header=BB3_1 Depth=1
	movq	-32(%rbp), %rax
	subq	$2097152, %rax          # imm = 0x200000
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movb	-33(%rbp), %cl
	movl	-40(%rbp), %r8d
	movl	-44(%rbp), %r9d
	movl	-48(%rbp), %r10d
	movq	%rsp, %rax
	movl	%r10d, (%rax)
	movzbl	%cl, %ecx
	callq	VirtMemMan_MapLPage
	jmp	.LBB3_25
.LBB3_6:                                #   in Loop: Header=BB3_1 Depth=1
	cmpq	$1073741824, -32(%rbp)  # imm = 0x40000000
	jne	.LBB3_8
# BB#7:                                 #   in Loop: Header=BB3_1 Depth=1
	movq	-32(%rbp), %rax
	subq	$1073741824, %rax       # imm = 0x40000000
	movq	%rax, -32(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movb	-33(%rbp), %cl
	movl	-40(%rbp), %r8d
	movl	-44(%rbp), %r9d
	movl	-48(%rbp), %r10d
	movq	%rsp, %rax
	movl	%r10d, (%rax)
	movzbl	%cl, %ecx
	callq	VirtMemMan_MapHPage
	jmp	.LBB3_24
.LBB3_8:                                #   in Loop: Header=BB3_1 Depth=1
	cmpq	$1073741824, -32(%rbp)  # imm = 0x40000000
	jb	.LBB3_12
# BB#9:                                 #   in Loop: Header=BB3_1 Depth=1
	movq	-16(%rbp), %rax
	andq	$1073741823, %rax       # imm = 0x3FFFFFFF
	cmpq	$0, %rax
	jne	.LBB3_12
# BB#10:                                #   in Loop: Header=BB3_1 Depth=1
	movq	-24(%rbp), %rax
	andq	$1073741823, %rax       # imm = 0x3FFFFFFF
	cmpq	$0, %rax
	jne	.LBB3_12
# BB#11:                                #   in Loop: Header=BB3_1 Depth=1
	movl	$1073741824, %eax       # imm = 0x40000000
	movl	%eax, %ecx
	movq	-32(%rbp), %rdx
	subq	$1073741824, %rdx       # imm = 0x40000000
	movq	%rdx, -32(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movb	-33(%rbp), %r8b
	movl	-40(%rbp), %r9d
	movl	-44(%rbp), %eax
	movl	-48(%rbp), %r10d
	movq	%rsp, %r11
	movl	%r10d, 8(%r11)
	movl	%eax, (%r11)
	movzbl	%r8b, %r8d
	movl	$1073741824, %eax       # imm = 0x40000000
	movl	%eax, %r11d
	movq	%rcx, -56(%rbp)         # 8-byte Spill
	movq	%r11, %rcx
	callq	VirtMemMan_Map
	movq	-16(%rbp), %rcx
	addq	$1073741824, %rcx       # imm = 0x40000000
	movq	%rcx, -16(%rbp)
	movq	-24(%rbp), %rcx
	addq	$1073741824, %rcx       # imm = 0x40000000
	movq	%rcx, -24(%rbp)
	jmp	.LBB3_23
.LBB3_12:                               #   in Loop: Header=BB3_1 Depth=1
	cmpq	$2097152, -32(%rbp)     # imm = 0x200000
	jb	.LBB3_16
# BB#13:                                #   in Loop: Header=BB3_1 Depth=1
	movq	-16(%rbp), %rax
	andq	$2097151, %rax          # imm = 0x1FFFFF
	cmpq	$0, %rax
	jne	.LBB3_16
# BB#14:                                #   in Loop: Header=BB3_1 Depth=1
	movq	-24(%rbp), %rax
	andq	$2097151, %rax          # imm = 0x1FFFFF
	cmpq	$0, %rax
	jne	.LBB3_16
# BB#15:                                #   in Loop: Header=BB3_1 Depth=1
	movl	$2097152, %eax          # imm = 0x200000
	movl	%eax, %ecx
	movq	-32(%rbp), %rdx
	subq	$2097152, %rdx          # imm = 0x200000
	movq	%rdx, -32(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movb	-33(%rbp), %r8b
	movl	-40(%rbp), %r9d
	movl	-44(%rbp), %eax
	movl	-48(%rbp), %r10d
	movq	%rsp, %r11
	movl	%r10d, 8(%r11)
	movl	%eax, (%r11)
	movzbl	%r8b, %r8d
	movl	$2097152, %eax          # imm = 0x200000
	movl	%eax, %r11d
	movq	%rcx, -64(%rbp)         # 8-byte Spill
	movq	%r11, %rcx
	callq	VirtMemMan_Map
	movq	-16(%rbp), %rcx
	addq	$2097152, %rcx          # imm = 0x200000
	movq	%rcx, -16(%rbp)
	movq	-24(%rbp), %rcx
	addq	$2097152, %rcx          # imm = 0x200000
	movq	%rcx, -24(%rbp)
	jmp	.LBB3_22
.LBB3_16:                               #   in Loop: Header=BB3_1 Depth=1
	cmpq	$4096, -32(%rbp)        # imm = 0x1000
	jb	.LBB3_20
# BB#17:                                #   in Loop: Header=BB3_1 Depth=1
	movq	-16(%rbp), %rax
	andq	$4095, %rax             # imm = 0xFFF
	cmpq	$0, %rax
	jne	.LBB3_20
# BB#18:                                #   in Loop: Header=BB3_1 Depth=1
	movq	-24(%rbp), %rax
	andq	$4095, %rax             # imm = 0xFFF
	cmpq	$0, %rax
	jne	.LBB3_20
# BB#19:                                #   in Loop: Header=BB3_1 Depth=1
	movl	$4096, %eax             # imm = 0x1000
	movl	%eax, %ecx
	movq	-32(%rbp), %rdx
	subq	$4096, %rdx             # imm = 0x1000
	movq	%rdx, -32(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movb	-33(%rbp), %r8b
	movl	-40(%rbp), %r9d
	movl	-44(%rbp), %eax
	movl	-48(%rbp), %r10d
	movq	%rsp, %r11
	movl	%r10d, 8(%r11)
	movl	%eax, (%r11)
	movzbl	%r8b, %r8d
	movl	$4096, %eax             # imm = 0x1000
	movl	%eax, %r11d
	movq	%rcx, -72(%rbp)         # 8-byte Spill
	movq	%r11, %rcx
	callq	VirtMemMan_Map
	movq	-16(%rbp), %rcx
	addq	$4096, %rcx             # imm = 0x1000
	movq	%rcx, -16(%rbp)
	movq	-24(%rbp), %rcx
	addq	$4096, %rcx             # imm = 0x1000
	movq	%rcx, -24(%rbp)
	jmp	.LBB3_21
.LBB3_20:
	jmp	.LBB3_27
.LBB3_21:                               #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_22
.LBB3_22:                               #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_23
.LBB3_23:                               #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_24
.LBB3_24:                               #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_25
.LBB3_25:                               #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_26
.LBB3_26:                               #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_1
.LBB3_27:
	addq	$96, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	VirtMemMan_Map, .Lfunc_end3-VirtMemMan_Map
	.cfi_endproc

	.globl	VirtMemMan_SetCurrent
	.align	16, 0x90
	.type	VirtMemMan_SetCurrent,@function
VirtMemMan_SetCurrent:                  # @VirtMemMan_SetCurrent
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
	movq	virtMemData, %rdi
	movq	8(%rdi), %rdi
	movq	%rdi, -16(%rbp)
	movq	-8(%rbp), %rdi
	callq	GetPhysicalAddress
	#APP
	movq	%rax, %cr3
	#NO_APP
	movq	-8(%rbp), %rax
	movq	virtMemData, %rdi
	movq	%rax, 8(%rdi)
	movq	-16(%rbp), %rax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	VirtMemMan_SetCurrent, .Lfunc_end4-VirtMemMan_SetCurrent
	.cfi_endproc

	.globl	VirtMemMan_AllocCoreLocalData
	.align	16, 0x90
	.type	VirtMemMan_AllocCoreLocalData,@function
VirtMemMan_AllocCoreLocalData:          # @VirtMemMan_AllocCoreLocalData
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
	subq	$24, %rsp
	movq	%rdi, -16(%rbp)
	movq	coreLocalSpace, %rax
	cmpq	%rax, %rdi
	ja	.LBB5_3
# BB#1:
	movq	coreLocalSpace, %rax
	cmpq	$0, %rax
	jbe	.LBB5_3
# BB#2:
	movq	virtMemData, %rax
	movq	coreLocalSpace, %rcx
	movl	$16384, %edx            # imm = 0x4000
	movl	%edx, %esi
	subq	%rcx, %rsi
	addq	%rsi, %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	coreLocalSpace, %rcx
	subq	%rax, %rcx
	movq	%rcx, coreLocalSpace
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB5_4
.LBB5_3:
	movq	$0, -8(%rbp)
.LBB5_4:
	movq	-8(%rbp), %rax
	addq	$24, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	VirtMemMan_AllocCoreLocalData, .Lfunc_end5-VirtMemMan_AllocCoreLocalData
	.cfi_endproc

	.globl	VirtMemMan_CreateInstance
	.align	16, 0x90
	.type	VirtMemMan_CreateInstance,@function
VirtMemMan_CreateInstance:              # @VirtMemMan_CreateInstance
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
	callq	MemMan_Alloc
	movq	%rax, -8(%rbp)
	xorl	%ecx, %ecx
	movl	%ecx, %edi
	movq	%rax, %rsi
	movl	%ecx, -12(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -8(%rbp)
	movl	$4096, %ecx             # imm = 0x1000
	movl	%ecx, %edx
	movq	%rax, %rdi
	movl	-12(%rbp), %esi         # 4-byte Reload
	callq	memset
	movq	virtMemData, %rdx
	movq	(%rdx), %rdx
	movq	%rdx, %rdi
	movq	%rax, -24(%rbp)         # 8-byte Spill
	callq	GetPhysicalAddress
	movq	-8(%rbp), %rdx
	movq	%rax, 4088(%rdx)
	movq	-8(%rbp), %rax
	movq	4088(%rax), %rax
	orq	$1, %rax
	movq	-8(%rbp), %rdx
	movq	%rax, 4088(%rdx)
	movq	-8(%rbp), %rax
	movq	4088(%rax), %rax
	orq	$2, %rax
	movq	-8(%rbp), %rdx
	movq	%rax, 4088(%rdx)
	movq	-8(%rbp), %rax
	movq	4088(%rax), %rax
	movq	-8(%rbp), %rdx
	movq	%rax, 4088(%rdx)
	movq	-8(%rbp), %rax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	VirtMemMan_CreateInstance, .Lfunc_end6-VirtMemMan_CreateInstance
	.cfi_endproc

	.globl	VirtMemMan_GetCurrent
	.align	16, 0x90
	.type	VirtMemMan_GetCurrent,@function
VirtMemMan_GetCurrent:                  # @VirtMemMan_GetCurrent
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
	movq	virtMemData, %rax
	movq	8(%rax), %rax
	popq	%rbp
	retq
.Lfunc_end7:
	.size	VirtMemMan_GetCurrent, .Lfunc_end7-VirtMemMan_GetCurrent
	.cfi_endproc

	.globl	VirtMemMan_MapLPage
	.align	16, 0x90
	.type	VirtMemMan_MapLPage,@function
VirtMemMan_MapLPage:                    # @VirtMemMan_MapLPage
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
	subq	$80, %rsp
	movb	%cl, %al
	movl	16(%rbp), %ecx
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -25(%rbp)
	movl	%r8d, -32(%rbp)
	movl	%r9d, -36(%rbp)
	movl	%ecx, -40(%rbp)
	movq	-24(%rbp), %rdx
	shrq	$21, %rdx
	shlq	$21, %rdx
	movq	%rdx, -24(%rbp)
	movq	-16(%rbp), %rdx
	shrq	$39, %rdx
	andq	$511, %rdx              # imm = 0x1FF
	movl	%edx, %ecx
	movl	%ecx, -44(%rbp)
	movq	-16(%rbp), %rdx
	shrq	$30, %rdx
	andq	$511, %rdx              # imm = 0x1FF
	movl	%edx, %ecx
	movl	%ecx, -48(%rbp)
	movq	-16(%rbp), %rdx
	shrq	$21, %rdx
	andq	$511, %rdx              # imm = 0x1FF
	movl	%edx, %ecx
	movl	%ecx, -52(%rbp)
	movq	-8(%rbp), %rdi
	movl	-44(%rbp), %esi
	movl	-48(%rbp), %edx
	callq	VirtMemMan_SetupPDEntry
	xorl	%ecx, %ecx
	movl	-44(%rbp), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %r10
	movabsq	$1152921504606842880, %r11 # imm = 0xFFFFFFFFFFFF000
	andq	(%r10,%rdi,8), %r11
	xorl	%edi, %edi
	movq	%r11, %rsi
	movl	%ecx, -76(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	xorl	%ecx, %ecx
	movq	%rax, -64(%rbp)
	movl	-48(%rbp), %edx
	movl	%edx, %eax
	movq	-64(%rbp), %rsi
	movabsq	$1152921504606842880, %r10 # imm = 0xFFFFFFFFFFFF000
	andq	(%rsi,%rax,8), %r10
	xorl	%edi, %edi
	movq	%r10, %rsi
	movl	%ecx, -80(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -72(%rbp)
	movq	-24(%rbp), %rax
	movl	-52(%rbp), %ecx
	movl	%ecx, %esi
	movq	-72(%rbp), %r10
	movq	%rax, (%r10,%rsi,8)
	cmpb	$0, -25(%rbp)
	je	.LBB8_2
# BB#1:
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$1, %rcx
	movl	-52(%rbp), %eax
	movl	%eax, %edx
	movq	-72(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB8_2:
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$128, %rcx
	movl	-52(%rbp), %eax
	movl	%eax, %edx
	movq	-72(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	movl	-32(%rbp), %eax
	shll	$3, %eax
	movl	%eax, %eax
	movl	%eax, %edx
	orq	%rdx, %rcx
	movl	-52(%rbp), %eax
	movl	%eax, %edx
	movq	-72(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
	movl	-36(%rbp), %eax
	andl	$2, %eax
	cmpl	$0, %eax
	je	.LBB8_4
# BB#3:
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$2, %rcx
	movl	-52(%rbp), %eax
	movl	%eax, %edx
	movq	-72(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB8_4:
	movl	-36(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB8_6
# BB#5:
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movabsq	$9223372036854775807, %rsi # imm = 0x7FFFFFFFFFFFFFFF
	andq	(%rdx,%rcx,8), %rsi
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movq	%rsi, (%rdx,%rcx,8)
	jmp	.LBB8_7
.LBB8_6:
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movabsq	$-9223372036854775808, %rsi # imm = 0x8000000000000000
	orq	(%rdx,%rcx,8), %rsi
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movq	%rsi, (%rdx,%rcx,8)
.LBB8_7:
	movl	-40(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB8_9
# BB#8:
	movl	-52(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$4, %rcx
	movl	-52(%rbp), %eax
	movl	%eax, %edx
	movq	-72(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB8_9:
	movq	-8(%rbp), %rax
	movq	virtMemData, %rcx
	cmpq	8(%rcx), %rax
	jne	.LBB8_11
# BB#10:
	movq	-16(%rbp), %rax
	#APP
	invlpg	(%rax)
	#NO_APP
.LBB8_11:
	addq	$80, %rsp
	popq	%rbp
	retq
.Lfunc_end8:
	.size	VirtMemMan_MapLPage, .Lfunc_end8-VirtMemMan_MapLPage
	.cfi_endproc

	.align	16, 0x90
	.type	VirtMemMan_SetupPDPTEntry,@function
VirtMemMan_SetupPDPTEntry:              # @VirtMemMan_SetupPDPTEntry
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	-12(%rbp), %esi
	movl	%esi, %edi
	movq	-8(%rbp), %rax
	movabsq	$1152921504606842880, %rcx # imm = 0xFFFFFFFFFFFF000
	andq	(%rax,%rdi,8), %rcx
	cmpq	$0, %rcx
	jne	.LBB9_2
# BB#1:
	callq	MemMan_Alloc
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	orq	$1, %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	orq	$2, %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movl	-12(%rbp), %ecx
	movl	%ecx, %edx
	movq	-8(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
.LBB9_2:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end9:
	.size	VirtMemMan_SetupPDPTEntry, .Lfunc_end9-VirtMemMan_SetupPDPTEntry
	.cfi_endproc

	.align	16, 0x90
	.type	VirtMemMan_SetupPDEntry,@function
VirtMemMan_SetupPDEntry:                # @VirtMemMan_SetupPDEntry
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rdi
	movl	-12(%rbp), %esi
	callq	VirtMemMan_SetupPDPTEntry
	xorl	%edx, %edx
	movl	-12(%rbp), %esi
	movl	%esi, %edi
	movq	-8(%rbp), %rax
	movabsq	$1152921504606842880, %rcx # imm = 0xFFFFFFFFFFFF000
	andq	(%rax,%rdi,8), %rcx
	xorl	%edi, %edi
	movq	%rcx, %rsi
	movl	%edx, -28(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -24(%rbp)
	movl	-16(%rbp), %edx
	movl	%edx, %eax
	movq	-24(%rbp), %rcx
	movabsq	$1152921504606842880, %rsi # imm = 0xFFFFFFFFFFFF000
	andq	(%rcx,%rax,8), %rsi
	cmpq	$0, %rsi
	jne	.LBB10_2
# BB#1:
	callq	MemMan_Alloc
	movl	-16(%rbp), %ecx
	movl	%ecx, %edx
	movq	-24(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
	movl	-16(%rbp), %ecx
	movl	%ecx, %eax
	movq	-24(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	orq	$1, %rax
	movl	-16(%rbp), %ecx
	movl	%ecx, %edx
	movq	-24(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
	movl	-16(%rbp), %ecx
	movl	%ecx, %eax
	movq	-24(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	orq	$2, %rax
	movl	-16(%rbp), %ecx
	movl	%ecx, %edx
	movq	-24(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
	movl	-16(%rbp), %ecx
	movl	%ecx, %eax
	movq	-24(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	movl	-16(%rbp), %ecx
	movl	%ecx, %edx
	movq	-24(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
.LBB10_2:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end10:
	.size	VirtMemMan_SetupPDEntry, .Lfunc_end10-VirtMemMan_SetupPDEntry
	.cfi_endproc

	.globl	VirtMemMan_MapSPage
	.align	16, 0x90
	.type	VirtMemMan_MapSPage,@function
VirtMemMan_MapSPage:                    # @VirtMemMan_MapSPage
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp33:
	.cfi_def_cfa_offset 16
.Ltmp34:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp35:
	.cfi_def_cfa_register %rbp
	subq	$96, %rsp
	movb	%cl, %al
	movl	16(%rbp), %ecx
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movb	%al, -25(%rbp)
	movl	%r8d, -32(%rbp)
	movl	%r9d, -36(%rbp)
	movl	%ecx, -40(%rbp)
	movq	-24(%rbp), %rdx
	shrq	$12, %rdx
	shlq	$12, %rdx
	movq	%rdx, -24(%rbp)
	movq	-16(%rbp), %rdx
	shrq	$39, %rdx
	andq	$511, %rdx              # imm = 0x1FF
	movl	%edx, %ecx
	movl	%ecx, -44(%rbp)
	movq	-16(%rbp), %rdx
	shrq	$30, %rdx
	andq	$511, %rdx              # imm = 0x1FF
	movl	%edx, %ecx
	movl	%ecx, -48(%rbp)
	movq	-16(%rbp), %rdx
	shrq	$21, %rdx
	andq	$511, %rdx              # imm = 0x1FF
	movl	%edx, %ecx
	movl	%ecx, -52(%rbp)
	movq	-16(%rbp), %rdx
	shrq	$12, %rdx
	andq	$511, %rdx              # imm = 0x1FF
	movl	%edx, %ecx
	movl	%ecx, -56(%rbp)
	movq	-8(%rbp), %rdi
	movl	-44(%rbp), %esi
	movl	-48(%rbp), %edx
	movl	-52(%rbp), %ecx
	callq	VirtMemMan_SetupPTEntry
	xorl	%ecx, %ecx
	movl	-44(%rbp), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %r10
	movabsq	$1152921504606842880, %r11 # imm = 0xFFFFFFFFFFFF000
	andq	(%r10,%rdi,8), %r11
	xorl	%edi, %edi
	movq	%r11, %rsi
	movl	%ecx, -84(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	xorl	%ecx, %ecx
	movq	%rax, -64(%rbp)
	movl	-48(%rbp), %edx
	movl	%edx, %eax
	movq	-64(%rbp), %rsi
	movabsq	$1152921504606842880, %r10 # imm = 0xFFFFFFFFFFFF000
	andq	(%rsi,%rax,8), %r10
	xorl	%edi, %edi
	movq	%r10, %rsi
	movl	%ecx, -88(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	xorl	%ecx, %ecx
	movq	%rax, -72(%rbp)
	movl	-52(%rbp), %edx
	movl	%edx, %eax
	movq	-72(%rbp), %rsi
	movabsq	$1152921504606842880, %r10 # imm = 0xFFFFFFFFFFFF000
	andq	(%rsi,%rax,8), %r10
	xorl	%edi, %edi
	movq	%r10, %rsi
	movl	%ecx, -92(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -80(%rbp)
	movq	-24(%rbp), %rax
	movl	-56(%rbp), %ecx
	movl	%ecx, %esi
	movq	-80(%rbp), %r10
	movq	%rax, (%r10,%rsi,8)
	cmpb	$0, -25(%rbp)
	je	.LBB11_2
# BB#1:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-80(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$1, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-80(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB11_2:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-80(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	movl	-32(%rbp), %eax
	shll	$3, %eax
	movl	%eax, %eax
	movl	%eax, %edx
	orq	%rdx, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-80(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
	movl	-36(%rbp), %eax
	andl	$2, %eax
	cmpl	$0, %eax
	je	.LBB11_4
# BB#3:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-80(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$2, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-80(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB11_4:
	movl	-36(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB11_6
# BB#5:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-80(%rbp), %rdx
	movabsq	$9223372036854775807, %rsi # imm = 0x7FFFFFFFFFFFFFFF
	andq	(%rdx,%rcx,8), %rsi
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-80(%rbp), %rdx
	movq	%rsi, (%rdx,%rcx,8)
	jmp	.LBB11_7
.LBB11_6:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-80(%rbp), %rdx
	movabsq	$-9223372036854775808, %rsi # imm = 0x8000000000000000
	orq	(%rdx,%rcx,8), %rsi
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-80(%rbp), %rdx
	movq	%rsi, (%rdx,%rcx,8)
.LBB11_7:
	movl	-40(%rbp), %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB11_9
# BB#8:
	movl	-56(%rbp), %eax
	movl	%eax, %ecx
	movq	-80(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	orq	$4, %rcx
	movl	-56(%rbp), %eax
	movl	%eax, %edx
	movq	-80(%rbp), %rsi
	movq	%rcx, (%rsi,%rdx,8)
.LBB11_9:
	movq	-8(%rbp), %rax
	movq	virtMemData, %rcx
	cmpq	8(%rcx), %rax
	jne	.LBB11_11
# BB#10:
	movq	-16(%rbp), %rax
	#APP
	invlpg	(%rax)
	#NO_APP
.LBB11_11:
	addq	$96, %rsp
	popq	%rbp
	retq
.Lfunc_end11:
	.size	VirtMemMan_MapSPage, .Lfunc_end11-VirtMemMan_MapSPage
	.cfi_endproc

	.align	16, 0x90
	.type	VirtMemMan_SetupPTEntry,@function
VirtMemMan_SetupPTEntry:                # @VirtMemMan_SetupPTEntry
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp36:
	.cfi_def_cfa_offset 16
.Ltmp37:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp38:
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	%ecx, -20(%rbp)
	movq	-8(%rbp), %rdi
	movl	-12(%rbp), %esi
	movl	-16(%rbp), %edx
	callq	VirtMemMan_SetupPDEntry
	xorl	%ecx, %ecx
	movl	-12(%rbp), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rax
	movabsq	$1152921504606842880, %r8 # imm = 0xFFFFFFFFFFFF000
	andq	(%rax,%rdi,8), %r8
	xorl	%edi, %edi
	movq	%r8, %rsi
	movl	%ecx, -44(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	xorl	%ecx, %ecx
	movq	%rax, -32(%rbp)
	movl	-16(%rbp), %edx
	movl	%edx, %eax
	movq	-32(%rbp), %rsi
	movabsq	$1152921504606842880, %r8 # imm = 0xFFFFFFFFFFFF000
	andq	(%rsi,%rax,8), %r8
	xorl	%edi, %edi
	movq	%r8, %rsi
	movl	%ecx, -48(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -40(%rbp)
	movl	-20(%rbp), %ecx
	movl	%ecx, %eax
	movq	-40(%rbp), %rsi
	movabsq	$1152921504606842880, %r8 # imm = 0xFFFFFFFFFFFF000
	andq	(%rsi,%rax,8), %r8
	cmpq	$0, %r8
	jne	.LBB12_2
# BB#1:
	callq	MemMan_Alloc
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	movq	-40(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
	movl	-20(%rbp), %ecx
	movl	%ecx, %eax
	movq	-40(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	orq	$1, %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	movq	-40(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
	movl	-20(%rbp), %ecx
	movl	%ecx, %eax
	movq	-40(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	orq	$2, %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	movq	-40(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
	movl	-20(%rbp), %ecx
	movl	%ecx, %eax
	movq	-40(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	movl	-20(%rbp), %ecx
	movl	%ecx, %edx
	movq	-40(%rbp), %rsi
	movq	%rax, (%rsi,%rdx,8)
.LBB12_2:
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end12:
	.size	VirtMemMan_SetupPTEntry, .Lfunc_end12-VirtMemMan_SetupPTEntry
	.cfi_endproc

	.globl	VirtMemMan_GetPhysicalAddress
	.align	16, 0x90
	.type	VirtMemMan_GetPhysicalAddress,@function
VirtMemMan_GetPhysicalAddress:          # @VirtMemMan_GetPhysicalAddress
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp39:
	.cfi_def_cfa_offset 16
.Ltmp40:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp41:
	.cfi_def_cfa_register %rbp
	subq	$128, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-24(%rbp), %rsi
	movq	%rsi, -32(%rbp)
	movq	-32(%rbp), %rsi
	shrq	$39, %rsi
	andq	$511, %rsi              # imm = 0x1FF
	movl	%esi, %eax
	movl	%eax, -36(%rbp)
	movq	-32(%rbp), %rsi
	shrq	$30, %rsi
	andq	$511, %rsi              # imm = 0x1FF
	movl	%esi, %eax
	movl	%eax, -40(%rbp)
	movq	-32(%rbp), %rsi
	shrq	$21, %rsi
	andq	$511, %rsi              # imm = 0x1FF
	movl	%esi, %eax
	movl	%eax, -44(%rbp)
	movq	-32(%rbp), %rsi
	shrq	$12, %rsi
	andq	$511, %rsi              # imm = 0x1FF
	movl	%esi, %eax
	movl	%eax, -48(%rbp)
	movl	-36(%rbp), %eax
	movl	%eax, %esi
	movq	-16(%rbp), %rdi
	movq	(%rdi,%rsi,8), %rsi
	andq	$1, %rsi
	cmpq	$0, %rsi
	je	.LBB13_14
# BB#1:
	xorl	%eax, %eax
	movl	-36(%rbp), %ecx
	movl	%ecx, %edx
	movq	-16(%rbp), %rsi
	movabsq	$1152921504606842880, %rdi # imm = 0xFFFFFFFFFFFF000
	andq	(%rsi,%rdx,8), %rdi
	xorl	%ecx, %ecx
	movq	%rdi, -80(%rbp)         # 8-byte Spill
	movl	%ecx, %edi
	movq	-80(%rbp), %rsi         # 8-byte Reload
	movl	%eax, -84(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -56(%rbp)
	movl	-40(%rbp), %ecx
	movl	%ecx, %eax
	movq	-56(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	shrq	$7, %rax
	andq	$1, %rax
	cmpq	$0, %rax
	je	.LBB13_4
# BB#2:
	movl	-40(%rbp), %eax
	movl	%eax, %ecx
	movq	-56(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	andq	$1, %rcx
	cmpq	$0, %rcx
	je	.LBB13_4
# BB#3:
	movl	-40(%rbp), %eax
	movl	%eax, %ecx
	movq	-56(%rbp), %rdx
	movabsq	$1152921503533105152, %rsi # imm = 0xFFFFFFFC0000000
	andq	(%rdx,%rcx,8), %rsi
	movq	-32(%rbp), %rcx
	andq	$1073741823, %rcx       # imm = 0x3FFFFFFF
	addq	%rcx, %rsi
	movq	%rsi, -8(%rbp)
	jmp	.LBB13_15
.LBB13_4:
	movl	-40(%rbp), %eax
	movl	%eax, %ecx
	movq	-56(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	andq	$1, %rcx
	cmpq	$0, %rcx
	je	.LBB13_13
# BB#5:
	xorl	%eax, %eax
	movl	-40(%rbp), %ecx
	movl	%ecx, %edx
	movq	-56(%rbp), %rsi
	movabsq	$1152921504606842880, %rdi # imm = 0xFFFFFFFFFFFF000
	andq	(%rsi,%rdx,8), %rdi
	xorl	%ecx, %ecx
	movq	%rdi, -96(%rbp)         # 8-byte Spill
	movl	%ecx, %edi
	movq	-96(%rbp), %rsi         # 8-byte Reload
	movl	%eax, -100(%rbp)        # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -64(%rbp)
	movl	-44(%rbp), %ecx
	movl	%ecx, %eax
	movq	-64(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	shrq	$7, %rax
	andq	$1, %rax
	cmpq	$0, %rax
	je	.LBB13_8
# BB#6:
	movl	-44(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	andq	$1, %rcx
	cmpq	$0, %rcx
	je	.LBB13_8
# BB#7:
	movl	-44(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movabsq	$1152921504605798400, %rsi # imm = 0xFFFFFFFFFF00000
	andq	(%rdx,%rcx,8), %rsi
	movq	-32(%rbp), %rcx
	andq	$2097151, %rcx          # imm = 0x1FFFFF
	addq	%rcx, %rsi
	movq	%rsi, -8(%rbp)
	jmp	.LBB13_15
.LBB13_8:
	movl	-44(%rbp), %eax
	movl	%eax, %ecx
	movq	-64(%rbp), %rdx
	movq	(%rdx,%rcx,8), %rcx
	andq	$1, %rcx
	cmpq	$0, %rcx
	je	.LBB13_12
# BB#9:
	xorl	%eax, %eax
	movl	-44(%rbp), %ecx
	movl	%ecx, %edx
	movq	-64(%rbp), %rsi
	movabsq	$1152921504606842880, %rdi # imm = 0xFFFFFFFFFFFF000
	andq	(%rsi,%rdx,8), %rdi
	xorl	%ecx, %ecx
	movq	%rdi, -112(%rbp)        # 8-byte Spill
	movl	%ecx, %edi
	movq	-112(%rbp), %rsi        # 8-byte Reload
	movl	%eax, -116(%rbp)        # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -72(%rbp)
	movl	-48(%rbp), %ecx
	movl	%ecx, %eax
	movq	-72(%rbp), %rdx
	movq	(%rdx,%rax,8), %rax
	andq	$1, %rax
	cmpq	$0, %rax
	je	.LBB13_11
# BB#10:
	movl	-48(%rbp), %eax
	movl	%eax, %ecx
	movq	-72(%rbp), %rdx
	movabsq	$1152921504606842880, %rsi # imm = 0xFFFFFFFFFFFF000
	andq	(%rdx,%rcx,8), %rsi
	movq	-32(%rbp), %rcx
	andq	$4095, %rcx             # imm = 0xFFF
	addq	%rcx, %rsi
	movq	%rsi, -8(%rbp)
	jmp	.LBB13_15
.LBB13_11:
	jmp	.LBB13_12
.LBB13_12:
	jmp	.LBB13_13
.LBB13_13:
	jmp	.LBB13_14
.LBB13_14:
	movq	$0, -8(%rbp)
.LBB13_15:
	movq	-8(%rbp), %rax
	addq	$128, %rsp
	popq	%rbp
	retq
.Lfunc_end13:
	.size	VirtMemMan_GetPhysicalAddress, .Lfunc_end13-VirtMemMan_GetPhysicalAddress
	.cfi_endproc

	.globl	VirtMemMan_GetVirtualAddress
	.align	16, 0x90
	.type	VirtMemMan_GetVirtualAddress,@function
VirtMemMan_GetVirtualAddress:           # @VirtMemMan_GetVirtualAddress
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp42:
	.cfi_def_cfa_offset 16
.Ltmp43:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp44:
	.cfi_def_cfa_register %rbp
	subq	$24, %rsp
	movl	%edi, -12(%rbp)
	movq	%rsi, -24(%rbp)
	cmpl	$0, -12(%rbp)
	jne	.LBB14_2
# BB#1:
	movabsq	$4294967296, %rax       # imm = 0x100000000
	xorl	%ecx, %ecx
	movl	%ecx, %edx
	movabsq	$8589934592, %rsi       # imm = 0x200000000
	movq	-24(%rbp), %rdi
	movabsq	$-17179869184, %r8      # imm = 0xFFFFFFFC00000000
	addq	%r8, %rdi
	movq	-24(%rbp), %r8
	cmpq	%rax, %r8
	cmovbq	%rsi, %rdx
	addq	%rdx, %rdi
	movq	%rdi, -8(%rbp)
	jmp	.LBB14_11
.LBB14_2:
	cmpl	$2, -12(%rbp)
	jne	.LBB14_5
# BB#3:
	movabsq	$4294967296, %rax       # imm = 0x100000000
	movq	-24(%rbp), %rcx
	cmpq	%rax, %rcx
	jae	.LBB14_5
# BB#4:
	movq	-24(%rbp), %rax
	movabsq	$-17179869184, %rcx     # imm = 0xFFFFFFFC00000000
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB14_11
.LBB14_5:
	cmpl	$1, -12(%rbp)
	jne	.LBB14_8
# BB#6:
	movabsq	$4294967296, %rax       # imm = 0x100000000
	movq	-24(%rbp), %rcx
	cmpq	%rax, %rcx
	jae	.LBB14_8
# BB#7:
	movq	-24(%rbp), %rax
	movabsq	$-21474836480, %rcx     # imm = 0xFFFFFFFB00000000
	addq	%rcx, %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB14_11
.LBB14_8:
	jmp	.LBB14_9
.LBB14_9:
	jmp	.LBB14_10
.LBB14_10:
	movq	$0, -8(%rbp)
.LBB14_11:
	movq	-8(%rbp), %rax
	addq	$24, %rsp
	popq	%rbp
	retq
.Lfunc_end14:
	.size	VirtMemMan_GetVirtualAddress, .Lfunc_end14-VirtMemMan_GetVirtualAddress
	.cfi_endproc

	.globl	VirtMemMan_FindFreeAddress
	.align	16, 0x90
	.type	VirtMemMan_FindFreeAddress,@function
VirtMemMan_FindFreeAddress:             # @VirtMemMan_FindFreeAddress
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp45:
	.cfi_def_cfa_offset 16
.Ltmp46:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp47:
	.cfi_def_cfa_register %rbp
	subq	$160, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	-16(%rbp), %rsi
	andq	$4095, %rsi             # imm = 0xFFF
	cmpq	$0, %rsi
	je	.LBB15_2
# BB#1:
	movq	-16(%rbp), %rax
	shrq	$12, %rax
	addq	$1, %rax
	shlq	$12, %rax
	movq	%rax, -16(%rbp)
.LBB15_2:
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	$0, -40(%rbp)
	movq	$0, -48(%rbp)
	movq	$0, -56(%rbp)
	movl	$256, -60(%rbp)         # imm = 0x100
	movl	-20(%rbp), %ecx
	andl	$1, %ecx
	cmpl	$0, %ecx
	je	.LBB15_4
# BB#3:
	movl	$0, -60(%rbp)
.LBB15_4:
	movslq	-60(%rbp), %rax
	movq	%rax, -72(%rbp)
.LBB15_5:                               # =>This Loop Header: Depth=1
                                        #     Child Loop BB15_20 Depth 2
                                        #       Child Loop BB15_35 Depth 3
                                        #         Child Loop BB15_50 Depth 4
	xorl	%eax, %eax
	movb	%al, %cl
	movq	-72(%rbp), %rdx
	movl	-60(%rbp), %eax
	addl	$128, %eax
	movslq	%eax, %rsi
	cmpq	%rsi, %rdx
	movb	%cl, -121(%rbp)         # 1-byte Spill
	jae	.LBB15_7
# BB#6:                                 #   in Loop: Header=BB15_5 Depth=1
	movq	-40(%rbp), %rax
	cmpq	-32(%rbp), %rax
	setb	%cl
	movb	%cl, -121(%rbp)         # 1-byte Spill
.LBB15_7:                               #   in Loop: Header=BB15_5 Depth=1
	movb	-121(%rbp), %al         # 1-byte Reload
	testb	$1, %al
	jne	.LBB15_8
	jmp	.LBB15_78
.LBB15_8:                               #   in Loop: Header=BB15_5 Depth=1
	movq	-72(%rbp), %rax
	movq	-8(%rbp), %rcx
	movq	(%rcx,%rax,8), %rax
	andq	$1, %rax
	cmpq	$0, %rax
	jne	.LBB15_14
# BB#9:                                 #   in Loop: Header=BB15_5 Depth=1
	movabsq	$274877906944, %rax     # imm = 0x4000000000
	cmpq	%rax, -32(%rbp)
	jb	.LBB15_14
# BB#10:                                #   in Loop: Header=BB15_5 Depth=1
	movabsq	$549755813888, %rax     # imm = 0x8000000000
	cmpq	%rax, -32(%rbp)
	ja	.LBB15_14
# BB#11:                                #   in Loop: Header=BB15_5 Depth=1
	cmpq	$0, -40(%rbp)
	jne	.LBB15_13
# BB#12:                                #   in Loop: Header=BB15_5 Depth=1
	movq	-72(%rbp), %rax
	shlq	$39, %rax
	movq	%rax, -56(%rbp)
.LBB15_13:                              #   in Loop: Header=BB15_5 Depth=1
	movq	-72(%rbp), %rax
	movq	%rax, -48(%rbp)
	movabsq	$274877906944, %rax     # imm = 0x4000000000
	addq	-40(%rbp), %rax
	movq	%rax, -40(%rbp)
	jmp	.LBB15_76
.LBB15_14:                              #   in Loop: Header=BB15_5 Depth=1
	movq	-72(%rbp), %rax
	movq	-8(%rbp), %rcx
	movq	(%rcx,%rax,8), %rax
	andq	$1, %rax
	cmpq	$1, %rax
	jne	.LBB15_72
# BB#15:                                #   in Loop: Header=BB15_5 Depth=1
	xorl	%eax, %eax
	movq	-72(%rbp), %rcx
	movq	-8(%rbp), %rdx
	movabsq	$1152921504606842880, %rsi # imm = 0xFFFFFFFFFFFF000
	andq	(%rdx,%rcx,8), %rsi
	xorl	%edi, %edi
	movl	%eax, -128(%rbp)        # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -80(%rbp)
	cmpq	$0, -40(%rbp)
	jbe	.LBB15_19
# BB#16:                                #   in Loop: Header=BB15_5 Depth=1
	cmpq	$0, -72(%rbp)
	je	.LBB15_19
# BB#17:                                #   in Loop: Header=BB15_5 Depth=1
	movq	-72(%rbp), %rax
	subq	$1, %rax
	cmpq	-48(%rbp), %rax
	je	.LBB15_19
# BB#18:                                #   in Loop: Header=BB15_5 Depth=1
	movq	$0, -40(%rbp)
.LBB15_19:                              #   in Loop: Header=BB15_5 Depth=1
	movq	$0, -88(%rbp)
.LBB15_20:                              #   Parent Loop BB15_5 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB15_35 Depth 3
                                        #         Child Loop BB15_50 Depth 4
	xorl	%eax, %eax
	movb	%al, %cl
	cmpq	$512, -88(%rbp)         # imm = 0x200
	movb	%cl, -129(%rbp)         # 1-byte Spill
	jae	.LBB15_22
# BB#21:                                #   in Loop: Header=BB15_20 Depth=2
	movq	-40(%rbp), %rax
	cmpq	-32(%rbp), %rax
	setb	%cl
	movb	%cl, -129(%rbp)         # 1-byte Spill
.LBB15_22:                              #   in Loop: Header=BB15_20 Depth=2
	movb	-129(%rbp), %al         # 1-byte Reload
	testb	$1, %al
	jne	.LBB15_23
	jmp	.LBB15_71
.LBB15_23:                              #   in Loop: Header=BB15_20 Depth=2
	movq	-88(%rbp), %rax
	movq	-80(%rbp), %rcx
	movq	(%rcx,%rax,8), %rax
	andq	$1, %rax
	cmpq	$0, %rax
	jne	.LBB15_28
# BB#24:                                #   in Loop: Header=BB15_20 Depth=2
	movq	-32(%rbp), %rax
	subq	-40(%rbp), %rax
	cmpq	$1073741824, %rax       # imm = 0x40000000
	jb	.LBB15_28
# BB#25:                                #   in Loop: Header=BB15_20 Depth=2
	cmpq	$0, -40(%rbp)
	jne	.LBB15_27
# BB#26:                                #   in Loop: Header=BB15_20 Depth=2
	movq	-72(%rbp), %rax
	shlq	$39, %rax
	movq	-88(%rbp), %rcx
	shlq	$30, %rcx
	orq	%rcx, %rax
	movq	%rax, -56(%rbp)
.LBB15_27:                              #   in Loop: Header=BB15_20 Depth=2
	movq	-40(%rbp), %rax
	addq	$1073741824, %rax       # imm = 0x40000000
	movq	%rax, -40(%rbp)
	movq	-88(%rbp), %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB15_69
.LBB15_28:                              #   in Loop: Header=BB15_20 Depth=2
	movq	-88(%rbp), %rax
	movq	-80(%rbp), %rcx
	movq	(%rcx,%rax,8), %rax
	andq	$1, %rax
	cmpq	$1, %rax
	jne	.LBB15_68
# BB#29:                                #   in Loop: Header=BB15_20 Depth=2
	movq	-32(%rbp), %rax
	subq	-40(%rbp), %rax
	cmpq	$1073741824, %rax       # imm = 0x40000000
	jae	.LBB15_68
# BB#30:                                #   in Loop: Header=BB15_20 Depth=2
	xorl	%eax, %eax
	movq	-88(%rbp), %rcx
	movq	-80(%rbp), %rdx
	movabsq	$1152921504606842880, %rsi # imm = 0xFFFFFFFFFFFF000
	andq	(%rdx,%rcx,8), %rsi
	xorl	%edi, %edi
	movl	%eax, -136(%rbp)        # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -96(%rbp)
	cmpq	$0, -40(%rbp)
	jbe	.LBB15_34
# BB#31:                                #   in Loop: Header=BB15_20 Depth=2
	cmpq	$0, -88(%rbp)
	je	.LBB15_34
# BB#32:                                #   in Loop: Header=BB15_20 Depth=2
	movq	-88(%rbp), %rax
	subq	$1, %rax
	cmpq	-48(%rbp), %rax
	je	.LBB15_34
# BB#33:                                #   in Loop: Header=BB15_20 Depth=2
	movq	$0, -40(%rbp)
.LBB15_34:                              #   in Loop: Header=BB15_20 Depth=2
	movq	$0, -104(%rbp)
.LBB15_35:                              #   Parent Loop BB15_5 Depth=1
                                        #     Parent Loop BB15_20 Depth=2
                                        # =>    This Loop Header: Depth=3
                                        #         Child Loop BB15_50 Depth 4
	xorl	%eax, %eax
	movb	%al, %cl
	cmpq	$512, -104(%rbp)        # imm = 0x200
	movb	%cl, -137(%rbp)         # 1-byte Spill
	jae	.LBB15_37
# BB#36:                                #   in Loop: Header=BB15_35 Depth=3
	movq	-40(%rbp), %rax
	cmpq	-32(%rbp), %rax
	setb	%cl
	movb	%cl, -137(%rbp)         # 1-byte Spill
.LBB15_37:                              #   in Loop: Header=BB15_35 Depth=3
	movb	-137(%rbp), %al         # 1-byte Reload
	testb	$1, %al
	jne	.LBB15_38
	jmp	.LBB15_67
.LBB15_38:                              #   in Loop: Header=BB15_35 Depth=3
	movq	-104(%rbp), %rax
	movq	-96(%rbp), %rcx
	movq	(%rcx,%rax,8), %rax
	andq	$1, %rax
	cmpq	$0, %rax
	jne	.LBB15_43
# BB#39:                                #   in Loop: Header=BB15_35 Depth=3
	movq	-32(%rbp), %rax
	subq	-40(%rbp), %rax
	cmpq	$2097152, %rax          # imm = 0x200000
	jb	.LBB15_43
# BB#40:                                #   in Loop: Header=BB15_35 Depth=3
	cmpq	$0, -40(%rbp)
	jne	.LBB15_42
# BB#41:                                #   in Loop: Header=BB15_35 Depth=3
	movq	-72(%rbp), %rax
	shlq	$39, %rax
	movq	-88(%rbp), %rcx
	shlq	$30, %rcx
	orq	%rcx, %rax
	movq	-104(%rbp), %rcx
	shlq	$21, %rcx
	orq	%rcx, %rax
	movq	%rax, -56(%rbp)
.LBB15_42:                              #   in Loop: Header=BB15_35 Depth=3
	movq	-40(%rbp), %rax
	addq	$2097152, %rax          # imm = 0x200000
	movq	%rax, -40(%rbp)
	movq	-104(%rbp), %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB15_65
.LBB15_43:                              #   in Loop: Header=BB15_35 Depth=3
	movq	-104(%rbp), %rax
	movq	-96(%rbp), %rcx
	movq	(%rcx,%rax,8), %rax
	andq	$1, %rax
	cmpq	$1, %rax
	jne	.LBB15_64
# BB#44:                                #   in Loop: Header=BB15_35 Depth=3
	movq	-32(%rbp), %rax
	subq	-40(%rbp), %rax
	cmpq	$2097152, %rax          # imm = 0x200000
	jae	.LBB15_64
# BB#45:                                #   in Loop: Header=BB15_35 Depth=3
	xorl	%eax, %eax
	movq	-104(%rbp), %rcx
	movq	-96(%rbp), %rdx
	movabsq	$1152921504606842880, %rsi # imm = 0xFFFFFFFFFFFF000
	andq	(%rdx,%rcx,8), %rsi
	xorl	%edi, %edi
	movl	%eax, -144(%rbp)        # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -112(%rbp)
	cmpq	$0, -40(%rbp)
	jbe	.LBB15_49
# BB#46:                                #   in Loop: Header=BB15_35 Depth=3
	cmpq	$0, -104(%rbp)
	je	.LBB15_49
# BB#47:                                #   in Loop: Header=BB15_35 Depth=3
	movq	-104(%rbp), %rax
	subq	$1, %rax
	cmpq	-48(%rbp), %rax
	je	.LBB15_49
# BB#48:                                #   in Loop: Header=BB15_35 Depth=3
	movq	$0, -40(%rbp)
.LBB15_49:                              #   in Loop: Header=BB15_35 Depth=3
	movq	$0, -120(%rbp)
.LBB15_50:                              #   Parent Loop BB15_5 Depth=1
                                        #     Parent Loop BB15_20 Depth=2
                                        #       Parent Loop BB15_35 Depth=3
                                        # =>      This Inner Loop Header: Depth=4
	xorl	%eax, %eax
	movb	%al, %cl
	cmpq	$512, -120(%rbp)        # imm = 0x200
	movb	%cl, -145(%rbp)         # 1-byte Spill
	jae	.LBB15_52
# BB#51:                                #   in Loop: Header=BB15_50 Depth=4
	movq	-40(%rbp), %rax
	cmpq	-32(%rbp), %rax
	setb	%cl
	movb	%cl, -145(%rbp)         # 1-byte Spill
.LBB15_52:                              #   in Loop: Header=BB15_50 Depth=4
	movb	-145(%rbp), %al         # 1-byte Reload
	testb	$1, %al
	jne	.LBB15_53
	jmp	.LBB15_63
.LBB15_53:                              #   in Loop: Header=BB15_50 Depth=4
	cmpq	$0, -120(%rbp)
	je	.LBB15_56
# BB#54:                                #   in Loop: Header=BB15_50 Depth=4
	movq	-48(%rbp), %rax
	movq	-120(%rbp), %rcx
	subq	$1, %rcx
	cmpq	%rcx, %rax
	je	.LBB15_56
# BB#55:                                #   in Loop: Header=BB15_50 Depth=4
	movq	$0, -40(%rbp)
.LBB15_56:                              #   in Loop: Header=BB15_50 Depth=4
	movq	-120(%rbp), %rax
	movq	-112(%rbp), %rcx
	movq	(%rcx,%rax,8), %rax
	andq	$1, %rax
	cmpq	$0, %rax
	jne	.LBB15_61
# BB#57:                                #   in Loop: Header=BB15_50 Depth=4
	movq	-32(%rbp), %rax
	subq	-40(%rbp), %rax
	cmpq	$4096, %rax             # imm = 0x1000
	jb	.LBB15_61
# BB#58:                                #   in Loop: Header=BB15_50 Depth=4
	cmpq	$0, -40(%rbp)
	jne	.LBB15_60
# BB#59:                                #   in Loop: Header=BB15_50 Depth=4
	movq	-72(%rbp), %rax
	shlq	$39, %rax
	movq	-88(%rbp), %rcx
	shlq	$30, %rcx
	orq	%rcx, %rax
	movq	-104(%rbp), %rcx
	shlq	$21, %rcx
	orq	%rcx, %rax
	movq	-120(%rbp), %rcx
	shlq	$12, %rcx
	orq	%rcx, %rax
	movq	%rax, -56(%rbp)
.LBB15_60:                              #   in Loop: Header=BB15_50 Depth=4
	movq	-40(%rbp), %rax
	addq	$4096, %rax             # imm = 0x1000
	movq	%rax, -40(%rbp)
	movq	-120(%rbp), %rax
	movq	%rax, -48(%rbp)
.LBB15_61:                              #   in Loop: Header=BB15_50 Depth=4
	jmp	.LBB15_62
.LBB15_62:                              #   in Loop: Header=BB15_50 Depth=4
	movq	-120(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -120(%rbp)
	jmp	.LBB15_50
.LBB15_63:                              #   in Loop: Header=BB15_35 Depth=3
	jmp	.LBB15_64
.LBB15_64:                              #   in Loop: Header=BB15_35 Depth=3
	jmp	.LBB15_65
.LBB15_65:                              #   in Loop: Header=BB15_35 Depth=3
	jmp	.LBB15_66
.LBB15_66:                              #   in Loop: Header=BB15_35 Depth=3
	movq	-104(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -104(%rbp)
	jmp	.LBB15_35
.LBB15_67:                              #   in Loop: Header=BB15_20 Depth=2
	jmp	.LBB15_68
.LBB15_68:                              #   in Loop: Header=BB15_20 Depth=2
	jmp	.LBB15_69
.LBB15_69:                              #   in Loop: Header=BB15_20 Depth=2
	jmp	.LBB15_70
.LBB15_70:                              #   in Loop: Header=BB15_20 Depth=2
	movq	-88(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -88(%rbp)
	jmp	.LBB15_20
.LBB15_71:                              #   in Loop: Header=BB15_5 Depth=1
	jmp	.LBB15_75
.LBB15_72:                              #   in Loop: Header=BB15_5 Depth=1
	cmpq	$0, -40(%rbp)
	jne	.LBB15_74
# BB#73:                                #   in Loop: Header=BB15_5 Depth=1
	movq	-72(%rbp), %rax
	shlq	$39, %rax
	movq	%rax, -56(%rbp)
.LBB15_74:                              #   in Loop: Header=BB15_5 Depth=1
	jmp	.LBB15_75
.LBB15_75:                              #   in Loop: Header=BB15_5 Depth=1
	jmp	.LBB15_76
.LBB15_76:                              #   in Loop: Header=BB15_5 Depth=1
	jmp	.LBB15_77
.LBB15_77:                              #   in Loop: Header=BB15_5 Depth=1
	movq	-72(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -72(%rbp)
	jmp	.LBB15_5
.LBB15_78:
	movq	-56(%rbp), %rax
	shrq	$47, %rax
	andq	$1, %rax
	cmpq	$0, %rax
	je	.LBB15_80
# BB#79:
	movabsq	$-281474976710656, %rax # imm = 0xFFFF000000000000
	orq	-56(%rbp), %rax
	movq	%rax, -56(%rbp)
.LBB15_80:
	cmpq	$0, -56(%rbp)
	jne	.LBB15_82
# BB#81:
	movq	$4096, -56(%rbp)        # imm = 0x1000
.LBB15_82:
	movq	-56(%rbp), %rax
	addq	$160, %rsp
	popq	%rbp
	retq
.Lfunc_end15:
	.size	VirtMemMan_FindFreeAddress, .Lfunc_end15-VirtMemMan_FindFreeAddress
	.cfi_endproc

	.type	virtMemData,@object     # @virtMemData
	.local	virtMemData
	.comm	virtMemData,8,8
	.type	coreLocalSpace,@object  # @coreLocalSpace
	.local	coreLocalSpace
	.comm	coreLocalSpace,8,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
