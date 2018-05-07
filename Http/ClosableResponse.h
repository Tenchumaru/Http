#pragma once

#include "Response.h"

class ClosableResponse : public Response {
public:
	ClosableResponse(TcpSocket& client) : Response(client) {}
	using Response::Close;
};
