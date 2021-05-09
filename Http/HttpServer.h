#pragma once

#include "SecureFibrousSocketServer.h"
#include "Response.h"

class HttpServer : public SecureFibrousSocketServer {
public:
	HttpServer() = default;
	HttpServer(HttpServer const&) = delete;
	HttpServer(HttpServer&&) noexcept = default;
	HttpServer& operator=(HttpServer const&) = delete;
	HttpServer& operator=(HttpServer&&) noexcept = default;
	virtual ~HttpServer() = default;

private:
	char const* ProcessRequest(char const* begin, char const* body, char* next, char const* end, TcpSocket& clientSocket) const;
	virtual char const* DispatchRequest(char const* begin, char const* body, char* next, char const* end, TcpSocket& clientSocket, Response& response) const = 0;
	void InternalHandleImpl(std::unique_ptr<FibrousTcpSocket> clientSocket) override;
};
