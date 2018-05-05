#include "stdafx.h"
#include "../Json/Json.h"
#include "../Http/HttpServer.h"

void HandleJson(Request const& request, Response& response) {
	request, response;
	std::stringstream ss;
	std::unordered_map<std::string, std::string> object;
	object.insert({ "message", "Hello, World!" });
	WriteJson(ss, object);
	response.Headers.insert({ "Content-Type", "application/json" });
	response.Ok(ss.str());
}

int main() {
	HttpServer server;
	server.Add("/json", HandleJson);
	server.Listen(6006);
	return 0;
}
