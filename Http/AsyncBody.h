#pragma once

#include "AsyncSocket.h"

class AsyncBody {
public:
	AsyncBody(char const* begin, char*& next, int size, AsyncSocket& socket) : begin(begin), next(next), size(size), socket(socket) {}
	AsyncBody() = delete;
	AsyncBody(AsyncBody const&) = delete;
	AsyncBody(AsyncBody&&) noexcept = default;
	AsyncBody& operator=(AsyncBody const&) = delete;
	AsyncBody& operator=(AsyncBody&&) noexcept = default;
	~AsyncBody() = default;

private:
	char const* begin;
	char*& next;
	int size;
	AsyncSocket& socket;
};
