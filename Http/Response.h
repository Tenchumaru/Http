#pragma once

#include "HeaderMap.h"
#include "TcpSocket.h"

class Response {
public:
	Response();
	~Response();
	HeaderMap& GetHeaders() { return headers; }
	void Ok(std::string const& text);
	void End(unsigned short responseCode, std::string const& text= empty);
	void Send(TcpSocket& client);
	__declspec(property(get=GetHeaders)) HeaderMap& Headers;

private:
	HeaderMap headers;
	static std::string const empty;
	std::string response;
};
