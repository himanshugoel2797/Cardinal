	.text
	.file	"apic/apic_init.c"
	.globl	APIC_Initialize
	.align	16, 0x90
	.type	APIC_Initialize,@function
APIC_Initialize:                        # @APIC_Initialize
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
	subq	$160, %rsp
	movl	$1, %eax
	movl	$0, %ecx
	movl	$1, %edi
	xorl	%esi, %esi
	movl	%eax, -92(%rbp)         # 4-byte Spill
	movl	%ecx, -96(%rbp)         # 4-byte Spill
	callq	CPUID_RequestInfo
	movl	$3, %eax
	movl	$512, %ecx              # imm = 0x200
	movl	$3, %edi
	movl	$512, %esi              # imm = 0x200
	movl	%eax, -100(%rbp)        # 4-byte Spill
	movl	%ecx, -104(%rbp)        # 4-byte Spill
	callq	CPUID_FeatureIsAvailable
	movb	%al, -5(%rbp)
	cmpb	$0, -5(%rbp)
	jne	.LBB0_2
# BB#1:
	movl	$-1, -4(%rbp)
	jmp	.LBB0_41
.LBB0_2:
	movq	$0, smp_threads
	callq	APIC_LocalInitialize
	movq	$.L.str, %rdi
	xorl	%esi, %esi
	movb	%al, -105(%rbp)         # 1-byte Spill
	callq	ACPITables_FindTable
	movq	%rax, -16(%rbp)
	movl	$1, -20(%rbp)
	cmpq	$0, -16(%rbp)
	je	.LBB0_39
# BB#3:
	jmp	.LBB0_4
.LBB0_4:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_6 Depth 2
                                        #       Child Loop BB0_8 Depth 3
                                        #         Child Loop BB0_18 Depth 4
	cmpq	$0, -16(%rbp)
	je	.LBB0_38
# BB#5:                                 #   in Loop: Header=BB0_4 Depth=1
	movq	-16(%rbp), %rax
	movl	4(%rax), %ecx
	subl	$8, %ecx
	movl	%ecx, %ecx
	movl	%ecx, %eax
	subq	$36, %rax
	movl	%eax, %ecx
	movl	%ecx, -24(%rbp)
	movl	$0, -28(%rbp)
.LBB0_6:                                #   Parent Loop BB0_4 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB0_8 Depth 3
                                        #         Child Loop BB0_18 Depth 4
	cmpl	$2, -28(%rbp)
	jge	.LBB0_37
# BB#7:                                 #   in Loop: Header=BB0_6 Depth=2
	movl	$0, -32(%rbp)
.LBB0_8:                                #   Parent Loop BB0_4 Depth=1
                                        #     Parent Loop BB0_6 Depth=2
                                        # =>    This Loop Header: Depth=3
                                        #         Child Loop BB0_18 Depth 4
	movl	-32(%rbp), %eax
	cmpl	-24(%rbp), %eax
	jae	.LBB0_35
# BB#9:                                 #   in Loop: Header=BB0_8 Depth=3
	movl	-32(%rbp), %eax
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	leaq	44(%rdx,%rcx), %rsi
	movq	%rsi, -40(%rbp)
	movzbl	44(%rdx,%rcx), %eax
	movl	%eax, %ecx
	movq	%rcx, %rdx
	subq	$3, %rdx
	movq	%rcx, -120(%rbp)        # 8-byte Spill
	movq	%rdx, -128(%rbp)        # 8-byte Spill
	ja	.LBB0_29
# BB#42:                                #   in Loop: Header=BB0_8 Depth=3
	movq	-120(%rbp), %rax        # 8-byte Reload
	movq	.LJTI0_0(,%rax,8), %rcx
	jmpq	*%rcx
.LBB0_10:                               #   in Loop: Header=BB0_8 Depth=3
	cmpl	$0, -28(%rbp)
	je	.LBB0_12
