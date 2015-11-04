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
	subq	$112, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movq	rsdp, %rdi
	cmpq	$0, %rdi
	jne	.LBB2_2
# BB#1:
	movq	$0, -8(%rbp)
	jmp	.LBB2_28
.LBB2_2:
	movq	rsdp, %rax
	movzbl	15(%rax), %ecx
	cmpl	$0, %ecx
	jne	.LBB2_15
# BB#3:
	movq	rsdp, %rax
	movl	16(%rax), %ecx
	movl	%ecx, %edi
	callq	GetVirtualAddress
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	callq	ACPITables_ValidateChecksum
	cmpb	$0, %al
	jne	.LBB2_5
# BB#4:
	movq	$-1, %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_28
.LBB2_5:
	movq	-32(%rbp), %rax
	movl	4(%rax), %ecx
	movl	%ecx, %eax
	subq	$36, %rax
	shrq	$2, %rax
	movl	%eax, %ecx
	movl	%ecx, -36(%rbp)
	movl	$0, -40(%rbp)
	movl	$0, -44(%rbp)
.LBB2_6:                                # =>This Inner Loop Header: Depth=1
	movl	-44(%rbp), %eax
	cmpl	-36(%rbp), %eax
	jge	.LBB2_14
# BB#7:                                 #   in Loop: Header=BB2_6 Depth=1
	movslq	-44(%rbp), %rax
	movq	-32(%rbp), %rcx
	movl	36(%rcx,%rax,4), %edx
	movl	%edx, %edi
	callq	GetVirtualAddress
	movl	$4, %edx
	movl	%edx, %ecx
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movl	$4, %edx
                                        # kill: RDX<def> EDX<kill>
	movq	%rcx, -96(%rbp)         # 8-byte Spill
	callq	strncmp
	cmpl	$0, %eax
	jne	.LBB2_12
# BB#8:                                 #   in Loop: Header=BB2_6 Depth=1
	movq	-56(%rbp), %rdi
	callq	ACPITables_ValidateChecksum
	movzbl	%al, %ecx
	cmpl	$0, %ecx
	je	.LBB2_12
# BB#9:                                 #   in Loop: Header=BB2_6 Depth=1
	movl	-40(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jne	.LBB2_11
# BB#10:
	movq	-56(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_28
.LBB2_11:                               #   in Loop: Header=BB2_6 Depth=1
	movl	-40(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -40(%rbp)
.LBB2_12:                               #   in Loop: Header=BB2_6 Depth=1
	jmp	.LBB2_13
.LBB2_13:                               #   in Loop: Header=BB2_6 Depth=1
	movl	-44(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -44(%rbp)
	jmp	.LBB2_6
.LBB2_14:
	jmp	.LBB2_27
.LBB2_15:
	movq	rsdp, %rax
	movq	24(%rax), %rdi
	callq	GetVirtualAddress
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	callq	ACPITables_ValidateChecksum
	cmpb	$0, %al
	jne	.LBB2_17
# BB#16:
	movq	$-1, %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_28
.LBB2_17:
	movq	-64(%rbp), %rax
	movl	4(%rax), %ecx
	movl	%ecx, %eax
	subq	$36, %rax
	shrq	$3, %rax
	movl	%eax, %ecx
	movl	%ecx, -68(%rbp)
	movl	$0, -72(%rbp)
	movl	$0, -76(%rbp)
.LBB2_18:                               # =>This Inner Loop Header: Depth=1
	movl	-76(%rbp), %eax
	cmpl	-68(%rbp), %eax
	jge	.LBB2_26
# BB#19:                                #   in Loop: Header=BB2_18 Depth=1
	movslq	-76(%rbp), %rax
	movq	-64(%rbp), %rcx
	movq	40(%rcx,%rax,8), %rdi
	callq	GetVirtualAddress
	movl	$4, %edx
	movl	%edx, %ecx
	movq	%rax, -88(%rbp)
	movq	-88(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movl	$4, %edx
                                        # kill: RDX<def> EDX<kill>
	movq	%rcx, -104(%rbp)        # 8-byte Spill
	callq	strncmp
	cmpl	$0, %eax
	jne	.LBB2_24
# BB#20:                                #   in Loop: Header=BB2_18 Depth=1
	movq	-88(%rbp), %rdi
	callq	ACPITables_ValidateChecksum
	movzbl	%al, %ecx
	cmpl	$0, %ecx
	je	.LBB2_24
# BB#21:                                #   in Loop: Header=BB2_18 Depth=1
	movl	-72(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jne	.LBB2_23
# BB#22:
	movq	-88(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_28
.LBB2_23:                               #   in Loop: Header=BB2_18 Depth=1
	movl	-72(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -72(%rbp)
.LBB2_24:                               #   in Loop: Header=BB2_18 Depth=1
	jmp	.LBB2_25
.LBB2_25:                               #   in Loop: Header=BB2_18 Depth=1
	movl	-76(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -76(%rbp)
	jmp	.LBB2_18
.LBB2_26:
	jmp	.LBB2_27
.LBB2_27:
	movq	$0, -8(%rbp)
.LBB2_28:
	movq	-8(%rbp), %rax
	addq	$112, %rsp
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
