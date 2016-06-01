	.text
	.file	"managers/thread_manager/thread.c"
	.globl	Thread_Initialize
	.align	16, 0x90
	.type	Thread_Initialize,@function
Thread_Initialize:                      # @Thread_Initialize
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
	movq	%rax, vLow
	callq	List_Create
	movq	%rax, low
	callq	List_Create
	movq	%rax, medium
	callq	List_Create
	movq	%rax, neutral
	callq	List_Create
	movq	%rax, high
	callq	List_Create
	movq	%rax, vHigh
	callq	List_Create
	movq	%rax, max
	callq	List_Create
	movq	%rax, thds
	callq	List_Create
	movq	%rax, cores
	callq	CreateSpinlock
	movq	%rax, vLow_s
	callq	CreateSpinlock
	movq	%rax, low_s
	callq	CreateSpinlock
	movq	%rax, medium_s
	callq	CreateSpinlock
	movq	%rax, neutral_s
	callq	CreateSpinlock
	movq	%rax, high_s
	callq	CreateSpinlock
	movq	%rax, vHigh_s
	callq	CreateSpinlock
	movq	%rax, max_s
	callq	CreateSpinlock
	movq	%rax, thds_s
	callq	CreateSpinlock
	movq	%rax, core_s
	popq	%rbp
	retq
.Lfunc_end0:
	.size	Thread_Initialize, .Lfunc_end0-Thread_Initialize
	.cfi_endproc

	.globl	CreateThread
	.align	16, 0x90
	.type	CreateThread,@function
CreateThread:                           # @CreateThread
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
	movl	$72, %eax
	movl	%eax, %ecx
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movl	$72, %eax
	movl	%eax, %edi
	movq	%rcx, -40(%rbp)         # 8-byte Spill
	callq	kmalloc
	movq	%rax, -32(%rbp)
	movq	-24(%rbp), %rax
	movq	-32(%rbp), %rcx
	movq	%rax, 24(%rcx)
	movq	-32(%rbp), %rax
	movl	$0, 32(%rax)
	movq	-32(%rbp), %rax
	movl	$3, 36(%rax)
	movq	-16(%rbp), %rax
	movq	-32(%rbp), %rcx
	movq	%rax, (%rcx)
	movq	-32(%rbp), %rax
	movq	$0, 64(%rax)
	movq	-16(%rbp), %rdi
	movq	-32(%rbp), %rax
	addq	$16, %rax
	movq	%rax, %rsi
	callq	GetProcessReference
	cmpl	$2, %eax
	jne	.LBB1_2
# BB#1:
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	callq	kfree
	movq	$-1, -8(%rbp)
	jmp	.LBB1_3
.LBB1_2:
	callq	new_uid
	movq	-32(%rbp), %rcx
	movq	%rax, 8(%rcx)
	movl	$16384, %edx            # imm = 0x4000
	movl	%edx, %edi
	callq	kmalloc
	movq	-32(%rbp), %rcx
	movq	%rax, 40(%rcx)
	movq	neutral_s, %rdi
	callq	LockSpinlock
	movq	neutral, %rdi
	movq	-32(%rbp), %rsi
	movb	%al, -41(%rbp)          # 1-byte Spill
	callq	List_AddEntry
	movq	neutral_s, %rdi
	movl	%eax, -48(%rbp)         # 4-byte Spill
	callq	UnlockSpinlock
	movq	thds_s, %rdi
	movb	%al, -49(%rbp)          # 1-byte Spill
	callq	LockSpinlock
	movq	thds, %rdi
	movq	-32(%rbp), %rsi
	movb	%al, -50(%rbp)          # 1-byte Spill
	callq	List_AddEntry
	movq	thds_s, %rdi
	movl	%eax, -56(%rbp)         # 4-byte Spill
	callq	UnlockSpinlock
	movq	-32(%rbp), %rcx
	movq	8(%rcx), %rcx
	movq	%rcx, -8(%rbp)
	movb	%al, -57(%rbp)          # 1-byte Spill
.LBB1_3:
	movq	-8(%rbp), %rax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	CreateThread, .Lfunc_end1-CreateThread
	.cfi_endproc

	.globl	SetThreadState
	.align	16, 0x90
	.type	SetThreadState,@function
