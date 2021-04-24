#include "pch.h"
#include "Socket.h"

Socket::~Socket() {
	Close();
}

void Socket::Close() {
	close(socket);
}
