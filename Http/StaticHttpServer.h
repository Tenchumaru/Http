#pragma once

#include "HttpServer.h"

class StaticHttpServer : public HttpServer {
public:
	StaticHttpServer() = default;
	StaticHttpServer(StaticHttpServer const&) = delete;
	StaticHttpServer(StaticHttpServer&&) = default;
	StaticHttpServer& operator=(StaticHttpServer const&) = delete;
	StaticHttpServer& operator=(StaticHttpServer&&) = default;
	~StaticHttpServer() override = default;

private:
	TcpSocketFactory::fn_t GetConnectFn() const override;
};
