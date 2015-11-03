	.text
	.file	"common/sprintf.c"
	.globl	itoa
	.align	16, 0x90
	.type	itoa,@function
itoa:                                   # @itoa
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
	subq	$624, %rsp              # imm = 0x270
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	$.L.str, -552(%rbp)
	cmpl	$16, -20(%rbp)
	jne	.LBB0_6
# BB#1:
	movl	$0, -556(%rbp)
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	cmpl	$16, -556(%rbp)
	jge	.LBB0_5
# BB#3:                                 #   in Loop: Header=BB0_2 Depth=1
	movl	$15, %eax
	movq	-8(%rbp), %rcx
	movl	-556(%rbp), %edx
	shll	$2, %edx
	movl	%edx, %edx
	movl	%edx, %esi
	movq	%rcx, -576(%rbp)        # 8-byte Spill
	movq	%rsi, %rcx
                                        # kill: CL<def> RCX<kill>
	movq	-576(%rbp), %rsi        # 8-byte Reload
	sarq	%cl, %rsi
	andq	$15, %rsi
	movq	-552(%rbp), %rdi
	movb	(%rdi,%rsi), %cl
	subl	-556(%rbp), %eax
	movslq	%eax, %rsi
	movb	%cl, -544(%rbp,%rsi)
# BB#4:                                 #   in Loop: Header=BB0_2 Depth=1
	movl	-556(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -556(%rbp)
	jmp	.LBB0_2
.LBB0_5:
	movb	$0, -528(%rbp)
	jmp	.LBB0_24
.LBB0_6:
	cmpl	$2, -20(%rbp)
	jne	.LBB0_12
# BB#7:
	movl	$0, -560(%rbp)
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	cmpl	$64, -560(%rbp)
	jge	.LBB0_11
# BB#9:                                 #   in Loop: Header=BB0_8 Depth=1
	movl	$63, %eax
	movq	-8(%rbp), %rcx
	movl	-560(%rbp), %edx
	movl	%edx, %esi
	movq	%rcx, -584(%rbp)        # 8-byte Spill
	movq	%rsi, %rcx
                                        # kill: CL<def> RCX<kill>
	movq	-584(%rbp), %rsi        # 8-byte Reload
	sarq	%cl, %rsi
	andq	$1, %rsi
	movq	-552(%rbp), %rdi
	movb	(%rdi,%rsi), %cl
	subl	-560(%rbp), %eax
	movslq	%eax, %rsi
	movb	%cl, -544(%rbp,%rsi)
# BB#10:                                #   in Loop: Header=BB0_8 Depth=1
	movl	-560(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -560(%rbp)
	jmp	.LBB0_8
.LBB0_11:
	movb	$0, -480(%rbp)
	jmp	.LBB0_23
.LBB0_12:
	cmpl	$16, -20(%rbp)
	jge	.LBB0_21
# BB#13:
	movl	$0, -564(%rbp)
	movl	$0, -568(%rbp)
	cmpq	$0, -8(%rbp)
	jge	.LBB0_15
# BB#14:
	movabsq	$0, %rax
	movl	$1, -568(%rbp)
	subq	-8(%rbp), %rax
	movq	%rax, -8(%rbp)
.LBB0_15:
	jmp	.LBB0_16
.LBB0_16:                               # =>This Inner Loop Header: Depth=1
	movq	-8(%rbp), %rax
	movslq	-20(%rbp), %rcx
	cqto
	idivq	%rcx
	movq	-552(%rbp), %rax
	movb	(%rax,%rdx), %sil
	movl	-564(%rbp), %edi
	movl	%edi, %r8d
	addl	$1, %r8d
	movl	%r8d, -564(%rbp)
	movslq	%edi, %rax
	movb	%sil, -544(%rbp,%rax)
	movslq	-20(%rbp), %rax
	movq	-8(%rbp), %rcx
	movq	%rax, -592(%rbp)        # 8-byte Spill
	movq	%rcx, %rax
	cqto
	movq	-592(%rbp), %rcx        # 8-byte Reload
	idivq	%rcx
	movq	%rax, -8(%rbp)
# BB#17:                                #   in Loop: Header=BB0_16 Depth=1
	cmpq	$0, -8(%rbp)
	jne	.LBB0_16
# BB#18:
	cmpl	$0, -568(%rbp)
	je	.LBB0_20
# BB#19:
	movl	-564(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	movl	%ecx, -564(%rbp)
	movslq	%eax, %rdx
	movb	$45, -544(%rbp,%rdx)
.LBB0_20:
	leaq	-544(%rbp), %rdi
	movslq	-564(%rbp), %rax
	movb	$0, -544(%rbp,%rax)
	callq	strrev
	jmp	.LBB0_22
.LBB0_21:
	movb	$0, -544(%rbp)
.LBB0_22:
	jmp	.LBB0_23
.LBB0_23:
	jmp	.LBB0_24
.LBB0_24:
	leaq	-544(%rbp), %rax
	movq	-16(%rbp), %rdi
	movq	%rdi, -600(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	%rax, -608(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-600(%rbp), %rdi        # 8-byte Reload
	movq	-608(%rbp), %rsi        # 8-byte Reload
	movq	%rax, %rdx
	callq	memcpy
	leaq	-544(%rbp), %rdi
	movq	-16(%rbp), %rdx
	movq	%rax, -616(%rbp)        # 8-byte Spill
	movq	%rdx, -624(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-624(%rbp), %rdx        # 8-byte Reload
	addq	%rax, %rdx
	movq	%rdx, %rax
	addq	$624, %rsp              # imm = 0x270
	popq	%rbp
	retq
.Ltmp3:
	.size	itoa, .Ltmp3-itoa
	.cfi_endproc

	.globl	utoa
	.align	16, 0x90
	.type	utoa,@function
utoa:                                   # @utoa
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp4:
	.cfi_def_cfa_offset 16
.Ltmp5:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp6:
	.cfi_def_cfa_register %rbp
	subq	$640, %rsp              # imm = 0x280
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	$.L.str, -552(%rbp)
	cmpl	$16, -20(%rbp)
	jne	.LBB1_6
# BB#1:
	movl	$0, -556(%rbp)
.LBB1_2:                                # =>This Inner Loop Header: Depth=1
	cmpl	$8, -556(%rbp)
	jge	.LBB1_5
# BB#3:                                 #   in Loop: Header=BB1_2 Depth=1
	movl	$7, %eax
	movq	-8(%rbp), %rcx
	movl	-556(%rbp), %edx
	shll	$2, %edx
	movl	%edx, %edx
	movl	%edx, %esi
	movq	%rcx, -576(%rbp)        # 8-byte Spill
	movq	%rsi, %rcx
                                        # kill: CL<def> RCX<kill>
	movq	-576(%rbp), %rsi        # 8-byte Reload
	shrq	%cl, %rsi
	andq	$15, %rsi
	movq	-552(%rbp), %rdi
	movb	(%rdi,%rsi), %cl
	subl	-556(%rbp), %eax
	movslq	%eax, %rsi
	movb	%cl, -544(%rbp,%rsi)
# BB#4:                                 #   in Loop: Header=BB1_2 Depth=1
	movl	-556(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -556(%rbp)
	jmp	.LBB1_2
.LBB1_5:
	movb	$0, -536(%rbp)
	jmp	.LBB1_22
.LBB1_6:
	cmpl	$2, -20(%rbp)
	jne	.LBB1_12
# BB#7:
	movl	$0, -560(%rbp)
.LBB1_8:                                # =>This Inner Loop Header: Depth=1
	cmpl	$32, -560(%rbp)
	jge	.LBB1_11
# BB#9:                                 #   in Loop: Header=BB1_8 Depth=1
	movl	$31, %eax
	movq	-8(%rbp), %rcx
	movl	-560(%rbp), %edx
	movl	%edx, %esi
	movq	%rcx, -584(%rbp)        # 8-byte Spill
	movq	%rsi, %rcx
                                        # kill: CL<def> RCX<kill>
	movq	-584(%rbp), %rsi        # 8-byte Reload
	shrq	%cl, %rsi
	andq	$1, %rsi
	movq	-552(%rbp), %rdi
	movb	(%rdi,%rsi), %cl
	subl	-560(%rbp), %eax
	movslq	%eax, %rsi
	movb	%cl, -544(%rbp,%rsi)
# BB#10:                                #   in Loop: Header=BB1_8 Depth=1
	movl	-560(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -560(%rbp)
	jmp	.LBB1_8
.LBB1_11:
	movb	$0, -512(%rbp)
	jmp	.LBB1_21
.LBB1_12:
	cmpl	$16, -20(%rbp)
	jge	.LBB1_19
# BB#13:
	movl	$0, -564(%rbp)
.LBB1_14:                               # =>This Inner Loop Header: Depth=1
	movq	-8(%rbp), %rax
	movslq	-20(%rbp), %rcx
	xorl	%edx, %edx
                                        # kill: RDX<def> EDX<kill>
	divq	%rcx
	movq	-552(%rbp), %rax
	movb	(%rax,%rdx), %sil
	movl	-564(%rbp), %edi
	movl	%edi, %r8d
	addl	$1, %r8d
	movl	%r8d, -564(%rbp)
	movslq	%edi, %rax
	movb	%sil, -544(%rbp,%rax)
	movslq	-20(%rbp), %rax
	movq	-8(%rbp), %rcx
	movq	%rax, -592(%rbp)        # 8-byte Spill
	movq	%rcx, %rax
	xorl	%edi, %edi
	movl	%edi, %edx
	movq	-592(%rbp), %rcx        # 8-byte Reload
	divq	%rcx
	movq	%rax, -8(%rbp)
# BB#15:                                #   in Loop: Header=BB1_14 Depth=1
	movb	$0, %al
	cmpq	$0, -8(%rbp)
	movb	%al, -593(%rbp)         # 1-byte Spill
	je	.LBB1_17
# BB#16:                                #   in Loop: Header=BB1_14 Depth=1
	cmpl	$511, -564(%rbp)        # imm = 0x1FF
	setl	%al
	movb	%al, -593(%rbp)         # 1-byte Spill
.LBB1_17:                               #   in Loop: Header=BB1_14 Depth=1
	movb	-593(%rbp), %al         # 1-byte Reload
	testb	$1, %al
	jne	.LBB1_14
# BB#18:
	leaq	-544(%rbp), %rdi
	movslq	-564(%rbp), %rax
	movb	$0, -544(%rbp,%rax)
	callq	strrev
	jmp	.LBB1_20
.LBB1_19:
	movb	$0, -544(%rbp)
.LBB1_20:
	jmp	.LBB1_21
.LBB1_21:
	jmp	.LBB1_22
.LBB1_22:
	leaq	-544(%rbp), %rax
	movq	-16(%rbp), %rdi
	movq	%rdi, -608(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	%rax, -616(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-608(%rbp), %rdi        # 8-byte Reload
	movq	-616(%rbp), %rsi        # 8-byte Reload
	movq	%rax, %rdx
	callq	memcpy
	leaq	-544(%rbp), %rdi
	movq	-16(%rbp), %rdx
	movq	%rax, -624(%rbp)        # 8-byte Spill
	movq	%rdx, -632(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-632(%rbp), %rdx        # 8-byte Reload
	addq	%rax, %rdx
	movq	%rdx, %rax
	addq	$640, %rsp              # imm = 0x280
	popq	%rbp
	retq
.Ltmp7:
	.size	utoa, .Ltmp7-utoa
	.cfi_endproc

	.globl	vsnprintf
	.align	16, 0x90
	.type	vsnprintf,@function
vsnprintf:                              # @vsnprintf
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp8:
	.cfi_def_cfa_offset 16
.Ltmp9:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp10:
	.cfi_def_cfa_register %rbp
	subq	$448, %rsp              # imm = 0x1C0
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movl	$0, -28(%rbp)
	movq	$0, -40(%rbp)
.LBB2_1:                                # =>This Inner Loop Header: Depth=1
	movq	-40(%rbp), %rax
	movq	-16(%rbp), %rdi
	movq	%rax, -88(%rbp)         # 8-byte Spill
	callq	strlen
	movq	-88(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB2_7
# BB#2:                                 #   in Loop: Header=BB2_1 Depth=1
	movq	-40(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	(%rcx,%rax), %edx
	cmpl	$37, %edx
	jne	.LBB2_5
# BB#3:                                 #   in Loop: Header=BB2_1 Depth=1
	movq	-40(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	1(%rcx,%rax), %edx
	cmpl	$37, %edx
	je	.LBB2_5
# BB#4:                                 #   in Loop: Header=BB2_1 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
.LBB2_5:                                #   in Loop: Header=BB2_1 Depth=1
	jmp	.LBB2_6
.LBB2_6:                                #   in Loop: Header=BB2_1 Depth=1
	movq	-40(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -40(%rbp)
	jmp	.LBB2_1
.LBB2_7:
	movq	$0, -48(%rbp)
.LBB2_8:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB2_12 Depth 2
                                        #       Child Loop BB2_42 Depth 3
                                        #       Child Loop BB2_17 Depth 3
                                        #       Child Loop BB2_32 Depth 3
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rdi
	movq	%rax, -96(%rbp)         # 8-byte Spill
	callq	strlen
	movq	-96(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB2_75
# BB#9:                                 #   in Loop: Header=BB2_8 Depth=1
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	(%rcx,%rax), %edx
	cmpl	$37, %edx
	je	.LBB2_11
# BB#10:                                #   in Loop: Header=BB2_8 Depth=1
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rcx
	movb	(%rcx,%rax), %dl
	movq	-8(%rbp), %rax
	movb	%dl, (%rax)
	movq	-8(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -8(%rbp)
	jmp	.LBB2_73
.LBB2_11:                               #   in Loop: Header=BB2_8 Depth=1
	movq	-48(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -48(%rbp)
	movl	$1, -52(%rbp)
	movb	$32, -53(%rbp)
	movl	$0, -60(%rbp)
	movl	$0, -64(%rbp)
.LBB2_12:                               #   Parent Loop BB2_8 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB2_42 Depth 3
                                        #       Child Loop BB2_17 Depth 3
                                        #       Child Loop BB2_32 Depth 3
	cmpl	$0, -52(%rbp)
	je	.LBB2_72
# BB#13:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	(%rcx,%rax), %edx
	movl	%edx, %esi
	subl	$87, %esi
	movl	%edx, -100(%rbp)        # 4-byte Spill
	movl	%esi, -104(%rbp)        # 4-byte Spill
	jg	.LBB2_77
	jmp	.LBB2_76
.LBB2_76:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	addl	$-35, %eax
	movl	%eax, %ecx
	subl	$35, %eax
	movq	%rcx, -112(%rbp)        # 8-byte Spill
	movl	%eax, -116(%rbp)        # 4-byte Spill
	ja	.LBB2_71
# BB#83:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-112(%rbp), %rax        # 8-byte Reload
	movq	.LJTI2_1(,%rax,8), %rcx
	jmpq	*%rcx
.LBB2_77:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$97, %eax
	movl	%eax, -120(%rbp)        # 4-byte Spill
	jg	.LBB2_79
	jmp	.LBB2_78
.LBB2_78:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$88, %eax
	movl	%eax, -124(%rbp)        # 4-byte Spill
	je	.LBB2_57
	jmp	.LBB2_71
.LBB2_79:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$119, %eax
	movl	%eax, -128(%rbp)        # 4-byte Spill
	jg	.LBB2_81
	jmp	.LBB2_80
.LBB2_80:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	addl	$-98, %eax
	movl	%eax, %ecx
	subl	$19, %eax
	movq	%rcx, -136(%rbp)        # 8-byte Spill
	movl	%eax, -140(%rbp)        # 4-byte Spill
	ja	.LBB2_71
# BB#82:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-136(%rbp), %rax        # 8-byte Reload
	movq	.LJTI2_0(,%rax,8), %rcx
	jmpq	*%rcx
.LBB2_81:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$120, %eax
	movl	%eax, -144(%rbp)        # 4-byte Spill
	je	.LBB2_57
	jmp	.LBB2_71
.LBB2_14:                               #   in Loop: Header=BB2_12 Depth=2
	movb	$48, -53(%rbp)
	movq	-48(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB2_71
.LBB2_15:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -8(%rbp)
	movb	$48, (%rax)
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -8(%rbp)
	movb	$120, (%rax)
	movq	-48(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB2_71
.LBB2_16:                               #   in Loop: Header=BB2_12 Depth=2
	jmp	.LBB2_17
.LBB2_17:                               #   Parent Loop BB2_8 Depth=1
                                        #     Parent Loop BB2_12 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	movb	$0, %al
	movq	-48(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movsbl	(%rdx,%rcx), %esi
	subl	$48, %esi
	cmpl	$9, %esi
	movb	%al, -145(%rbp)         # 1-byte Spill
	jg	.LBB2_19
# BB#18:                                #   in Loop: Header=BB2_17 Depth=3
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	(%rcx,%rax), %edx
	subl	$48, %edx
	cmpl	$0, %edx
	setge	%sil
	movb	%sil, -145(%rbp)        # 1-byte Spill
.LBB2_19:                               #   in Loop: Header=BB2_17 Depth=3
	movb	-145(%rbp), %al         # 1-byte Reload
	testb	$1, %al
	jne	.LBB2_20
	jmp	.LBB2_21
.LBB2_20:                               #   in Loop: Header=BB2_17 Depth=3
	imull	$10, -60(%rbp), %eax
	movq	-48(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movsbl	(%rdx,%rcx), %esi
	subl	$48, %esi
	addl	%esi, %eax
	movl	%eax, -60(%rbp)
	movq	-48(%rbp), %rcx
	addq	$1, %rcx
	movq	%rcx, -48(%rbp)
	jmp	.LBB2_17
.LBB2_21:                               #   in Loop: Header=BB2_12 Depth=2
	jmp	.LBB2_71
.LBB2_22:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -160(%rbp)        # 8-byte Spill
	movl	%ecx, -164(%rbp)        # 4-byte Spill
	ja	.LBB2_24
# BB#23:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-160(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-164(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -176(%rbp)        # 8-byte Spill
	jmp	.LBB2_25
.LBB2_24:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-160(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -176(%rbp)        # 8-byte Spill
.LBB2_25:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-176(%rbp), %rax        # 8-byte Reload
	movl	(%rax), %ecx
	movl	%ecx, -60(%rbp)
	jmp	.LBB2_71
.LBB2_26:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-48(%rbp), %rax
	movq	%rax, %rcx
	incq	%rcx
	movq	%rcx, -48(%rbp)
	movq	-16(%rbp), %rcx
	movsbl	1(%rcx,%rax), %edx
	subl	$42, %edx
	movl	%edx, -180(%rbp)        # 4-byte Spill
	jne	.LBB2_31
	jmp	.LBB2_27
.LBB2_27:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -192(%rbp)        # 8-byte Spill
	movl	%ecx, -196(%rbp)        # 4-byte Spill
	ja	.LBB2_29
# BB#28:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-192(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-196(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -208(%rbp)        # 8-byte Spill
	jmp	.LBB2_30
.LBB2_29:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-192(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -208(%rbp)        # 8-byte Spill
.LBB2_30:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-208(%rbp), %rax        # 8-byte Reload
	movl	(%rax), %ecx
	movl	%ecx, -64(%rbp)
	jmp	.LBB2_37
.LBB2_31:                               #   in Loop: Header=BB2_12 Depth=2
	jmp	.LBB2_32
.LBB2_32:                               #   Parent Loop BB2_8 Depth=1
                                        #     Parent Loop BB2_12 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	movb	$0, %al
	movq	-48(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movsbl	(%rdx,%rcx), %esi
	subl	$48, %esi
	cmpl	$9, %esi
	movb	%al, -209(%rbp)         # 1-byte Spill
	jg	.LBB2_34
# BB#33:                                #   in Loop: Header=BB2_32 Depth=3
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	(%rcx,%rax), %edx
	subl	$48, %edx
	cmpl	$0, %edx
	setge	%sil
	movb	%sil, -209(%rbp)        # 1-byte Spill
.LBB2_34:                               #   in Loop: Header=BB2_32 Depth=3
	movb	-209(%rbp), %al         # 1-byte Reload
	testb	$1, %al
	jne	.LBB2_35
	jmp	.LBB2_36
.LBB2_35:                               #   in Loop: Header=BB2_32 Depth=3
	imull	$10, -60(%rbp), %eax
	movq	-48(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movsbl	(%rdx,%rcx), %esi
	subl	$48, %esi
	addl	%esi, %eax
	movl	%eax, -60(%rbp)
	movq	-48(%rbp), %rcx
	addq	$1, %rcx
	movq	%rcx, -48(%rbp)
	jmp	.LBB2_32
.LBB2_36:                               #   in Loop: Header=BB2_12 Depth=2
	jmp	.LBB2_37
.LBB2_37:                               #   in Loop: Header=BB2_12 Depth=2
	jmp	.LBB2_71
.LBB2_38:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -224(%rbp)        # 8-byte Spill
	movl	%ecx, -228(%rbp)        # 4-byte Spill
	ja	.LBB2_40
# BB#39:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-224(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-228(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -240(%rbp)        # 8-byte Spill
	jmp	.LBB2_41
.LBB2_40:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-224(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -240(%rbp)        # 8-byte Spill
.LBB2_41:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-240(%rbp), %rax        # 8-byte Reload
	movl	$10, %ecx
	movslq	(%rax), %rdi
	movq	-8(%rbp), %rax
	movq	%rax, %rdx
	addq	$1, %rdx
	movq	%rdx, -8(%rbp)
	movl	$10, %edx
	movq	%rax, %rsi
	movl	%ecx, -244(%rbp)        # 4-byte Spill
	callq	itoa
	movq	-8(%rbp), %rsi
	subq	%rsi, %rax
	movl	%eax, %ecx
	movl	%ecx, -68(%rbp)
	movl	-68(%rbp), %ecx
	movq	-8(%rbp), %rax
	movslq	%ecx, %rsi
	addq	%rsi, %rax
	movq	%rax, -8(%rbp)
.LBB2_42:                               #   Parent Loop BB2_8 Depth=1
                                        #     Parent Loop BB2_12 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	movl	-68(%rbp), %eax
	cmpl	-60(%rbp), %eax
	jge	.LBB2_44
# BB#43:                                #   in Loop: Header=BB2_42 Depth=3
	movb	-53(%rbp), %al
	movq	-8(%rbp), %rcx
	movq	%rcx, %rdx
	addq	$1, %rdx
	movq	%rdx, -8(%rbp)
	movb	%al, (%rcx)
	movl	-68(%rbp), %esi
	addl	$1, %esi
	movl	%esi, -68(%rbp)
	jmp	.LBB2_42
.LBB2_44:                               #   in Loop: Header=BB2_12 Depth=2
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_45:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -256(%rbp)        # 8-byte Spill
	movl	%ecx, -260(%rbp)        # 4-byte Spill
	ja	.LBB2_47
# BB#46:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-256(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-260(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -272(%rbp)        # 8-byte Spill
	jmp	.LBB2_48
.LBB2_47:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-256(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -272(%rbp)        # 8-byte Spill
.LBB2_48:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-272(%rbp), %rax        # 8-byte Reload
	movl	$10, %ecx
	movl	(%rax), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rsi
	movl	$10, %edx
	movl	%ecx, -276(%rbp)        # 4-byte Spill
	callq	utoa
	movq	%rax, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_49:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -288(%rbp)        # 8-byte Spill
	movl	%ecx, -292(%rbp)        # 4-byte Spill
	ja	.LBB2_51
# BB#50:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-288(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-292(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -304(%rbp)        # 8-byte Spill
	jmp	.LBB2_52
.LBB2_51:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-288(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -304(%rbp)        # 8-byte Spill
.LBB2_52:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-304(%rbp), %rax        # 8-byte Reload
	movl	$2, %ecx
	movl	(%rax), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rsi
	movl	$2, %edx
	movl	%ecx, -308(%rbp)        # 4-byte Spill
	callq	utoa
	movq	%rax, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_53:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -320(%rbp)        # 8-byte Spill
	movl	%ecx, -324(%rbp)        # 4-byte Spill
	ja	.LBB2_55
# BB#54:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-320(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-324(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -336(%rbp)        # 8-byte Spill
	jmp	.LBB2_56
.LBB2_55:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-320(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -336(%rbp)        # 8-byte Spill
.LBB2_56:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-336(%rbp), %rax        # 8-byte Reload
	movl	$8, %ecx
	movl	(%rax), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rsi
	movl	$8, %edx
	movl	%ecx, -340(%rbp)        # 4-byte Spill
	callq	utoa
	movq	%rax, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_57:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -352(%rbp)        # 8-byte Spill
	movl	%ecx, -356(%rbp)        # 4-byte Spill
	ja	.LBB2_59
# BB#58:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-352(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-356(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -368(%rbp)        # 8-byte Spill
	jmp	.LBB2_60
.LBB2_59:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-352(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -368(%rbp)        # 8-byte Spill
.LBB2_60:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-368(%rbp), %rax        # 8-byte Reload
	movl	$16, %ecx
	movl	(%rax), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rsi
	movl	$16, %edx
	movl	%ecx, -372(%rbp)        # 4-byte Spill
	callq	utoa
	movq	%rax, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_61:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-8(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_62:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -384(%rbp)        # 8-byte Spill
	movl	%ecx, -388(%rbp)        # 4-byte Spill
	ja	.LBB2_64
# BB#63:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-384(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-388(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -400(%rbp)        # 8-byte Spill
	jmp	.LBB2_65
.LBB2_64:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-384(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -400(%rbp)        # 8-byte Spill
.LBB2_65:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-400(%rbp), %rax        # 8-byte Reload
	movl	(%rax), %ecx
	andl	$255, %ecx
	movb	%cl, %dl
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	addq	$1, %rsi
	movq	%rsi, -8(%rbp)
	movb	%dl, (%rax)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_66:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -408(%rbp)        # 8-byte Spill
	movl	%ecx, -412(%rbp)        # 4-byte Spill
	ja	.LBB2_68
# BB#67:                                #   in Loop: Header=BB2_12 Depth=2
	movq	-408(%rbp), %rax        # 8-byte Reload
	movq	16(%rax), %rcx
	movl	-412(%rbp), %edx        # 4-byte Reload
	movslq	%edx, %rsi
	addq	%rsi, %rcx
	addl	$8, %edx
	movl	%edx, (%rax)
	movq	%rcx, -424(%rbp)        # 8-byte Spill
	jmp	.LBB2_69
.LBB2_68:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-408(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -424(%rbp)        # 8-byte Spill
.LBB2_69:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-424(%rbp), %rax        # 8-byte Reload
	movq	(%rax), %rax
	movq	%rax, -80(%rbp)
	movq	-8(%rbp), %rdi
	movq	-80(%rbp), %rsi
	movq	-80(%rbp), %rax
	movq	%rdi, -432(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	%rsi, -440(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-432(%rbp), %rdi        # 8-byte Reload
	movq	-440(%rbp), %rsi        # 8-byte Reload
	movq	%rax, %rdx
	callq	memcpy
	movq	-80(%rbp), %rdi
	movq	%rax, -448(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-8(%rbp), %rdx
	addq	%rax, %rdx
	movq	%rdx, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_70:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	addq	$1, %rcx
	movq	%rcx, -8(%rbp)
	movb	$37, (%rax)
	movl	$0, -52(%rbp)
.LBB2_71:                               #   in Loop: Header=BB2_12 Depth=2
	jmp	.LBB2_12
.LBB2_72:                               #   in Loop: Header=BB2_8 Depth=1
	jmp	.LBB2_73
.LBB2_73:                               #   in Loop: Header=BB2_8 Depth=1
	jmp	.LBB2_74
.LBB2_74:                               #   in Loop: Header=BB2_8 Depth=1
	movq	-48(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB2_8
.LBB2_75:
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	movb	$0, (%rcx)
	addq	$448, %rsp              # imm = 0x1C0
	popq	%rbp
	retq
.Ltmp11:
	.size	vsnprintf, .Ltmp11-vsnprintf
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI2_0:
	.quad	.LBB2_49
	.quad	.LBB2_62
	.quad	.LBB2_38
	.quad	.LBB2_71
	.quad	.LBB2_61
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_38
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_53
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_66
	.quad	.LBB2_71
	.quad	.LBB2_45
.LJTI2_1:
	.quad	.LBB2_15
	.quad	.LBB2_71
	.quad	.LBB2_70
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_22
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_26
	.quad	.LBB2_71
	.quad	.LBB2_14
	.quad	.LBB2_16
	.quad	.LBB2_16
	.quad	.LBB2_16
	.quad	.LBB2_16
	.quad	.LBB2_16
	.quad	.LBB2_16
	.quad	.LBB2_16
	.quad	.LBB2_16
	.quad	.LBB2_16
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_71
	.quad	.LBB2_61

	.text
	.globl	sprintf
	.align	16, 0x90
	.type	sprintf,@function
sprintf:                                # @sprintf
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
	subq	$416, %rsp              # imm = 0x1A0
	testb	%al, %al
	movaps	%xmm7, -240(%rbp)       # 16-byte Spill
	movaps	%xmm6, -256(%rbp)       # 16-byte Spill
	movaps	%xmm5, -272(%rbp)       # 16-byte Spill
	movaps	%xmm4, -288(%rbp)       # 16-byte Spill
	movaps	%xmm3, -304(%rbp)       # 16-byte Spill
	movaps	%xmm2, -320(%rbp)       # 16-byte Spill
	movaps	%xmm1, -336(%rbp)       # 16-byte Spill
	movaps	%xmm0, -352(%rbp)       # 16-byte Spill
	movq	%rdi, -360(%rbp)        # 8-byte Spill
	movq	%r9, -368(%rbp)         # 8-byte Spill
	movq	%r8, -376(%rbp)         # 8-byte Spill
	movq	%rcx, -384(%rbp)        # 8-byte Spill
	movq	%rdx, -392(%rbp)        # 8-byte Spill
	movq	%rsi, -400(%rbp)        # 8-byte Spill
	je	.LBB3_2
# BB#1:
	movaps	-352(%rbp), %xmm0       # 16-byte Reload
	movaps	%xmm0, -176(%rbp)
	movaps	-336(%rbp), %xmm1       # 16-byte Reload
	movaps	%xmm1, -160(%rbp)
	movaps	-320(%rbp), %xmm2       # 16-byte Reload
	movaps	%xmm2, -144(%rbp)
	movaps	-304(%rbp), %xmm3       # 16-byte Reload
	movaps	%xmm3, -128(%rbp)
	movaps	-288(%rbp), %xmm4       # 16-byte Reload
	movaps	%xmm4, -112(%rbp)
	movaps	-272(%rbp), %xmm5       # 16-byte Reload
	movaps	%xmm5, -96(%rbp)
	movaps	-256(%rbp), %xmm6       # 16-byte Reload
	movaps	%xmm6, -80(%rbp)
	movaps	-240(%rbp), %xmm7       # 16-byte Reload
	movaps	%xmm7, -64(%rbp)
.LBB3_2:
	movq	-368(%rbp), %rax        # 8-byte Reload
	movq	%rax, -184(%rbp)
	movq	-376(%rbp), %rcx        # 8-byte Reload
	movq	%rcx, -192(%rbp)
	movq	-384(%rbp), %rdx        # 8-byte Reload
	movq	%rdx, -200(%rbp)
	movq	-392(%rbp), %rsi        # 8-byte Reload
	movq	%rsi, -208(%rbp)
	movq	-400(%rbp), %rdi        # 8-byte Reload
	movq	-360(%rbp), %r8         # 8-byte Reload
	movq	%r8, -8(%rbp)
	movq	%rdi, -16(%rbp)
	leaq	-224(%rbp), %rdi
	movq	%rdi, -32(%rbp)
	leaq	16(%rbp), %rdi
	movq	%rdi, -40(%rbp)
	movl	$48, -44(%rbp)
	movl	$16, -48(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	leaq	-48(%rbp), %rdx
	callq	vsnprintf
	xorl	%r9d, %r9d
	movl	%eax, -404(%rbp)        # 4-byte Spill
	movl	%r9d, %eax
	addq	$416, %rsp              # imm = 0x1A0
	popq	%rbp
	retq
.Ltmp15:
	.size	sprintf, .Ltmp15-sprintf
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"0123456789ABCDEF"
	.size	.L.str, 17


	.ident	"clang version 3.5.0 (tags/RELEASE_350/final)"
	.section	".note.GNU-stack","",@progbits
