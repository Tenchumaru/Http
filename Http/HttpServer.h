#pragma once

#include "TcpSocketFactory.h"

class HttpServer {
public:
	HttpServer() = default;
	HttpServer(HttpServer const&) = delete;
	HttpServer(HttpServer&&) = default;
	HttpServer& operator=(HttpServer const&) = delete;
	HttpServer& operator=(HttpServer&&) = default;
	virtual ~HttpServer() = default;

	void ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile);
	void Run(unsigned short port);

private:
	std::string certificateChainFile;
	std::string privateKeyFile;

	virtual TcpSocketFactory::fn_t GetConnectFn() const = 0;
};
