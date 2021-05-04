#pragma once

#include "AsyncResponse.h"

class ClosableAsyncResponse : public AsyncResponse {
public:
	ClosableAsyncResponse(AsyncSocket& socket, char* begin, char* end) : AsyncResponse(socket, begin, end) {}
	ClosableAsyncResponse() = delete;
	ClosableAsyncResponse(ClosableAsyncResponse const&) = delete;
	ClosableAsyncResponse(ClosableAsyncResponse&&) noexcept = default;
	ClosableAsyncResponse& operator=(ClosableAsyncResponse const&) = delete;
	ClosableAsyncResponse& operator=(ClosableAsyncResponse&&) noexcept = default;
	~ClosableAsyncResponse() = default;
	using AsyncResponse::Close;
};
