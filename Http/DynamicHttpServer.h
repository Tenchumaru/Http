#pragma once

#include "HttpServer.h"
#include "Request.h"
#include "RequestParser.h"
#include "ClosableResponse.h"

class DynamicHttpServer : public HttpServer {
public:
	using fn_t = std::function<void(Request const&, Response&)>;

	DynamicHttpServer() = default;
	DynamicHttpServer(DynamicHttpServer const&) = delete;
	DynamicHttpServer(DynamicHttpServer&&) noexcept = default;
	DynamicHttpServer& operator=(DynamicHttpServer const&) = delete;
	DynamicHttpServer& operator=(DynamicHttpServer&&) noexcept = default;
	~DynamicHttpServer() = default;
	void Add(char const* path, fn_t fn);

private:
	std::vector<std::pair<std::string, fn_t>> handlers;
	Request request;
	RequestParser parser;

	char const* DispatchRequest(char const* begin, char const* body, char const* end, TcpSocket& socket, Response& response) const override;
	bool InternalHandle(Response& response) const;
};
