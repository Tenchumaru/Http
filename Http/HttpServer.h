#pragma once

#include "Socket.h"
#include "Request.h"
#include "Response.h"

class HttpServer {
public:
	using fn_t= std::function<void(Request const&, Response&)>;

	HttpServer();
	~HttpServer();
	void Add(char const* path, fn_t fn);
	void Listen(unsigned short port);

private:
	std::vector<std::pair<std::string, fn_t>> handlers;
};
