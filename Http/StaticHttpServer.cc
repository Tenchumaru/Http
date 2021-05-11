#include "pch.h"
#include "StaticHttpServer.h"

extern char const* Dispatch(char const* begin, char const* body, char const* end, TcpSocket& socket, Response& response);

char const* StaticHttpServer::DispatchRequest(char const* begin, char const* body, char const* end, TcpSocket& socket, Response& response) const {
	return Dispatch(begin, body, end, socket, response);
}
