#pragma once

#include "TcpSocket.h"

class Response {
public:
	Response();
	~Response();
	void Ok(std::string const& text);
	void End(unsigned short responseCode);
	void Send(TcpSocket& client);
};
