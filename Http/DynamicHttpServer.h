#pragma once

#include "HttpServer.h"
#include "Request.h"
#include "Response.h"
#include "TcpSocketFactory.h"

class DynamicHttpServer : public HttpServer {
public:
	using fn_t = std::function<void(Request const&, Response&)>;

	DynamicHttpServer() = default;
	DynamicHttpServer(DynamicHttpServer const&) = delete;
	DynamicHttpServer(DynamicHttpServer&&) = default;
	DynamicHttpServer& operator=(DynamicHttpServer const&) = delete;
	DynamicHttpServer& operator=(DynamicHttpServer&&) = default;
	~DynamicHttpServer() = default;

	void Add(char const* path, fn_t fn);

private:
	std::vector<std::pair<std::string, fn_t>> handlers;

	TcpSocketFactory::fn_t GetConnectFn() const override;
};
