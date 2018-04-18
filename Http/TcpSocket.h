#pragma once

#include "Socket.h"

class TcpSocket : public Socket {
public:
	TcpSocket(SOCKET socket);
	~TcpSocket();
	int Receive(char* buffer, size_t bufferSize);
	int Send(char const* buffer, size_t bufferSize);

protected:
	virtual int InternalReceive(char* buffer, size_t bufferSize);
	virtual int InternalSend(char const* buffer, size_t bufferSize);
};
