#pragma once

#include "AsyncSocket.h"
#include "Task.h"

class AsyncSocketServer {
public:
	AsyncSocketServer() = default;
	AsyncSocketServer(AsyncSocketServer const&) = delete;
	AsyncSocketServer(AsyncSocketServer&&) = default;
	AsyncSocketServer& operator=(AsyncSocketServer const&) = delete;
	AsyncSocketServer& operator=(AsyncSocketServer&&) = default;
	virtual ~AsyncSocketServer() = default;
	void Run(char const* service);
	void Run(unsigned short port);

protected:
	virtual Task<std::pair<std::unique_ptr<AsyncSocket>, int>> Accept(SOCKET serverSocket, socklen_t addressSize);

private:
	std::vector<base_promise_type*> promises;
	std::vector<pollfd> sockets;

	Task<void> AcceptAndHandle(SOCKET serverSocket, socklen_t addressSize);
	void AddPromise(base_promise_type* promise);
	virtual Task<void> Handle(std::unique_ptr<AsyncSocket> clientSocket);
	static std::pair<SOCKET, socklen_t> Open(char const* service);
	void ProcessPromise(base_promise_type* promise);

	static base_promise_type* GetInnermostPromise(base_promise_type* promise) {
		base_promise_type* inner_promise = promise;
		while (inner_promise->inner_promise) {
			inner_promise = inner_promise->inner_promise;
		}
		return inner_promise;
	}
};
