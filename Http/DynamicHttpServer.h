#pragma once

#include "HttpServer.h"
#include "Request.h"
#include "RequestParser.h"
#include "ClosableResponse.h"
#include "Body.h"

class DynamicHttpServer : public HttpServer {
public:
	using fn_t = std::function<void(Request const&, Body&&, Response&)>;

	DynamicHttpServer() = default;
	DynamicHttpServer(DynamicHttpServer const&) = delete;
	DynamicHttpServer& operator=(DynamicHttpServer const&) = delete;
	~DynamicHttpServer() = default;
	void Add(char const* path, fn_t fn);

private:
	std::vector<std::pair<std::string, fn_t>> handlers;
	Request request;
	RequestParser parser;

	char const* DispatchRequest(char const* begin, char const* body, char const* end, ClientSocket& socket, Response& response) const override;
};
