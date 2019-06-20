#pragma once

#include "Socket.h"
#include "Response.h"

class HttpServer {
public:
	HttpServer() = default;
	HttpServer(HttpServer const&) = delete;
	HttpServer(HttpServer&&) = default;
	HttpServer& operator=(HttpServer const&) = delete;
	HttpServer& operator=(HttpServer&&) = default;
	~HttpServer() = default;

	void ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile);
	void Listen(unsigned short port);

private:
	std::string certificateChainFile;
	std::string privateKeyFile;
};
