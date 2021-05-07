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

std::pair<size_t, int> FibrousTcpSocket::InternalReceive(char* buffer, size_t bufferSize) {
	decltype(InternalReceive(nullptr, 0)) result;
	do {
		result = TcpSocket::InternalReceive(buffer, bufferSize);
	} while (IsAwaiting(result.second, POLLIN));
	return result;
}

std::pair<size_t, int> FibrousTcpSocket::InternalSend(char const* buffer, size_t bufferSize) {
	decltype(InternalSend(nullptr, 0)) result;
	do {
		result = TcpSocket::InternalSend(buffer, bufferSize);
	} while (IsAwaiting(result.second, POLLOUT));
	return result;
}

bool FibrousTcpSocket::IsAwaiting(int errorCode, short pollValue) {
	if (IsAwaitable(errorCode)) {
		awaitFn(socket, pollValue);
		return true;
	}
	return false;
}
