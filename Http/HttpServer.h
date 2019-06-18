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

	void Listen(unsigned short port);
};
