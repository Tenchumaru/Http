#include "pch.h"
#include "FibrousTcpSocket.h"

int FibrousTcpSocket::InternalReceive(char* buffer, size_t bufferSize) {
	int result;
	do {
		result = TcpSocket::InternalReceive(buffer, bufferSize);
	} while (IsAwaiting(result, POLLIN));
	return result;
}

int FibrousTcpSocket::InternalSend(char const* buffer, size_t bufferSize) {
	int result;
	do {
		result = TcpSocket::InternalSend(buffer, bufferSize);
	} while (IsAwaiting(result, POLLOUT));
	return result;
}

bool FibrousTcpSocket::IsAwaiting(int result, short pollValue) {
	if (result < 0) {
		auto v = errno;
		if (v == EALREADY || v == EWOULDBLOCK) {
			awaitFn(socket, pollValue);
			return true;
		}
	}
	return false;
}
