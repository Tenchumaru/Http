#pragma once

#include "Response.h"

class ClosableResponse : public Response {
public:
	ClosableResponse(Date const& date, TcpSocket& socket, char* begin, char* end) : Response(date, socket, begin, end) {}
	ClosableResponse() = delete;
	ClosableResponse(ClosableResponse const&) = delete;
	ClosableResponse(ClosableResponse&&) noexcept = default;
	ClosableResponse& operator=(ClosableResponse const&) = delete;
	ClosableResponse& operator=(ClosableResponse&&) noexcept = delete;
	~ClosableResponse() = default;

	using Response::Close;
};
