#pragma once

#include "Socket.h"

class Response {
public:
	Response();
	~Response();
	void Ok(std::string const& text);
	void End(unsigned short responseCode);
	void Send(Socket& client);
};
