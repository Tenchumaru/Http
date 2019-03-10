#pragma once

#include "TcpSocket.h"

class DispatchParser {
public:
	DispatchParser(TcpSocket& client);
	~DispatchParser();

private:
	TcpSocket& client;
};
