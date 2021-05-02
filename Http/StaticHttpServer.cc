#include "pch.h"
#include "StaticHttpServer.h"

extern char const* Dispatch(char const* begin, char const* body, char* next, char const* end, TcpSocket& socket, Response& response);

char const* StaticHttpServer::DispatchRequest(char const* begin, char const* body, char* next, char const* end, TcpSocket& socket, Response& response) const {
	return Dispatch(begin, body, next, end, socket, response);
}
