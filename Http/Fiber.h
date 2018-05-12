#pragma once

using FiberFn = void(*)(void* parameter);
static_assert(sizeof(FiberFn) == sizeof(void*), "unexpected size");

void* ConvertThreadToFiber(void* parameter);
bool ConvertFiberToThread();
void* CreateFiber(size_t stackSize, FiberFn fn, void* parameter);
void DeleteFiber(void* fiber);
void* GetCurrentFiber();
void* GetFiberData();
void SwitchToFiber(void* fiber);
