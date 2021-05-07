#include "pch.h"
#include "SecureFibrousTcpSocket.h"

SecureFibrousTcpSocket::SecureFibrousTcpSocket(SOCKET socket, fn_t awaitFn, SSL_CTX* sslContext) :
	SecureFibrousTcpSocket(FibrousTcpSocket(socket, awaitFn), sslContext) {}

SecureFibrousTcpSocket::SecureFibrousTcpSocket(SecureFibrousTcpSocket&& that) noexcept : FibrousTcpSocket(std::move(that)) {
	SwapPrivates(that);
}

SecureFibrousTcpSocket::SecureFibrousTcpSocket(FibrousTcpSocket&& that, SSL_CTX* sslContext) : FibrousTcpSocket(std::move(that)), ssl(SSL_new(sslContext)) {
	if (!ssl) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureFibrousTcpSocket::SecureFibrousTcpSocket.SSL_new");
	}
	if (!SSL_set_fd(ssl, static_cast<int>(socket))) {
		ERR_print_errors_fp(stderr);
		throw std::runtime_error("SecureFibrousTcpSocket::SecureFibrousTcpSocket.SSL_set_fd");
	}
}

SecureFibrousTcpSocket::~SecureFibrousTcpSocket() {
	SSL_free(ssl);
}

SecureFibrousTcpSocket& SecureFibrousTcpSocket::operator=(SecureFibrousTcpSocket&& that) noexcept {
	SwapPrivates(that);
	FibrousTcpSocket::operator=(std::move(that));
	return *this;
}

int SecureFibrousTcpSocket::Accept() noexcept {
	auto fn = [this] {
		return SSL_accept(ssl);
	};
	int result = Invoke(fn);
	if (result == 0) {
		result = -1;
	}
	return result;
}

int SecureFibrousTcpSocket::Connect(sockaddr const* address, size_t addressSize) noexcept {
	int result = FibrousTcpSocket::Connect(address, addressSize);
	if (result == 0) {
		auto fn = [this] {
			return SSL_connect(ssl);
		};
		result = Invoke(fn);
		if (result == 0) {
			result = -1;
		}
	}
	return result;
}

int SecureFibrousTcpSocket::InternalReceive(char* buffer, size_t bufferSize) {
	if (!buffer || !bufferSize) {
		assert(buffer && bufferSize);
		return 0;
	}
	auto fn = [this, buffer, bufferSize = static_cast<int>(bufferSize)]{
		return SSL_read(ssl, buffer, bufferSize);
	};
	auto result = Invoke(fn);
	if (result <= 0 && SSL_get_error(ssl, result) == SSL_ERROR_ZERO_RETURN) {
		result = 0;
	}
	return result;
}

int SecureFibrousTcpSocket::InternalSend(char const* buffer, size_t bufferSize) {
	if (!buffer || !bufferSize) {
		assert(buffer && bufferSize);
		return 0;
	}
	auto fn = [this, buffer, bufferSize = static_cast<int>(bufferSize)]{
		return SSL_write(ssl, buffer, bufferSize);
	};
	auto result = Invoke(fn);
	if (result <= 0 && SSL_get_error(ssl, result) == SSL_ERROR_ZERO_RETURN) {
		result = 0;
	}
	return result;
}

int SecureFibrousTcpSocket::Invoke(std::function<int()> fn) {
	int result;
	while (result = fn(), result <= 0) {
		int errorCode = SSL_get_error(ssl, result);
		if (errorCode == SSL_ERROR_WANT_READ) {
			// Await the read and try again.
			Await(POLLIN);
		} else if (errorCode == SSL_ERROR_WANT_WRITE) {
			// Await the write and try again.
			Await(POLLOUT);
		} else {
			break;
		}
	}
	return result;
}

void SecureFibrousTcpSocket::SwapPrivates(SecureFibrousTcpSocket& that) {
	if (ssl) {
		SSL_free(ssl);
		ssl = nullptr;
	}
	std::swap(ssl, that.ssl);
}
