	.text
	.file	"managers/process_manager/process.c"
	.globl	ProcessSys_Initialize
	.align	16, 0x90
	.type	ProcessSys_Initialize,@function
ProcessSys_Initialize:                  # @ProcessSys_Initialize
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
	movq	%rdi, -8(%rbp)
	movl	$496, %eax              # imm = 0x1F0
	movl	%eax, %edi
	callq	kmalloc
	movq	%rax, root
	movq	$0, (%rax)
	movq	root, %rax
	addq	$8, %rax
	movq	$.L.str, %rsi
	movq	%rax, %rdi
	callq	strcpy
	movq	root, %rsi
	movl	$3, 264(%rsi)
	movq	root, %rsi
	movl	$1, 268(%rsi)
	movq	root, %rsi
	movl	$4, 272(%rsi)
	movq	%rax, -16(%rbp)         # 8-byte Spill
	callq	GetActiveVirtualMemoryInstance
	movq	root, %rsi
	movq	%rax, 456(%rsi)
	movq	-8(%rbp), %rax
	movq	root, %rsi
	movq	%rax, 464(%rsi)
	movq	root, %rax
	movq	$0, 480(%rax)
	movq	root, %rax
	movq	$0, 488(%rax)
	callq	List_Create
	movq	root, %rsi
	movq	%rax, 472(%rsi)
	callq	List_Create
	movq	%rax, processes
	movq	root, %rsi
	movq	%rax, %rdi
	callq	List_AddEntry
	movl	%eax, -20(%rbp)         # 4-byte Spill
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	ProcessSys_Initialize, .Lfunc_end0-ProcessSys_Initialize
	.cfi_endproc

	.globl	ForkProcess
	.align	16, 0x90
	.type	ForkProcess,@function
