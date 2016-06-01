	.text
	.file	"bootstrap.c"
	.globl	bootstrap_render
	.align	16, 0x90
	.type	bootstrap_render,@function
bootstrap_render:                       # @bootstrap_render
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
	movl	%edi, -4(%rbp)
	callq	GetBootInfo
	movq	%rax, -16(%rbp)
	movl	$0, -20(%rbp)
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movl	-20(%rbp), %eax
	movq	-16(%rbp), %rcx
	movl	72(%rcx), %edx
	movq	-16(%rbp), %rcx
	imull	64(%rcx), %edx
	cmpl	%edx, %eax
	jae	.LBB0_4
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-4(%rbp), %eax
	movq	-16(%rbp), %rcx
	movq	56(%rcx), %rcx
	movl	-20(%rbp), %edx
	movl	%edx, %esi
	addq	%rsi, %rcx
	movl	%eax, (%rcx)
# BB#3:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	-20(%rbp), %eax
	addl	$4, %eax
	movl	%eax, -20(%rbp)
	jmp	.LBB0_1
.LBB0_4:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	bootstrap_render, .Lfunc_end0-bootstrap_render
	.cfi_endproc

	.globl	bootstrap_pagefault_handler
	.align	16, 0x90
	.type	bootstrap_pagefault_handler,@function
bootstrap_pagefault_handler:            # @bootstrap_pagefault_handler
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
	pushq	%rbx
	subq	$24, %rsp
.Ltmp6:
	.cfi_offset %rbx, -24
	movl	$4294967040, %eax       # imm = 0xFFFFFF00
	xorl	%ecx, %ecx
	movl	%ecx, %edx
	movq	%rdi, -16(%rbp)
	movq	-16(%rbp), %rdi
	subq	120(%rdi), %rdx
	movq	-16(%rbp), %rdi
	movq	%rdx, 120(%rdi)
	movl	$-256, %edi
	movl	%eax, -20(%rbp)         # 4-byte Spill
	callq	bootstrap_render
	movq	-16(%rbp), %rdx
	movq	136(%rdx), %rax
	movq	-16(%rbp), %rdx
	movq	120(%rdx), %rbx
	#APP
	hlt
	#NO_APP
	xorl	%ecx, %ecx
	movl	%ecx, %eax
	movq	-16(%rbp), %rdx
	subq	120(%rdx), %rax
	movq	-16(%rbp), %rdx
	movq	%rax, 120(%rdx)
	addq	$24, %rsp
	popq	%rbx
	popq	%rbp
	retq
.Lfunc_end1:
	.size	bootstrap_pagefault_handler, .Lfunc_end1-bootstrap_pagefault_handler
	.cfi_endproc

	.globl	bootstrap_kernel_panic
	.align	16, 0x90
	.type	bootstrap_kernel_panic,@function
bootstrap_kernel_panic:                 # @bootstrap_kernel_panic
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp7:
	.cfi_def_cfa_offset 16
.Ltmp8:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp9:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movb	%dil, %al
	movb	%al, -1(%rbp)
.LBB2_1:                                # =>This Inner Loop Header: Depth=1
	movl	$255, %eax
	movzbl	-1(%rbp), %ecx
	subl	%ecx, %eax
	orl	$16711680, %eax         # imm = 0xFF0000
	movl	%eax, %edi
	callq	bootstrap_render
	jmp	.LBB2_1
.Lfunc_end2:
	.size	bootstrap_kernel_panic, .Lfunc_end2-bootstrap_kernel_panic
	.cfi_endproc

	.section	.entry_point,"ax",@progbits
	.globl	bootstrap_kernel
	.align	16, 0x90
	.type	bootstrap_kernel,@function
bootstrap_kernel:                       # @bootstrap_kernel
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp10:
	.cfi_def_cfa_offset 16
.Ltmp11:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp12:
	.cfi_def_cfa_register %rbp
	subq	$80, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	callq	VirtMemMan_InitializeBootstrap
	movq	-8(%rbp), %rdi
	callq	ParseAndSaveBootInformation
	callq	GetBootInfo
	xorl	%ecx, %ecx
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	56(%rax), %rsi
	xorl	%edi, %edi
	movl	%ecx, -32(%rbp)         # 4-byte Spill
	callq	GetVirtualAddress
	movq	-24(%rbp), %rsi
	movq	%rax, 56(%rsi)
	cmpq	$920085129, -16(%rbp)   # imm = 0x36D76289
	je	.LBB3_2
# BB#1:
	movl	$255, %eax
	movl	$255, %edi
	movl	%eax, -36(%rbp)         # 4-byte Spill
	callq	bootstrap_kernel_panic
