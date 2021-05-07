#include "pch.h"
#include "FibrousTcpSocket.h"

FibrousTcpSocket::FibrousTcpSocket(SOCKET socket, fn_t awaitFn) : TcpSocket(socket), awaitFn(awaitFn) {
	if (!SetNonblocking(socket)) {
		throw std::runtime_error("FibrousTcpSocket::FibrousTcpSocket.SetNonblocking");
	}
}

FibrousTcpSocket& FibrousTcpSocket::operator=(FibrousTcpSocket&& that) noexcept {
	TcpSocket::operator=(std::move(that));
	return *this;
}

bool FibrousTcpSocket::IsAwaitable(int errorValue) {
	return errorValue == EALREADY || errorValue == EINPROGRESS || errorValue == EWOULDBLOCK;
}

int FibrousTcpSocket::Connect(sockaddr const* address, size_t addressSize) noexcept {
	int result;
	do {
		result = TcpSocket::Connect(address, addressSize);
	} while (IsAwaiting(result, POLLOUT));
	return result;
}

void FibrousTcpSocket::Await(short pollValue) {
	awaitFn(socket, pollValue);
}

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
	if (result < 0 && IsAwaitable(errno)) {
		awaitFn(socket, pollValue);
		return true;
	}
	return false;
}
