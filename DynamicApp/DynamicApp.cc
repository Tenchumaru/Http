#include "pch.h"
#include "../Json/Json.h"
#include "../Http/Http.h"
#include "../Http/DynamicHttpServer.h"

void HandleJson(Request const& request, Body&&, Response& response) {
	std::stringstream ss;
	std::unordered_map<std::string, std::string> object;
	object.insert({ "message", "Hello, World!" });
	WriteJson(ss, object);
	response.WriteStatusLine(StatusLines::OK);
	auto it = request.Headers.find("Origin");
	if (it != request.Headers.cend()) {
		response.WriteHeader("Access-Control-Allow-Credentials", "true");
		response.WriteHeader("Access-Control-Allow-Origin", it->second);
	}
	response.WriteHeader("Content-Type", "application/json");
	response << ss.str();
}

void HandlePlainText(Request const& request, Body&&, Response& response) {
	auto it = request.Headers.find("Origin");
	if (it != request.Headers.cend()) {
		response.WriteHeader("Access-Control-Allow-Credentials", "true");
		response.WriteHeader("Access-Control-Allow-Origin", it->second);
	}
	response.WriteHeader("Content-Type", "text/plain");
	response << "Hello, World!";
}

int main(int argc, char* argv[]) {
	char const* prog = strrchr(argv[0], '\\');
	if (prog) {
		++prog;
	} else {
		prog = argv[0];
	}
	DynamicHttpServer server;
	if (argc == 3) {
		server.ConfigureSecurity(argv[1], argv[2]);
	} else if (argc != 1) {
		std::cout << "usage: " << prog << " [certificateChainFile privateKeyFile]" << std::endl;
		return 2;
	}
	server.Add("/json", HandleJson);
	server.Add("/plaintext", HandlePlainText);
	server.Run(6006);
	return 0;
}