.LBB3_2:
	xorl	%eax, %eax
	movb	%al, %cl
	movb	%cl, %al
	callq	GDT_Initialize
	callq	IDT_Initialize
	movl	$0, -28(%rbp)
.LBB3_3:                                # =>This Inner Loop Header: Depth=1
	cmpl	$31, -28(%rbp)
	jge	.LBB3_6
# BB#4:                                 #   in Loop: Header=BB3_3 Depth=1
	movl	-28(%rbp), %eax
	movb	%al, %cl
	movzbl	%cl, %edi
	movq	$bootstrap_pagefault_handler, %rsi
	callq	IDT_RegisterHandler
# BB#5:                                 #   in Loop: Header=BB3_3 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB3_3
.LBB3_6:
	movl	$4096, %eax             # imm = 0x1000
	movl	%eax, %ecx
	movq	%rcx, %rdi
	movq	%rcx, -48(%rbp)         # 8-byte Spill
	callq	bootstrap_malloc
	movl	$1, %edx
	movl	%edx, %edi
	movq	%rax, %rsi
	movl	%edx, -52(%rbp)         # 4-byte Spill
	callq	GDT_SetIST
	movl	$8, %edx
	movl	$142, %edi
	movl	%edi, -56(%rbp)         # 4-byte Spill
	movl	%edx, %edi
	movl	%edx, %esi
	movl	-56(%rbp), %r8d         # 4-byte Reload
	movl	%edx, -60(%rbp)         # 4-byte Spill
	movl	%r8d, %edx
	movl	-52(%rbp), %ecx         # 4-byte Reload
	callq	IDT_ChangeEntry
	movq	-48(%rbp), %rdi         # 8-byte Reload
	callq	bootstrap_malloc
	movl	$2, %ecx
	movl	%ecx, %edi
	movq	%rax, %rsi
	movl	%ecx, -64(%rbp)         # 4-byte Spill
	callq	GDT_SetIST
	movl	$18, %edi
	movl	-60(%rbp), %esi         # 4-byte Reload
	movl	-56(%rbp), %edx         # 4-byte Reload
	movl	-64(%rbp), %ecx         # 4-byte Reload
	callq	IDT_ChangeEntry
	callq	FPU_Initialize
	callq	MemMan_Initialize
	movl	%eax, -68(%rbp)         # 4-byte Spill
	callq	VirtMemMan_Initialize
	callq	ACPITables_Initialize
	movb	%al, -69(%rbp)          # 1-byte Spill
	callq	SMP_IncrementCoreCount
	callq	RTC_Initialize
	movq	-24(%rbp), %r9
	movq	56(%r9), %rdi
	callq	GetPhysicalAddress
	movq	-24(%rbp), %rdi
	movq	%rax, 56(%rdi)
	movq	-24(%rbp), %rax
	movq	56(%rax), %rsi
	movl	-64(%rbp), %edi         # 4-byte Reload
	callq	GetVirtualAddress
	movq	-24(%rbp), %rsi
	movq	%rax, 56(%rsi)
	movq	-48(%rbp), %rdi         # 8-byte Reload
	callq	bootstrap_malloc
	addq	$4096, %rax             # imm = 0x1000
	movq	%rax, %rdi
	callq	SetKernelStack
	movq	-48(%rbp), %rdi         # 8-byte Reload
	callq	bootstrap_malloc
	addq	$4096, %rax             # imm = 0x1000
	movq	%rax, %rdi
	callq	SetUserStack
	movl	$1, smp_sync_base
	callq	APIC_Initialize
	movl	%eax, -76(%rbp)         # 4-byte Spill
	callq	kernel_main_init
	#APP
	cli
	hlt

	#NO_APP
	addq	$80, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	bootstrap_kernel, .Lfunc_end3-bootstrap_kernel
	.cfi_endproc

	.text
	.globl	target_device_setup
	.align	16, 0x90
	.type	target_device_setup,@function
target_device_setup:                    # @target_device_setup
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp13:
	.cfi_def_cfa_offset 16
