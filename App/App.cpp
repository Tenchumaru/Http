#include "stdafx.h"
#include "../Http/HttpServer.h"

void HandleFoo(Request const& request, Response& response) {
	request, response;
	response.Headers.insert({ "Content-Type", "application/json" });
	response.Ok("{\"foo\":1}");
}

void HandleBar(Request const& request, Response& response) {
	request, response;
	response.Headers.insert({ "Content-Type", "application/json" });
	response.Ok("{\"bar\":1}");
}

int main() {
	HttpServer server;
	server.Add("/foo/", HandleFoo);
	server.Add("/bar/", HandleBar);
	server.Listen(6006);
	return 0;
}
