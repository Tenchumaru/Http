#include "stdafx.h"
#include "FibrousTcpSocket.h"

FibrousTcpSocket::FibrousTcpSocket(SOCKET socket) : TcpSocket(socket) {}

FibrousTcpSocket::~FibrousTcpSocket() {}
