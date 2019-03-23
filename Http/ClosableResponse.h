#pragma once

#include "Response.h"

class ClosableResponse : public Response {
public:
	ClosableResponse(TcpSocket& client, char* begin, char* end) : Response(client, begin, end) {}
	using Response::Close;
};
