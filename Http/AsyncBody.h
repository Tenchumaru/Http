#pragma once

#include "AsyncSocket.h"

class AsyncBody {
public:
	AsyncBody(char const* begin, char*& next, int size, AsyncSocket& socket) : begin(begin), next(next), size(size), socket(socket) {}
	AsyncBody() = delete;
	AsyncBody(AsyncBody const&) = delete;
	AsyncBody(AsyncBody&&) = default;
	AsyncBody& operator=(AsyncBody const&) = delete;
	AsyncBody& operator=(AsyncBody&&) = default;
	~AsyncBody() = default;

private:
	char const* begin;
	char*& next;
	int size;
	AsyncSocket& socket;
};
