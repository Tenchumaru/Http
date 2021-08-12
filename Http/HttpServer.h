#pragma once

#include "SecureFibrousSocketServer.h"
#include "Date.h"
#include "Response.h"

class HttpServer : public SecureFibrousSocketServer {
public:
	HttpServer() = default;
	HttpServer(HttpServer const&) = delete;
	HttpServer& operator=(HttpServer const&) = delete;
	virtual ~HttpServer() = default;

protected:
	Date date;

private:
	char const* ProcessRequest(char const* begin, char const* body, char const* end, TcpSocket& clientSocket) const;
	virtual char const* DispatchRequest(char const* begin, char const* body, char const* end, TcpSocket& clientSocket, Response& response) const = 0;
	void InternalHandleImpl(std::unique_ptr<FibrousTcpSocket> clientSocket) override;
};