.Ltmp14:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp15:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	callq	APIC_GetTimerFrequency
	movl	$33, %ecx
	movl	$1000, %edx             # imm = 0x3E8
	movl	%edx, %esi
	xorl	%edx, %edx
                                        # kill: RDX<def> EDX<kill>
	divq	%rsi
	movl	$33, %edi
	movq	%rax, %rsi
	movl	%ecx, -4(%rbp)          # 4-byte Spill
	callq	SetPeriodicPreemptVector
	movl	$800, %ecx              # imm = 0x320
	movl	$33, %edi
	movl	$800, %r8d              # imm = 0x320
	movl	$33, %esi
	movl	%edi, -8(%rbp)          # 4-byte Spill
	movl	%r8d, %edi
	movl	%ecx, -12(%rbp)         # 4-byte Spill
	callq	APIC_SetVector
	callq	APIC_GetTimerFrequency
	movl	$1000, %ecx             # imm = 0x3E8
	movl	%ecx, %r9d
	xorl	%ecx, %ecx
	movl	%ecx, %edx
	divq	%r9
	movl	%eax, %ecx
	movl	%ecx, %edi
	callq	APIC_SetTimerValue
	movl	$1, %ecx
	movl	$1, %edi
	movl	%ecx, -16(%rbp)         # 4-byte Spill
	callq	APIC_SetTimerMode
	movl	$800, %ecx              # imm = 0x320
	movl	$1, %esi
	movl	$800, %edi              # imm = 0x320
	movl	$1, %r8d
	movl	%esi, -20(%rbp)         # 4-byte Spill
	movl	%r8d, %esi
	movl	%ecx, -24(%rbp)         # 4-byte Spill
	callq	APIC_SetEnableInterrupt
	callq	pci_Initialize
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	target_device_setup, .Lfunc_end4-target_device_setup
	.cfi_endproc

	.globl	get_perf_counter
	.align	16, 0x90
	.type	get_perf_counter,@function
get_perf_counter:                       # @get_perf_counter
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp16:
	.cfi_def_cfa_offset 16
.Ltmp17:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp18:
	.cfi_def_cfa_register %rbp
	xorl	%eax, %eax
	popq	%rbp
	retq
.Lfunc_end5:
	.size	get_perf_counter, .Lfunc_end5-get_perf_counter
	.cfi_endproc

	.globl	smp_unlock_cores
	.align	16, 0x90
	.type	smp_unlock_cores,@function
smp_unlock_cores:                       # @smp_unlock_cores
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp19:
	.cfi_def_cfa_offset 16
.Ltmp20:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp21:
	.cfi_def_cfa_register %rbp
	movl	$0, smp_sync_base
	popq	%rbp
	retq
.Lfunc_end6:
	.size	smp_unlock_cores, .Lfunc_end6-smp_unlock_cores
	.cfi_endproc

	.section	.tramp_handler,"ax",@progbits
	.globl	smp_bootstrap
	.align	16, 0x90
	.type	smp_bootstrap,@function
smp_bootstrap:                          # @smp_bootstrap
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp22:
	.cfi_def_cfa_offset 16
.Ltmp23:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp24:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	callq	SMP_LockTrampoline
	movl	$16384, %eax            # imm = 0x4000
	movl	%eax, %ecx
	movl	$16384, %eax            # imm = 0x4000
	movl	%eax, %edi
	movq	%rcx, -24(%rbp)         # 8-byte Spill
	callq	bootstrap_malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	addq	$16384, %rax            # imm = 0x4000
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	#APP
	movq	%rax, %rsp
	#NO_APP
	xorl	%edx, %edx
	movb	%dl, %sil
	movb	%sil, %al
	callq	GDT_Initialize
	callq	IDT_Initialize
	callq	FPU_Initialize
	callq	VirtMemMan_InitializeBootstrap
	callq	VirtMemMan_Initialize
	callq	SMP_GetCoreCount
	movl	%eax, -12(%rbp)
	callq	APIC_LocalInitialize
	#APP
	sti
	#NO_APP
	movb	%al, -25(%rbp)          # 1-byte Spill
	callq	APIC_CallibrateTimer
	#APP
	cli
	#NO_APP
	callq	SMP_IncrementCoreCount
	callq	SMP_UnlockTrampoline
.LBB7_1:                                # =>This Inner Loop Header: Depth=1
	movl	smp_sync_base, %eax
	cmpl	$0, %eax
	je	.LBB7_3
# BB#2:                                 #   in Loop: Header=BB7_1 Depth=1
	jmp	.LBB7_1
.LBB7_3:
	movl	-12(%rbp), %edi
	movq	$get_perf_counter, %rsi
	callq	smp_core_main
.LBB7_4:                                # =>This Inner Loop Header: Depth=1
	jmp	.LBB7_4
.Lfunc_end7:
	.size	smp_bootstrap, .Lfunc_end7-smp_bootstrap
	.cfi_endproc

	.type	smp_sync_base,@object   # @smp_sync_base
	.local	smp_sync_base
	.comm	smp_sync_base,4,4

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
