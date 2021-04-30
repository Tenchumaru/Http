#include "pch.h"
#include "Socket.h"
#include "AsyncSocketServer.h"

#ifdef _WIN32
inline static int poll(_Inout_ LPWSAPOLLFD fdArray, _In_ size_t fds, _In_ INT timeout) {
	return WSAPoll(fdArray, static_cast<ULONG>(fds), timeout);
}
#endif

using namespace std::literals;

std::pair<SOCKET, socklen_t> AsyncSocketServer::Open(char const* service) {
#ifdef _WIN32
	// Initialize the socket library.
	static bool isInitialized = [] {
		WSADATA wsaData;
		auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result) {
			std::cerr << "WSAStartup error " << result << std::endl;
			throw std::runtime_error("AsyncSocketServer::Open.WSAStartup");
		}
		return true;
	}();
#endif

	// Create and bind the server socket.
	addrinfo hints{};
	hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6.
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // Use INADDR_ANY for the resulting addresses.
	addrinfo* addresses;
	if (getaddrinfo(nullptr, service, &hints, &addresses)) {
		throw std::runtime_error("AsyncSocketServer::Open.getaddrinfo");
	}
	SOCKET serverSocket = INVALID_SOCKET;
	socklen_t addressSize = 0;
	try {
		for (auto const* rp = addresses; rp != nullptr; rp = rp->ai_next) {
			serverSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (serverSocket == INVALID_SOCKET) {
				continue;
			}
			int reuse = 1;
			if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char const*>(&reuse), sizeof(reuse)) < 0) {
				throw std::runtime_error("AsyncSocketServer::Open.setsockopt(SO_REUSEADDR)");
			}
#ifdef SO_REUSEPORT
			if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
				throw std::runtime_error("AsyncSocketServer::Open.setsockopt(SO_REUSEPORT)");
			}
#endif
			addressSize = static_cast<decltype(addressSize)>(rp->ai_addrlen);
			if (bind(serverSocket, rp->ai_addr, addressSize) == 0) {
				break;
			}
#ifdef _DEBUG
			std::cerr << "bind failed; error " << errno << std::endl;
#endif
			close(serverSocket);
			serverSocket = INVALID_SOCKET;
		}
		freeaddrinfo(addresses);
	} catch (std::exception const&) {
		freeaddrinfo(addresses);
		throw;
	}
	if (serverSocket == INVALID_SOCKET) {
		throw std::runtime_error("AsyncSocketServer::Open.socket");
	}

	// Listen for client connections.
	check(listen(serverSocket, 1));
	if (!Socket::SetNonblocking(serverSocket)) {
		throw std::runtime_error("AsyncSocketServer::Open.Socket::SetNonblocking");
	}
	return { serverSocket, addressSize };
}

void AsyncSocketServer::Run(char const* service) {
	// Create the server socket.
	auto [serverSocket, addressSize] = Open(service);

	// Accept and handle connections from it.
	AddPromise(AcceptAndHandle(serverSocket, addressSize).promise);

	// Start polling.
	decltype(promises) current_promises;
	decltype(sockets) current_sockets;
	while (!promises.empty()) {
		// Clear and swap to process current data and collect next data.
		current_promises.clear();
		current_sockets.clear();
		std::swap(current_promises, promises);
		std::swap(current_sockets, sockets);

		// Await socket poll.
		check(poll(current_sockets.data(), current_sockets.size(), -1));

		// Resume coroutines for sockets that are ready.  Retain the promises and sockets of those that are not.
		for (decltype(current_sockets.size()) i = 0, n = current_sockets.size(); i < n; ++i) {
			auto const& pollResult = current_sockets[i];
			if (pollResult.revents) {
				auto* promise = current_promises[i];
#ifdef _DEBUG
				if (!dynamic_cast<SocketAwaiter*>(promise->awaiter)) {
					throw std::runtime_error("!dynamic_cast<SocketAwaiter*>(promise->awaiter)");
				}
#endif
				auto* const socketAwaiter = static_cast<SocketAwaiter*>(promise->awaiter);
				int err = errno;
				err = pollResult.events & pollResult.revents ? 0 : pollResult.revents & POLLHUP ? ECONNRESET : err ? err : EINVAL;
				socketAwaiter->rv = err;
				ProcessPromise(promise);
			} else {
				promises.push_back(current_promises[i]);
				sockets.push_back(pollResult);
			}
		}
	}
}

void AsyncSocketServer::Run(unsigned short port) {
	char buf[6];
	sprintf_s(buf, "%d", port);
	Run(buf);
}

