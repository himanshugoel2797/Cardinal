	.text
	.file	"hal/memory.c"
	.globl	GetVirtualAddress
	.align	16, 0x90
	.type	GetVirtualAddress,@function
GetVirtualAddress:                      # @GetVirtualAddress
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
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	-4(%rbp), %edi
	movq	-16(%rbp), %rsi
	callq	VirtMemMan_GetVirtualAddress
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	GetVirtualAddress, .Lfunc_end0-GetVirtualAddress
	.cfi_endproc

	.globl	GetPhysicalAddress
	.align	16, 0x90
	.type	GetPhysicalAddress,@function
GetPhysicalAddress:                     # @GetPhysicalAddress
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
	movq	%rdi, -8(%rbp)
	callq	GetActiveVirtualMemoryInstance
	movq	-8(%rbp), %rsi
	movq	%rax, %rdi
	callq	GetPhysicalAddressUID
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	GetPhysicalAddress, .Lfunc_end1-GetPhysicalAddress
	.cfi_endproc

	.globl	GetPhysicalAddressUID
	.align	16, 0x90
	.type	GetPhysicalAddressUID,@function
GetPhysicalAddressUID:                  # @GetPhysicalAddressUID
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
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	callq	VirtMemMan_GetPhysicalAddress
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	GetPhysicalAddressUID, .Lfunc_end2-GetPhysicalAddressUID
	.cfi_endproc

	.globl	GetActiveVirtualMemoryInstance
	.align	16, 0x90
	.type	GetActiveVirtualMemoryInstance,@function
GetActiveVirtualMemoryInstance:         # @GetActiveVirtualMemoryInstance
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
	callq	VirtMemMan_GetCurrent
	popq	%rbp
	retq
.Lfunc_end3:
	.size	GetActiveVirtualMemoryInstance, .Lfunc_end3-GetActiveVirtualMemoryInstance
	.cfi_endproc

	.globl	CreateVirtualMemoryInstance
	.align	16, 0x90
	.type	CreateVirtualMemoryInstance,@function
CreateVirtualMemoryInstance:            # @CreateVirtualMemoryInstance
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
	movq	%rdi, -16(%rbp)
	cmpq	$0, -16(%rbp)
	je	.LBB4_2
# BB#1:
	callq	VirtMemMan_CreateInstance
	movq	-16(%rbp), %rcx
	movq	%rax, (%rcx)
	movl	$0, -4(%rbp)
	jmp	.LBB4_3
.LBB4_2:
	movl	$64, -4(%rbp)
.LBB4_3:
	movl	-4(%rbp), %eax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	CreateVirtualMemoryInstance, .Lfunc_end4-CreateVirtualMemoryInstance
	.cfi_endproc

	.globl	SetActiveVirtualMemoryInstance
	.align	16, 0x90
	.type	SetActiveVirtualMemoryInstance,@function
SetActiveVirtualMemoryInstance:         # @SetActiveVirtualMemoryInstance
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
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	callq	VirtMemMan_SetCurrent
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	SetActiveVirtualMemoryInstance, .Lfunc_end5-SetActiveVirtualMemoryInstance
	.cfi_endproc

	.globl	MapPage
	.align	16, 0x90
	.type	MapPage,@function
MapPage:                                # @MapPage
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
	subq	$96, %rsp
	movl	24(%rbp), %eax
	movl	16(%rbp), %r10d
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	%rcx, -40(%rbp)
	movq	%r8, -48(%rbp)
	movl	%r9d, -52(%rbp)
	movl	%r10d, -56(%rbp)
	movl	%eax, -60(%rbp)
	movl	$0, -64(%rbp)
	cmpl	$1, -52(%rbp)
	jne	.LBB6_2
# BB#1:
	movl	$2, -64(%rbp)
	jmp	.LBB6_9
.LBB6_2:
	cmpl	$0, -52(%rbp)
	jne	.LBB6_4
# BB#3:
	movl	$0, -64(%rbp)
	jmp	.LBB6_8
.LBB6_4:
	cmpl	$2, -52(%rbp)
	jne	.LBB6_6
# BB#5:
	movl	$1, -64(%rbp)
	jmp	.LBB6_7
