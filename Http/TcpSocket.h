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

	// TODO:  consider returning size-error pairs.
	int Receive(char* buffer, size_t bufferSize);
	int Send(char const* buffer, size_t bufferSize);

protected:
	virtual int InternalReceive(char* buffer, size_t bufferSize);
	virtual int InternalSend(char const* buffer, size_t bufferSize);
};
