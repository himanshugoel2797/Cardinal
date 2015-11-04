	.text
	.file	"main.c"
	.globl	kernel_main
	.align	16, 0x90
	.type	kernel_main,@function
kernel_main:                            # @kernel_main
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
	popq	%rbp
	retq
.Lfunc_end0:
	.size	kernel_main, .Lfunc_end0-kernel_main
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
