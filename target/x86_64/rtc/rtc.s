	.text
	.file	"rtc/rtc.c"
	.globl	RTC_Initialize
	.align	16, 0x90
	.type	RTC_Initialize,@function
RTC_Initialize:                         # @RTC_Initialize
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
	subq	$16, %rsp
	movq	$.L.str, %rdi
	xorl	%esi, %esi
	callq	ACPITables_FindTable
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	je	.LBB0_2
# BB#1:
	movq	-8(%rbp), %rax
	movzbl	108(%rax), %ecx
	movl	%ecx, century_register
.LBB0_2:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	RTC_Initialize, .Lfunc_end0-RTC_Initialize
	.cfi_endproc

	.globl	RTC_SelectRegister
	.align	16, 0x90
	.type	RTC_SelectRegister,@function
RTC_SelectRegister:                     # @RTC_SelectRegister
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
	movb	%sil, %al
	movb	%dil, %cl
	movl	$112, %esi
	movb	%cl, -1(%rbp)
	movb	%al, -2(%rbp)
	movzbl	-1(%rbp), %edi
	shll	$7, %edi
	movzbl	-2(%rbp), %edx
	orl	%edx, %edi
	movb	%dil, %al
	movzbl	%al, %edx
	movl	$112, %edi
	movl	%esi, -8(%rbp)          # 4-byte Spill
	movl	%edx, %esi
	callq	outb
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	RTC_SelectRegister, .Lfunc_end1-RTC_SelectRegister
	.cfi_endproc

	.globl	RTC_SetRegister
	.align	16, 0x90
	.type	RTC_SetRegister,@function
RTC_SetRegister:                        # @RTC_SetRegister
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
	subq	$16, %rsp
	movb	%dl, %al
	movb	%sil, %cl
	movb	%dil, %r8b
	movb	%r8b, -1(%rbp)
	movb	%cl, -2(%rbp)
	movb	%al, -3(%rbp)
	movb	-2(%rbp), %al
	movb	-3(%rbp), %cl
	movzbl	%al, %edi
	movzbl	%cl, %esi
	callq	RTC_SelectRegister
	movl	$113, %edx
	movb	-1(%rbp), %al
	movzbl	%al, %esi
	movl	$113, %edi
	movl	%edx, -8(%rbp)          # 4-byte Spill
	callq	outb
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	RTC_SetRegister, .Lfunc_end2-RTC_SetRegister
	.cfi_endproc

	.globl	RTC_GetRegister
	.align	16, 0x90
	.type	RTC_GetRegister,@function
RTC_GetRegister:                        # @RTC_GetRegister
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
	movb	%sil, %al
	movb	%dil, %cl
	movb	%cl, -1(%rbp)
	movb	%al, -2(%rbp)
	movb	-1(%rbp), %al
	movb	-2(%rbp), %cl
	movzbl	%al, %edi
	movzbl	%cl, %esi
	callq	RTC_SelectRegister
	movl	$113, %esi
	movl	$113, %edi
	movl	%esi, -8(%rbp)          # 4-byte Spill
	callq	inb
	movzbl	%al, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	RTC_GetRegister, .Lfunc_end3-RTC_GetRegister
	.cfi_endproc

	.globl	RTC_UpdateInProgress
	.align	16, 0x90
	.type	RTC_UpdateInProgress,@function
RTC_UpdateInProgress:                   # @RTC_UpdateInProgress
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
	movl	$1, %eax
	movl	$10, %ecx
	movl	$1, %edi
	movl	$10, %esi
	movl	%eax, -4(%rbp)          # 4-byte Spill
	movl	%ecx, -8(%rbp)          # 4-byte Spill
	callq	RTC_GetRegister
	movzbl	%al, %ecx
	andl	$128, %ecx
	movb	%cl, %al
	movzbl	%al, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	RTC_UpdateInProgress, .Lfunc_end4-RTC_UpdateInProgress
	.cfi_endproc

	.globl	RTC_GetRTCTime
	.align	16, 0x90
	.type	RTC_GetRTCTime,@function
RTC_GetRTCTime:                         # @RTC_GetRTCTime
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
	subq	$64, %rsp
	movq	%rdi, -8(%rbp)
	movb	$0, -17(%rbp)
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	callq	RTC_UpdateInProgress
	cmpb	$0, %al
	je	.LBB5_3
