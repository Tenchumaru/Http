#pragma once

#include "TcpSocketFactory.h"
#include "Response.h"

class HttpServer {
public:
	HttpServer() = default;
	HttpServer(HttpServer const&) = delete;
	HttpServer(HttpServer&&) noexcept = default;
	HttpServer& operator=(HttpServer const&) = delete;
	HttpServer& operator=(HttpServer&&) noexcept = default;
	virtual ~HttpServer() = default;

	void ConfigureSecurity(char const* certificateChainFile, char const* privateKeyFile);
	void Run(unsigned short port);

private:
	std::string certificateChainFile;
	std::string privateKeyFile;

	TcpSocketFactory::fn_t GetConnectFn() const;
	char const* ProcessRequest(char const* begin, char const* body, char* next, char const* end, TcpSocket& socket) const;
	virtual char const* DispatchRequest(char const* begin, char const* body, char* next, char const* end, TcpSocket& socket, Response& response) const = 0;
};
