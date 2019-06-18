#pragma once

#include "TcpSocket.h"

class Body {
public:
	Body(char const* begin, char*& next, int size, TcpSocket& client) : begin(begin), next(next), size(size), client(client) {}
	Body() = delete;
	Body(Body const&) = delete;
	Body(Body&&) = default;
	Body& operator=(Body const&) = delete;
	Body& operator=(Body&&) = default;
	~Body() = default;

private:
	char const* begin;
	char*& next;
	int size;
	TcpSocket& client;
};
