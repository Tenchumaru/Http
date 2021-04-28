#include "pch.h"
#include "../Json/Json.h"
#include "../Http/Http.h"
#include "../Http/DynamicHttpServer.h"

Task<void> HandleJson(Request const& request, AsyncResponse& response) {
	std::stringstream ss;
	std::unordered_map<std::string, std::string> object;
	object.insert({ "message", "Hello, World!" });
	WriteJson(ss, object);
	response.WriteStatus(StatusLines::OK);
	auto it = request.Headers.find("origin");
	if (it != request.Headers.cend()) {
		response.WriteHeader("Access-Control-Allow-Credentials", "true");
		response.WriteHeader("Access-Control-Allow-Origin", it->second);
	}
	response.WriteHeader("Content-Type", "application/json");
	auto s = ss.str();
	response.WriteHeader("Content-Length", std::to_string(s.size()));
	return response.Write(s);
}

Task<void> HandlePlainText(Request const& request, AsyncResponse& response) {
	auto it = request.Headers.find("origin");
	if (it != request.Headers.cend()) {
		response.WriteHeader("Access-Control-Allow-Credentials", "true");
		response.WriteHeader("Access-Control-Allow-Origin", it->second);
	}
	response.WriteHeader("Content-Type", "text/plain");
	return response.Write("Hello, World!");
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
