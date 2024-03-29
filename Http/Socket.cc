#include "pch.h"
#include "Socket.h"

Socket::Socket(Socket&& that) noexcept : socket(INVALID_SOCKET) {
	std::swap(socket, that.socket);
}

Socket& Socket::operator=(Socket&& that) noexcept {
	Close();
	std::swap(socket, that.socket);
	return *this;
}

Socket::~Socket() {
	Close();
}

void Socket::Close() {
	close(socket);
	socket = INVALID_SOCKET;
}

bool Socket::SetNonblocking(SOCKET socket) {
#ifdef _WIN32
	u_long value = 1;
	auto const result = ioctlsocket(socket, FIONBIO, &value);
	if (result != 0) {
		std::perror("Socket::SetNonblocking ioctlsocket failure");
		return false;
	}
#else
	auto result = fcntl(socket, F_GETFL, 0);
	if (result >= 0) {
		result |= O_NONBLOCK;
		result = fcntl(socket, F_SETFL, result);
	}
	if (result != 0) {
		perror("Socket::SetNonblocking fcntl failure");
		return false;
	}
#endif
	return true;
}
