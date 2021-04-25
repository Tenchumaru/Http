#include "pch.h"
#include "Task.h"

BaseAwaiter::BaseAwaiter(base_promise_type* promise) : promise(promise) {
	promise->awaiter = this;
}

BaseAwaiter::~BaseAwaiter() {
	promise->awaiter = nullptr;
}

template<typename T>
T Awaiter<T>::await_resume() {
	if (promise->awaiter != this) {
		throw std::runtime_error("promise->awaiter != this");
	} else if (promise->inner_promise) {
		throw std::runtime_error("promise->inner_promise");
	}
	promise->awaiter = nullptr;
	if (exception) {
		std::rethrow_exception(*exception);
	}
	return rv;
}

void Awaiter<void>::await_resume() {
	if (promise->awaiter != this) {
		throw std::runtime_error("promise->awaiter != this");
	} else if (promise->inner_promise) {
		throw std::runtime_error("promise->inner_promise");
	}
	promise->awaiter = nullptr;
	if (exception) {
		std::rethrow_exception(*exception);
	}
}

template<typename T>
SocketAwaiter Task<T>::promise_type::await_transform(SocketAwaitable&& awaitable) {
	return SocketAwaiter{ this, std::move(awaitable) };
}

template<typename T>
template<typename U>
Awaiter<U> Task<T>::promise_type::await_transform(Task<U>&& task) {
	inner_promise = task.promise;
	task.promise->outer_promise = this;
	return Awaiter<U>{ this };
}

template<typename T>
void Task<T>::promise_type::SetOuterAwaiterImpl() {
	if (!dynamic_cast<Awaiter<T>*>(outer_promise->awaiter)) {
		throw std::runtime_error("!dynamic_cast<Awaiter<T>*>(outer_promise->awaiter)");
	}
	auto* const p = static_cast<Awaiter<T>*>(outer_promise->awaiter);
	p->rv = value;
}

SocketAwaiter Task<void>::promise_type::await_transform(SocketAwaitable&& awaitable) {
	return SocketAwaiter{ this, std::move(awaitable) };
}

template<typename T>
Awaiter<T> Task<void>::promise_type::await_transform(Task<T>&& task) {
	inner_promise = task.promise;
	task.promise->outer_promise = this;
	return Awaiter<T>{ this };
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
