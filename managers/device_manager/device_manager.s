	.text
	.file	"managers/device_manager/device_manager.c"
	.globl	DeviceManager_Initialize
	.align	16, 0x90
	.type	DeviceManager_Initialize,@function
DeviceManager_Initialize:               # @DeviceManager_Initialize
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
	movq	%rax, buses
	popq	%rbp
	retq
.Lfunc_end0:
	.size	DeviceManager_Initialize, .Lfunc_end0-DeviceManager_Initialize
	.cfi_endproc

	.globl	RegisterBus
	.align	16, 0x90
	.type	RegisterBus,@function
RegisterBus:                            # @RegisterBus
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
	subq	$112, %rsp
	movq	%rdi, -16(%rbp)
	movq	$0, -24(%rbp)
.LBB1_1:                                # =>This Inner Loop Header: Depth=1
	movq	-24(%rbp), %rax
	movq	buses, %rdi
	movq	%rax, -48(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-48(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB1_6
# BB#2:                                 #   in Loop: Header=BB1_1 Depth=1
	movq	buses, %rdi
	movq	-24(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-32(%rbp), %rax
	movq	%rdi, -56(%rbp)         # 8-byte Spill
	movq	%rax, %rdi
	movq	%rsi, -64(%rbp)         # 8-byte Spill
	callq	strlen
	movq	-56(%rbp), %rdi         # 8-byte Reload
	movq	-64(%rbp), %rsi         # 8-byte Reload
	movq	%rax, %rdx
	callq	strncmp
	cmpl	$0, %eax
	jne	.LBB1_4
# BB#3:
	movl	$5, -4(%rbp)
	jmp	.LBB1_13
.LBB1_4:                                #   in Loop: Header=BB1_1 Depth=1
	jmp	.LBB1_5
.LBB1_5:                                #   in Loop: Header=BB1_1 Depth=1
	movq	-24(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -24(%rbp)
	jmp	.LBB1_1
.LBB1_6:
	movl	$264, %eax              # imm = 0x108
	movl	%eax, %ecx
	movl	$264, %eax              # imm = 0x108
	movl	%eax, %edi
	movq	%rcx, -72(%rbp)         # 8-byte Spill
	callq	kmalloc
	movq	%rax, -40(%rbp)
	cmpq	$0, -40(%rbp)
	jne	.LBB1_8
# BB#7:
	movl	$2, -4(%rbp)
	jmp	.LBB1_13
.LBB1_8:
	movq	-40(%rbp), %rdi
	movq	-16(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rdi, -80(%rbp)         # 8-byte Spill
	movq	%rax, %rdi
	movq	%rdx, -88(%rbp)         # 8-byte Spill
	callq	strlen
	movl	$256, %ecx              # imm = 0x100
	movl	%ecx, %edx
	movl	$256, %ecx              # imm = 0x100
	movl	%ecx, %esi
	movq	-80(%rbp), %rdi         # 8-byte Reload
	movq	-88(%rbp), %r8          # 8-byte Reload
	movq	%rdx, -96(%rbp)         # 8-byte Spill
	movq	%r8, %rdx
	movq	%rax, %rcx
	callq	strcpy_s
	movq	%rax, -104(%rbp)        # 8-byte Spill
	callq	List_Create
	movq	-40(%rbp), %rcx
	movq	%rax, 256(%rcx)
	movq	-40(%rbp), %rax
	cmpq	$0, 256(%rax)
	jne	.LBB1_10
# BB#9:
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	callq	kfree
	movl	$2, -4(%rbp)
	jmp	.LBB1_13
.LBB1_10:
	movq	buses, %rdi
	movq	-40(%rbp), %rax
	movq	%rax, %rsi
	callq	List_AddEntry
	cmpl	$0, %eax
	je	.LBB1_12
# BB#11:
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	callq	kfree
	movl	$2, -4(%rbp)
	jmp	.LBB1_13
.LBB1_12:
	movl	$0, -4(%rbp)
.LBB1_13:
	movl	-4(%rbp), %eax
	addq	$112, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	RegisterBus, .Lfunc_end1-RegisterBus
	.cfi_endproc

	.globl	RegisterDevice
	.align	16, 0x90
	.type	RegisterDevice,@function
RegisterDevice:                         # @RegisterDevice
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
	subq	$176, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movq	%rcx, -40(%rbp)
	cmpq	$0, -40(%rbp)
	jne	.LBB2_2
# BB#1:
	movl	$3, -4(%rbp)
	jmp	.LBB2_19
.LBB2_2:
	movq	$0, -48(%rbp)
.LBB2_3:                                # =>This Inner Loop Header: Depth=1
	movq	-48(%rbp), %rax
	movq	buses, %rdi
	movq	%rax, -88(%rbp)         # 8-byte Spill
	callq	List_Length
	movq	-88(%rbp), %rdi         # 8-byte Reload
	cmpq	%rax, %rdi
	jae	.LBB2_18
# BB#4:                                 #   in Loop: Header=BB2_3 Depth=1
	movq	buses, %rdi
	movq	-48(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rdi
	movq	-24(%rbp), %rsi
	movq	-56(%rbp), %rax
	movq	%rdi, -96(%rbp)         # 8-byte Spill
	movq	%rax, %rdi
	movq	%rsi, -104(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-96(%rbp), %rdi         # 8-byte Reload
	movq	-104(%rbp), %rsi        # 8-byte Reload
	movq	%rax, %rdx
	callq	strncmp
	cmpl	$0, %eax
	jne	.LBB2_16
# BB#5:
	movq	$0, -64(%rbp)
.LBB2_6:                                # =>This Inner Loop Header: Depth=1
	movq	-64(%rbp), %rax
	movq	-56(%rbp), %rcx
	movq	256(%rcx), %rdi
	movq	%rax, -112(%rbp)        # 8-byte Spill
	callq	List_Length
	movq	-112(%rbp), %rcx        # 8-byte Reload
	cmpq	%rax, %rcx
	jae	.LBB2_11
# BB#7:                                 #   in Loop: Header=BB2_6 Depth=1
	movq	-56(%rbp), %rax
	movq	256(%rax), %rdi
	movq	-64(%rbp), %rsi
	callq	List_EntryAt
	movq	%rax, -72(%rbp)
	movq	-72(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-72(%rbp), %rax
	movq	%rdi, -120(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	%rsi, -128(%rbp)        # 8-byte Spill
	callq	strlen
	movq	-120(%rbp), %rdi        # 8-byte Reload
	movq	-128(%rbp), %rsi        # 8-byte Reload
	movq	%rax, %rdx
	callq	strncmp
	cmpl	$0, %eax
	jne	.LBB2_9
# BB#8:
	movl	$4, -4(%rbp)
	jmp	.LBB2_19
.LBB2_9:                                #   in Loop: Header=BB2_6 Depth=1
	jmp	.LBB2_10
.LBB2_10:                               #   in Loop: Header=BB2_6 Depth=1
	movq	-64(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -64(%rbp)
	jmp	.LBB2_6
.LBB2_11:
	movl	$272, %eax              # imm = 0x110
	movl	%eax, %ecx
	movl	$272, %eax              # imm = 0x110
	movl	%eax, %edi
	movq	%rcx, -136(%rbp)        # 8-byte Spill
	callq	kmalloc
	movq	%rax, -80(%rbp)
	cmpq	$0, -80(%rbp)
	jne	.LBB2_13
# BB#12:
	movl	$2, -4(%rbp)
	jmp	.LBB2_19
.LBB2_13:
	movq	-80(%rbp), %rdi
	movq	-16(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rdi, -144(%rbp)        # 8-byte Spill
	movq	%rax, %rdi
	movq	%rdx, -152(%rbp)        # 8-byte Spill
	callq	strlen
	movl	$256, %ecx              # imm = 0x100
	movl	%ecx, %edx
	movl	$256, %ecx              # imm = 0x100
	movl	%ecx, %esi
	movq	-144(%rbp), %rdi        # 8-byte Reload
	movq	-152(%rbp), %r8         # 8-byte Reload
	movq	%rdx, -160(%rbp)        # 8-byte Spill
	movq	%r8, %rdx
	movq	%rax, %rcx
	callq	strcpy_s
	movl	-28(%rbp), %r9d
	movq	-80(%rbp), %rcx
	movl	%r9d, 256(%rcx)
	movq	-40(%rbp), %rcx
	movq	-80(%rbp), %rdx
	movq	%rcx, 264(%rdx)
	movq	-56(%rbp), %rcx
	movq	256(%rcx), %rdi
	movq	-80(%rbp), %rcx
	movq	%rcx, %rsi
	movq	%rax, -168(%rbp)        # 8-byte Spill
	callq	List_AddEntry
	cmpl	$0, %eax
	je	.LBB2_15
# BB#14:
	movq	-80(%rbp), %rax
	movq	%rax, %rdi
	callq	kfree
	movl	$2, -4(%rbp)
	jmp	.LBB2_19
.LBB2_15:
	movl	$0, -4(%rbp)
	jmp	.LBB2_19
.LBB2_16:                               #   in Loop: Header=BB2_3 Depth=1
	jmp	.LBB2_17
.LBB2_17:                               #   in Loop: Header=BB2_3 Depth=1
	movq	-48(%rbp), %rax
	addq	$1, %rax
	movq	%rax, -48(%rbp)
	jmp	.LBB2_3
.LBB2_18:
	movl	$1, -4(%rbp)
.LBB2_19:
	movl	-4(%rbp), %eax
	addq	$176, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	RegisterDevice, .Lfunc_end2-RegisterDevice
	.cfi_endproc

	.type	buses,@object           # @buses
	.comm	buses,8,8

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
