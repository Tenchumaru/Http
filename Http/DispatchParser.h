#pragma once

#include "TcpSocket.h"
#include "AsyncSocket.h"

class DispatchParser {
public:
	explicit DispatchParser(TcpSocket& socket);
	DispatchParser() = delete;
	DispatchParser(DispatchParser const&) = delete;
	DispatchParser(DispatchParser&&) = default;
	DispatchParser& operator=(DispatchParser const&) = delete;
	DispatchParser& operator=(DispatchParser&&) = default;
	~DispatchParser() = default;
	static Task<void> Async(AsyncSocket& socket);
};