# BB#2:                                 #   in Loop: Header=BB5_1 Depth=1
	jmp	.LBB5_1
.LBB5_3:
	movl	$1, %eax
	xorl	%esi, %esi
	movl	%eax, %edi
	movl	%eax, -32(%rbp)         # 4-byte Spill
	callq	RTC_GetRegister
	movb	%al, -9(%rbp)
	movl	$2, %esi
	movl	-32(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movb	%al, -10(%rbp)
	movl	$4, %esi
	movl	-32(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movb	%al, -11(%rbp)
	movl	$7, %esi
	movl	-32(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movb	%al, -12(%rbp)
	movl	$8, %esi
	movl	-32(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movb	%al, -13(%rbp)
	movl	$9, %esi
	movl	-32(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movzbl	%al, %esi
	movw	%si, %cx
	movw	%cx, -16(%rbp)
	movl	century_register, %esi
	cmpl	$0, %esi
	je	.LBB5_5
# BB#4:
	movl	$1, %eax
	movl	century_register, %ecx
	movb	%cl, %dl
	movzbl	%dl, %esi
	movl	$1, %edi
	movl	%eax, -36(%rbp)         # 4-byte Spill
	callq	RTC_GetRegister
	movb	%al, -17(%rbp)
.LBB5_5:
	jmp	.LBB5_6
.LBB5_6:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB5_7 Depth 2
	movb	-9(%rbp), %al
	movb	%al, -18(%rbp)
	movb	-10(%rbp), %al
	movb	%al, -19(%rbp)
	movb	-11(%rbp), %al
	movb	%al, -20(%rbp)
	movb	-12(%rbp), %al
	movb	%al, -21(%rbp)
	movb	-13(%rbp), %al
	movb	%al, -22(%rbp)
	movw	-16(%rbp), %cx
	movb	%cl, %al
	movb	%al, -23(%rbp)
	movb	-17(%rbp), %al
	movb	%al, -24(%rbp)
.LBB5_7:                                #   Parent Loop BB5_6 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	callq	RTC_UpdateInProgress
	cmpb	$0, %al
	je	.LBB5_9
# BB#8:                                 #   in Loop: Header=BB5_7 Depth=2
	jmp	.LBB5_7
.LBB5_9:                                #   in Loop: Header=BB5_6 Depth=1
	movl	$1, %eax
	xorl	%esi, %esi
	movl	%eax, %edi
	movl	%eax, -40(%rbp)         # 4-byte Spill
	callq	RTC_GetRegister
	movb	%al, -9(%rbp)
	movl	$2, %esi
	movl	-40(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movb	%al, -10(%rbp)
	movl	$4, %esi
	movl	-40(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movb	%al, -11(%rbp)
	movl	$7, %esi
	movl	-40(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movb	%al, -12(%rbp)
	movl	$8, %esi
	movl	-40(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movb	%al, -13(%rbp)
	movl	$9, %esi
	movl	-40(%rbp), %edi         # 4-byte Reload
	callq	RTC_GetRegister
	movzbl	%al, %esi
	movw	%si, %cx
	movw	%cx, -16(%rbp)
	movl	century_register, %esi
	cmpl	$0, %esi
	je	.LBB5_11
# BB#10:                                #   in Loop: Header=BB5_6 Depth=1
	movl	$1, %eax
	movl	century_register, %ecx
	movb	%cl, %dl
	movzbl	%dl, %esi
	movl	$1, %edi
	movl	%eax, -44(%rbp)         # 4-byte Spill
	callq	RTC_GetRegister
	movb	%al, -17(%rbp)
.LBB5_11:                               #   in Loop: Header=BB5_6 Depth=1
	jmp	.LBB5_12
.LBB5_12:                               #   in Loop: Header=BB5_6 Depth=1
	movb	$1, %al
	movzbl	-18(%rbp), %ecx
	movzbl	-9(%rbp), %edx
	cmpl	%edx, %ecx
	movb	%al, -45(%rbp)          # 1-byte Spill
	jne	.LBB5_19
# BB#13:                                #   in Loop: Header=BB5_6 Depth=1
	movb	$1, %al
	movzbl	-19(%rbp), %ecx
	movzbl	-10(%rbp), %edx
	cmpl	%edx, %ecx
	movb	%al, -45(%rbp)          # 1-byte Spill
	jne	.LBB5_19
# BB#14:                                #   in Loop: Header=BB5_6 Depth=1
	movb	$1, %al
	movzbl	-20(%rbp), %ecx
	movzbl	-11(%rbp), %edx
	cmpl	%edx, %ecx
	movb	%al, -45(%rbp)          # 1-byte Spill
	jne	.LBB5_19
# BB#15:                                #   in Loop: Header=BB5_6 Depth=1
	movb	$1, %al
	movzbl	-21(%rbp), %ecx
	movzbl	-12(%rbp), %edx
	cmpl	%edx, %ecx
	movb	%al, -45(%rbp)          # 1-byte Spill
	jne	.LBB5_19
# BB#16:                                #   in Loop: Header=BB5_6 Depth=1
	movb	$1, %al
	movzbl	-22(%rbp), %ecx
	movzbl	-13(%rbp), %edx
	cmpl	%edx, %ecx
	movb	%al, -45(%rbp)          # 1-byte Spill
	jne	.LBB5_19
# BB#17:                                #   in Loop: Header=BB5_6 Depth=1
	movb	$1, %al
	movzbl	-23(%rbp), %ecx
	movzwl	-16(%rbp), %edx
	cmpl	%edx, %ecx
	movb	%al, -45(%rbp)          # 1-byte Spill
	jne	.LBB5_19
# BB#18:                                #   in Loop: Header=BB5_6 Depth=1
	movzbl	-24(%rbp), %eax
	movzbl	-17(%rbp), %ecx
	cmpl	%ecx, %eax
	setne	%dl
	movb	%dl, -45(%rbp)          # 1-byte Spill
.LBB5_19:                               #   in Loop: Header=BB5_6 Depth=1
	movb	-45(%rbp), %al          # 1-byte Reload
	testb	$1, %al
	jne	.LBB5_6
# BB#20:
	movl	$1, %eax
	movl	$11, %ecx
	movl	$1, %edi
	movl	$11, %esi
	movl	%eax, -52(%rbp)         # 4-byte Spill
	movl	%ecx, -56(%rbp)         # 4-byte Spill
	callq	RTC_GetRegister
	movb	%al, -25(%rbp)
	movzbl	-25(%rbp), %ecx
	andl	$4, %ecx
	cmpl	$0, %ecx
	jne	.LBB5_24
# BB#21:
	movzbl	-9(%rbp), %eax
	movl	%eax, %ecx
	andl	$15, %ecx
	shrl	$4, %eax
	movl	%eax, %edx
	leal	(%rdx,%rdx,4), %eax
	movl	%ecx, %edx
	movl	%eax, %esi
	leal	(%rdx,%rsi,2), %eax
	movb	%al, %dil
	movb	%dil, -9(%rbp)
	movzbl	-10(%rbp), %eax
	movl	%eax, %ecx
	andl	$15, %ecx
	shrl	$4, %eax
	movl	%eax, %edx
	leal	(%rdx,%rdx,4), %eax
	movl	%ecx, %edx
	movl	%eax, %esi
	leal	(%rdx,%rsi,2), %eax
	movb	%al, %dil
	movb	%dil, -10(%rbp)
	movzbl	-11(%rbp), %eax
	movl	%eax, %ecx
	andl	$15, %ecx
	movl	%eax, %r8d
	andl	$112, %r8d
	shrl	$4, %r8d
	movl	%r8d, %edx
	leal	(%rdx,%rdx,4), %r8d
	movl	%ecx, %edx
	movl	%r8d, %esi
	leal	(%rdx,%rsi,2), %ecx
	andl	$128, %eax
	orl	%eax, %ecx
	movb	%cl, %dil
	movb	%dil, -11(%rbp)
	movzbl	-12(%rbp), %eax
	movl	%eax, %ecx
	andl	$15, %ecx
	shrl	$4, %eax
	movl	%eax, %edx
	leal	(%rdx,%rdx,4), %eax
	movl	%ecx, %edx
	movl	%eax, %esi
	leal	(%rdx,%rsi,2), %eax
	movb	%al, %dil
	movb	%dil, -12(%rbp)
	movzbl	-13(%rbp), %eax
	movl	%eax, %ecx
	andl	$15, %ecx
	shrl	$4, %eax
	movl	%eax, %edx
	leal	(%rdx,%rdx,4), %eax
	movl	%ecx, %edx
	movl	%eax, %esi
	leal	(%rdx,%rsi,2), %eax
	movb	%al, %dil
	movb	%dil, -13(%rbp)
	movzwl	-16(%rbp), %eax
	movl	%eax, %ecx
	andl	$15, %ecx
	shrl	$4, %eax
	movl	%eax, %edx
	leal	(%rdx,%rdx,4), %eax
	movl	%ecx, %edx
	movl	%eax, %esi
	leal	(%rdx,%rsi,2), %eax
	movw	%ax, %r9w
	movw	%r9w, -16(%rbp)
	movl	century_register, %eax
	cmpl	$0, %eax
	je	.LBB5_23
# BB#22:
	movl	$16, %eax
	movzbl	-17(%rbp), %ecx
	andl	$15, %ecx
	movzbl	-17(%rbp), %edx
	movl	%eax, -60(%rbp)         # 4-byte Spill
	movl	%edx, %eax
	cltd
	movl	-60(%rbp), %esi         # 4-byte Reload
	idivl	%esi
	imull	$10, %eax, %eax
	addl	%eax, %ecx
	movb	%cl, %dil
	movb	%dil, -17(%rbp)
.LBB5_23:
	jmp	.LBB5_24
.LBB5_24:
	movzbl	-25(%rbp), %eax
	andl	$2, %eax
	cmpl	$0, %eax
	jne	.LBB5_27
# BB#25:
	movzbl	-11(%rbp), %eax
	andl	$128, %eax
	cmpl	$0, %eax
	je	.LBB5_27
# BB#26:
	movl	$24, %eax
	movzbl	-11(%rbp), %ecx
	andl	$127, %ecx
	addl	$12, %ecx
	movl	%eax, -64(%rbp)         # 4-byte Spill
	movl	%ecx, %eax
	cltd
	movl	-64(%rbp), %ecx         # 4-byte Reload
	idivl	%ecx
	movb	%dl, %sil
	movb	%sil, -11(%rbp)
.LBB5_27:
	movl	century_register, %eax
	cmpl	$0, %eax
	je	.LBB5_29
# BB#28:
	movzbl	-17(%rbp), %eax
	imull	$100, %eax, %eax
	movzwl	-16(%rbp), %ecx
	addl	%eax, %ecx
	movw	%cx, %dx
	movw	%dx, -16(%rbp)
	jmp	.LBB5_32
.LBB5_29:
	movzwl	-16(%rbp), %eax
	addl	$2000, %eax             # imm = 0x7D0
	movw	%ax, %cx
	movw	%cx, -16(%rbp)
	movzwl	-16(%rbp), %eax
	cmpl	$2016, %eax             # imm = 0x7E0
	jge	.LBB5_31
# BB#30:
	movzwl	-16(%rbp), %eax
	addl	$100, %eax
	movw	%ax, %cx
	movw	%cx, -16(%rbp)
.LBB5_31:
	jmp	.LBB5_32
.LBB5_32:
	movb	-9(%rbp), %al
	movq	-8(%rbp), %rcx
	movb	%al, (%rcx)
	movb	-10(%rbp), %al
	movq	-8(%rbp), %rcx
	movb	%al, 1(%rcx)
	movb	-11(%rbp), %al
	movq	-8(%rbp), %rcx
	movb	%al, 2(%rcx)
	movb	-12(%rbp), %al
	movq	-8(%rbp), %rcx
	movb	%al, 3(%rcx)
	movb	-13(%rbp), %al
	movq	-8(%rbp), %rcx
	movb	%al, 4(%rcx)
	movw	-16(%rbp), %dx
	movq	-8(%rbp), %rcx
	movw	%dx, 6(%rcx)
	movb	-17(%rbp), %al
	movq	-8(%rbp), %rcx
	movb	%al, 8(%rcx)
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	RTC_GetRTCTime, .Lfunc_end5-RTC_GetRTCTime
	.cfi_endproc

	.type	century_register,@object # @century_register
	.bss
	.globl	century_register
	.align	4
century_register:
	.long	0                       # 0x0
	.size	century_register, 4

	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"FACP"
	.size	.L.str, 5


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
