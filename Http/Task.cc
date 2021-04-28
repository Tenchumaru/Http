#include "pch.h"
#include "Task.h"

BaseAwaiter::BaseAwaiter(base_promise_type* promise) : promise(promise) {
	promise->awaiter = this;
}

BaseAwaiter::~BaseAwaiter() {
	promise->awaiter = nullptr;
}

SocketAwaiter Task<void>::promise_type::await_transform(SocketAwaitable&& awaitable) {
	return SocketAwaiter{ this, std::move(awaitable) };
}

void base_promise_type::SetOuterAwaiter() {
	if (outer_promise && outer_promise->awaiter) {
		if (exception) {
			outer_promise->awaiter->exception = exception;
		} else {
			SetOuterAwaiterImpl();
		}
	}
}
