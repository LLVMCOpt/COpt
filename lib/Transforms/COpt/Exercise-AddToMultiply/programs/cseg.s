	.text
	.file	"cseg.c"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
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
	xorl	%eax, %eax
	movl	$12, %ecx
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$1, -24(%rbp)
	movl	%eax, -28(%rbp)         # 4-byte Spill
	movl	%ecx, %eax
	cltd
	idivl	-24(%rbp)
	movl	%eax, -20(%rbp)
	movl	%ecx, %eax
	cltd
	idivl	-24(%rbp)
	movl	%eax, -24(%rbp)
	movl	-28(%rbp), %eax         # 4-byte Reload
	popq	%rbp
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc


	.ident	"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"
	.section	".note.GNU-stack","",@progbits
