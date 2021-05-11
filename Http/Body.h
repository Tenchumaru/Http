#pragma once

#include "TcpSocket.h"

class Body {
public:
	Body(char const* begin, char const* end, int size, TcpSocket& socket) : begin(begin), end(end), size(size), socket(socket) {}
	Body() = delete;
	Body(Body const&) = delete;
	Body(Body&& that) noexcept : begin(that.begin), end(that.end), size(that.size), socket(that.socket) {}
	Body& operator=(Body const&) = delete;
	Body& operator=(Body&&) noexcept = delete;
	~Body() = default;

private:
	char const* begin;
	char const* end;
	int size;
	TcpSocket& socket;
};
