#pragma once

#include "Socket.h"

class TcpSocket : public Socket {
public:
	explicit TcpSocket(SOCKET socket) : Socket(socket) {}
	TcpSocket() = delete;
	TcpSocket(TcpSocket const&) = delete;
	TcpSocket(TcpSocket&& that) noexcept : Socket(INVALID_SOCKET) { std::swap(socket, that.socket); }
	TcpSocket& operator=(TcpSocket const&) = delete;
	TcpSocket& operator=(TcpSocket&& that) noexcept;
	~TcpSocket() = default;
	virtual int Connect(sockaddr const* address, size_t addressSize) noexcept;
	std::pair<size_t, int> Receive(char* buffer, size_t bufferSize);
	std::pair<size_t, int> Send(char const* buffer, size_t bufferSize);

protected:
	virtual std::pair<size_t, int> InternalReceive(char* buffer, size_t bufferSize);
	virtual std::pair<size_t, int> InternalSend(char const* buffer, size_t bufferSize);
};
