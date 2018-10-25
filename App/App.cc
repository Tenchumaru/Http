#include "stdafx.h"
#include "../Json/Json.h"
#include "../Http/HttpServer.h"

void HandleJson(Request const& request, Response& response) {
	std::stringstream ss;
	std::unordered_map<std::string, std::string> object;
	object.insert({ "message", "Hello, World!" });
	WriteJson(ss, object);
	response.Headers.insert({ "Access-Control-Allow-Credentials", "true" });
	response.Headers.insert({ "Access-Control-Allow-Origin", request.Headers.at("origin") });
	response.Headers.insert({ "Content-Type", "application/json" });
	response.Ok(ss.str());
}

void HandlePlainText(Request const& request, Response& response) {
	response.Headers.insert({ "Access-Control-Allow-Credentials", "true" });
	response.Headers.insert({ "Access-Control-Allow-Origin", request.Headers.at("origin") });
	response.Headers.insert({ "Content-Type", "text/plain" });
	response.Ok("Hello, World!");
}

int main() {
	HttpServer server;
	server.Add("/json", HandleJson);
	server.Add("/plaintext", HandlePlainText);
	server.Listen(6006);
	return 0;
}
