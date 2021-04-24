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
	T await_resume();
};

template<>
struct Awaiter<void> : public BaseAwaiter {
	Awaiter(base_promise_type* promise) : BaseAwaiter(promise) {}
	bool await_ready() {
		return is_ready;
	}
	template<typename T>
	void await_suspend(T) {}
	void await_resume();
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

		SocketAwaiter await_transform(SocketAwaitable&& awaitable);
		template<typename T>
		Awaiter<T> await_transform(Task<T>&& task);
		Task<T> get_return_object() {
			return Task<T>{ this };
		}
		void return_value(T value_) {
			value = value_;
		}
		void SetOuterAwaiterImpl() override;
	};

	base_promise_type* promise{};
};

template<>
struct Task<void> {
	struct promise_type : public base_promise_type {
		SocketAwaiter await_transform(SocketAwaitable&& awaitable);
		template<typename T>
		Awaiter<T> await_transform(Task<T>&& task);
		Task<void> get_return_object() {
			return Task<void>{ this };
		}
		void return_void() {}
		void SetOuterAwaiterImpl() override {}
	};

	base_promise_type* promise{};
};

class AsyncSocketServer {
public:
	AsyncSocketServer() = default;
	AsyncSocketServer(AsyncSocketServer const&) = delete;
	AsyncSocketServer(AsyncSocketServer&&) = default;
	AsyncSocketServer& operator=(AsyncSocketServer const&) = delete;
	AsyncSocketServer& operator=(AsyncSocketServer&&) = default;
	virtual ~AsyncSocketServer() = default;
	void Run(char const* service);

private:
	std::vector<base_promise_type*> promises;

	Task<void> AcceptAndHandle(SOCKET serverSocket, socklen_t addressSize);
	void AddPromise(base_promise_type* promise);
	Task<void> Handle(SOCKET clientSocket);
	static std::pair<SOCKET, socklen_t> Open(char const* service);
	void ProcessPromise(base_promise_type* promise, std::unordered_map<SOCKET, base_promise_type*>& map, std::vector<pollfd>& sockets);

	static base_promise_type* GetInnermostPromise(base_promise_type* promise) {
		base_promise_type* inner_promise = promise;
		while (inner_promise->inner_promise) {
			inner_promise = inner_promise->inner_promise;
		}
		return inner_promise;
	}
};
