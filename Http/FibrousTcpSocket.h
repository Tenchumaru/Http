#pragma once

#include "TcpSocket.h"

class FibrousTcpSocket : public TcpSocket {
public:
	FibrousTcpSocket(SOCKET socket);
	~FibrousTcpSocket();
};
