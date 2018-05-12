#include "stdafx.h"
#include "fiber_.h"
#include "Fiber.h"

namespace {
	size_t const stackBlockSize = 1 << 16;
	fiber_t mainFiber;
	fiber_t* currentFiber;
}

static void StartFiber(fiber_t* fiber) {
	try {
		fiber->fn(fiber->parameter);
	} catch(std::exception const& ex) {
		std::cout << "fiber function error: " << ex.what() << std::endl;
	}
	SwitchToFiber(&mainFiber);
}

void* ConvertThreadToFiber(void* parameter) {
	if(currentFiber != nullptr) {
		return nullptr;
	}
	mainFiber.parameter = parameter;
	currentFiber = &mainFiber;
	return currentFiber;
}

bool ConvertFiberToThread() {
	if(currentFiber == nullptr) {
		return false;
	}
	currentFiber = nullptr;
	return true;
}

void* CreateFiber(size_t stackSize, FiberFn fn, void* parameter) {
	stackSize = (stackSize + stackBlockSize - 1) & -stackBlockSize;
	auto* stack = new char[stackSize];
	auto* fiber = reinterpret_cast<fiber_t*>(stack + stackSize - 256);
	initialize_fiber(fiber);
	fiber->rip = StartFiber;
	fiber->rsp = reinterpret_cast<char*>(fiber) - 72;
	fiber->fn = fn;
	fiber->parameter = parameter;
	fiber->stack = stack;
	return fiber;
}

void DeleteFiber(void* fiber) {
	auto* p = reinterpret_cast<fiber_t*>(fiber);
	delete[] p->stack;
}

void* GetCurrentFiber() {
	return currentFiber;
}

void* GetFiberData() {
	return currentFiber->parameter;
}

void SwitchToFiber(void* fiber) {
	if(initialize_fiber(currentFiber) == 0) {
		currentFiber = reinterpret_cast<fiber_t*>(fiber);
		switch_fiber(currentFiber, 1);
	}
}
