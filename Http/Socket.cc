#include "stdafx.h"
#include "Socket.h"

Socket::Socket(SOCKET socket) : socket(socket) {}

Socket::~Socket() {
	close(socket);
}
