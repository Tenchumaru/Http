#pragma once

#include "Response.h"

class ClosableResponse : public Response {
public:
	ClosableResponse(TcpSocket& socket, char* begin, char* end) : Response(socket, begin, end) {}
	ClosableResponse() = delete;
	ClosableResponse(ClosableResponse const&) = delete;
	ClosableResponse(ClosableResponse&&) = default;
	ClosableResponse& operator=(ClosableResponse const&) = delete;
	ClosableResponse& operator=(ClosableResponse&&) = default;
	~ClosableResponse() = default;

	using Response::Close;
};
