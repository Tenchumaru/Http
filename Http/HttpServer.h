#pragma once

#include "Socket.h"
#include "Response.h"

class HttpServer {
public:
	HttpServer();
	~HttpServer();
	void Listen(unsigned short port);
};
