	.text
	.file	"pci/pci.c"
	.globl	pci_getDeviceCount
	.align	16, 0x90
	.type	pci_getDeviceCount,@function
pci_getDeviceCount:                     # @pci_getDeviceCount
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
	movl	pci_deviceCount, %eax
	popq	%rbp
	retq
.Lfunc_end0:
	.size	pci_getDeviceCount, .Lfunc_end0-pci_getDeviceCount
	.cfi_endproc

	.globl	pci_getDeviceInfo
	.align	16, 0x90
	.type	pci_getDeviceInfo,@function
pci_getDeviceInfo:                      # @pci_getDeviceInfo
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
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	testq	%rsi, %rsi
	sete	%al
	movzbl	%al, %edi
	movl	-4(%rbp), %ecx
	movl	pci_deviceCount, %edx
	cmpl	%edx, %ecx
	setae	%al
	andb	$1, %al
	movzbl	%al, %ecx
	orl	%ecx, %edi
	cmpl	$0, %edi
	je	.LBB1_2
# BB#1:
	jmp	.LBB1_3
.LBB1_2:
	movl	$128, %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdi
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	shlq	$7, %rdx
	leaq	pci_devices(%rdx), %rdx
	movl	$128, %eax
	movl	%eax, %esi
	movq	%rsi, -24(%rbp)         # 8-byte Spill
	movq	%rdx, %rsi
	movq	-24(%rbp), %rdx         # 8-byte Reload
	movq	%rcx, -32(%rbp)         # 8-byte Spill
	callq	memcpy
	movq	%rax, -40(%rbp)         # 8-byte Spill
.LBB1_3:
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	pci_getDeviceInfo, .Lfunc_end1-pci_getDeviceInfo
	.cfi_endproc

	.globl	pci_readDWord
	.align	16, 0x90
	.type	pci_readDWord,@function
pci_readDWord:                          # @pci_readDWord
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
	movl	$3320, %eax             # imm = 0xCF8
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	-4(%rbp), %ecx
	shll	$16, %ecx
	orl	$2147483648, %ecx       # imm = 0x80000000
	movl	-8(%rbp), %edx
	shll	$11, %edx
	orl	%edx, %ecx
	movl	-12(%rbp), %edx
	shll	$8, %edx
	orl	%edx, %ecx
	movl	-16(%rbp), %edx
	andl	$252, %edx
	orl	%edx, %ecx
	movl	$3320, %edi             # imm = 0xCF8
	movl	%ecx, %esi
	movl	%eax, -20(%rbp)         # 4-byte Spill
	callq	outl
	movl	$3324, %eax             # imm = 0xCFC
	movl	$3324, %edi             # imm = 0xCFC
	movl	%eax, -24(%rbp)         # 4-byte Spill
	callq	inl
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	pci_readDWord, .Lfunc_end2-pci_readDWord
	.cfi_endproc

	.globl	pci_writeDWord
	.align	16, 0x90
	.type	pci_writeDWord,@function
pci_writeDWord:                         # @pci_writeDWord
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
	movl	$3320, %eax             # imm = 0xCF8
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
	movl	-4(%rbp), %ecx
	shll	$16, %ecx
	orl	$2147483648, %ecx       # imm = 0x80000000
	movl	-8(%rbp), %edx
	shll	$11, %edx
	orl	%edx, %ecx
	movl	-12(%rbp), %edx
	shll	$8, %edx
	orl	%edx, %ecx
	movl	-16(%rbp), %edx
	andl	$252, %edx
	orl	%edx, %ecx
	movl	$3320, %edi             # imm = 0xCF8
	movl	%ecx, %esi
	movl	%eax, -24(%rbp)         # 4-byte Spill
	callq	outl
	movl	$3324, %eax             # imm = 0xCFC
	movl	-20(%rbp), %esi
	movl	$3324, %edi             # imm = 0xCFC
	movl	%eax, -28(%rbp)         # 4-byte Spill
	callq	outl
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	pci_writeDWord, .Lfunc_end3-pci_writeDWord
	.cfi_endproc

	.globl	pci_enableMSI
	.align	16, 0x90
	.type	pci_enableMSI,@function