# BB#11:                                #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_12:                               #   in Loop: Header=BB0_8 Depth=3
	movq	-40(%rbp), %rax
	movq	%rax, -48(%rbp)
	movl	$16, %ecx
	movl	%ecx, %edi
	callq	bootstrap_malloc
	movq	%rax, -56(%rbp)
	movq	-48(%rbp), %rdi
	movzbl	3(%rdi), %ecx
	movl	%ecx, (%rax)
	movq	-48(%rbp), %rax
	movzbl	2(%rax), %ecx
	movq	-56(%rbp), %rax
	movl	%ecx, 4(%rax)
	movq	smp_threads, %rax
	movq	-56(%rbp), %rdi
	movq	%rax, 8(%rdi)
	movq	-56(%rbp), %rax
	movq	%rax, smp_threads
	movq	-48(%rbp), %rax
	movzbl	3(%rax), %ecx
	movl	%ecx, -132(%rbp)        # 4-byte Spill
	callq	APIC_GetID
	movzbl	%al, %ecx
	movl	-132(%rbp), %edx        # 4-byte Reload
	cmpl	%ecx, %edx
	je	.LBB0_14
# BB#13:                                #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_14
.LBB0_14:                               #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_15:                               #   in Loop: Header=BB0_8 Depth=3
	cmpl	$0, -28(%rbp)
	je	.LBB0_17
