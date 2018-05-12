/* extern "C" int initialize_fiber(fiber_t*); */
/* first arg in rdi */
/* second arg in rsi */
/* third arg in rdx */

.text
.globl initialize_fiber
.type initialize_fiber,@function
.align 16

initialize_fiber:
	popq	%rdx
	movq	%rbx,	0x00(%rdi)
	movq	%rbp,	0x08(%rdi)
	movq	%r12,	0x10(%rdi)
	movq	%r13,	0x18(%rdi)
	movq	%r14,	0x20(%rdi)
	movq	%r15,	0x28(%rdi)
	movq	%rdx,	0x30(%rdi)
	movq	%rsp,	0x38(%rdi)
	stmxcsr			0x40(%rdi)
	fnstcw			0x44(%rdi)
	xorq	%rax,	%rax
	jmpq			*%rdx
.size initialize_fiber,.-initialize_fiber

.section .note.GNU-stack,"",%progbits
