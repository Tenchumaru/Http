#include "stdafx.h"
#include "Socket.h"

Socket::Socket(SOCKET socket) : socket(socket) {}

Socket::~Socket() {
	Close();
}

void Socket::Close() {
	close(socket);
}
