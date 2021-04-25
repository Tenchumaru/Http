#include "pch.h"
#include "AsyncSocket.h"

AsyncSocket::AsyncSocket(SOCKET socket) : Socket(socket) {
	if (!SetNonblocking(socket)) {
		close(socket);
		throw std::runtime_error("AsyncSocket::AsyncSocket.SetNonblocking");
	}
}

Task<std::pair<size_t, int>> AsyncSocket::InternalReceive(void* buffer, size_t bufferSize) {
	auto fn = [this, p = static_cast<char*>(buffer), n = static_cast<int>(bufferSize)]{
		return recv(socket, p, n, 0);
	};
	int n;
	while (n = fn(), n < 0) {
		int errorCode = errno;
		if (errorCode == EWOULDBLOCK) {
			errorCode = co_await SocketAwaitable{ socket, POLLIN };
		}
		if (errorCode) {
			co_return{ 0, errorCode };
		}
	}
	co_return{ static_cast<size_t>(n), 0 };
}

Task<std::pair<size_t, int>> AsyncSocket::InternalSend(void const* buffer, size_t bufferSize) {
	auto fn = [this, p = static_cast<char const*>(buffer), n = static_cast<int>(bufferSize)]{
		return send(socket, p, n, 0);
	};
	int n;
	while (n = fn(), n < 0) {
		int errorCode = errno;
		if (errorCode == EWOULDBLOCK) {
			errorCode = co_await SocketAwaitable{ socket, POLLIN };
		}
		if (errorCode) {
			co_return{ 0, errorCode };
		}
	}
	co_return{ static_cast<size_t>(n), 0 };
}
