#pragma once

struct base_promise_type;

struct BaseAwaiter {
	std::optional<std::exception_ptr> exception;
	base_promise_type* promise;
	bool is_ready{};

	BaseAwaiter(base_promise_type* promise);
	virtual ~BaseAwaiter();
};

struct base_promise_type {
	std::optional<std::exception_ptr> exception;
	BaseAwaiter* awaiter{};
	base_promise_type* inner_promise{};
	base_promise_type* outer_promise{};

	virtual ~base_promise_type() {}
	std::suspend_never initial_suspend() {
		return {};
	}
	std::suspend_always final_suspend() noexcept {
		return {}; // https://lewissbaker.github.io/2018/09/05/understanding-the-promise-type#the-final-suspend-point
	}
	void unhandled_exception() noexcept(std::is_nothrow_assignable_v<std::optional<std::exception_ptr>, std::exception_ptr>) {
		exception = std::current_exception();
	}
	void SetOuterAwaiter();
	virtual void SetOuterAwaiterImpl() = 0;
};

template<typename T>
struct Awaiter : public BaseAwaiter {
	T rv{};

	Awaiter(base_promise_type* promise) : BaseAwaiter(promise) {}
	bool await_ready() {
		return is_ready;
	}
	template<typename T>
	void await_suspend(T) {}
	T await_resume() {
		if (promise->awaiter != this) {
			throw std::runtime_error("promise->awaiter != this");
		} else if (promise->inner_promise) {
			throw std::runtime_error("promise->inner_promise");
		}
		promise->awaiter = nullptr;
		if (exception) {
			std::rethrow_exception(*exception);
		}
		return std::move(rv);
	}
};

template<>
struct Awaiter<void> : public BaseAwaiter {
	Awaiter(base_promise_type* promise) : BaseAwaiter(promise) {}
	bool await_ready() {
		return is_ready;
	}
	template<typename T>
	void await_suspend(T) {}
	void await_resume() {
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
};

struct SocketAwaitable {
	SOCKET fd;
	short pollValue;
};

struct SocketAwaiter : public Awaiter<int> {
	SocketAwaitable awaitable;

	SocketAwaiter(base_promise_type* promise, SocketAwaitable&& awaitable) : Awaiter(promise), awaitable(std::move(awaitable)) {}
};

template<typename T>
struct Task {
	struct promise_type : public base_promise_type {
		T value{};

		SocketAwaiter await_transform(SocketAwaitable&& awaitable) {
			return SocketAwaiter{ this, std::move(awaitable) };
		}
		template<typename U>
		Awaiter<U> await_transform(Task<U>&& task) {
			inner_promise = task.promise;
			task.promise->outer_promise = this;
			return Awaiter<U>{ this };
		}
		Task<T> get_return_object() {
			return Task<T>{ this };
		}
		void return_value(T const& value_) {
			value = value_;
		}
		void return_value(T&& value_) {
			value = std::move(value_);
		}
		void SetOuterAwaiterImpl() override {
#ifdef _DEBUG
			if (!dynamic_cast<Awaiter<T>*>(outer_promise->awaiter)) {
				throw std::runtime_error("!dynamic_cast<Awaiter<T>*>(outer_promise->awaiter)");
			}
#endif
			auto* const p = static_cast<Awaiter<T>*>(outer_promise->awaiter);
			p->rv = std::move(value);
		}
	};

	base_promise_type* promise{};
};

template<>
struct Task<void> {
	struct promise_type : public base_promise_type {
		SocketAwaiter await_transform(SocketAwaitable&& awaitable);
		template<typename T>
		Awaiter<T> await_transform(Task<T>&& task) {
			inner_promise = task.promise;
			task.promise->outer_promise = this;
			return Awaiter<T>{ this };
		}
		Task<void> get_return_object() {
			return Task<void>{ this };
		}
		void return_void() {}
		void SetOuterAwaiterImpl() override {}
	};

	base_promise_type* promise{};
};
