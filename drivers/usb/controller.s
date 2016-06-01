	.text
	.file	"drivers/usb/controller.c"
	.globl	USB_AddController
	.align	16, 0x90
	.type	USB_AddController,@function
USB_AddController:                      # @USB_AddController
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
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movl	-4(%rbp), %edi
	testl	%edi, %edi
	jne	.LBB0_2
	jmp	.LBB0_1
.LBB0_1:
	movq	-16(%rbp), %rdi
	movq	-24(%rbp), %rsi
	movq	-32(%rbp), %rdx
	callq	UHCI_ControllerAdded
	jmp	.LBB0_3
.LBB0_2:
	jmp	.LBB0_3
.LBB0_3:
	addq	$32, %rsp
	popq	%rbp
	retq
.Lfunc_end0:
	.size	USB_AddController, .Lfunc_end0-USB_AddController
	.cfi_endproc


	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
