#pragma once

#include "HttpServer.h"

class StaticHttpServer : public HttpServer {
public:
	StaticHttpServer() = default;
	StaticHttpServer(StaticHttpServer const&) = delete;
	StaticHttpServer& operator=(StaticHttpServer const&) = delete;
	~StaticHttpServer() override = default;

private:
	char const* DispatchRequest(char const* begin, char const* body, char const* end, TcpSocket& socket, Response& response) const override;
};
