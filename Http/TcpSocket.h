#pragma once

#include "Socket.h"

class TcpSocket : public Socket {
public:
	explicit TcpSocket(SOCKET socket) : Socket(socket) {}
	TcpSocket() = delete;
	TcpSocket(TcpSocket const&) = delete;
	TcpSocket(TcpSocket&&) = default;
	TcpSocket& operator=(TcpSocket const&) = delete;
	TcpSocket& operator=(TcpSocket&&) = default;
	~TcpSocket() = default;

	int Receive(char* buffer, size_t bufferSize); // TODO:  consider returning a size-error pair.
	int Send(char const* buffer, size_t bufferSize);

protected:
	virtual int InternalReceive(char* buffer, size_t bufferSize);
	virtual int InternalSend(char const* buffer, size_t bufferSize);
};