pci_enableMSI:                          # @pci_enableMSI
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
	subq	$48, %rsp
	movl	$52, %eax
	movl	%edi, -8(%rbp)
	movb	$0, -9(%rbp)
	movl	-8(%rbp), %edi
	movl	%edi, %ecx
	shlq	$7, %rcx
	movl	pci_devices+8(%rcx), %edi
	movl	pci_devices+12(%rcx), %esi
	movl	pci_devices+16(%rcx), %edx
	movl	$4, %r8d
	movl	%r8d, %ecx
	movl	%eax, -32(%rbp)         # 4-byte Spill
	movl	%r8d, -36(%rbp)         # 4-byte Spill
	callq	pci_readDWord
	movl	%eax, -16(%rbp)
	andl	$-1025, %eax            # imm = 0xFFFFFFFFFFFFFBFF
	movl	%eax, -16(%rbp)
	orl	$4, %eax
	movl	%eax, -16(%rbp)
	movl	-8(%rbp), %ecx
	movl	%ecx, %r9d
	shlq	$7, %r9
	movl	pci_devices+8(%r9), %edi
	movl	pci_devices+12(%r9), %esi
	movl	pci_devices+16(%r9), %edx
	movl	-36(%rbp), %ecx         # 4-byte Reload
	movl	%eax, %r8d
	callq	pci_writeDWord
	movl	-8(%rbp), %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	leaq	pci_devices(%r9), %r10
	movl	pci_devices+8(%r9), %edi
	movl	pci_devices+12(%r9), %esi
	movl	16(%r10), %edx
	movl	$52, %ecx
	callq	pci_readDWord
	andl	$255, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	shll	$8, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	andl	$65535, %eax            # imm = 0xFFFF
	movl	%eax, -28(%rbp)
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movl	-28(%rbp), %eax
	andl	$255, %eax
	cmpl	$5, %eax
	je	.LBB4_5
# BB#2:                                 #   in Loop: Header=BB4_1 Depth=1
	movl	-28(%rbp), %eax
	movl	%eax, -24(%rbp)
	cmpl	$0, -24(%rbp)
	jne	.LBB4_4
# BB#3:
	movb	$0, -1(%rbp)
	jmp	.LBB4_6
.LBB4_4:                                #   in Loop: Header=BB4_1 Depth=1
	movl	-8(%rbp), %eax
	movl	%eax, %ecx
	shlq	$7, %rcx
	leaq	pci_devices(%rcx), %rdx
	movl	pci_devices+8(%rcx), %edi
	movl	pci_devices+12(%rcx), %esi
	movl	16(%rdx), %edx
	movl	-24(%rbp), %eax
	shrl	$8, %eax
	movl	%eax, %ecx
	callq	pci_readDWord
	andl	$65535, %eax            # imm = 0xFFFF
	movl	%eax, -28(%rbp)
	jmp	.LBB4_1
.LBB4_5:
	movl	-24(%rbp), %eax
	shrl	$8, %eax
	movl	%eax, -24(%rbp)
	movl	-8(%rbp), %ecx
	movl	%ecx, %edx
	shlq	$7, %rdx
	movl	pci_devices+8(%rdx), %edi
	movl	pci_devices+12(%rdx), %esi
	movl	pci_devices+16(%rdx), %edx
	movl	%eax, %ecx
	callq	pci_readDWord
	movl	%eax, -16(%rbp)
	shrl	$23, %eax
	movb	%al, %r8b
	movb	%r8b, -9(%rbp)
	movl	$65536, -16(%rbp)       # imm = 0x10000
	movl	-8(%rbp), %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	movl	pci_devices+8(%r9), %edi
	movl	pci_devices+12(%r9), %esi
	movl	pci_devices+16(%r9), %edx
	movl	-24(%rbp), %ecx
	movl	$65536, %r8d            # imm = 0x10000
	callq	pci_writeDWord
	movl	-8(%rbp), %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	movl	pci_devices+8(%r9), %edi
	movl	pci_devices+12(%r9), %esi
	movl	pci_devices+16(%r9), %edx
	movl	-24(%rbp), %eax
	addl	$4, %eax
	movl	%eax, %ecx
	callq	pci_readDWord
	movl	%eax, -16(%rbp)
	movl	$-18874368, -16(%rbp)   # imm = 0xFFFFFFFFFEE00000
	callq	APIC_GetID
	movzbl	%al, %ecx
	shll	$12, %ecx
	movl	-16(%rbp), %edx
	orl	%ecx, %edx
	movl	%edx, -16(%rbp)
	movl	-8(%rbp), %ecx
	movl	%ecx, %r9d
	shlq	$7, %r9
	leaq	pci_devices(%r9), %r10
	movl	pci_devices+8(%r9), %edi
	movl	pci_devices+12(%r9), %esi
	movl	16(%r10), %edx
	movl	-24(%rbp), %ecx
	addl	$4, %ecx
	movl	-16(%rbp), %r8d
	callq	pci_writeDWord
	movb	$1, -1(%rbp)
