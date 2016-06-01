	.text
	.file	"debug_gfx.c"
	.globl	debug_gfx_writeLine
	.align	16, 0x90
	.type	debug_gfx_writeLine,@function
debug_gfx_writeLine:                    # @debug_gfx_writeLine
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
	subq	$736, %rsp              # imm = 0x2E0
	testb	%al, %al
	movaps	%xmm7, -544(%rbp)       # 16-byte Spill
	movaps	%xmm6, -560(%rbp)       # 16-byte Spill
	movaps	%xmm5, -576(%rbp)       # 16-byte Spill
	movaps	%xmm4, -592(%rbp)       # 16-byte Spill
	movaps	%xmm3, -608(%rbp)       # 16-byte Spill
	movaps	%xmm2, -624(%rbp)       # 16-byte Spill
	movaps	%xmm1, -640(%rbp)       # 16-byte Spill
	movaps	%xmm0, -656(%rbp)       # 16-byte Spill
	movq	%rdi, -664(%rbp)        # 8-byte Spill
	movq	%r9, -672(%rbp)         # 8-byte Spill
	movq	%r8, -680(%rbp)         # 8-byte Spill
	movq	%rcx, -688(%rbp)        # 8-byte Spill
	movq	%rdx, -696(%rbp)        # 8-byte Spill
	movq	%rsi, -704(%rbp)        # 8-byte Spill
	je	.LBB0_14
# BB#13:
	movaps	-656(%rbp), %xmm0       # 16-byte Reload
	movaps	%xmm0, -480(%rbp)
	movaps	-640(%rbp), %xmm1       # 16-byte Reload
	movaps	%xmm1, -464(%rbp)
	movaps	-624(%rbp), %xmm2       # 16-byte Reload
	movaps	%xmm2, -448(%rbp)
	movaps	-608(%rbp), %xmm3       # 16-byte Reload
	movaps	%xmm3, -432(%rbp)
	movaps	-592(%rbp), %xmm4       # 16-byte Reload
	movaps	%xmm4, -416(%rbp)
	movaps	-576(%rbp), %xmm5       # 16-byte Reload
	movaps	%xmm5, -400(%rbp)
	movaps	-560(%rbp), %xmm6       # 16-byte Reload
	movaps	%xmm6, -384(%rbp)
	movaps	-544(%rbp), %xmm7       # 16-byte Reload
	movaps	%xmm7, -368(%rbp)
.LBB0_14:
	movq	-672(%rbp), %rax        # 8-byte Reload
	movq	%rax, -488(%rbp)
	movq	-680(%rbp), %rcx        # 8-byte Reload
	movq	%rcx, -496(%rbp)
	movq	-688(%rbp), %rdx        # 8-byte Reload
	movq	%rdx, -504(%rbp)
	movq	-696(%rbp), %rsi        # 8-byte Reload
	movq	%rsi, -512(%rbp)
	movq	-704(%rbp), %rdi        # 8-byte Reload
	movq	%rdi, -520(%rbp)
	movq	-664(%rbp), %r8         # 8-byte Reload
	movq	%r8, -8(%rbp)
	movl	$256, %r9d              # imm = 0x100
	movl	%r9d, %edx
	leaq	-272(%rbp), %r8
	xorl	%esi, %esi
	movq	%r8, %rdi
	movq	%r8, -712(%rbp)         # 8-byte Spill
	callq	memset
	leaq	-528(%rbp), %rcx
	movq	%rcx, -288(%rbp)
	leaq	16(%rbp), %rcx
	movq	%rcx, -296(%rbp)
	movl	$48, -300(%rbp)
	movl	$8, -304(%rbp)
	movq	-8(%rbp), %rsi
	leaq	-304(%rbp), %rdx
	movq	-712(%rbp), %rdi        # 8-byte Reload
	movq	%rax, -720(%rbp)        # 8-byte Spill
	callq	vsnprintf
	movl	%eax, -724(%rbp)        # 4-byte Spill
	callq	GetBootInfo
	movq	%rax, -312(%rbp)
	movl	yPos, %r9d
	movl	%r9d, -316(%rbp)
	movl	xPos, %r9d
	movl	%r9d, -320(%rbp)
	movq	-312(%rbp), %rax
	movl	64(%rax), %r9d
	movl	%r9d, -324(%rbp)
	movq	-312(%rbp), %rax
	movq	56(%rax), %rax
	movq	%rax, -336(%rbp)
	movl	$0, -340(%rbp)
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_3 Depth 2
                                        #       Child Loop BB0_5 Depth 3
	movslq	-340(%rbp), %rax
	movsbl	-272(%rbp,%rax), %ecx
	cmpl	$0, %ecx
	je	.LBB0_12