# BB#16:                                #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_17:                               #   in Loop: Header=BB0_8 Depth=3
	movq	-40(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	movl	4(%rax), %ecx
	movl	%ecx, %edi
	callq	GetVirtualAddress
	movq	-64(%rbp), %rdi
	movl	8(%rdi), %esi
	movq	%rax, %rdi
	callq	IOAPIC_Initialize
	movl	$0, -68(%rbp)
	movb	%al, -133(%rbp)         # 1-byte Spill
.LBB0_18:                               #   Parent Loop BB0_4 Depth=1
                                        #     Parent Loop BB0_6 Depth=2
                                        #       Parent Loop BB0_8 Depth=3
                                        # =>      This Inner Loop Header: Depth=4
	cmpl	$16, -68(%rbp)
	jge	.LBB0_21
# BB#19:                                #   in Loop: Header=BB0_18 Depth=4
	movl	-68(%rbp), %eax
	movb	%al, %cl
	movl	-68(%rbp), %eax
	addl	$32, %eax
	movb	%al, %dl
	movb	%cl, -134(%rbp)         # 1-byte Spill
	movb	%dl, -135(%rbp)         # 1-byte Spill
	callq	APIC_GetID
	movl	$0, %esi
	movzbl	%al, %edi
	movl	%edi, %edx
	movb	-134(%rbp), %al         # 1-byte Reload
	movzbl	%al, %edi
	movb	-135(%rbp), %cl         # 1-byte Reload
	movzbl	%cl, %r8d
	xorl	%r9d, %r9d
	movl	%esi, -140(%rbp)        # 4-byte Spill
	movl	%r8d, %esi
	movl	%r9d, %ecx
	movl	%r9d, %r8d
	callq	IOAPIC_MapIRQ
# BB#20:                                #   in Loop: Header=BB0_18 Depth=4
	movl	-68(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -68(%rbp)
	jmp	.LBB0_18
.LBB0_21:                               #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_22:                               #   in Loop: Header=BB0_8 Depth=3
	cmpl	$0, -28(%rbp)
	jne	.LBB0_24
# BB#23:                                #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_24:                               #   in Loop: Header=BB0_8 Depth=3
	movq	-40(%rbp), %rax
	movq	%rax, -80(%rbp)
	movq	-80(%rbp), %rax
	movzwl	8(%rax), %ecx
	andl	$3, %ecx
	movl	%ecx, -84(%rbp)
	movq	-80(%rbp), %rax
	movzwl	8(%rax), %ecx
	sarl	$2, %ecx
	andl	$3, %ecx
	movl	%ecx, -88(%rbp)
	movq	-80(%rbp), %rax
	movzbl	3(%rax), %ecx
	cmpl	$0, %ecx
	jne	.LBB0_26
# BB#25:                                #   in Loop: Header=BB0_8 Depth=3
	movq	-80(%rbp), %rax
	movl	4(%rax), %ecx
	movb	%cl, %dl
	movq	-80(%rbp), %rax
	movzbl	3(%rax), %ecx
	addl	$32, %ecx
	movb	%cl, %sil
	movb	%dl, -141(%rbp)         # 1-byte Spill
	movb	%sil, -142(%rbp)        # 1-byte Spill
	callq	APIC_GetID
	movl	$0, %ecx
	movzbl	%al, %edi
	movl	%edi, %edx
	movl	-88(%rbp), %edi
	sarl	$1, %edi
	movb	%dil, %al
	movl	-84(%rbp), %edi
	sarl	$1, %edi
	movb	%dil, %sil
	movb	-141(%rbp), %r8b        # 1-byte Reload
	movzbl	%r8b, %edi
	movb	-142(%rbp), %r9b        # 1-byte Reload
	movzbl	%r9b, %r10d
	movzbl	%al, %r11d
	movzbl	%sil, %r8d
	xorl	%r9d, %r9d
	movl	%r10d, %esi
	movl	%ecx, -148(%rbp)        # 4-byte Spill
	movl	%r11d, %ecx
	callq	IOAPIC_MapIRQ
	jmp	.LBB0_27
.LBB0_26:                               #   in Loop: Header=BB0_8 Depth=3
	movq	-80(%rbp), %rax
	movl	4(%rax), %ecx
	movb	%cl, %dl
	movq	-80(%rbp), %rax
	movb	3(%rax), %sil
	movb	%dl, -149(%rbp)         # 1-byte Spill
	movb	%sil, -150(%rbp)        # 1-byte Spill
	callq	APIC_GetID
	movl	$0, %ecx
	movzbl	%al, %edi
	movl	%edi, %edx
	movl	-88(%rbp), %edi
	sarl	$1, %edi
	movb	%dil, %al
	movl	-84(%rbp), %edi
	sarl	$1, %edi
	movb	%dil, %sil
	movb	-149(%rbp), %r8b        # 1-byte Reload
	movzbl	%r8b, %edi
	movb	-150(%rbp), %r9b        # 1-byte Reload
	movzbl	%r9b, %r10d
	movzbl	%al, %r11d
	movzbl	%sil, %r8d
	xorl	%r9d, %r9d
	movl	%r10d, %esi
	movl	%ecx, -156(%rbp)        # 4-byte Spill
	movl	%r11d, %ecx
	callq	IOAPIC_MapIRQ
.LBB0_27:                               #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_28:                               #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_29:                               #   in Loop: Header=BB0_8 Depth=3
	cmpl	$0, -28(%rbp)
	je	.LBB0_31
# BB#30:                                #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_31:                               #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_32
.LBB0_32:                               #   in Loop: Header=BB0_8 Depth=3
	movq	-40(%rbp), %rax
	movzbl	1(%rax), %ecx
	movl	-32(%rbp), %edx
	addl	%ecx, %edx
	movl	%edx, -32(%rbp)
	movq	-40(%rbp), %rax
	movzbl	1(%rax), %ecx
	cmpl	$0, %ecx
	jne	.LBB0_34
# BB#33:                                #   in Loop: Header=BB0_8 Depth=3
	movl	-32(%rbp), %eax
	addl	$8, %eax
	movl	%eax, -32(%rbp)
.LBB0_34:                               #   in Loop: Header=BB0_8 Depth=3
	jmp	.LBB0_8
.LBB0_35:                               #   in Loop: Header=BB0_6 Depth=2
	jmp	.LBB0_36
.LBB0_36:                               #   in Loop: Header=BB0_6 Depth=2
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB0_6
.LBB0_37:                               #   in Loop: Header=BB0_4 Depth=1
	movl	-20(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	movl	%ecx, -20(%rbp)
	movq	$.L.str, %rdi
	movl	%eax, %esi
	callq	ACPITables_FindTable
	movq	%rax, -16(%rbp)
	jmp	.LBB0_4
.LBB0_38:
	jmp	.LBB0_40
.LBB0_39:
	movl	$-1, -4(%rbp)
	jmp	.LBB0_41
.LBB0_40:
	movl	$0, -4(%rbp)
.LBB0_41:
	movl	-4(%rbp), %eax
	addq	$160, %rsp
	popq	%rbp
	retq
.Ltmp3:
	.size	APIC_Initialize, .Ltmp3-APIC_Initialize
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI0_0:
	.quad	.LBB0_10
	.quad	.LBB0_15
	.quad	.LBB0_22
	.quad	.LBB0_28

	.type	smp_threads,@object     # @smp_threads
	.local	smp_threads
	.comm	smp_threads,8,8
	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"APIC"
	.size	.L.str, 5


	.ident	"clang version 3.5.0 (tags/RELEASE_350/final)"
	.section	".note.GNU-stack","",@progbits
