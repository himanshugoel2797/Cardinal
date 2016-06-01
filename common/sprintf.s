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
	subq	$368, %rsp              # imm = 0x170
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	$.L.str, -296(%rbp)
	cmpl	$16, -20(%rbp)
	jne	.LBB0_6
# BB#1:
	movl	$0, -300(%rbp)
.LBB0_2:                                # =>This Inner Loop Header: Depth=1
	cmpl	$16, -300(%rbp)
	jge	.LBB0_5
# BB#3:                                 #   in Loop: Header=BB0_2 Depth=1
	movl	$15, %eax
	movq	-8(%rbp), %rcx
	movl	-300(%rbp), %edx
	shll	$2, %edx
	movl	%edx, %edx
	movl	%edx, %esi
	movq	%rcx, -320(%rbp)        # 8-byte Spill
	movq	%rsi, %rcx
                                        # kill: CL<def> RCX<kill>
	movq	-320(%rbp), %rsi        # 8-byte Reload
	sarq	%cl, %rsi
	andq	$15, %rsi
	movq	-296(%rbp), %rdi
	movb	(%rdi,%rsi), %cl
	subl	-300(%rbp), %eax
	movslq	%eax, %rsi
	movb	%cl, -288(%rbp,%rsi)
# BB#4:                                 #   in Loop: Header=BB0_2 Depth=1
	movl	-300(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -300(%rbp)
	jmp	.LBB0_2
.LBB0_5:
	movb	$0, -272(%rbp)
	jmp	.LBB0_24
.LBB0_6:
	cmpl	$2, -20(%rbp)
	jne	.LBB0_12
# BB#7:
	movl	$0, -304(%rbp)
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	cmpl	$64, -304(%rbp)
	jge	.LBB0_11
# BB#9:                                 #   in Loop: Header=BB0_8 Depth=1
	movl	$63, %eax
	movq	-8(%rbp), %rcx
	movl	-304(%rbp), %edx
	movl	%edx, %esi
	movq	%rcx, -328(%rbp)        # 8-byte Spill
	movq	%rsi, %rcx
                                        # kill: CL<def> RCX<kill>
	movq	-328(%rbp), %rsi        # 8-byte Reload
	sarq	%cl, %rsi
	andq	$1, %rsi
	movq	-296(%rbp), %rdi
	movb	(%rdi,%rsi), %cl
	subl	-304(%rbp), %eax
	movslq	%eax, %rsi
	movb	%cl, -288(%rbp,%rsi)
# BB#10:                                #   in Loop: Header=BB0_8 Depth=1
	movl	-304(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -304(%rbp)
	jmp	.LBB0_8
.LBB0_11:
	movb	$0, -224(%rbp)
	jmp	.LBB0_23
.LBB0_12:
	cmpl	$16, -20(%rbp)
	jge	.LBB0_21
# BB#13:
	movl	$0, -308(%rbp)
	movl	$0, -312(%rbp)
	cmpq	$0, -8(%rbp)
	jge	.LBB0_15
# BB#14:
	xorl	%eax, %eax
	movl	%eax, %ecx
	movl	$1, -312(%rbp)
	subq	-8(%rbp), %rcx
	movq	%rcx, -8(%rbp)
.LBB0_15:
	jmp	.LBB0_16
.LBB0_16:                               # =>This Inner Loop Header: Depth=1
	movq	-8(%rbp), %rax
	movslq	-20(%rbp), %rcx
	cqto
	idivq	%rcx
	movq	-296(%rbp), %rcx
	movb	(%rcx,%rdx), %sil
	movl	-308(%rbp), %edi
	movl	%edi, %r8d
	addl	$1, %r8d
	movl	%r8d, -308(%rbp)
	movslq	%edi, %rcx
	movb	%sil, -288(%rbp,%rcx)
	movslq	-20(%rbp), %rcx
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	cqto
	idivq	%rcx
	movq	%rax, -8(%rbp)
# BB#17:                                #   in Loop: Header=BB0_16 Depth=1
	cmpq	$0, -8(%rbp)
	jne	.LBB0_16
# BB#18:
	cmpl	$0, -312(%rbp)
	je	.LBB0_20
# BB#19:
	movl	-308(%rbp), %eax
	movl	%eax, %ecx
	addl	$1, %ecx
	movl	%ecx, -308(%rbp)
	movslq	%eax, %rdx
	movb	$45, -288(%rbp,%rdx)
.LBB0_20:
	leaq	-288(%rbp), %rdi
	movslq	-308(%rbp), %rax
	movb	$0, -288(%rbp,%rax)
	callq	strrev
	jmp	.LBB0_22
.LBB0_21:
	movb	$0, -288(%rbp)
.LBB0_22:
	jmp	.LBB0_23
.LBB0_23:
	jmp	.LBB0_24
.LBB0_24:
	leaq	-288(%rbp), %rax
	movq	-16(%rbp), %rdi
	movq	%rdi, -336(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	%rax, -344(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-336(%rbp), %rdi        # 8-byte Reload
	movq	-344(%rbp), %rsi        # 8-byte Reload
	movq	%rax, %rdx
	callq	memcpy
	leaq	-288(%rbp), %rdi
	movq	-16(%rbp), %rdx
	movq	%rax, -352(%rbp)        # 8-byte Spill
	movq	%rdx, -360(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-360(%rbp), %rdx        # 8-byte Reload
	addq	%rax, %rdx
	movq	%rdx, %rax
	addq	$368, %rsp              # imm = 0x170
	popq	%rbp
	retq
.Lfunc_end0:
	.size	itoa, .Lfunc_end0-itoa
	.cfi_endproc

	.globl	utoa
	.align	16, 0x90
	.type	utoa,@function
utoa:                                   # @utoa
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
	subq	$368, %rsp              # imm = 0x170
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	$.L.str, -296(%rbp)
	cmpl	$0, -20(%rbp)
	jne	.LBB1_2
# BB#1:
	movl	$16, -20(%rbp)
.LBB1_2:
	cmpl	$16, -20(%rbp)
	jne	.LBB1_8
# BB#3:
	movl	$0, -300(%rbp)
.LBB1_4:                                # =>This Inner Loop Header: Depth=1
	cmpl	$8, -300(%rbp)
	jge	.LBB1_7
# BB#5:                                 #   in Loop: Header=BB1_4 Depth=1
	movl	$7, %eax
	movq	-8(%rbp), %rcx
	movl	-300(%rbp), %edx
	shll	$2, %edx
	movl	%edx, %edx
	movl	%edx, %esi
	movq	%rcx, -320(%rbp)        # 8-byte Spill
	movq	%rsi, %rcx
                                        # kill: CL<def> RCX<kill>
	movq	-320(%rbp), %rsi        # 8-byte Reload
	shrq	%cl, %rsi
	andq	$15, %rsi
	movq	-296(%rbp), %rdi
	movb	(%rdi,%rsi), %cl
	subl	-300(%rbp), %eax
	movslq	%eax, %rsi
	movb	%cl, -288(%rbp,%rsi)
# BB#6:                                 #   in Loop: Header=BB1_4 Depth=1
	movl	-300(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -300(%rbp)
	jmp	.LBB1_4
.LBB1_7:
	movb	$0, -280(%rbp)
	jmp	.LBB1_24
.LBB1_8:
	cmpl	$2, -20(%rbp)
	jne	.LBB1_14
# BB#9:
	movl	$0, -304(%rbp)
.LBB1_10:                               # =>This Inner Loop Header: Depth=1
	cmpl	$32, -304(%rbp)
	jge	.LBB1_13
# BB#11:                                #   in Loop: Header=BB1_10 Depth=1
	movl	$31, %eax
	movq	-8(%rbp), %rcx
	movl	-304(%rbp), %edx
	movl	%edx, %esi
	movq	%rcx, -328(%rbp)        # 8-byte Spill
	movq	%rsi, %rcx
                                        # kill: CL<def> RCX<kill>
	movq	-328(%rbp), %rsi        # 8-byte Reload
	shrq	%cl, %rsi
	andq	$1, %rsi
	movq	-296(%rbp), %rdi
	movb	(%rdi,%rsi), %cl
	subl	-304(%rbp), %eax
	movslq	%eax, %rsi
	movb	%cl, -288(%rbp,%rsi)
# BB#12:                                #   in Loop: Header=BB1_10 Depth=1
	movl	-304(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -304(%rbp)
	jmp	.LBB1_10
.LBB1_13:
	movb	$0, -256(%rbp)
	jmp	.LBB1_23
.LBB1_14:
	cmpl	$16, -20(%rbp)
	jge	.LBB1_21
# BB#15:
	movl	$0, -308(%rbp)
.LBB1_16:                               # =>This Inner Loop Header: Depth=1
	movq	-8(%rbp), %rax
	movslq	-20(%rbp), %rcx
	xorl	%edx, %edx
                                        # kill: RDX<def> EDX<kill>
	divq	%rcx
	movq	-296(%rbp), %rcx
	movb	(%rcx,%rdx), %sil
	movl	-308(%rbp), %edi
	movl	%edi, %r8d
	addl	$1, %r8d
	movl	%r8d, -308(%rbp)
	movslq	%edi, %rcx
	movb	%sil, -288(%rbp,%rcx)
	movslq	-20(%rbp), %rcx
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edi, %edi
	movl	%edi, %edx
	divq	%rcx
	movq	%rax, -8(%rbp)
# BB#17:                                #   in Loop: Header=BB1_16 Depth=1
	xorl	%eax, %eax
	movb	%al, %cl
	cmpq	$0, -8(%rbp)
	movb	%cl, -329(%rbp)         # 1-byte Spill
	je	.LBB1_19
# BB#18:                                #   in Loop: Header=BB1_16 Depth=1
	cmpl	$511, -308(%rbp)        # imm = 0x1FF
	setl	%al
	movb	%al, -329(%rbp)         # 1-byte Spill
.LBB1_19:                               #   in Loop: Header=BB1_16 Depth=1
	movb	-329(%rbp), %al         # 1-byte Reload
	testb	$1, %al
	jne	.LBB1_16
# BB#20:
	leaq	-288(%rbp), %rdi
	movslq	-308(%rbp), %rax
	movb	$0, -288(%rbp,%rax)
	callq	strrev
	jmp	.LBB1_22
.LBB1_21:
	movb	$0, -288(%rbp)
.LBB1_22:
	jmp	.LBB1_23
.LBB1_23:
	jmp	.LBB1_24
.LBB1_24:
	leaq	-288(%rbp), %rax
	movq	-16(%rbp), %rdi
	movq	%rdi, -344(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	%rax, -352(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-344(%rbp), %rdi        # 8-byte Reload
	movq	-352(%rbp), %rsi        # 8-byte Reload
	movq	%rax, %rdx
	callq	memcpy
	leaq	-288(%rbp), %rdi
	movq	-16(%rbp), %rdx
	movq	%rax, -360(%rbp)        # 8-byte Spill
	movq	%rdx, -368(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-368(%rbp), %rdx        # 8-byte Reload
	addq	%rax, %rdx
	movq	%rdx, %rax
	addq	$368, %rsp              # imm = 0x170
	popq	%rbp
	retq
.Lfunc_end1:
	.size	utoa, .Lfunc_end1-utoa
	.cfi_endproc

	.globl	vsnprintf
	.align	16, 0x90
	.type	vsnprintf,@function
vsnprintf:                              # @vsnprintf
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
	subq	$480, %rsp              # imm = 0x1E0
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
	subl	$35, %esi
	movl	%edx, -100(%rbp)        # 4-byte Spill
	movl	%esi, -104(%rbp)        # 4-byte Spill
	je	.LBB2_15
	jmp	.LBB2_76
.LBB2_76:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$37, %eax
	movl	%eax, -108(%rbp)        # 4-byte Spill
	je	.LBB2_70
	jmp	.LBB2_77
.LBB2_77:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$42, %eax
	movl	%eax, -112(%rbp)        # 4-byte Spill
	je	.LBB2_22
	jmp	.LBB2_78
.LBB2_78:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$46, %eax
	movl	%eax, -116(%rbp)        # 4-byte Spill
	je	.LBB2_26
	jmp	.LBB2_79
.LBB2_79:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$48, %eax
	movl	%eax, -120(%rbp)        # 4-byte Spill
	je	.LBB2_14
	jmp	.LBB2_80
.LBB2_80:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	addl	$-49, %eax
	subl	$9, %eax
	movl	%eax, -124(%rbp)        # 4-byte Spill
	jb	.LBB2_16
	jmp	.LBB2_81
.LBB2_81:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$70, %eax
	movl	%eax, -128(%rbp)        # 4-byte Spill
	je	.LBB2_61
	jmp	.LBB2_82
.LBB2_82:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$88, %eax
	movl	%eax, -132(%rbp)        # 4-byte Spill
	je	.LBB2_57
	jmp	.LBB2_83
.LBB2_83:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$98, %eax
	movl	%eax, -136(%rbp)        # 4-byte Spill
	je	.LBB2_49
	jmp	.LBB2_84
.LBB2_84:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$99, %eax
	movl	%eax, -140(%rbp)        # 4-byte Spill
	je	.LBB2_62
	jmp	.LBB2_85
.LBB2_85:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$100, %eax
	movl	%eax, -144(%rbp)        # 4-byte Spill
	je	.LBB2_38
	jmp	.LBB2_86
.LBB2_86:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$102, %eax
	movl	%eax, -148(%rbp)        # 4-byte Spill
	je	.LBB2_61
	jmp	.LBB2_87
.LBB2_87:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$105, %eax
	movl	%eax, -152(%rbp)        # 4-byte Spill
	je	.LBB2_38
	jmp	.LBB2_88
.LBB2_88:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$111, %eax
	movl	%eax, -156(%rbp)        # 4-byte Spill
	je	.LBB2_53
	jmp	.LBB2_89
.LBB2_89:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$115, %eax
	movl	%eax, -160(%rbp)        # 4-byte Spill
	je	.LBB2_66
	jmp	.LBB2_90
.LBB2_90:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$117, %eax
	movl	%eax, -164(%rbp)        # 4-byte Spill
	je	.LBB2_45
	jmp	.LBB2_91
.LBB2_91:                               #   in Loop: Header=BB2_12 Depth=2
	movl	-100(%rbp), %eax        # 4-byte Reload
	subl	$120, %eax
	movl	%eax, -168(%rbp)        # 4-byte Spill
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
	xorl	%eax, %eax
	movb	%al, %cl
	movq	-48(%rbp), %rdx
	movq	-16(%rbp), %rsi
	movsbl	(%rsi,%rdx), %eax
	subl	$48, %eax
	cmpl	$9, %eax
	movb	%cl, -169(%rbp)         # 1-byte Spill
	jg	.LBB2_19
# BB#18:                                #   in Loop: Header=BB2_17 Depth=3
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	(%rcx,%rax), %edx
	subl	$48, %edx
	cmpl	$0, %edx
	setge	%sil
	movb	%sil, -169(%rbp)        # 1-byte Spill
.LBB2_19:                               #   in Loop: Header=BB2_17 Depth=3
	movb	-169(%rbp), %al         # 1-byte Reload
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
	movq	%rax, -184(%rbp)        # 8-byte Spill
	movl	%ecx, -188(%rbp)        # 4-byte Spill
	ja	.LBB2_24
# BB#23:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-188(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-184(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -200(%rbp)        # 8-byte Spill
	jmp	.LBB2_25
.LBB2_24:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-184(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -200(%rbp)        # 8-byte Spill
.LBB2_25:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-200(%rbp), %rax        # 8-byte Reload
	movl	(%rax), %ecx
	movl	%ecx, -60(%rbp)
	jmp	.LBB2_71
.LBB2_26:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-48(%rbp), %rax
	movq	%rax, %rcx
	incq	%rcx
	movq	%rcx, -48(%rbp)
	movq	-16(%rbp), %rcx
	movzbl	1(%rcx,%rax), %edx
	subl	$42, %edx
	movl	%edx, -204(%rbp)        # 4-byte Spill
	jne	.LBB2_31
	jmp	.LBB2_27
.LBB2_27:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -216(%rbp)        # 8-byte Spill
	movl	%ecx, -220(%rbp)        # 4-byte Spill
	ja	.LBB2_29
# BB#28:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-220(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-216(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -232(%rbp)        # 8-byte Spill
	jmp	.LBB2_30
.LBB2_29:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-216(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -232(%rbp)        # 8-byte Spill
.LBB2_30:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-232(%rbp), %rax        # 8-byte Reload
	movl	(%rax), %ecx
	movl	%ecx, -64(%rbp)
	jmp	.LBB2_37
.LBB2_31:                               #   in Loop: Header=BB2_12 Depth=2
	jmp	.LBB2_32
.LBB2_32:                               #   Parent Loop BB2_8 Depth=1
                                        #     Parent Loop BB2_12 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	xorl	%eax, %eax
	movb	%al, %cl
	movq	-48(%rbp), %rdx
	movq	-16(%rbp), %rsi
	movsbl	(%rsi,%rdx), %eax
	subl	$48, %eax
	cmpl	$9, %eax
	movb	%cl, -233(%rbp)         # 1-byte Spill
	jg	.LBB2_34
# BB#33:                                #   in Loop: Header=BB2_32 Depth=3
	movq	-48(%rbp), %rax
	movq	-16(%rbp), %rcx
	movsbl	(%rcx,%rax), %edx
	subl	$48, %edx
	cmpl	$0, %edx
	setge	%sil
	movb	%sil, -233(%rbp)        # 1-byte Spill
.LBB2_34:                               #   in Loop: Header=BB2_32 Depth=3
	movb	-233(%rbp), %al         # 1-byte Reload
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
	movq	%rax, -248(%rbp)        # 8-byte Spill
	movl	%ecx, -252(%rbp)        # 4-byte Spill
	ja	.LBB2_40
# BB#39:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-252(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-248(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -264(%rbp)        # 8-byte Spill
	jmp	.LBB2_41
.LBB2_40:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-248(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -264(%rbp)        # 8-byte Spill
.LBB2_41:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-264(%rbp), %rax        # 8-byte Reload
	movl	$10, %ecx
	movslq	(%rax), %rdi
	movq	-8(%rbp), %rsi
	movl	$10, %edx
	movl	%ecx, -268(%rbp)        # 4-byte Spill
	callq	itoa
	movq	-8(%rbp), %rsi
	subq	%rsi, %rax
	movl	%eax, %ecx
	movl	%ecx, -68(%rbp)
	movl	-68(%rbp), %ecx
	addl	$1, %ecx
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
	movq	%rax, -280(%rbp)        # 8-byte Spill
	movl	%ecx, -284(%rbp)        # 4-byte Spill
	ja	.LBB2_47
# BB#46:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-284(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-280(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -296(%rbp)        # 8-byte Spill
	jmp	.LBB2_48
.LBB2_47:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-280(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -296(%rbp)        # 8-byte Spill
.LBB2_48:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-296(%rbp), %rax        # 8-byte Reload
	movl	$10, %ecx
	movl	(%rax), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rsi
	movl	$10, %edx
	movl	%ecx, -300(%rbp)        # 4-byte Spill
	callq	utoa
	movq	%rax, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_49:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -312(%rbp)        # 8-byte Spill
	movl	%ecx, -316(%rbp)        # 4-byte Spill
	ja	.LBB2_51
# BB#50:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-316(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-312(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -328(%rbp)        # 8-byte Spill
	jmp	.LBB2_52
.LBB2_51:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-312(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -328(%rbp)        # 8-byte Spill
.LBB2_52:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-328(%rbp), %rax        # 8-byte Reload
	movl	$2, %ecx
	movl	(%rax), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rsi
	movl	$2, %edx
	movl	%ecx, -332(%rbp)        # 4-byte Spill
	callq	utoa
	movq	%rax, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_53:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -344(%rbp)        # 8-byte Spill
	movl	%ecx, -348(%rbp)        # 4-byte Spill
	ja	.LBB2_55
# BB#54:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-348(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-344(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -360(%rbp)        # 8-byte Spill
	jmp	.LBB2_56
.LBB2_55:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-344(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -360(%rbp)        # 8-byte Spill
.LBB2_56:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-360(%rbp), %rax        # 8-byte Reload
	movl	$8, %ecx
	movl	(%rax), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rsi
	movl	$8, %edx
	movl	%ecx, -364(%rbp)        # 4-byte Spill
	callq	utoa
	movq	%rax, -8(%rbp)
	movl	$0, -52(%rbp)
	jmp	.LBB2_71
.LBB2_57:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-24(%rbp), %rax
	movl	(%rax), %ecx
	cmpl	$40, %ecx
	movq	%rax, -376(%rbp)        # 8-byte Spill
	movl	%ecx, -380(%rbp)        # 4-byte Spill
	ja	.LBB2_59
# BB#58:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-380(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-376(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -392(%rbp)        # 8-byte Spill
	jmp	.LBB2_60
.LBB2_59:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-376(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -392(%rbp)        # 8-byte Spill
.LBB2_60:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-392(%rbp), %rax        # 8-byte Reload
	movl	$16, %ecx
	movl	(%rax), %edx
	movl	%edx, %edi
	movq	-8(%rbp), %rsi
	movl	$16, %edx
	movl	%ecx, -396(%rbp)        # 4-byte Spill
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
	movq	%rax, -408(%rbp)        # 8-byte Spill
	movl	%ecx, -412(%rbp)        # 4-byte Spill
	ja	.LBB2_64
# BB#63:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-412(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-408(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -424(%rbp)        # 8-byte Spill
	jmp	.LBB2_65
.LBB2_64:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-408(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -424(%rbp)        # 8-byte Spill
.LBB2_65:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-424(%rbp), %rax        # 8-byte Reload
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
	movq	%rax, -432(%rbp)        # 8-byte Spill
	movl	%ecx, -436(%rbp)        # 4-byte Spill
	ja	.LBB2_68
# BB#67:                                #   in Loop: Header=BB2_12 Depth=2
	movl	-436(%rbp), %eax        # 4-byte Reload
	movslq	%eax, %rcx
	movq	-432(%rbp), %rdx        # 8-byte Reload
	addq	16(%rdx), %rcx
	addl	$8, %eax
	movl	%eax, (%rdx)
	movq	%rcx, -448(%rbp)        # 8-byte Spill
	jmp	.LBB2_69
.LBB2_68:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-432(%rbp), %rax        # 8-byte Reload
	movq	8(%rax), %rcx
	movq	%rcx, %rdx
	addq	$8, %rcx
	movq	%rcx, 8(%rax)
	movq	%rdx, -448(%rbp)        # 8-byte Spill
.LBB2_69:                               #   in Loop: Header=BB2_12 Depth=2
	movq	-448(%rbp), %rax        # 8-byte Reload
	movq	(%rax), %rax
	movq	%rax, -80(%rbp)
	movq	-8(%rbp), %rdi
	movq	-80(%rbp), %rsi
	movq	-80(%rbp), %rax
	movq	%rdi, -456(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	%rsi, -464(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-456(%rbp), %rdi        # 8-byte Reload
	movq	-464(%rbp), %rsi        # 8-byte Reload
	movq	%rax, %rdx
	callq	memcpy
	movq	-80(%rbp), %rdi
	movq	%rax, -472(%rbp)        # 8-byte Spill
	callq	strlen
	addq	-8(%rbp), %rax
	movq	%rax, -8(%rbp)
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
	xorl	%eax, %eax
	movq	-8(%rbp), %rcx
	movb	$0, (%rcx)
	addq	$480, %rsp              # imm = 0x1E0
	popq	%rbp
	retq
.Lfunc_end2:
	.size	vsnprintf, .Lfunc_end2-vsnprintf
	.cfi_endproc

	.globl	sprintf
	.align	16, 0x90
	.type	sprintf,@function
sprintf:                                # @sprintf
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
.Lfunc_end3:
	.size	sprintf, .Lfunc_end3-sprintf
	.cfi_endproc

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"0123456789ABCDEF"
	.size	.L.str, 17


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
