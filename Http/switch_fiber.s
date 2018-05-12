/* extern "C" __declspec(noreturn) void switch_fiber(fiber_t*, uintptr_t); */
/* first arg in rdi */
/* second arg in rsi */
/* third arg in rdx */

.text
.globl switch_fiber
.type switch_fiber,@function
.align 16

switch_fiber:
	movq	%rdi,		%rdx
	movq	%rsi,		%rax
	movq	0x00(%rdx),	%rbx
	movq	0x08(%rdx),	%rbp
	movq	0x10(%rdx),	%r12
	movq	0x18(%rdx),	%r13
	movq	0x20(%rdx),	%r14
	movq	0x28(%rdx),	%r15
	movq	0x30(%rdx),	%rcx
	movq	0x38(%rdx),	%rsp
	ldmxcsr	0x40(%rdx)
	fldcw	0x44(%rdx)
	jmpq	*%rcx
.size switch_fiber,.-switch_fiber

.section .note.GNU-stack,"",%progbits