SetThreadState:                         # @SetThreadState
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
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	$0, -24(%rbp)
.LBB2_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	thds, %rdi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-40(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB2_6
# BB#2:                                 #   in Loop: Header=BB2_1 Depth=1
	movq	thds, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-8(%rbp), %rax
	jne	.LBB2_4
# BB#3:
	movl	-12(%rbp), %eax
	movq	-32(%rbp), %rcx
	movl	%eax, 32(%rcx)
	jmp	.LBB2_6
.LBB2_4:                                #   in Loop: Header=BB2_1 Depth=1
	jmp	.LBB2_5
.LBB2_5:                                #   in Loop: Header=BB2_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB2_1
.LBB2_6:
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	SetThreadState, .Lfunc_end2-SetThreadState
	.cfi_endproc

	.globl	SleepThread
	.align	16, 0x90
	.type	SleepThread,@function
SleepThread:                            # @SleepThread
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
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	$0, -24(%rbp)
.LBB3_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	thds, %rdi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-40(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB3_6
# BB#2:                                 #   in Loop: Header=BB3_1 Depth=1
	movq	thds, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-8(%rbp), %rax
	jne	.LBB3_4
# BB#3:
	movq	-32(%rbp), %rax
	movl	$3, 32(%rax)
	movq	-16(%rbp), %rax
	movq	-32(%rbp), %rcx
	movq	%rax, 64(%rcx)
	jmp	.LBB3_6
.LBB3_4:                                #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_5
.LBB3_5:                                #   in Loop: Header=BB3_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB3_1
.LBB3_6:
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	SleepThread, .Lfunc_end3-SleepThread
	.cfi_endproc

	.globl	GetThreadState
	.align	16, 0x90
	.type	GetThreadState,@function
GetThreadState:                         # @GetThreadState
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
	movq	%rdi, -16(%rbp)
	movq	$0, -24(%rbp)
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	thds, %rdi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-40(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB4_6
# BB#2:                                 #   in Loop: Header=BB4_1 Depth=1
	movq	thds, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-16(%rbp), %rax
	jne	.LBB4_4
# BB#3:
	movq	-32(%rbp), %rax
	movl	32(%rax), %ecx
	movl	%ecx, -4(%rbp)
	jmp	.LBB4_7
.LBB4_4:                                #   in Loop: Header=BB4_1 Depth=1
	jmp	.LBB4_5
.LBB4_5:                                #   in Loop: Header=BB4_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB4_1
.LBB4_6:
	movl	$-1, -4(%rbp)
.LBB4_7:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	GetThreadState, .Lfunc_end4-GetThreadState
	.cfi_endproc

	.globl	SetThreadBasePriority
	.align	16, 0x90
	.type	SetThreadBasePriority,@function
SetThreadBasePriority:                  # @SetThreadBasePriority
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
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	$0, -24(%rbp)
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	thds, %rdi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-40(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB5_6
# BB#2:                                 #   in Loop: Header=BB5_1 Depth=1
	movq	thds, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-8(%rbp), %rax
	jne	.LBB5_4
# BB#3:
	movl	-12(%rbp), %eax
	movq	-32(%rbp), %rcx
	movl	%eax, 36(%rcx)
	jmp	.LBB5_6
.LBB5_4:                                #   in Loop: Header=BB5_1 Depth=1
	jmp	.LBB5_5
.LBB5_5:                                #   in Loop: Header=BB5_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB5_1
.LBB5_6:
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	SetThreadBasePriority, .Lfunc_end5-SetThreadBasePriority
	.cfi_endproc

	.globl	GetThreadPriority
	.align	16, 0x90
	.type	GetThreadPriority,@function
GetThreadPriority:                      # @GetThreadPriority
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
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movq	$0, -24(%rbp)
.LBB6_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	thds, %rdi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-40(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB6_6
# BB#2:                                 #   in Loop: Header=BB6_1 Depth=1
	movq	thds, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-16(%rbp), %rax
	jne	.LBB6_4
# BB#3:
	movq	-32(%rbp), %rax
	movl	36(%rax), %ecx
	movl	%ecx, -4(%rbp)
	jmp	.LBB6_7
.LBB6_4:                                #   in Loop: Header=BB6_1 Depth=1
	jmp	.LBB6_5
.LBB6_5:                                #   in Loop: Header=BB6_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB6_1
.LBB6_6:
	movl	$-1, -4(%rbp)
.LBB6_7:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	GetThreadPriority, .Lfunc_end6-GetThreadPriority
	.cfi_endproc

	.globl	SetThreadCoreAffinity
	.align	16, 0x90
	.type	SetThreadCoreAffinity,@function
SetThreadCoreAffinity:                  # @SetThreadCoreAffinity
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp21:
	.cfi_def_cfa_offset 16
.Ltmp22:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp23:
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	$0, -24(%rbp)
.LBB7_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	thds, %rdi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-40(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB7_6
# BB#2:                                 #   in Loop: Header=BB7_1 Depth=1
	movq	thds, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-8(%rbp), %rax
	jne	.LBB7_4
# BB#3:
	movl	-12(%rbp), %eax
	movq	-32(%rbp), %rcx
	movl	%eax, 56(%rcx)
	jmp	.LBB7_6
.LBB7_4:                                #   in Loop: Header=BB7_1 Depth=1
	jmp	.LBB7_5
.LBB7_5:                                #   in Loop: Header=BB7_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB7_1
.LBB7_6:
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end7:
	.size	SetThreadCoreAffinity, .Lfunc_end7-SetThreadCoreAffinity
	.cfi_endproc

	.globl	GetThreadCoreAffinity
	.align	16, 0x90
	.type	GetThreadCoreAffinity,@function
GetThreadCoreAffinity:                  # @GetThreadCoreAffinity
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp24:
	.cfi_def_cfa_offset 16
.Ltmp25:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp26:
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -16(%rbp)
	movq	$0, -24(%rbp)
.LBB8_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	thds, %rdi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-40(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB8_6
# BB#2:                                 #   in Loop: Header=BB8_1 Depth=1
	movq	thds, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-16(%rbp), %rax
	jne	.LBB8_4
# BB#3:
	movq	-32(%rbp), %rax
	movl	56(%rax), %ecx
	movl	%ecx, -4(%rbp)
	jmp	.LBB8_7
.LBB8_4:                                #   in Loop: Header=BB8_1 Depth=1
	jmp	.LBB8_5
.LBB8_5:                                #   in Loop: Header=BB8_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB8_1
.LBB8_6:
	movl	$-1, -4(%rbp)
.LBB8_7:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end8:
	.size	GetThreadCoreAffinity, .Lfunc_end8-GetThreadCoreAffinity
	.cfi_endproc

	.globl	FreeThread
	.align	16, 0x90
	.type	FreeThread,@function
FreeThread:                             # @FreeThread
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp27:
	.cfi_def_cfa_offset 16
.Ltmp28:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp29:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	$0, -16(%rbp)
.LBB9_1:                                # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	movq	thds, %rdi
	movq	%rax, -32(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-32(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB9_6
# BB#2:                                 #   in Loop: Header=BB9_1 Depth=1
	movq	thds, %rdi
	movq	-16(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	-8(%rbp), %rax
	jne	.LBB9_4
# BB#3:                                 #   in Loop: Header=BB9_1 Depth=1
	movq	-24(%rbp), %rax
	movl	$4, 32(%rax)
.LBB9_4:                                #   in Loop: Header=BB9_1 Depth=1
	jmp	.LBB9_5
.LBB9_5:                                #   in Loop: Header=BB9_1 Depth=1
	movq	-16(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -16(%rbp)
	jmp	.LBB9_1
.LBB9_6:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end9:
	.size	FreeThread, .Lfunc_end9-FreeThread
	.cfi_endproc

	.globl	YieldThread
	.align	16, 0x90
	.type	YieldThread,@function
YieldThread:                            # @YieldThread
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp30:
	.cfi_def_cfa_offset 16
.Ltmp31:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp32:
	.cfi_def_cfa_register %rbp
	popq	%rbp
	retq
.Lfunc_end10:
	.size	YieldThread, .Lfunc_end10-YieldThread
	.cfi_endproc

	.globl	SetPeriodicPreemptVector
	.align	16, 0x90
	.type	SetPeriodicPreemptVector,@function
SetPeriodicPreemptVector:               # @SetPeriodicPreemptVector
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp33:
	.cfi_def_cfa_offset 16
.Ltmp34:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp35:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	-4(%rbp), %edi
	movq	$TaskSwitch, %rsi
	callq	RegisterInterruptHandler
	movl	-4(%rbp), %edi
	movl	%edi, preempt_vector
	movq	-16(%rbp), %rsi
	movq	%rsi, preempt_frequency
	movl	%eax, -20(%rbp)         # 4-byte Spill
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end11:
	.size	SetPeriodicPreemptVector, .Lfunc_end11-SetPeriodicPreemptVector
	.cfi_endproc

	.align	16, 0x90
	.type	TaskSwitch,@function
TaskSwitch:                             # @TaskSwitch
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp36:
	.cfi_def_cfa_offset 16
.Ltmp37:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp38:
	.cfi_def_cfa_register %rbp
	subq	$128, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$0, -8(%rbp)
	movq	thds, %rdi
	xorl	%esi, %esi
	movl	%esi, %eax
	movq	%rax, %rsi
	movq	%rax, -32(%rbp)         # 8-byte Spill
	callq	List_EntryAt
	movq	%rax, -16(%rbp)
	movq	thds, %rdi
	movq	-32(%rbp), %rsi         # 8-byte Reload
	callq	List_Remove
	movq	thds, %rdi
	movq	-16(%rbp), %rsi
	callq	List_AddEntry
	movl	invokeCount, %ecx
	incl	%ecx
	movl	%ecx, invokeCount
	movl	%eax, -36(%rbp)         # 4-byte Spill
.LBB12_1:                               # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	cmpl	$4, 32(%rax)
	jne	.LBB12_3
# BB#2:                                 #   in Loop: Header=BB12_1 Depth=1
	movq	thds, %rax
	movq	%rax, %rdi
	movq	%rax, -48(%rbp)         # 8-byte Spill
	callq	List_Length
	decq	%rax
	movq	-48(%rbp), %rdi         # 8-byte Reload
	movq	%rax, %rsi
	callq	List_Remove
	movq	-16(%rbp), %rax
	movq	40(%rax), %rdi
	callq	kfree
	movq	-16(%rbp), %rdi
	callq	kfree
	movq	thds, %rdi
	xorl	%ecx, %ecx
	movl	%ecx, %eax
	movq	%rax, %rsi
	movq	%rax, -56(%rbp)         # 8-byte Spill
	callq	List_EntryAt
	movq	%rax, -16(%rbp)
	movq	thds, %rdi
	movq	-56(%rbp), %rsi         # 8-byte Reload
	callq	List_Remove
	movq	thds, %rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	callq	List_AddEntry
	movl	%eax, -60(%rbp)         # 4-byte Spill
	jmp	.LBB12_1
.LBB12_3:
	jmp	.LBB12_4
.LBB12_4:                               # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	cmpl	$2, 32(%rax)
	jne	.LBB12_6
# BB#5:                                 #   in Loop: Header=BB12_4 Depth=1
	movq	thds, %rdi
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	%rcx, %rsi
	movq	%rcx, -72(%rbp)         # 8-byte Spill
	callq	List_EntryAt
	movq	%rax, -16(%rbp)
	movq	thds, %rdi
	movq	-72(%rbp), %rsi         # 8-byte Reload
	callq	List_Remove
	movq	thds, %rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	callq	List_AddEntry
	movl	%eax, -76(%rbp)         # 4-byte Spill
	jmp	.LBB12_4
.LBB12_6:
	jmp	.LBB12_7
.LBB12_7:                               # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	cmpl	$3, 32(%rax)
	jne	.LBB12_15
# BB#8:                                 #   in Loop: Header=BB12_7 Depth=1
	movl	$1000, %eax             # imm = 0x3E8
	movl	%eax, %ecx
	movq	-16(%rbp), %rdx
	movq	64(%rdx), %rdx
	movq	preempt_frequency, %rax
	xorl	%esi, %esi
	movq	%rdx, -88(%rbp)         # 8-byte Spill
	movl	%esi, %edx
	divq	%rcx
	movq	-88(%rbp), %rcx         # 8-byte Reload
	cmpq	%rax, %rcx
	jbe	.LBB12_10
# BB#9:                                 #   in Loop: Header=BB12_7 Depth=1
	movl	$1000, %eax             # imm = 0x3E8
	movl	%eax, %ecx
	movq	preempt_frequency, %rax
	xorl	%edx, %edx
                                        # kill: RDX<def> EDX<kill>
	divq	%rcx
	movq	%rax, -96(%rbp)         # 8-byte Spill
	jmp	.LBB12_11
.LBB12_10:                              #   in Loop: Header=BB12_7 Depth=1
	movq	-16(%rbp), %rax
	movq	64(%rax), %rax
	movq	%rax, -96(%rbp)         # 8-byte Spill
.LBB12_11:                              #   in Loop: Header=BB12_7 Depth=1
	movq	-96(%rbp), %rax         # 8-byte Reload
	movq	-16(%rbp), %rcx
	movq	64(%rcx), %rdx
	subq	%rax, %rdx
	movq	%rdx, 64(%rcx)
	movq	-16(%rbp), %rax
	cmpq	$0, 64(%rax)
	jne	.LBB12_13
# BB#12:
	movq	-16(%rbp), %rax
	movl	$1, 32(%rax)
	jmp	.LBB12_15
.LBB12_13:                              #   in Loop: Header=BB12_7 Depth=1
	movq	thds, %rdi
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	%rcx, %rsi
	movq	%rcx, -104(%rbp)        # 8-byte Spill
	callq	List_EntryAt
	movq	%rax, -16(%rbp)
	movq	thds, %rdi
	movq	-104(%rbp), %rsi        # 8-byte Reload
	callq	List_Remove
	movq	thds, %rdi
	movq	-16(%rbp), %rax
	movq	%rax, %rsi
	callq	List_AddEntry
	movl	%eax, -108(%rbp)        # 4-byte Spill
# BB#14:                                #   in Loop: Header=BB12_7 Depth=1
	jmp	.LBB12_7
.LBB12_15:
	movl	invokeCount, %eax
	cmpl	$1, %eax
	jne	.LBB12_17
# BB#16:
	movq	cur_thread, %rax
	movq	24(%rax), %rax
	#APP
	hlt
	#NO_APP
.LBB12_17:
	movq	cur_thread, %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, cur_thread
	movq	-16(%rbp), %rax
	movq	16(%rax), %rax
	movq	456(%rax), %rdi
	callq	SetActiveVirtualMemoryInstance
	movq	-16(%rbp), %rdi
	cmpl	$1, 32(%rdi)
	movq	%rax, -120(%rbp)        # 8-byte Spill
	jne	.LBB12_19
# BB#18:
	movq	-24(%rbp), %rdi
	movq	-16(%rbp), %rsi
	callq	SwapThreadOnInterrupt
	jmp	.LBB12_22
.LBB12_19:
	movq	-16(%rbp), %rax
	cmpl	$0, 32(%rax)
	jne	.LBB12_21
# BB#20:
	movl	-4(%rbp), %edi
	callq	HandleInterruptNoReturn
	movq	-16(%rbp), %rdi
	callq	SwitchAndInitializeThread
.LBB12_21:
	jmp	.LBB12_22
.LBB12_22:
	addq	$128, %rsp
	popq	%rbp
	retq
.Lfunc_end12:
	.size	TaskSwitch, .Lfunc_end12-TaskSwitch
	.cfi_endproc

	.globl	SwitchThread
	.align	16, 0x90
	.type	SwitchThread,@function
SwitchThread:                           # @SwitchThread
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp39:
	.cfi_def_cfa_offset 16
.Ltmp40:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp41:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	thds, %rdi
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	%rcx, %rsi
	movq	%rcx, -8(%rbp)          # 8-byte Spill
	callq	List_EntryAt
	movq	%rax, cur_thread
	movq	thds, %rdi
	movq	-8(%rbp), %rsi          # 8-byte Reload
	callq	List_Remove
	movq	thds, %rdi
	movq	cur_thread, %rsi
	callq	List_AddEntry
	movq	cur_thread, %rcx
	cmpl	$0, 32(%rcx)
	movl	%eax, -12(%rbp)         # 4-byte Spill
	jne	.LBB13_2
# BB#1:
	movq	cur_thread, %rax
	movl	$1, 32(%rax)
	movq	cur_thread, %rdi
	callq	SwitchAndInitializeThread
	jmp	.LBB13_8
.LBB13_2:
	movq	cur_thread, %rax
	cmpl	$1, 32(%rax)
	jne	.LBB13_4
# BB#3:
	jmp	.LBB13_7
.LBB13_4:
	movq	cur_thread, %rax
	cmpl	$2, 32(%rax)
	jne	.LBB13_6
# BB#5:
	jmp	.LBB13_8
.LBB13_6:
	jmp	.LBB13_7
.LBB13_7:
	jmp	.LBB13_8
.LBB13_8:
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end13:
	.size	SwitchThread, .Lfunc_end13-SwitchThread
	.cfi_endproc

	.globl	CoreUpdate
	.align	16, 0x90
	.type	CoreUpdate,@function
CoreUpdate:                             # @CoreUpdate
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp42:
	.cfi_def_cfa_offset 16
.Ltmp43:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp44:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	$0, -4(%rbp)
.LBB14_1:                               # =>This Inner Loop Header: Depth=1
	callq	SwitchThread
	jmp	.LBB14_1
.Lfunc_end14:
	.size	CoreUpdate, .Lfunc_end14-CoreUpdate
	.cfi_endproc

	.globl	RegisterCore
	.align	16, 0x90
	.type	RegisterCore,@function
RegisterCore:                           # @RegisterCore
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp45:
	.cfi_def_cfa_offset 16
.Ltmp46:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp47:
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$16, %edi
                                        # kill: RDI<def> EDI<kill>
	callq	kmalloc
	movq	%rax, -24(%rbp)
	movslq	-4(%rbp), %rsi
	movq	%rsi, (%rax)
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rsi
	movq	%rax, 8(%rsi)
	movq	cores, %rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	callq	List_AddEntry
	movl	%eax, -28(%rbp)         # 4-byte Spill
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end15:
	.size	RegisterCore, .Lfunc_end15-RegisterCore
	.cfi_endproc

	.globl	GetCoreLoad
	.align	16, 0x90
	.type	GetCoreLoad,@function
GetCoreLoad:                            # @GetCoreLoad
	.cfi_startproc
# BB#0:
	pushq	%rbp
.Ltmp48:
	.cfi_def_cfa_offset 16
.Ltmp49:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
.Ltmp50:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -8(%rbp)
	movq	cores, %rax
	movl	%edi, -12(%rbp)         # 4-byte Spill
	movq	%rax, %rdi
	callq	List_Length
	movl	%eax, %ecx
	movl	-12(%rbp), %edx         # 4-byte Reload
	cmpl	%ecx, %edx
	jle	.LBB16_2
# BB#1:
	movl	$-1, -4(%rbp)
	jmp	.LBB16_3
.LBB16_2:
	movq	cores, %rdi
	movslq	-8(%rbp), %rsi
	callq	List_EntryAt
	callq	*8(%rax)
	movl	%eax, -4(%rbp)
.LBB16_3:
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end16:
	.size	GetCoreLoad, .Lfunc_end16-GetCoreLoad
	.cfi_endproc

	.type	vLow,@object            # @vLow
	.local	vLow
	.comm	vLow,8,8
	.type	low,@object             # @low
	.local	low
	.comm	low,8,8
	.type	medium,@object          # @medium
	.local	medium
	.comm	medium,8,8
	.type	neutral,@object         # @neutral
	.local	neutral
	.comm	neutral,8,8
	.type	high,@object            # @high
	.local	high
	.comm	high,8,8
	.type	vHigh,@object           # @vHigh
	.local	vHigh
	.comm	vHigh,8,8
	.type	max,@object             # @max
	.local	max
	.comm	max,8,8
	.type	thds,@object            # @thds
	.local	thds
	.comm	thds,8,8
	.type	cores,@object           # @cores
	.local	cores
	.comm	cores,8,8
	.type	vLow_s,@object          # @vLow_s
	.local	vLow_s
	.comm	vLow_s,8,8
	.type	low_s,@object           # @low_s
	.local	low_s
	.comm	low_s,8,8
	.type	medium_s,@object        # @medium_s
	.local	medium_s
	.comm	medium_s,8,8
	.type	neutral_s,@object       # @neutral_s
	.local	neutral_s
	.comm	neutral_s,8,8
	.type	high_s,@object          # @high_s
	.local	high_s
	.comm	high_s,8,8
	.type	vHigh_s,@object         # @vHigh_s
	.local	vHigh_s
	.comm	vHigh_s,8,8
	.type	max_s,@object           # @max_s
	.local	max_s
	.comm	max_s,8,8
	.type	thds_s,@object          # @thds_s
	.local	thds_s
	.comm	thds_s,8,8
	.type	core_s,@object          # @core_s
	.local	core_s
	.comm	core_s,8,8
	.type	invokeCount,@object     # @invokeCount
	.bss
	.globl	invokeCount
	.align	4
invokeCount:
	.long	0                       # 0x0
	.size	invokeCount, 4

	.type	preempt_vector,@object  # @preempt_vector
	.local	preempt_vector
	.comm	preempt_vector,4,4
	.type	preempt_frequency,@object # @preempt_frequency
	.local	preempt_frequency
	.comm	preempt_frequency,8,8
	.type	cur_thread,@object      # @cur_thread
	.local	cur_thread
	.comm	cur_thread,8,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