ForkProcess:                            # @ForkProcess
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
	subq	$48, %rsp
	movl	$496, %eax              # imm = 0x1F0
	movl	%eax, %ecx
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$496, %eax              # imm = 0x1F0
	movl	%eax, %edi
	movq	%rcx, -40(%rbp)         # 8-byte Spill
	callq	kmalloc
	movq	%rax, -24(%rbp)
	callq	new_uid
	movq	-24(%rbp), %rcx
	movq	%rax, (%rcx)
	movq	-8(%rbp), %rax
	movl	264(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%edx, 264(%rax)
	movq	-8(%rbp), %rax
	movl	268(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%edx, 268(%rax)
	movq	-8(%rbp), %rax
	movl	272(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%edx, 272(%rax)
	movl	$0, -28(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	cmpl	$22, -28(%rbp)
	jge	.LBB1_4
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	movslq	-28(%rbp), %rax
	movq	-8(%rbp), %rcx
	movq	280(%rcx,%rax,8), %rax
	movslq	-28(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	%rax, 280(%rdx,%rcx,8)
# BB#3:                                 #   in Loop: Header=BB1_1 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	.LBB1_1
.LBB1_4:
	movq	-8(%rbp), %rax
	movq	456(%rax), %rdi
	movq	464(%rax), %rsi
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	addq	$456, %rcx              # imm = 0x1C8
	addq	$464, %rax              # imm = 0x1D0
	movq	%rcx, %rdx
	movq	%rax, %rcx
	callq	ForkTable
	movq	-8(%rbp), %rcx
	movq	480(%rcx), %rcx
	movq	-24(%rbp), %rdx
	movq	%rcx, 488(%rdx)
	movq	-24(%rbp), %rcx
	movq	-8(%rbp), %rdx
	movq	%rcx, 480(%rdx)
	movl	%eax, -44(%rbp)         # 4-byte Spill
	callq	List_Create
	movq	-24(%rbp), %rcx
	movq	%rax, 472(%rcx)
	movq	processes, %rdi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	callq	List_AddEntry
	xorl	%r8d, %r8d
	movq	-24(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movq	%rcx, (%rdx)
	movl	%eax, -48(%rbp)         # 4-byte Spill
	movl	%r8d, %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	ForkProcess, .Lfunc_end1-ForkProcess
	.cfi_endproc

	.globl	GetProcessInformation
	.align	16, 0x90
	.type	GetProcessInformation,@function
GetProcessInformation:                  # @GetProcessInformation
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
	subq	$80, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	$0, -32(%rbp)
.LBB2_1:                                # =>This Inner Loop Header: Depth=1
	movq	-32(%rbp), %rax
	movq	processes, %rdi
	movq	%rax, -48(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-48(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB2_6
# BB#2:                                 #   in Loop: Header=BB2_1 Depth=1
	movq	processes, %rdi
	movq	-32(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	movq	(%rax), %rax
	cmpq	-16(%rbp), %rax
	jne	.LBB2_4
# BB#3:
	movl	$496, %eax              # imm = 0x1F0
	movl	%eax, %ecx
	movq	-24(%rbp), %rdx
	movq	-40(%rbp), %rsi
	movl	$496, %eax              # imm = 0x1F0
	movl	%eax, %edi
	movq	%rdi, -56(%rbp)         # 8-byte Spill
	movq	%rdx, %rdi
	movq	-56(%rbp), %rdx         # 8-byte Reload
	movq	%rcx, -64(%rbp)         # 8-byte Spill
	callq	memcpy
	movl	$0, -4(%rbp)
	movq	%rax, -72(%rbp)         # 8-byte Spill
	jmp	.LBB2_7
.LBB2_4:                                #   in Loop: Header=BB2_1 Depth=1
	jmp	.LBB2_5
.LBB2_5:                                #   in Loop: Header=BB2_1 Depth=1
	movq	-32(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -32(%rbp)
	jmp	.LBB2_1
.LBB2_6:
	movl	$2, -4(%rbp)
.LBB2_7:
	movl	-4(%rbp), %eax
	addq	$80, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	GetProcessInformation, .Lfunc_end2-GetProcessInformation
	.cfi_endproc

	.globl	GetProcessReference
	.align	16, 0x90
	.type	GetProcessReference,@function
GetProcessReference:                    # @GetProcessReference
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
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	$0, -32(%rbp)
.LBB3_1:                                # =>This Inner Loop Header: Depth=1
	movq	-32(%rbp), %rax
	movq	processes, %rdi
	movq	%rax, -48(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-48(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB3_6
# BB#2:                                 #   in Loop: Header=BB3_1 Depth=1
	movq	processes, %rdi
	movq	-32(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	movq	(%rax), %rax
	cmpq	-16(%rbp), %rax
	jne	.LBB3_4
# BB#3:
	movq	-40(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	%rax, (%rcx)
	movl	$0, -4(%rbp)
	jmp	.LBB3_7
.LBB3_4:                                #   in Loop: Header=BB3_1 Depth=1
	jmp	.LBB3_5
.LBB3_5:                                #   in Loop: Header=BB3_1 Depth=1
	movq	-32(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -32(%rbp)
	jmp	.LBB3_1
.LBB3_6:
	movl	$2, -4(%rbp)
.LBB3_7:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	GetProcessReference, .Lfunc_end3-GetProcessReference
	.cfi_endproc

	.globl	KillProcess
	.align	16, 0x90
	.type	KillProcess,@function
KillProcess:                            # @KillProcess
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
	subq	$64, %rsp
	movq	%rdi, -16(%rbp)
	movq	$0, -24(%rbp)
.LBB4_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	processes, %rdi
	movq	%rax, -48(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-48(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB4_10
# BB#2:                                 #   in Loop: Header=BB4_1 Depth=1
	movq	processes, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	cmpq	-16(%rbp), %rax
	jne	.LBB4_8
# BB#3:
	movq	-32(%rbp), %rax
	movl	$2, 264(%rax)
	movq	$0, -40(%rbp)
.LBB4_4:                                # =>This Inner Loop Header: Depth=1
	movq	-40(%rbp), %rax
	movq	-32(%rbp), %rcx
	movq	472(%rcx), %rdi
	movq	%rax, -56(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-56(%rbp), %rcx         # 8-byte Reload
	cmpq	%rax, %rcx
	jae	.LBB4_7
# BB#5:                                 #   in Loop: Header=BB4_4 Depth=1
	movq	-32(%rbp), %rax
	movq	472(%rax), %rdi
	movq	-40(%rbp), %rsi
	callq	List_EntryAt
	movl	$4, %ecx
	movl	$4, %esi
	movq	%rax, %rdi
	movl	%ecx, -60(%rbp)         # 4-byte Spill
	callq	SetThreadState
# BB#6:                                 #   in Loop: Header=BB4_4 Depth=1
	movq	-40(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -40(%rbp)
	jmp	.LBB4_4
.LBB4_7:
	movl	$0, -4(%rbp)
	jmp	.LBB4_11
.LBB4_8:                                #   in Loop: Header=BB4_1 Depth=1
	jmp	.LBB4_9
.LBB4_9:                                #   in Loop: Header=BB4_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB4_1
.LBB4_10:
	movl	$2, -4(%rbp)
.LBB4_11:
	movl	-4(%rbp), %eax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	KillProcess, .Lfunc_end4-KillProcess
	.cfi_endproc

	.globl	SleepProcess
	.align	16, 0x90
	.type	SleepProcess,@function
SleepProcess:                           # @SleepProcess
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
	movq	%rdi, -16(%rbp)
	movq	$0, -24(%rbp)
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	processes, %rdi
	movq	%rax, -40(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-40(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB5_6
# BB#2:                                 #   in Loop: Header=BB5_1 Depth=1
	movq	processes, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	cmpq	-16(%rbp), %rax
	jne	.LBB5_4
# BB#3:
	movq	-32(%rbp), %rax
	movl	$1, 264(%rax)
	movl	$0, -4(%rbp)
	jmp	.LBB5_7
.LBB5_4:                                #   in Loop: Header=BB5_1 Depth=1
	jmp	.LBB5_5
.LBB5_5:                                #   in Loop: Header=BB5_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB5_1
.LBB5_6:
	movl	$2, -4(%rbp)
.LBB5_7:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	SleepProcess, .Lfunc_end5-SleepProcess
	.cfi_endproc

	.globl	RegisterSignalHandler
	.align	16, 0x90
	.type	RegisterSignalHandler,@function
RegisterSignalHandler:                  # @RegisterSignalHandler
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
	subq	$64, %rsp
	movq	%rdi, -16(%rbp)
	movl	%esi, -20(%rbp)
	movq	%rdx, -32(%rbp)
	movq	$0, -40(%rbp)
.LBB6_1:                                # =>This Inner Loop Header: Depth=1
	movq	-40(%rbp), %rax
	movq	processes, %rdi
	movq	%rax, -64(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-64(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB6_12
# BB#2:                                 #   in Loop: Header=BB6_1 Depth=1
	movq	processes, %rdi
	movq	-40(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	movq	(%rax), %rax
	cmpq	-16(%rbp), %rax
	jne	.LBB6_10
# BB#3:
	movl	$0, -52(%rbp)
.LBB6_4:                                # =>This Inner Loop Header: Depth=1
	cmpl	$22, -52(%rbp)
	jge	.LBB6_9
# BB#5:                                 #   in Loop: Header=BB6_4 Depth=1
	movl	-20(%rbp), %eax
	movl	-52(%rbp), %ecx
                                        # kill: CL<def> ECX<kill>
	shrl	%cl, %eax
	andl	$1, %eax
	cmpl	$0, %eax
	je	.LBB6_7
# BB#6:                                 #   in Loop: Header=BB6_4 Depth=1
	movq	-32(%rbp), %rax
	movslq	-52(%rbp), %rcx
	movq	-48(%rbp), %rdx
	movq	%rax, 280(%rdx,%rcx,8)
.LBB6_7:                                #   in Loop: Header=BB6_4 Depth=1
	jmp	.LBB6_8
.LBB6_8:                                #   in Loop: Header=BB6_4 Depth=1
	movl	-52(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -52(%rbp)
	jmp	.LBB6_4
.LBB6_9:
	movl	$0, -4(%rbp)
	jmp	.LBB6_13
.LBB6_10:                               #   in Loop: Header=BB6_1 Depth=1
	jmp	.LBB6_11
.LBB6_11:                               #   in Loop: Header=BB6_1 Depth=1
	movq	-40(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -40(%rbp)
	jmp	.LBB6_1
.LBB6_12:
	movl	$2, -4(%rbp)
.LBB6_13:
	movl	-4(%rbp), %eax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	RegisterSignalHandler, .Lfunc_end6-RegisterSignalHandler
	.cfi_endproc

	.type	root,@object            # @root
	.local	root
	.comm	root,8,8
	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Root Process"
	.size	.L.str, 13

	.type	processes,@object       # @processes
	.local	processes
	.comm	processes,8,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
