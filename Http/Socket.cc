#include "stdafx.h"
#include "Socket.h"

Socket::Socket(SOCKET socket) : socket(socket) {}

Socket::~Socket() {}

size_t Socket::Receive(char const * buffer, size_t bufferSize) {
	// TODO
	buffer, bufferSize;
	return size_t();
}
