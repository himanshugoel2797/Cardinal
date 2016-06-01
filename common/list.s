	.text
	.file	"common/list.c"
	.globl	List_Create
	.align	16, 0x90
	.type	List_Create,@function
List_Create:                            # @List_Create
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
	movl	$40, %eax
	movl	%eax, %ecx
	movl	$40, %eax
	movl	%eax, %edi
	movq	%rcx, -16(%rbp)         # 8-byte Spill
	callq	kmalloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, (%rax)
	movq	-8(%rbp), %rax
	movq	$0, 8(%rax)
	movq	-8(%rbp), %rax
	movq	$0, 16(%rax)
	movq	-8(%rbp), %rax
	movq	$0, 24(%rax)
	movq	-8(%rbp), %rax
	movq	$0, 32(%rax)
	movq	-8(%rbp), %rax
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	List_Create, .Lfunc_end0-List_Create
	.cfi_endproc

	.globl	List_AddEntry
	.align	16, 0x90
	.type	List_AddEntry,@function
List_AddEntry:                          # @List_AddEntry
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
	movl	$24, %eax
	movl	%eax, %ecx
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movl	$24, %eax
	movl	%eax, %edi
	movq	%rcx, -40(%rbp)         # 8-byte Spill
	callq	kmalloc
	movq	%rax, -32(%rbp)
	cmpq	$0, -32(%rbp)
	jne	.LBB1_2
# BB#1:
	movl	$1, -4(%rbp)
	jmp	.LBB1_7
.LBB1_2:
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movq	-32(%rbp), %rcx
	movq	%rax, 16(%rcx)
	movq	-24(%rbp), %rax
	movq	-32(%rbp), %rcx
	movq	%rax, (%rcx)
	movq	-32(%rbp), %rax
	movq	-16(%rbp), %rcx
	movq	%rax, 8(%rcx)
	movq	-16(%rbp), %rax
	cmpq	$0, (%rax)
	jne	.LBB1_4
# BB#3:
	movq	-16(%rbp), %rax
	movq	8(%rax), %rax
	movq	-16(%rbp), %rcx
	movq	%rax, (%rcx)
.LBB1_4:
	movq	-16(%rbp), %rax
	cmpq	$0, 16(%rax)
	jne	.LBB1_6
# BB#5:
	movq	-16(%rbp), %rax
	movq	(%rax), %rax
	movq	-16(%rbp), %rcx
	movq	%rax, 16(%rcx)
.LBB1_6:
	movq	-16(%rbp), %rax
	movq	32(%rax), %rcx
	addq	$1, %rcx
	movq	%rcx, 32(%rax)
	movl	$0, -4(%rbp)
.LBB1_7:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
.Lfunc_end1:
	.size	List_AddEntry, .Lfunc_end1-List_AddEntry
	.cfi_endproc

	.globl	List_Length
	.align	16, 0x90
	.type	List_Length,@function
List_Length:                            # @List_Length
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
	pushq	%rax
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movq	32(%rdi), %rax
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end2:
	.size	List_Length, .Lfunc_end2-List_Length
	.cfi_endproc

	.globl	List_Remove
	.align	16, 0x90
	.type	List_Remove,@function
List_Remove:                            # @List_Remove
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
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rsi
	cmpq	$0, 32(%rsi)
	jne	.LBB3_2
# BB#1:
	jmp	.LBB3_11
.LBB3_2:
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	callq	List_EntryAt
	movq	-8(%rbp), %rsi
	movq	16(%rsi), %rsi
	movq	%rsi, -24(%rbp)
	movq	-8(%rbp), %rsi
	movq	16(%rsi), %rsi
	cmpq	$0, 16(%rsi)
	movq	%rax, -32(%rbp)         # 8-byte Spill
	je	.LBB3_4
# BB#3:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	8(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	16(%rcx), %rcx
	movq	16(%rcx), %rcx
	movq	%rax, 8(%rcx)
.LBB3_4:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	cmpq	$0, 8(%rax)
	je	.LBB3_6
# BB#5:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	16(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	16(%rcx), %rcx
	movq	8(%rcx), %rcx
	movq	%rax, 16(%rcx)
.LBB3_6:
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	16(%rcx), %rax
	jne	.LBB3_8
# BB#7:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	8(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	%rax, (%rcx)
.LBB3_8:
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	movq	-8(%rbp), %rcx
	cmpq	16(%rcx), %rax
	jne	.LBB3_10
