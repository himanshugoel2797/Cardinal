	.text
	.file	"acpi/acpi_tables.c"
	.globl	ACPITables_Initialize
	.align	16, 0x90
	.type	ACPITables_Initialize,@function
ACPITables_Initialize:                  # @ACPITables_Initialize
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
	movq	$0, rsdp
	callq	GetBootInfo
	movq	40(%rax), %rax
	movq	%rax, -16(%rbp)
	movzbl	(%rax), %ecx
	movb	.L.str, %dl
	movsbl	%dl, %esi
	cmpl	%esi, %ecx
	jne	.LBB0_11
# BB#1:
	movq	-16(%rbp), %rdi
	movq	$.L.str, %rsi
	movl	$8, %eax
	movl	%eax, %edx
	callq	strncmp
	cmpl	$0, %eax
	jne	.LBB0_10
# BB#2:
	movq	-16(%rbp), %rax
	movq	%rax, rsdp
	movl	$0, -20(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -32(%rbp)
.LBB0_3:                                # =>This Inner Loop Header: Depth=1
	movq	-32(%rbp), %rax
	movq	-16(%rbp), %rcx
	addq	$20, %rcx
	cmpq	%rcx, %rax
	jae	.LBB0_6
# BB#4:                                 #   in Loop: Header=BB0_3 Depth=1
	movq	-32(%rbp), %rax
	movzbl	(%rax), %ecx
	addl	-20(%rbp), %ecx
	movl	%ecx, -20(%rbp)
# BB#5:                                 #   in Loop: Header=BB0_3 Depth=1
	movq	-32(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -32(%rbp)
	jmp	.LBB0_3
.LBB0_6:
	movl	-20(%rbp), %eax
	andl	$255, %eax
	cmpl	$0, %eax
	jne	.LBB0_8
# BB#7:
	movq	rsdp, %rax
	movb	15(%rax), %cl
	movb	%cl, -1(%rbp)
	jmp	.LBB0_12
.LBB0_8:
	movq	$0, rsdp
# BB#9:
	jmp	.LBB0_10
.LBB0_10:
	jmp	.LBB0_11
.LBB0_11:
	movb	$-1, -1(%rbp)
.LBB0_12:
	movzbl	-1(%rbp), %eax
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	ACPITables_Initialize, .Lfunc_end0-ACPITables_Initialize
	.cfi_endproc

	.globl	ACPITables_ValidateChecksum
	.align	16, 0x90
	.type	ACPITables_ValidateChecksum,@function
ACPITables_ValidateChecksum:            # @ACPITables_ValidateChecksum
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
	movb	$0, -9(%rbp)
	movl	$0, -16(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movl	-16(%rbp), %eax
	movq	-8(%rbp), %rcx
	cmpl	4(%rcx), %eax
	jae	.LBB1_4
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	movl	-16(%rbp), %eax
	movl	%eax, %ecx
	movq	-8(%rbp), %rdx
	movsbl	(%rdx,%rcx), %eax
	movzbl	-9(%rbp), %esi
	addl	%eax, %esi
	movb	%sil, %dil
	movb	%dil, -9(%rbp)
# BB#3:                                 #   in Loop: Header=BB1_1 Depth=1
	movl	-16(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	jmp	.LBB1_1
.LBB1_4:
	movzbl	-9(%rbp), %eax
	cmpl	$0, %eax
	sete	%cl
	andb	$1, %cl
	movzbl	%cl, %eax
	movb	%al, %cl
	movzbl	%cl, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	ACPITables_ValidateChecksum, .Lfunc_end1-ACPITables_ValidateChecksum
	.cfi_endproc

	.globl	ACPITables_FindTable
	.align	16, 0x90
	.type	ACPITables_FindTable,@function
ACPITables_FindTable:                   # @ACPITables_FindTable
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
	subq	$128, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movq	rsdp, %rdi
	cmpq	$0, %rdi
	jne	.LBB2_2
# BB#1:
	movq	$0, -8(%rbp)
	jmp	.LBB2_33
.LBB2_2:
	movq	rsdp, %rax
	movzbl	15(%rax), %ecx
	cmpl	$0, %ecx
	je	.LBB2_18
# BB#3:
	movq	rsdp, %rax
	cmpq	$0, 24(%rax)
	je	.LBB2_18
# BB#4:
	xorl	%eax, %eax
	movq	rsdp, %rcx
	movq	24(%rcx), %rsi
	xorl	%edi, %edi
	movl	%eax, -92(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	callq	ACPITables_ValidateChecksum
	cmpb	$0, %al
	jne	.LBB2_6
# BB#5:
	movq	$-1, %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_33
.LBB2_6:
	movq	-32(%rbp), %rax
	movl	4(%rax), %ecx
	movl	%ecx, %eax
	subq	$36, %rax
	shrq	$3, %rax
	movl	%eax, %ecx
	movl	%ecx, -36(%rbp)
	movl	$0, -40(%rbp)
	movl	$0, -44(%rbp)
.LBB2_7:                                # =>This Inner Loop Header: Depth=1
	movl	-44(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jge	.LBB2_17
# BB#8:                                 #   in Loop: Header=BB2_7 Depth=1
	movslq	-44(%rbp), %rax
	movq	-32(%rbp), %rcx
	cmpq	$0, 36(%rcx,%rax,8)
	jne	.LBB2_10
# BB#9:                                 #   in Loop: Header=BB2_7 Depth=1
	jmp	.LBB2_16
.LBB2_10:                               #   in Loop: Header=BB2_7 Depth=1
	xorl	%eax, %eax
	movslq	-44(%rbp), %rcx
	movq	-32(%rbp), %rdx
	movq	36(%rdx,%rcx,8), %rsi
	xorl	%edi, %edi
	movl	%eax, -96(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movl	$4, %edi
	movl	%edi, %ecx
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movl	$4, %r8d
	movl	%r8d, %edx
	movq	%rcx, -104(%rbp)        # 8-byte Spill
	callq	strncmp
	cmpl	$0, %eax
	jne	.LBB2_15
# BB#11:                                #   in Loop: Header=BB2_7 Depth=1
	movq	-56(%rbp), %rdi
	callq	ACPITables_ValidateChecksum
	movzbl	%al, %ecx
	cmpl	$0, %ecx
	je	.LBB2_15
# BB#12:                                #   in Loop: Header=BB2_7 Depth=1
	movl	-40(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jne	.LBB2_14
# BB#13:
	movq	-56(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_33
.LBB2_14:                               #   in Loop: Header=BB2_7 Depth=1
	movl	-40(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -40(%rbp)
.LBB2_15:                               #   in Loop: Header=BB2_7 Depth=1
	jmp	.LBB2_16
.LBB2_16:                               #   in Loop: Header=BB2_7 Depth=1
	movl	-44(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -44(%rbp)
	jmp	.LBB2_7
.LBB2_17:
	jmp	.LBB2_32
.LBB2_18:
	movq	rsdp, %rax
	movb	15(%rax), %cl
	testb	%cl, %cl
	sete	%cl
	movzbl	%cl, %edx
	cmpq	$0, 24(%rax)
	setne	%cl
	xorb	$-1, %cl
	andb	$1, %cl
	movzbl	%cl, %esi
	orl	%esi, %edx
	cmpl	$0, %edx
	je	.LBB2_31
# BB#19:
	xorl	%eax, %eax
	movq	rsdp, %rcx
	movl	16(%rcx), %edx
	movl	%edx, %esi
	xorl	%edi, %edi
	movl	%eax, -108(%rbp)        # 4-byte Spill
	callq	GetVirtualAddress
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	callq	ACPITables_ValidateChecksum
	cmpb	$0, %al
	jne	.LBB2_21
# BB#20:
	movq	$0, -8(%rbp)
	jmp	.LBB2_33
.LBB2_21:
	movq	-64(%rbp), %rax
	movl	4(%rax), %ecx
	movl	%ecx, %eax
	subq	$36, %rax
	shrq	$2, %rax
	movl	%eax, %ecx
	movl	%ecx, -68(%rbp)
	movl	$0, -72(%rbp)
	movl	$0, -76(%rbp)
.LBB2_22:                               # =>This Inner Loop Header: Depth=1
	movl	-76(%rbp), %eax
	cmpl	-68(%rbp), %eax
	jge	.LBB2_30
# BB#23:                                #   in Loop: Header=BB2_22 Depth=1
	xorl	%eax, %eax
	movslq	-76(%rbp), %rcx
	movq	-64(%rbp), %rdx
	movl	36(%rdx,%rcx,4), %esi
                                        # kill: RSI<def> ESI<kill>
	xorl	%edi, %edi
	movl	%eax, -112(%rbp)        # 4-byte Spill
	callq	GetVirtualAddress
	movl	$4, %edi
	movl	%edi, %ecx
	movq	%rax, -88(%rbp)
	movq	-88(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movl	$4, %r8d
	movl	%r8d, %edx
	movq	%rcx, -120(%rbp)        # 8-byte Spill
	callq	strncmp
	cmpl	$0, %eax
	jne	.LBB2_28
# BB#24:                                #   in Loop: Header=BB2_22 Depth=1
	movq	-88(%rbp), %rdi
	callq	ACPITables_ValidateChecksum
	movzbl	%al, %ecx
	cmpl	$0, %ecx
	je	.LBB2_28
# BB#25:                                #   in Loop: Header=BB2_22 Depth=1
	movl	-72(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jne	.LBB2_27
# BB#26:
	movq	-88(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_33
.LBB2_27:                               #   in Loop: Header=BB2_22 Depth=1
	movl	-72(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -72(%rbp)
.LBB2_28:                               #   in Loop: Header=BB2_22 Depth=1
	jmp	.LBB2_29
.LBB2_29:                               #   in Loop: Header=BB2_22 Depth=1
	movl	-76(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -76(%rbp)
	jmp	.LBB2_22
.LBB2_30:
	jmp	.LBB2_31
.LBB2_31:
	jmp	.LBB2_32
.LBB2_32:
	movq	$0, -8(%rbp)
.LBB2_33:
	movq	-8(%rbp), %rax
	addq	$128, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	ACPITables_FindTable, .Lfunc_end2-ACPITables_FindTable
	.cfi_endproc

	.type	rsdp,@object            # @rsdp
	.local	rsdp
	.comm	rsdp,8,8
	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"RSD PTR "
	.size	.L.str, 9


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
