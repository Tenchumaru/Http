#include "pch.h"
#include "TcpSocket.h"

TcpSocket& TcpSocket::operator=(TcpSocket&& that) noexcept {
	Close();
	std::swap(socket, that.socket);
	return *this;
}

int TcpSocket::Connect(sockaddr const* address, size_t addressSize) noexcept {
	return connect(socket, address, static_cast<int>(addressSize));
}

std::pair<size_t, int> TcpSocket::Receive(char* buffer, size_t bufferSize) {
	return InternalReceive(buffer, bufferSize);
}

std::pair<size_t, int> TcpSocket::Send(char const* buffer, size_t bufferSize) {
	return InternalSend(buffer, bufferSize);
}

std::pair<size_t, int> TcpSocket::InternalReceive(char* buffer, size_t bufferSize) {
	auto result = recv(socket, buffer, static_cast<int>(bufferSize), 0);
	if (result >= 0) {
		return{ result, 0 };
	}
	return{ 0, errno };
}

std::pair<size_t, int> TcpSocket::InternalSend(char const* buffer, size_t bufferSize) {
	auto result = send(socket, buffer, static_cast<int>(bufferSize), 0);
	if (result >= 0) {
		return{ result, 0 };
	}
	return{ 0, errno };
}
