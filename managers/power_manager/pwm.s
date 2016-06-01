	.text
	.file	"managers/power_manager/pwm.c"
	.globl	PWM_Initialize
	.align	16, 0x90
	.type	PWM_Initialize,@function
PWM_Initialize:                         # @PWM_Initialize
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
	callq	List_Create
	movq	%rax, pwm_data
	popq	%rbp
	retq
.Lfunc_end0:
	.size	PWM_Initialize, .Lfunc_end0-PWM_Initialize
	.cfi_endproc

	.globl	PWM_RegisterStateChangeHandler
	.align	16, 0x90
	.type	PWM_RegisterStateChangeHandler,@function
PWM_RegisterStateChangeHandler:         # @PWM_RegisterStateChangeHandler
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
	movq	%rdi, -16(%rbp)
	movq	pwm_data, %rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	callq	List_AddEntry
	cmpl	$0, %eax
	jne	.LBB1_2
# BB#1:
	movl	$1, -4(%rbp)
	jmp	.LBB1_3
.LBB1_2:
	movl	$0, -4(%rbp)
.LBB1_3:
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	PWM_RegisterStateChangeHandler, .Lfunc_end1-PWM_RegisterStateChangeHandler
	.cfi_endproc

	.globl	PWM_UnregisterHandler
	.align	16, 0x90
	.type	PWM_UnregisterHandler,@function
PWM_UnregisterHandler:                  # @PWM_UnregisterHandler
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
	movq	%rdi, -8(%rbp)
	movq	$0, -16(%rbp)
.LBB2_1:                                # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	movq	pwm_data, %rdi
	movq	%rax, -24(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-24(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB2_6
# BB#2:                                 #   in Loop: Header=BB2_1 Depth=1
	movq	pwm_data, %rdi
	movq	-16(%rbp), %rsi
	callq	List_EntryAt
	movq	-8(%rbp), %rsi
	cmpq	%rsi, %rax
	jne	.LBB2_4
# BB#3:                                 #   in Loop: Header=BB2_1 Depth=1
	movq	pwm_data, %rdi
	movq	-16(%rbp), %rsi
	callq	List_Remove
	movq	-16(%rbp), %rsi
	addq	$-1, %rsi
	movq	%rsi, -16(%rbp)
.LBB2_4:                                #   in Loop: Header=BB2_1 Depth=1
	jmp	.LBB2_5
.LBB2_5:                                #   in Loop: Header=BB2_1 Depth=1
	movq	-16(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -16(%rbp)
	jmp	.LBB2_1
.LBB2_6:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	PWM_UnregisterHandler, .Lfunc_end2-PWM_UnregisterHandler
	.cfi_endproc

	.type	pwm_data,@object        # @pwm_data
	.local	pwm_data
	.comm	pwm_data,8,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
