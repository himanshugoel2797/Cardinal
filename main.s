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
.Ltmp3:
	.size	kernel_main, .Ltmp3-kernel_main
	.cfi_endproc


	.ident	"clang version 3.5.0 (tags/RELEASE_350/final)"
	.section	".note.GNU-stack","",@progbits