.LBB4_6:
	movzbl	-1(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	pci_enableMSI, .Lfunc_end4-pci_enableMSI
	.cfi_endproc

	.globl	pci_regMSIVector
	.align	16, 0x90
	.type	pci_regMSIVector,@function
pci_regMSIVector:                       # @pci_regMSIVector
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
	subq	$48, %rsp
	movb	%sil, %al
	movl	$52, %esi
	movl	%edi, -4(%rbp)
	movb	%al, -5(%rbp)
	movl	-4(%rbp), %edi
	movl	%edi, %ecx
	shlq	$7, %rcx
	leaq	pci_devices(%rcx), %rdx
	movl	pci_devices+8(%rcx), %edi
	movl	pci_devices+12(%rcx), %r8d
	movl	16(%rdx), %edx
	movl	$52, %ecx
	movl	%esi, -36(%rbp)         # 4-byte Spill
	movl	%r8d, %esi
	callq	pci_readDWord
	andl	$255, %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	shll	$8, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	andl	$65535, %eax            # imm = 0xFFFF
	movl	%eax, -20(%rbp)
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	movl	-20(%rbp), %eax
	andl	$255, %eax
	cmpl	$5, %eax
	je	.LBB5_3
# BB#2:                                 #   in Loop: Header=BB5_1 Depth=1
	movl	-20(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	shlq	$7, %rcx
	leaq	pci_devices(%rcx), %rdx
	movl	pci_devices+8(%rcx), %edi
	movl	pci_devices+12(%rcx), %esi
	movl	16(%rdx), %edx
	movl	-16(%rbp), %eax
	shrl	$8, %eax
	movl	%eax, %ecx
	callq	pci_readDWord
	andl	$65535, %eax            # imm = 0xFFFF
	movl	%eax, -20(%rbp)
	jmp	.LBB5_1
.LBB5_3:
	movl	-16(%rbp), %eax
	shrl	$8, %eax
	movl	%eax, -16(%rbp)
	movl	-4(%rbp), %ecx
	movl	%ecx, %edx
	shlq	$7, %rdx
	movl	pci_devices+8(%rdx), %edi
	movl	pci_devices+12(%rdx), %esi
	movl	pci_devices+16(%rdx), %edx
	movl	%eax, %ecx
	callq	pci_readDWord
	movl	%eax, -24(%rbp)
	shrl	$23, %eax
	movb	%al, %r8b
	movb	%r8b, -25(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	leaq	pci_devices(%r9), %r10
	movl	pci_devices+8(%r9), %edi
	movl	pci_devices+12(%r9), %esi
	movl	16(%r10), %edx
	movl	-16(%rbp), %ecx
	movl	-24(%rbp), %r8d
	callq	pci_writeDWord
	movl	-16(%rbp), %eax
	addl	$8, %eax
	movl	%eax, -32(%rbp)
	cmpb	$0, -25(%rbp)
	je	.LBB5_5
# BB#4:
	movl	-32(%rbp), %eax
	addl	$4, %eax
	movl	%eax, -32(%rbp)
.LBB5_5:
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	shlq	$7, %rcx
	movl	pci_devices+8(%rcx), %edi
	movl	pci_devices+12(%rcx), %esi
	movl	pci_devices+16(%rcx), %edx
	movl	-32(%rbp), %ecx
	callq	pci_readDWord
	movl	%eax, -24(%rbp)
	andl	$-256, %eax
	movl	%eax, -24(%rbp)
	movzbl	-5(%rbp), %ecx
	orl	%ecx, %eax
	movl	%eax, -24(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %r8d
	shlq	$7, %r8
	leaq	pci_devices(%r8), %r9
	movl	pci_devices+8(%r8), %edi
	movl	pci_devices+12(%r8), %esi
	movl	16(%r9), %edx
	movl	-32(%rbp), %ecx
	movl	-24(%rbp), %r8d
	callq	pci_writeDWord
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	pci_regMSIVector, .Lfunc_end5-pci_regMSIVector
	.cfi_endproc

	.globl	pci_Initialize
	.align	16, 0x90
	.type	pci_Initialize,@function
pci_Initialize:                         # @pci_Initialize
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
	pushq	%rbx
	subq	$280, %rsp              # imm = 0x118
.Ltmp21:
	.cfi_offset %rbx, -24
	movq	$.L.str, %rdi
	xorl	%esi, %esi
	callq	ACPITables_FindTable
	movq	%rax, -16(%rbp)
	movl	4(%rax), %esi
	movl	%esi, %eax
	addq	$-44, %rax
	shrq	$4, %rax
	movq	%rax, mcfg_entry_count
	shlq	$4, %rax
	movq	%rax, %rdi
	callq	bootstrap_malloc
	movq	%rax, mcfg_table
	movq	-16(%rbp), %rdi
	addq	$48, %rdi
	movq	mcfg_entry_count, %rcx
	shlq	$4, %rcx
	movq	%rdi, -216(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	-216(%rbp), %rsi        # 8-byte Reload
	movq	%rcx, %rdx
	callq	memcpy
	movl	$0, -20(%rbp)
	movq	%rax, -224(%rbp)        # 8-byte Spill
.LBB6_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB6_3 Depth 2
                                        #       Child Loop BB6_8 Depth 3
                                        #         Child Loop BB6_14 Depth 4
	cmpl	$256, -20(%rbp)         # imm = 0x100
	jge	.LBB6_30
# BB#2:                                 #   in Loop: Header=BB6_1 Depth=1
	movl	$0, -24(%rbp)
.LBB6_3:                                #   Parent Loop BB6_1 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB6_8 Depth 3
                                        #         Child Loop BB6_14 Depth 4
	cmpl	$32, -24(%rbp)
	jge	.LBB6_28
# BB#4:                                 #   in Loop: Header=BB6_3 Depth=2
	xorl	%eax, %eax
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	xorl	%ecx, %ecx
	movl	%ecx, %edx
	movl	%eax, -228(%rbp)        # 4-byte Spill
	callq	pci_readDWord
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	shrl	$16, %eax
	cmpl	$65535, %eax            # imm = 0xFFFF
	je	.LBB6_26
# BB#5:                                 #   in Loop: Header=BB6_3 Depth=2
	xorl	%eax, %eax
	movl	$12, %ecx
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	xorl	%edx, %edx
	movl	$12, %r8d
	movl	%ecx, -232(%rbp)        # 4-byte Spill
	movl	%r8d, %ecx
	movl	%eax, -236(%rbp)        # 4-byte Spill
	callq	pci_readDWord
	movl	%eax, -32(%rbp)
	movl	$1, -36(%rbp)
	movl	-32(%rbp), %eax
	sarl	$23, %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB6_7
# BB#6:                                 #   in Loop: Header=BB6_3 Depth=2
	movl	$8, -36(%rbp)
.LBB6_7:                                #   in Loop: Header=BB6_3 Depth=2
	movl	$0, -40(%rbp)
.LBB6_8:                                #   Parent Loop BB6_1 Depth=1
                                        #     Parent Loop BB6_3 Depth=2
                                        # =>    This Loop Header: Depth=3
                                        #         Child Loop BB6_14 Depth 4
	movl	-40(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jge	.LBB6_25
# BB#9:                                 #   in Loop: Header=BB6_8 Depth=3
	xorl	%eax, %eax
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	xorl	%ecx, %ecx
	movl	%eax, -240(%rbp)        # 4-byte Spill
	callq	pci_readDWord
	shrl	$16, %eax
	cmpl	$65535, %eax            # imm = 0xFFFF
	je	.LBB6_23
# BB#10:                                #   in Loop: Header=BB6_8 Depth=3
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	movl	$8, %eax
	movl	%eax, %ecx
	movl	%eax, -244(%rbp)        # 4-byte Spill
	callq	pci_readDWord
	shrl	$24, %eax
	movb	%al, %r8b
	movl	pci_deviceCount, %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	movb	%r8b, pci_devices(%r9)
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	movl	-244(%rbp), %ecx        # 4-byte Reload
	callq	pci_readDWord
	shrl	$16, %eax
	movb	%al, %r8b
	movl	pci_deviceCount, %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	movb	%r8b, pci_devices+1(%r9)
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	movl	-244(%rbp), %ecx        # 4-byte Reload
	callq	pci_readDWord
	movl	pci_deviceCount, %ecx
	movl	%ecx, %r9d
	movq	%r9, %rbx
	shlq	$7, %rbx
	movb	%ah, %al  # NOREX
	movb	%al, pci_devices+2(%rbx)  # NOREX
	movl	-20(%rbp), %ecx
	movl	pci_deviceCount, %edx
	movl	%edx, %r9d
	shlq	$7, %r9
	movl	%ecx, pci_devices+8(%r9)
	movl	-24(%rbp), %ecx
	movl	pci_deviceCount, %edx
	movl	%edx, %r9d
	shlq	$7, %r9
	movl	%ecx, pci_devices+12(%r9)
	movl	-40(%rbp), %ecx
	movl	pci_deviceCount, %edx
	movl	%edx, %r9d
	shlq	$7, %r9
	movl	%ecx, pci_devices+16(%r9)
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	movl	$12, %ecx
	callq	pci_readDWord
	shrl	$16, %eax
	movb	%al, %r8b
	movl	pci_deviceCount, %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	movb	%r8b, pci_devices+120(%r9)
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	xorl	%eax, %eax
	movl	%eax, %ecx
	movl	%eax, -248(%rbp)        # 4-byte Spill
	callq	pci_readDWord
	shrl	$16, %eax
	movw	%ax, %r10w
	movl	pci_deviceCount, %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	movw	%r10w, pci_devices+4(%r9)
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	movl	-248(%rbp), %ecx        # 4-byte Reload
	callq	pci_readDWord
	movw	%ax, %r10w
	movl	pci_deviceCount, %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	movw	%r10w, pci_devices+6(%r9)
	movl	pci_deviceCount, %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	movzbl	pci_devices+120(%r9), %eax
	testl	%eax, %eax
	movl	%eax, -252(%rbp)        # 4-byte Spill
	je	.LBB6_11
	jmp	.LBB6_40
.LBB6_40:                               #   in Loop: Header=BB6_8 Depth=3
	movl	-252(%rbp), %eax        # 4-byte Reload
	subl	$1, %eax
	movl	%eax, -256(%rbp)        # 4-byte Spill
	je	.LBB6_12
	jmp	.LBB6_13
.LBB6_11:                               #   in Loop: Header=BB6_8 Depth=3
	movl	pci_deviceCount, %eax
	movl	%eax, %ecx
	shlq	$7, %rcx
	leaq	pci_devices(%rcx), %rcx
	movb	$6, 20(%rcx)
	jmp	.LBB6_13
.LBB6_12:                               #   in Loop: Header=BB6_8 Depth=3
	movl	pci_deviceCount, %eax
	movl	%eax, %ecx
	shlq	$7, %rcx
	leaq	pci_devices(%rcx), %rcx
	movb	$2, 20(%rcx)
.LBB6_13:                               #   in Loop: Header=BB6_8 Depth=3
	movl	$0, -44(%rbp)
	movl	pci_deviceCount, %eax
	movl	%eax, %ecx
	shlq	$7, %rcx
	leaq	pci_devices(%rcx), %rcx
	movzbl	20(%rcx), %eax
	movl	%eax, -48(%rbp)
	movb	$0, -49(%rbp)
.LBB6_14:                               #   Parent Loop BB6_1 Depth=1
                                        #     Parent Loop BB6_3 Depth=2
                                        #       Parent Loop BB6_8 Depth=3
                                        # =>      This Inner Loop Header: Depth=4
	movzbl	-49(%rbp), %eax
	cmpl	-48(%rbp), %eax
	jae	.LBB6_22
# BB#15:                                #   in Loop: Header=BB6_14 Depth=4
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	movzbl	-49(%rbp), %eax
	movl	%eax, %ecx
	leal	16(,%rcx,4), %ecx
	callq	pci_readDWord
	movl	%eax, %eax
	movl	%eax, %r8d
	movq	%r8, -64(%rbp)
	movzbl	-49(%rbp), %eax
	movl	%eax, %r8d
	movl	%r8d, %eax
	movl	%eax, %r9d
	leal	16(,%r9,4), %eax
	movl	pci_deviceCount, %ecx
	movl	%ecx, %r9d
	shlq	$7, %r9
	shlq	$4, %r8
	movl	%eax, pci_devices+32(%r9,%r8)
	movl	-64(%rbp), %eax
	andl	$1, %eax
	movb	%al, %r10b
	movzbl	-49(%rbp), %eax
	movl	%eax, %r8d
	movl	pci_deviceCount, %eax
	movl	%eax, %r9d
	shlq	$7, %r9
	leaq	pci_devices(%r9), %r9
	addq	$24, %r9
	shlq	$4, %r8
	addq	%r8, %r9
	movb	%r10b, 12(%r9)
	movq	-64(%rbp), %r8
	andq	$3, %r8
	cmpq	$2, %r8
	jne	.LBB6_17
# BB#16:                                #   in Loop: Header=BB6_14 Depth=4
	movb	-49(%rbp), %al
	incb	%al
	movb	%al, -49(%rbp)
	movl	-20(%rbp), %edi
	movl	-24(%rbp), %esi
	movl	-40(%rbp), %edx
	movzbl	-49(%rbp), %ecx
	movl	%ecx, %r8d
	leal	16(,%r8,4), %ecx
	callq	pci_readDWord
	movl	%eax, %eax
	movl	%eax, %r8d
	movq	%r8, -72(%rbp)
	shlq	$32, %r8
	movq	-64(%rbp), %r9
	orq	%r8, %r9
	movq	%r9, -64(%rbp)
	movl	pci_deviceCount, %eax
	movl	%eax, %r8d
	shlq	$7, %r8
	leaq	pci_devices(%r8), %r8
	movb	20(%r8), %r10b
	addb	$-1, %r10b
	movb	%r10b, 20(%r8)
.LBB6_17:                               #   in Loop: Header=BB6_14 Depth=4
	movq	-64(%rbp), %rax
	andq	$1, %rax
	cmpq	$0, %rax
	je	.LBB6_19
# BB#18:                                #   in Loop: Header=BB6_14 Depth=4
	movq	-64(%rbp), %rax
	andq	$-4, %rax
	movzbl	-49(%rbp), %ecx
	movl	%ecx, %edx
	movl	pci_deviceCount, %ecx
	movl	%ecx, %esi
	shlq	$7, %rsi
	leaq	pci_devices(%rsi), %rsi
	addq	$24, %rsi
	shlq	$4, %rdx
	addq	%rdx, %rsi
	movq	%rax, (%rsi)
	jmp	.LBB6_20
.LBB6_19:                               #   in Loop: Header=BB6_14 Depth=4
	movq	-64(%rbp), %rax
	andq	$-16, %rax
	movzbl	-49(%rbp), %ecx
	movl	%ecx, %edx
	movl	pci_deviceCount, %ecx
	movl	%ecx, %esi
	shlq	$7, %rsi
	leaq	pci_devices(%rsi), %rsi
	addq	$24, %rsi
	shlq	$4, %rdx
	addq	%rdx, %rsi
	movq	%rax, (%rsi)
.LBB6_20:                               #   in Loop: Header=BB6_14 Depth=4
	movl	-44(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -44(%rbp)
# BB#21:                                #   in Loop: Header=BB6_14 Depth=4
	movb	-49(%rbp), %al
	addb	$1, %al
	movb	%al, -49(%rbp)
	jmp	.LBB6_14
.LBB6_22:                               #   in Loop: Header=BB6_8 Depth=3
	movl	pci_deviceCount, %eax
	incl	%eax
	movl	%eax, pci_deviceCount
.LBB6_23:                               #   in Loop: Header=BB6_8 Depth=3
	jmp	.LBB6_24
.LBB6_24:                               #   in Loop: Header=BB6_8 Depth=3
	movl	-40(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -40(%rbp)
	jmp	.LBB6_8
.LBB6_25:                               #   in Loop: Header=BB6_3 Depth=2
	jmp	.LBB6_26
.LBB6_26:                               #   in Loop: Header=BB6_3 Depth=2
	jmp	.LBB6_27
.LBB6_27:                               #   in Loop: Header=BB6_3 Depth=2
	movl	-24(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -24(%rbp)
	jmp	.LBB6_3
.LBB6_28:                               #   in Loop: Header=BB6_1 Depth=1
	jmp	.LBB6_29
.LBB6_29:                               #   in Loop: Header=BB6_1 Depth=1
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -20(%rbp)
	jmp	.LBB6_1
.LBB6_30:
	movq	$.L.str.1, %rdi
	callq	RegisterBus
	movl	$0, -76(%rbp)
	movl	%eax, -260(%rbp)        # 4-byte Spill
.LBB6_31:                               # =>This Loop Header: Depth=1
                                        #     Child Loop BB6_33 Depth 2
	movl	-76(%rbp), %eax
	movl	pci_deviceCount, %ecx
	cmpl	%ecx, %eax
	jae	.LBB6_39
# BB#32:                                #   in Loop: Header=BB6_31 Depth=1
	movl	$-1, -80(%rbp)
.LBB6_33:                               #   Parent Loop BB6_31 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	-80(%rbp), %eax
	incl	%eax
	movl	%eax, -80(%rbp)
	movslq	%eax, %rcx
	imulq	$544, %rcx, %rcx        # imm = 0x220
	leaq	drivers(%rcx), %rcx
	cmpq	$0, 528(%rcx)
	je	.LBB6_37
# BB#34:                                #   in Loop: Header=BB6_33 Depth=2
	leaq	-208(%rbp), %rdi
	movl	-76(%rbp), %eax
	leaq	-208(%rbp), %rsi
	movq	%rdi, -272(%rbp)        # 8-byte Spill
	movl	%eax, %edi
	callq	pci_getDeviceInfo
	movslq	-80(%rbp), %rsi
	imulq	$544, %rsi, %rsi        # imm = 0x220
	leaq	drivers(%rsi), %rsi
	movq	-272(%rbp), %rdi        # 8-byte Reload
	callq	*528(%rsi)
	cmpb	$0, %al
	je	.LBB6_36
# BB#35:                                #   in Loop: Header=BB6_31 Depth=1
	movslq	-80(%rbp), %rax
	imulq	$544, %rax, %rax        # imm = 0x220
	leaq	drivers(%rax), %rdi
	leaq	drivers+256(%rax), %rsi
	movl	drivers+512(%rax), %edx
	movq	drivers+520(%rax), %rcx
	callq	RegisterDevice
	movslq	-80(%rbp), %rcx
	imulq	$544, %rcx, %rcx        # imm = 0x220
	movq	drivers+536(%rcx), %rcx
	movl	-76(%rbp), %edx
	movl	%edx, %esi
	shlq	$7, %rsi
	leaq	pci_devices(%rsi), %rdi
	movl	%eax, -276(%rbp)        # 4-byte Spill
	callq	*%rcx
	movb	%al, -277(%rbp)         # 1-byte Spill
	jmp	.LBB6_37
.LBB6_36:                               #   in Loop: Header=BB6_33 Depth=2
	jmp	.LBB6_33
.LBB6_37:                               #   in Loop: Header=BB6_31 Depth=1
	jmp	.LBB6_38
.LBB6_38:                               #   in Loop: Header=BB6_31 Depth=1
	movl	-76(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -76(%rbp)
	jmp	.LBB6_31
.LBB6_39:
	addq	$280, %rsp              # imm = 0x118
	popq	%rbx
	popq	%rbp
	retq
.Lfunc_end6:
	.size	pci_Initialize, .Lfunc_end6-pci_Initialize
	.cfi_endproc

	.type	drivers,@object         # @drivers
	.section	.rodata,"a",@progbits
	.globl	drivers
	.align	16
drivers:
	.asciz	"UHCI\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
	.asciz	"PCI\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
	.long	11                      # 0xb
	.zero	4
	.quad	UHCI_ActivationStateChange
	.quad	UHCI_DetectController
	.quad	UHCI_InitializeController
	.zero	544
	.size	drivers, 1088

	.type	pci_deviceCount,@object # @pci_deviceCount
	.local	pci_deviceCount
	.comm	pci_deviceCount,4,4
	.type	pci_devices,@object     # @pci_devices
	.local	pci_devices
	.comm	pci_devices,16384,16
	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"MCFG"
	.size	.L.str, 5

	.type	mcfg_entry_count,@object # @mcfg_entry_count
	.local	mcfg_entry_count
	.comm	mcfg_entry_count,8,8
	.type	mcfg_table,@object      # @mcfg_table
	.local	mcfg_table
	.comm	mcfg_table,8,8
	.type	.L.str.1,@object        # @.str.1
.L.str.1:
	.asciz	"PCI"
	.size	.L.str.1, 4


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