# BB#2:                                 #   in Loop: Header=BB0_1 Depth=1
	movl	$0, -344(%rbp)
.LBB0_3:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB0_5 Depth 3
	cmpl	$8, -344(%rbp)
	jge	.LBB0_10
# BB#4:                                 #   in Loop: Header=BB0_3 Depth=2
	movl	-320(%rbp), %eax
	movl	%eax, -348(%rbp)
.LBB0_5:                                #   Parent Loop BB0_1 Depth=1
                                        #     Parent Loop BB0_3 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	movl	-348(%rbp), %eax
	movl	-320(%rbp), %ecx
	addl	$13, %ecx
	cmpl	%ecx, %eax
	jge	.LBB0_8
# BB#6:                                 #   in Loop: Header=BB0_5 Depth=3
	movl	$8, %eax
	movl	-348(%rbp), %ecx
	movl	-320(%rbp), %edx
	subl	%edx, %ecx
	movl	$13, %edx
	subl	%ecx, %edx
	movslq	%edx, %rsi
	movslq	-340(%rbp), %rdi
	movsbl	-272(%rbp,%rdi), %ecx
	addl	$-32, %ecx
	movslq	%ecx, %rdi
	imulq	$13, %rdi, %rdi
	leaq	letters(%rdi), %rdi
	movzbl	(%rdi,%rsi), %ecx
	movl	-344(%rbp), %edx
	movl	%ecx, -728(%rbp)        # 4-byte Spill
	movl	%edx, %ecx
                                        # kill: CL<def> ECX<kill>
	movl	-728(%rbp), %edx        # 4-byte Reload
	sarl	%cl, %edx
	andl	$1, %edx
	imull	$-572662307, %edx, %edx # imm = 0xFFFFFFFFDDDDDDDD
	movl	-316(%rbp), %r8d
	subl	-344(%rbp), %eax
	addl	%eax, %r8d
	movl	-348(%rbp), %eax
	imull	-324(%rbp), %eax
	addl	%eax, %r8d
	movslq	%r8d, %rsi
	movq	-336(%rbp), %rdi
	movl	%edx, (%rdi,%rsi,4)
