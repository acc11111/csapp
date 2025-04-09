	.file	"printf.c"
	.text
	.p2align 4
	.globl	__printf_chk
	.type	__printf_chk, @function
__printf_chk:
.LFB23:
	.cfi_startproc
	endbr64
	subq	$216, %rsp
	.cfi_def_cfa_offset 224
	movq	%rsi, %r10
	movq	%rdx, 48(%rsp)
	movq	%rcx, 56(%rsp)
	movq	%r8, 64(%rsp)
	movq	%r9, 72(%rsp)
	testb	%al, %al
	je	.L2
	movaps	%xmm0, 80(%rsp)
	movaps	%xmm1, 96(%rsp)
	movaps	%xmm2, 112(%rsp)
	movaps	%xmm3, 128(%rsp)
	movaps	%xmm4, 144(%rsp)
	movaps	%xmm5, 160(%rsp)
	movaps	%xmm6, 176(%rsp)
	movaps	%xmm7, 192(%rsp)
.L2:
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	leaq	224(%rsp), %rax
	movq	%r10, %rdx
	movq	%rsp, %rcx
	movq	%rax, 8(%rsp)
	leaq	32(%rsp), %rax
	movl	$1, %esi
	movq	%rax, 16(%rsp)
	movq	stdout@GOTPCREL(%rip), %rax
	movl	$16, (%rsp)
	movq	(%rax), %rdi
	movl	$48, 4(%rsp)
	call	__vfprintf_chk@PLT
	movq	24(%rsp), %rdx
	subq	%fs:40, %rdx
	jne	.L6
	addq	$216, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L6:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE23:
	.size	__printf_chk, .-__printf_chk
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
