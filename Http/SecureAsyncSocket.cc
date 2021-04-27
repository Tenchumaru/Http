#include "pch.h"
#include "SecureAsyncSocket.h"

SecureAsyncSocket::SecureAsyncSocket(AsyncSocket&& socket, SSL_CTX* sslContext) :
	AsyncSocket(std::move(socket)),
	ssl(SSL_new(sslContext)) {
	if (!ssl) {
		perror("Cannot create SSL");
		throw std::runtime_error("SecureAsyncSocket::SecureAsyncSocket.SSL_new");
	}
	SSL_set_fd(ssl, static_cast<int>(this->socket));
}

SecureAsyncSocket::~SecureAsyncSocket() {
	SSL_free(ssl);
}

namespace {
	template<typename FN>
	Task<int> Invoke(FN fn, SSL* ssl, SOCKET socket) {
		int result;
		while (result = fn(), result < 0) {
			result = SSL_get_error(ssl, result);
			if (result == SSL_ERROR_WANT_READ) {
				// Await the read and try again.
				result = co_await SocketAwaitable{ socket, POLLIN };
				if (result) {
					co_return std::move(result);
				}
			} else if (result == SSL_ERROR_WANT_WRITE) {
				// Await the write and try again.
				result = co_await SocketAwaitable{ socket, POLLOUT };
				if (result) {
					co_return std::move(result);
				}
			} else {
				co_return std::move(result);
			}
		}
		if (result == 0) {
			result = SSL_get_error(ssl, result);
		} else {
			result = 0;
		}
		co_return std::move(result);
	}
}

Task<int> SecureAsyncSocket::Accept() {
	auto fn = [this] {
		return SSL_accept(ssl);
	};
	return Invoke(fn, ssl, socket);
}

// TODO:  invoke this method.
Task<int> SecureAsyncSocket::ShutDown() {
	auto result = co_await InternalShutDown();
	if (result == 0) {
		std::pair<size_t, int> pair;
		std::array<char, 999> buffer;
		do {
			pair = co_await InternalReceive(buffer.data(), buffer.size());
		} while (pair.first);
		if (pair.second) {
			result = pair.second;
		} else {
			result = co_await InternalShutDown();
		}
	} else {
		result = 0;
	}
	co_return std::move(result);
}

Task<std::pair<size_t, int>> SecureAsyncSocket::InternalReceive(void* buffer, size_t bufferSize) {
	if (!buffer || !bufferSize) {
		assert(buffer && bufferSize);
		co_return{ 0, 0 };
	}
	auto fn = [this, buffer, bufferSize = static_cast<int>(bufferSize)]{
		return SSL_read(ssl, buffer, bufferSize);
	};
	auto result = co_await Invoke(fn, ssl, socket);
	if (result > 0) {
		co_return{ result, 0 };
	}
	co_return{ 0, result == SSL_ERROR_ZERO_RETURN ? 0 : result };
}

Task<std::pair<size_t, int>> SecureAsyncSocket::InternalSend(void const* buffer, size_t bufferSize) {
	if (!buffer || !bufferSize) {
		assert(buffer && bufferSize);
		co_return{ 0, 0 };
	}
	auto fn = [this, buffer, bufferSize = static_cast<int>(bufferSize)]{
		return SSL_write(ssl, buffer, bufferSize);
	};
	auto result = co_await Invoke(fn, ssl, socket);
	if (result > 0) {
		co_return{ result, 0 };
	}
	co_return{ 0, result == SSL_ERROR_ZERO_RETURN ? 0 : result };
}

Task<int> SecureAsyncSocket::InternalShutDown() {
	int result;
	while (result = SSL_shutdown(ssl), result < 0) {
		result = SSL_get_error(ssl, result);
		if (result == SSL_ERROR_WANT_READ) {
			// Await the read and try again.
			result = co_await SocketAwaitable{ socket, POLLIN };
			if (result) {
				break;
			}
		} else if (result == SSL_ERROR_WANT_WRITE) {
			// Await the write and try again.
			result = co_await SocketAwaitable{ socket, POLLOUT };
			if (result) {
				break;
			}
		} else {
			break;
		}
	}
	co_return std::move(result);
}
