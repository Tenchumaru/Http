#pragma once

#ifdef _WIN32
# include <setjmp.h>
#endif
#include "Fiber.h"

struct fiber_t;

using StartFn = void(*)(struct fiber_t*);

struct fiber_t {
	void* rbx;
	void* rbp;
	void* r12;
	void* r13;
	void* r14;
	void* r15;
	StartFn rip;
	void* rsp;
	uint32_t mxcsr;
	uint16_t cw;
	uint16_t _1;
	FiberFn fn;
	void* parameter;
#ifdef _WIN32
	void* _2;
	SETJMP_FLOAT128 Xmm6;
	SETJMP_FLOAT128 Xmm7;
	SETJMP_FLOAT128 Xmm8;
	SETJMP_FLOAT128 Xmm9;
	SETJMP_FLOAT128 Xmm10;
	SETJMP_FLOAT128 Xmm11;
	SETJMP_FLOAT128 Xmm12;
	SETJMP_FLOAT128 Xmm13;
	SETJMP_FLOAT128 Xmm14;
	SETJMP_FLOAT128 Xmm15;
#endif
};
static_assert(sizeof(FiberFn) == sizeof(void*), "unexpected FiberFn size");
static_assert(sizeof(StartFn) == sizeof(void*), "unexpected StartFn size");

extern "C" {
	int initialize_fiber(fiber_t*);
	__declspec(noreturn) void switch_fiber(fiber_t*, uintptr_t);
}