.LBB6_6:
	movl	$8, -4(%rbp)
	jmp	.LBB6_22
.LBB6_7:
	jmp	.LBB6_8
.LBB6_8:
	jmp	.LBB6_9
.LBB6_9:
	movl	$0, -68(%rbp)
	movl	-60(%rbp), %eax
	andl	$2, %eax
	cmpl	$0, %eax
	je	.LBB6_11
# BB#10:
	movl	-68(%rbp), %eax
	orl	$1, %eax
	movl	%eax, -68(%rbp)
.LBB6_11:
	movl	-60(%rbp), %eax
	andl	$4, %eax
	cmpl	$0, %eax
	je	.LBB6_13
# BB#12:
	movl	-68(%rbp), %eax
	orl	$2, %eax
	movl	%eax, -68(%rbp)
.LBB6_13:
	movl	$0, -72(%rbp)
	movl	-60(%rbp), %eax
	andl	$0, %eax
	cmpl	$0, %eax
	jne	.LBB6_15
# BB#14:
	movl	-72(%rbp), %eax
	movl	%eax, -72(%rbp)
.LBB6_15:
	movl	-60(%rbp), %eax
	andl	$8, %eax
	cmpl	$8, %eax
	jne	.LBB6_17
# BB#16:
	movl	-72(%rbp), %eax
	orl	$1, %eax
	movl	%eax, -72(%rbp)
.LBB6_17:
	cmpq	$0, -24(%rbp)
	je	.LBB6_19