Task<std::pair<std::unique_ptr<AsyncSocket>, int>> AsyncSocketServer::Accept(SOCKET serverSocket, socklen_t addressSize) {
	auto fn = [serverSocket, addressSize] {
		auto n = addressSize;
		SOCKADDR_INET address;
		auto rv = accept(serverSocket, reinterpret_cast<sockaddr*>(&address), &n);
#ifdef _DEBUG
		if (rv != INVALID_SOCKET) {
			std::cout << "accepted socket " << rv << ", address family " << address.si_family << std::endl;
			if (address.si_family == AF_INET || address.si_family == AF_INET6) {
				char s[std::max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
				auto* p = inet_ntop(address.si_family, &address, s, sizeof(s));
				if (p) {
					std::cout << "\tIP address " << p << ", port " << address.Ipv4.sin_port << std::endl;
				} else {
					std::cerr << "\tcannot determine address; error " << errno << std::endl;
				}
			} else {
				std::cout << "\tunknown address family" << std::endl;
			}
		}
#endif
		return rv;
	};
	SOCKET clientSocket;
	while (clientSocket = fn(), clientSocket == INVALID_SOCKET) {
		int errorCode = errno;
		if (errorCode == EWOULDBLOCK) {
			errorCode = co_await SocketAwaitable{ serverSocket, POLLIN };
		}
		if (errorCode) {
			co_return{ std::unique_ptr<AsyncSocket>{}, errorCode };
		}
	}
	co_return{ std::make_unique<AsyncSocket>(clientSocket), 0 };
}

Task<void> AsyncSocketServer::AcceptAndHandle(SOCKET serverSocket, socklen_t addressSize) {
	for (;;) {
		auto [clientSocket, errorCode] = co_await Accept(serverSocket, addressSize);
		if (errorCode) {
			std::cerr << "accept error " << errorCode << std::endl;
		} else {
			AddPromise(Handle(std::move(clientSocket)).promise);
		}
	}
}

void AsyncSocketServer::AddPromise(base_promise_type* promise) {
	promise = GetInnermostPromise(promise);
	auto handle = std::coroutine_handle<base_promise_type>::from_promise(*promise);
	while (handle.done()) {
		promise->SetOuterAwaiter();
		promise = promise->outer_promise;
		handle.destroy();
		if (!promise) {
			return;
		}
		promise->inner_promise = nullptr;
		handle = std::coroutine_handle<base_promise_type>::from_promise(*promise);
		handle();
		if (promise->inner_promise) {
			promise = GetInnermostPromise(promise);
			handle = std::coroutine_handle<base_promise_type>::from_promise(*promise);
		}
	}
#ifdef _DEBUG
	if (!dynamic_cast<SocketAwaiter*>(promise->awaiter)) {
		throw std::runtime_error("!dynamic_cast<SocketAwaiter*>(promise->awaiter)");
	}
#endif
	promises.push_back(promise);
	auto* const socketAwaiter = static_cast<SocketAwaiter*>(promise->awaiter);
	sockets.push_back({ socketAwaiter->awaitable.fd, socketAwaiter->awaitable.pollValue });
}

Task<std::pair<size_t, int>> Receive(SOCKET clientSocket, void* p, size_t n) {
	auto fn = [clientSocket, p = static_cast<char*>(p), n = static_cast<int>(n)]{
		return recv(clientSocket, p, n, 0);
	};
	n = fn();
	if (n == -1) {
		int errorCode = errno;
		if (errorCode == EWOULDBLOCK) {
			errorCode = co_await SocketAwaitable{ clientSocket, POLLIN };
		}
		if (errorCode) {
			co_return{ size_t{}, errorCode };
		}
		n = fn();
	}
	if (n == -1) {
		co_return{ size_t{}, errno };
	}
	if (*static_cast<char const*>(p) == 'x') {
		throw std::runtime_error("x");
	}
	co_return{ static_cast<size_t>(n), 0 };
}

Task<std::pair<size_t, int>> Send(SOCKET clientSocket, void const* p, size_t n) {
	auto fn = [clientSocket, p = static_cast<char const*>(p), n = static_cast<int>(n)]{
		return send(clientSocket, p, n, 0);
	};
	n = fn();
	if (n == -1) {
		int errorCode = errno;
		if (errorCode == EWOULDBLOCK) {
			errorCode = co_await SocketAwaitable{ clientSocket, POLLOUT };
		}
		if (errorCode) {
			co_return{ size_t{}, errorCode };
		}
		n = fn();
	}
	if (n == -1) {
		co_return{ size_t{}, errno };
	}
	co_return{ static_cast<size_t>(n), 0 };
}

void AsyncSocketServer::ProcessPromise(base_promise_type* promise) {
	auto handle = std::coroutine_handle<base_promise_type>::from_promise(*promise);
	handle();
	AddPromise(promise);
}
