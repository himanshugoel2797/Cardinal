	.text
	.file	"drivers/usb/uhci/uhci_controller.c"
	.globl	UHCI_ControllerAdded
	.align	16, 0x90
	.type	UHCI_ControllerAdded,@function
UHCI_ControllerAdded:                   # @UHCI_ControllerAdded
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
	subq	$24, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	$0, -8(%rbp)
	movq	$0, -16(%rbp)
	movq	$0, -24(%rbp)
	addq	$24, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	UHCI_ControllerAdded, .Lfunc_end0-UHCI_ControllerAdded
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