# BB#18:
	movl	-52(%rbp), %eax
	movq	-24(%rbp), %rcx
	movl	%eax, (%rcx)
	movq	-40(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	%rcx, 8(%rdx)
	movq	-48(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	%rcx, 16(%rdx)
	movl	-60(%rbp), %eax
	movq	-24(%rbp), %rcx
	movl	%eax, 24(%rcx)
	movl	-56(%rbp), %eax
	movq	-24(%rbp), %rcx
	movl	%eax, 28(%rcx)
	movq	-24(%rbp), %rcx
	movq	$0, 32(%rcx)
	movq	-24(%rbp), %rcx
	movq	$0, 40(%rcx)
.LBB6_19:
	movl	-56(%rbp), %eax
	andl	$4, %eax
	cmpl	$0, %eax
	je	.LBB6_21
# BB#20:
	movl	-68(%rbp), %eax
	andl	$-3, %eax
	movl	%eax, -68(%rbp)
.LBB6_21:
	movl	$1, %eax
	movq	-16(%rbp), %rdi
	movq	-40(%rbp), %rsi
	movq	-32(%rbp), %rdx
	movq	-48(%rbp), %rcx
	movl	-64(%rbp), %r9d
	movl	-68(%rbp), %r8d
	movl	-72(%rbp), %r10d
	movq	%rsp, %r11
	movl	%r10d, 8(%r11)
	movl	%r8d, (%r11)
	movl	$1, %r8d
	movl	%eax, -76(%rbp)         # 4-byte Spill
	callq	VirtMemMan_Map
	movl	$0, -4(%rbp)
.LBB6_22:
	movl	-4(%rbp), %eax
	addq	$96, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	MapPage, .Lfunc_end6-MapPage
	.cfi_endproc

	.globl	UnmapPage
	.align	16, 0x90
	.type	UnmapPage,@function
UnmapPage:                              # @UnmapPage
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
	subq	$64, %rsp
	xorl	%eax, %eax
	movl	%eax, %ecx
	xorl	%eax, %eax
	movl	$2, %r8d
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movq	%rsp, %r9
	movl	$0, 8(%r9)
	movl	$0, (%r9)
	xorl	%r10d, %r10d
	movl	%r10d, %r9d
	movl	$2, %r11d
	movq	%rdx, -32(%rbp)         # 8-byte Spill
	movq	%r9, %rdx
	movq	-32(%rbp), %r9          # 8-byte Reload
	movq	%rcx, -40(%rbp)         # 8-byte Spill
	movq	%r9, %rcx
	movl	%r8d, -44(%rbp)         # 4-byte Spill
	movl	%r10d, %r8d
	movl	%r11d, %r9d
	movl	%eax, -48(%rbp)         # 4-byte Spill
	callq	VirtMemMan_Map
	xorl	%eax, %eax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end7:
	.size	UnmapPage, .Lfunc_end7-UnmapPage
	.cfi_endproc

	.globl	FindFreeVirtualAddress
	.align	16, 0x90
	.type	FindFreeVirtualAddress,@function
FindFreeVirtualAddress:                 # @FindFreeVirtualAddress
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
	subq	$64, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movl	%ecx, -36(%rbp)
	movl	%r8d, -40(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.LBB8_2
# BB#1:
	movl	$64, -4(%rbp)
	jmp	.LBB8_9
.LBB8_2:
	movl	$0, -44(%rbp)
	movl	-40(%rbp), %eax
	andl	$0, %eax
	cmpl	$0, %eax
	je	.LBB8_4
# BB#3:
	movl	-44(%rbp), %eax
	movl	%eax, -44(%rbp)
.LBB8_4:
	movl	-40(%rbp), %eax
	andl	$8, %eax
	cmpl	$0, %eax
	je	.LBB8_6
# BB#5:
	movl	-44(%rbp), %eax
	orl	$1, %eax
	movl	%eax, -44(%rbp)
.LBB8_6:
	movq	-16(%rbp), %rdi
	movq	-32(%rbp), %rsi
	movl	-44(%rbp), %edx
	callq	VirtMemMan_FindFreeAddress
	movq	%rax, -56(%rbp)
	cmpq	$0, -56(%rbp)
	je	.LBB8_8
# BB#7:
	movq	-56(%rbp), %rax
	movq	-24(%rbp), %rcx
	movq	%rax, (%rcx)
.LBB8_8:
	movl	$0, -4(%rbp)
.LBB8_9:
	movl	-4(%rbp), %eax
	addq	$64, %rsp
	popq	%rbp
	retq
.Lfunc_end8:
	.size	FindFreeVirtualAddress, .Lfunc_end8-FindFreeVirtualAddress
	.cfi_endproc

	.globl	ForkTable
	.align	16, 0x90
	.type	ForkTable,@function
ForkTable:                              # @ForkTable
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
	subq	$128, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	%rdx, -32(%rbp)
	movq	%rcx, -40(%rbp)
	cmpq	$0, -32(%rbp)
	jne	.LBB9_2
# BB#1:
	movl	$64, -4(%rbp)
	jmp	.LBB9_12
.LBB9_2:
	movq	-32(%rbp), %rdi
	callq	CreateVirtualMemoryInstance
	cmpq	$0, -40(%rbp)
	movl	%eax, -60(%rbp)         # 4-byte Spill
	jne	.LBB9_4
# BB#3:
	movl	$64, -4(%rbp)
	jmp	.LBB9_12
.LBB9_4:
	cmpq	$0, -24(%rbp)
	jne	.LBB9_6
# BB#5:
	movl	$64, -4(%rbp)
	jmp	.LBB9_12
.LBB9_6:
	movl	$56, %eax
	movl	%eax, %ecx
	movl	$56, %eax
	movl	%eax, %edi
	movq	%rcx, -72(%rbp)         # 8-byte Spill
	callq	kmalloc
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	movq	-40(%rbp), %rcx
	movq	%rax, (%rcx)
	movq	-24(%rbp), %rax
	movq	%rax, -56(%rbp)
.LBB9_7:                                # =>This Inner Loop Header: Depth=1
	movq	-32(%rbp), %rax
	movq	(%rax), %rdi
	movq	-48(%rbp), %rsi
	movq	-16(%rbp), %rax
	movq	-56(%rbp), %rcx
	movq	8(%rcx), %rcx
	movq	%rdi, -80(%rbp)         # 8-byte Spill
	movq	%rax, %rdi
	movq	%rsi, -88(%rbp)         # 8-byte Spill
	movq	%rcx, %rsi
	callq	GetPhysicalAddressUID
	movq	-56(%rbp), %rcx
	movq	8(%rcx), %rcx
	movq	-56(%rbp), %rsi
	movq	16(%rsi), %r8
	movq	-56(%rbp), %rsi
	movl	(%rsi), %r9d
	movq	-56(%rbp), %rsi
	movl	28(%rsi), %edx
	orl	$4, %edx
	movq	-56(%rbp), %rsi
	movl	24(%rsi), %r10d
	movq	%rsp, %rsi
	movl	%r10d, 8(%rsi)
	movl	%edx, (%rsi)
	movq	-80(%rbp), %rdi         # 8-byte Reload
	movq	-88(%rbp), %rsi         # 8-byte Reload
	movq	%rax, %rdx
	callq	MapPage
	movq	-56(%rbp), %rcx
	cmpq	$0, 48(%rcx)
	movl	%eax, -92(%rbp)         # 4-byte Spill
	je	.LBB9_9
# BB#8:                                 #   in Loop: Header=BB9_7 Depth=1
	movl	$56, %eax
	movl	%eax, %ecx
	movl	$56, %eax
	movl	%eax, %edi
	movq	%rcx, -104(%rbp)        # 8-byte Spill
	callq	kmalloc
	movq	-48(%rbp), %rcx
	movq	%rax, 48(%rcx)
	movq	-48(%rbp), %rax
	movq	48(%rax), %rax
	movq	%rax, -48(%rbp)
.LBB9_9:                                #   in Loop: Header=BB9_7 Depth=1
	jmp	.LBB9_10
.LBB9_10:                               #   in Loop: Header=BB9_7 Depth=1
	movq	-56(%rbp), %rax
	cmpq	$0, 48(%rax)
	jne	.LBB9_7
# BB#11:
	movl	$0, -4(%rbp)
.LBB9_12:
	movl	-4(%rbp), %eax
	addq	$128, %rsp
	popq	%rbp
	retq
.Lfunc_end9:
	.size	ForkTable, .Lfunc_end9-ForkTable
	.cfi_endproc

	.globl	AllocatePhysicalPage
	.align	16, 0x90
	.type	AllocatePhysicalPage,@function
AllocatePhysicalPage:                   # @AllocatePhysicalPage
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
	callq	MemMan_Alloc
	popq	%rbp
	retq
.Lfunc_end10:
	.size	AllocatePhysicalPage, .Lfunc_end10-AllocatePhysicalPage
	.cfi_endproc

	.globl	FreePhysicalPage
	.align	16, 0x90
	.type	FreePhysicalPage,@function
FreePhysicalPage:                       # @FreePhysicalPage
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
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	callq	MemMan_Free
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end11:
	.size	FreePhysicalPage, .Lfunc_end11-FreePhysicalPage
	.cfi_endproc

	.globl	AllocatePhysicalPageCont
	.align	16, 0x90
	.type	AllocatePhysicalPageCont,@function
AllocatePhysicalPageCont:               # @AllocatePhysicalPageCont
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
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %edi
	callq	MemMan_Alloc4KiBPageCont
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end12:
	.size	AllocatePhysicalPageCont, .Lfunc_end12-AllocatePhysicalPageCont
	.cfi_endproc

	.globl	FreePhysicalPageCont
	.align	16, 0x90
	.type	FreePhysicalPageCont,@function
FreePhysicalPageCont:                   # @FreePhysicalPageCont
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
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movq	-8(%rbp), %rdi
	movl	-12(%rbp), %esi
	callq	MemMan_FreeCont
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end13:
	.size	FreePhysicalPageCont, .Lfunc_end13-FreePhysicalPageCont
	.cfi_endproc

	.globl	AllocateAPLSMemory
	.align	16, 0x90
	.type	AllocateAPLSMemory,@function
AllocateAPLSMemory:                     # @AllocateAPLSMemory
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
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	callq	VirtMemMan_AllocCoreLocalData
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end14:
	.size	AllocateAPLSMemory, .Lfunc_end14-AllocateAPLSMemory
	.cfi_endproc

	.globl	GetCoreCount
	.align	16, 0x90
	.type	GetCoreCount,@function
GetCoreCount:                           # @GetCoreCount
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
	callq	SMP_GetCoreCount
	popq	%rbp
	retq
.Lfunc_end15:
	.size	GetCoreCount, .Lfunc_end15-GetCoreCount
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
