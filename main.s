	.text
	.file	"main.c"
	.globl	kernel_stall
	.align	16, 0x90
	.type	kernel_stall,@function
kernel_stall:                           # @kernel_stall
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
	#APP
	cli
	hlt
	#NO_APP
	popq	%rbp
	retq
.Lfunc_end0:
	.size	kernel_stall, .Lfunc_end0-kernel_stall
	.cfi_endproc

	.globl	kernel_main_init
	.align	16, 0x90
	.type	kernel_main_init,@function
kernel_main_init:                       # @kernel_main_init
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
	subq	$64, %rsp
	movl	$56, %eax
	movl	%eax, %ecx
	movl	$56, %eax
	movl	%eax, %edi
	movq	%rcx, -16(%rbp)         # 8-byte Spill
	callq	bootstrap_malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rdi
	callq	kmalloc_init
	movq	-8(%rbp), %rdi
	callq	ProcessSys_Initialize
	callq	Thread_Initialize
	xorl	%edx, %edx
	movl	%edx, %eax
	movq	$kernel_main, %rsi
	xorl	%edx, %edx
	movl	%edx, %edi
	movq	%rax, -24(%rbp)         # 8-byte Spill
	callq	CreateThread
	xorl	%edx, %edx
	movl	%edx, %ecx
	movq	$kernel_stall, %rsi
	xorl	%edx, %edx
	movl	%edx, %edi
	movq	%rax, -32(%rbp)         # 8-byte Spill
	movq	%rcx, -40(%rbp)         # 8-byte Spill
	callq	CreateThread
	movq	%rax, -48(%rbp)         # 8-byte Spill
	callq	smp_unlock_cores
	xorl	%edx, %edx
	xorl	%edi, %edi
	movl	%edx, -52(%rbp)         # 4-byte Spill
	callq	CoreUpdate
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	kernel_main_init, .Lfunc_end1-kernel_main_init
	.cfi_endproc

	.globl	kernel_main
	.align	16, 0x90
	.type	kernel_main,@function
kernel_main:                            # @kernel_main
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
	callq	Syscall_Initialize
	callq	DeviceManager_Initialize
	callq	target_device_setup
	#APP
	hlt
	#NO_APP
.LBB2_1:                                # =>This Inner Loop Header: Depth=1
	jmp	.LBB2_1
.Lfunc_end2:
	.size	kernel_main, .Lfunc_end2-kernel_main
	.cfi_endproc

	.globl	smp_core_main
	.align	16, 0x90
	.type	smp_core_main,@function
smp_core_main:                          # @smp_core_main
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
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$0, -4(%rbp)
	movq	$0, -16(%rbp)
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	smp_core_main, .Lfunc_end3-smp_core_main
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
