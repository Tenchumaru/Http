#include "pch.h"
#include "TcpSocket.h"

int TcpSocket::Receive(char* buffer, size_t bufferSize) {
	return InternalReceive(buffer, bufferSize);
}

int TcpSocket::Send(char const* buffer, size_t bufferSize) {
	return InternalSend(buffer, bufferSize);
}

int TcpSocket::InternalReceive(char* buffer, size_t bufferSize) {
	return recv(socket, buffer, static_cast<int>(bufferSize), 0);
}

int TcpSocket::InternalSend(char const* buffer, size_t bufferSize) {
	return send(socket, buffer, static_cast<int>(bufferSize), 0);
}
