#pragma once

#include "TcpSocket.h"

class Response {
public:
	Response();
	~Response();
	void Ok(std::string const& text);
	void End(unsigned short responseCode, std::string const& text= empty);
	void Send(TcpSocket& client);

private:
	static std::string const empty;
	std::string response;
};