# BB#7:                                 #   in Loop: Header=BB0_5 Depth=3
	movl	-348(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -348(%rbp)
	jmp	.LBB0_5
.LBB0_8:                                #   in Loop: Header=BB0_3 Depth=2
	movl	-316(%rbp), %eax
	addl	$8, %eax
	movl	%eax, -316(%rbp)
# BB#9:                                 #   in Loop: Header=BB0_3 Depth=2
	movl	-344(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -344(%rbp)
	jmp	.LBB0_3
.LBB0_10:                               #   in Loop: Header=BB0_1 Depth=1
	jmp	.LBB0_11
.LBB0_11:                               #   in Loop: Header=BB0_1 Depth=1
	movl	-340(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -340(%rbp)
	jmp	.LBB0_1
.LBB0_12:
	movl	-320(%rbp), %eax
	movl	%eax, xPos
	movl	-316(%rbp), %eax
	movl	%eax, yPos
	addq	$736, %rsp              # imm = 0x2E0
	popq	%rbp
	retq
.Lfunc_end0:
	.size	debug_gfx_writeLine, .Lfunc_end0-debug_gfx_writeLine
	.cfi_endproc

	.type	letters,@object         # @letters
	.data
	.globl	letters
	.align	16
letters:
	.zero	13
	.ascii	"\000\000\030\030\000\000\030\030\030\030\030\030\030"
	.ascii	"\000\000\000\000\000\000\000\000\0006666"
	.asciz	"\000\000\000ff\377ff\377ff\000"
	.ascii	"\000\000\030~\377\033\037~\370\330\377~\030"
	.ascii	"\000\000\016\033\333n0\030\fv\333\330p"
	.ascii	"\000\000\177\306\317\330pp\330\314\314l8"
	.ascii	"\000\000\000\000\000\000\000\000\000\030\034\f\016"
	.ascii	"\000\000\f\0300000000\030\f"
	.ascii	"\000\0000\030\f\f\f\f\f\f\f\0300"
	.asciz	"\000\000\000\000\231Z<\377<Z\231\000"
	.asciz	"\000\000\000\030\030\030\377\377\030\030\030\000"
	.asciz	"\000\0000\030\034\034\000\000\000\000\000\000"
	.asciz	"\000\000\000\000\000\000\377\377\000\000\000\000"
	.asciz	"\000\000\00088\000\000\000\000\000\000\000"
	.ascii	"\000``00\030\030\f\f\006\006\003\003"
	.ascii	"\000\000<f\303\343\363\333\317\307\303f<"
	.ascii	"\000\000~\030\030\030\030\030\030\030x8\030"
	.ascii	"\000\000\377\300\300`0\030\f\006\003\347~"
	.ascii	"\000\000~\347\003\003\007~\007\003\003\347~"
	.ascii	"\000\000\f\f\f\f\f\377\314l<\034\f"
	.ascii	"\000\000~\347\003\003\007\376\300\300\300\300\377"
	.ascii	"\000\000~\347\303\303\307\376\300\300\300\347~"
	.ascii	"\000\0000000\030\f\006\003\003\003\377"
	.ascii	"\000\000~\347\303\303\347~\347\303\303\347~"
	.ascii	"\000\000~\347\003\003\003\177\347\303\303\347~"
	.asciz	"\000\000\00088\000\00088\000\000\000"
	.asciz	"\000\0000\030\034\034\000\000\034\034\000\000"
	.ascii	"\000\000\006\f\0300`\300`0\030\f\006"
	.asciz	"\000\000\000\000\377\377\000\377\377\000\000\000"
	.ascii	"\000\000`0\030\f\006\003\006\f\0300`"
	.ascii	"\000\000\030\000\000\030\030\f\006\003\303\303~"
	.asciz	"\000\000?`\317\333\323\335\303~\000\000"
	.ascii	"\000\000\303\303\303\303\377\303\303\303f<\030"
	.ascii	"\000\000\376\307\303\303\307\376\307\303\303\307\376"
	.ascii	"\000\000~\347\300\300\300\300\300\300\300\347~"
	.ascii	"\000\000\374\316\307\303\303\303\303\303\307\316\374"
	.ascii	"\000\000\377\300\300\300\300\374\300\300\300\300\377"
	.ascii	"\000\000\300\300\300\300\300\300\374\300\300\300\377"
	.ascii	"\000\000~\347\303\303\317\300\300\300\300\347~"
	.ascii	"\000\000\303\303\303\303\303\377\303\303\303\303\303"
	.ascii	"\000\000~\030\030\030\030\030\030\030\030\030~"
	.ascii	"\000\000|\356\306\006\006\006\006\006\006\006\006"
	.ascii	"\000\000\303\306\314\330\360\340\360\330\314\306\303"
	.ascii	"\000\000\377\300\300\300\300\300\300\300\300\300\300"
	.ascii	"\000\000\303\303\303\303\303\303\333\377\377\347\303"
	.ascii	"\000\000\307\307\317\317\337\333\373\363\363\343\343"
	.ascii	"\000\000~\347\303\303\303\303\303\303\303\347~"
	.ascii	"\000\000\300\300\300\300\300\376\307\303\303\307\376"
	.ascii	"\000\000?n\337\333\303\303\303\303\303f<"
	.ascii	"\000\000\303\306\314\330\360\376\307\303\303\307\376"
	.ascii	"\000\000~\347\003\003\007~\340\300\300\347~"
	.ascii	"\000\000\030\030\030\030\030\030\030\030\030\030\377"
	.ascii	"\000\000~\347\303\303\303\303\303\303\303\303\303"
	.ascii	"\000\000\030<<ff\303\303\303\303\303\303"
	.ascii	"\000\000\303\347\377\377\333\333\303\303\303\303\303"
	.ascii	"\000\000\303ff<<\030<<ff\303"
	.ascii	"\000\000\030\030\030\030\030\030<<ff\303"
	.ascii	"\000\000\377\300\300`0~\f\006\003\003\377"
	.ascii	"\000\000<000000000<"
	.ascii	"\000\003\003\006\006\f\f\030\03000``"
	.ascii	"\000\000<\f\f\f\f\f\f\f\f\f<"
	.ascii	"\000\000\000\000\000\000\000\000\000\303f<\030"
	.asciz	"\377\377\000\000\000\000\000\000\000\000\000\000"
	.ascii	"\000\000\000\000\000\000\000\000\000\03080p"
	.asciz	"\000\000\177\303\303\177\003\303~\000\000\000"
	.ascii	"\000\000\376\303\303\303\303\376\300\300\300\300\300"
	.asciz	"\000\000~\303\300\300\300\303~\000\000\000"
	.ascii	"\000\000\177\303\303\303\303\177\003\003\003\003\003"
	.asciz	"\000\000\177\300\300\376\303\303~\000\000\000"
	.ascii	"\000\00000000\3740003\036"
	.asciz	"~\303\003\003\177\303\303\303~\000\000\000"
	.ascii	"\000\000\303\303\303\303\303\303\376\300\300\300\300"
	.asciz	"\000\000\030\030\030\030\030\030\030\000\000\030"
	.asciz	"8l\f\f\f\f\f\f\f\000\000\f"
	.ascii	"\000\000\306\314\370\360\330\314\306\300\300\300\300"
	.ascii	"\000\000~\030\030\030\030\030\030\030\030\030x"
	.asciz	"\000\000\333\333\333\333\333\333\376\000\000\000"
	.asciz	"\000\000\306\306\306\306\306\306\374\000\000\000"
	.asciz	"\000\000|\306\306\306\306\306|\000\000\000"
	.asciz	"\300\300\300\376\303\303\303\303\376\000\000\000"
	.asciz	"\003\003\003\177\303\303\303\303\177\000\000\000"
	.asciz	"\000\000\300\300\300\300\300\340\376\000\000\000"
	.asciz	"\000\000\376\003\003~\300\300\177\000\000\000"
	.asciz	"\000\000\03460000\374000"
	.asciz	"\000\000~\306\306\306\306\306\306\000\000\000"
	.asciz	"\000\000\030<<ff\303\303\000\000\000"
	.asciz	"\000\000\303\347\377\333\303\303\303\000\000\000"
	.asciz	"\000\000\303f<\030<f\303\000\000\000"
	.asciz	"\300``0\030<ff\303\000\000\000"
	.asciz	"\000\000\377`0\030\f\006\377\000\000\000"
	.ascii	"\000\000\017\030\030\0308\3608\030\030\030\017"
	.zero	13,24
	.ascii	"\000\000\360\030\030\030\034\017\034\030\030\030\360"
	.asciz	"\000\000\000\000\000\000\006\217\361`\000\000"
	.size	letters, 1235

	.type	yPos,@object            # @yPos
	.local	yPos
	.comm	yPos,4,4
	.type	xPos,@object            # @xPos
	.local	xPos
	.comm	xPos,4,4

	.ident	"clang version 3.7.0 (tags/RELEASE_370/final)"
	.section	".note.GNU-stack","",@progbits