# BB#9:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	16(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	%rax, 8(%rcx)
.LBB3_10:
	movq	-8(%rbp), %rax
	movq	(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	%rax, 16(%rcx)
	movq	-24(%rbp), %rdi
	callq	kfree
	movq	-8(%rbp), %rax
	movq	32(%rax), %rcx
	addq	$-1, %rcx
	movq	%rcx, 32(%rax)
.LBB3_11:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end3:
	.size	List_Remove, .Lfunc_end3-List_Remove
	.cfi_endproc

	.globl	List_EntryAt
	.align	16, 0x90
	.type	List_EntryAt,@function
List_EntryAt:                           # @List_EntryAt
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
	subq	$24, %rsp
	movq	%rdi, -16(%rbp)
	movq	%rsi, -24(%rbp)
	movq	-24(%rbp), %rsi
	movq	-16(%rbp), %rdi
	cmpq	32(%rdi), %rsi
	jb	.LBB4_2
# BB#1:
	movq	$0, -8(%rbp)
	jmp	.LBB4_15
.LBB4_2:
	movq	-16(%rbp), %rax
	movq	24(%rax), %rax
	cmpq	-24(%rbp), %rax
	jbe	.LBB4_7
# BB#3:
	jmp	.LBB4_4
.LBB4_4:                                # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	movq	24(%rax), %rcx
	movq	%rcx, %rdx
	addq	$-1, %rdx
	movq	%rdx, 24(%rax)
	cmpq	-24(%rbp), %rcx
	jbe	.LBB4_6
# BB#5:                                 #   in Loop: Header=BB4_4 Depth=1
	movq	-16(%rbp), %rax
	movq	16(%rax), %rax
	movq	16(%rax), %rax
	movq	-16(%rbp), %rcx
	movq	%rax, 16(%rcx)
	jmp	.LBB4_4
.LBB4_6:
	jmp	.LBB4_13
.LBB4_7:
	movq	-16(%rbp), %rax
	movq	24(%rax), %rax
	cmpq	-24(%rbp), %rax
	jae	.LBB4_12
# BB#8:
	jmp	.LBB4_9
.LBB4_9:                                # =>This Inner Loop Header: Depth=1
	movq	-16(%rbp), %rax
	movq	24(%rax), %rcx
	movq	%rcx, %rdx
	addq	$1, %rdx
	movq	%rdx, 24(%rax)
	cmpq	-24(%rbp), %rcx
	jbe	.LBB4_11
# BB#10:                                #   in Loop: Header=BB4_9 Depth=1
	movq	-16(%rbp), %rax
	movq	16(%rax), %rax
	movq	8(%rax), %rax
	movq	-16(%rbp), %rcx
	movq	%rax, 16(%rcx)
	jmp	.LBB4_9
.LBB4_11:
	jmp	.LBB4_12
.LBB4_12:
	jmp	.LBB4_13
.LBB4_13:
	jmp	.LBB4_14
.LBB4_14:
	movq	-16(%rbp), %rax
	movq	16(%rax), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
.LBB4_15:
	movq	-8(%rbp), %rax
	addq	$24, %rsp
	popq	%rbp
	retq
.Lfunc_end4:
	.size	List_EntryAt, .Lfunc_end4-List_EntryAt
	.cfi_endproc

	.globl	List_Free
	.align	16, 0x90
	.type	List_Free,@function
List_Free:                              # @List_Free
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
.LBB5_1:                                # =>This Inner Loop Header: Depth=1
	movq	-8(%rbp), %rdi
	callq	List_Length
	cmpq	$0, %rax
	jbe	.LBB5_3
# BB#2:                                 #   in Loop: Header=BB5_1 Depth=1
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	-8(%rbp), %rdi
	xorl	%eax, %eax
	movl	%eax, %esi
	movq	%rcx, -16(%rbp)         # 8-byte Spill
	callq	List_Remove
	jmp	.LBB5_1
.LBB5_3:
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	callq	kfree
	addq	$16, %rsp
	popq	%rbp
	retq
.Lfunc_end5:
	.size	List_Free, .Lfunc_end5-List_Free
	.cfi_endproc

	.globl	List_Next
	.align	16, 0x90
	.type	List_Next,@function
List_Next:                              # @List_Next
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
	pushq	%rax
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	movq	24(%rdi), %rdi
	movq	-8(%rbp), %rax
	movq	32(%rax), %rax
	subq	$1, %rax
	cmpq	%rax, %rdi
	jae	.LBB6_2
# BB#1:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rcx
	addq	$24, %rcx
	movq	%rcx, 16(%rax)
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	8(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	%rax, 16(%rcx)
.LBB6_2:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	(%rax), %rax
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end6:
	.size	List_Next, .Lfunc_end6-List_Next
	.cfi_endproc

	.globl	List_Prev
	.align	16, 0x90
	.type	List_Prev,@function
List_Prev:                              # @List_Prev
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
	pushq	%rax
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	cmpq	$0, 24(%rdi)
	jbe	.LBB7_2
# BB#1:
	movq	-8(%rbp), %rax
	movq	24(%rax), %rcx
	addq	$-1, %rcx
	movq	%rcx, 24(%rax)
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	16(%rax), %rax
	movq	-8(%rbp), %rcx
	movq	%rax, 16(%rcx)
.LBB7_2:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	(%rax), %rax
	addq	$8, %rsp
	popq	%rbp
	retq
.Lfunc_end7:
	.size	List_Prev, .Lfunc_end7-List_Prev
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
