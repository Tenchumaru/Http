#include "stdafx.h"
#include "../Json/Json.h"
#include "../Http/Http.h"
#include "../Http/StaticHttpServer.h"
#include "../Http/ClosableResponse.h"

// First, include it for the classes.
#include "StaticApp.inl"

void GET_helix_users(Query_id_login&& queries, Header_authorization&& headers, Response& response) {
	queries, headers;
	response << "Hello, World!";
}

void GET_helix_users_follows(Query_after_first_from_id_to_id&& queries, Header_clientXid&& headers, Response& response) {
	queries, headers;
	response << "Hello, World!";
}

void PUT_helix_users(Header_authorization_contentXlength&& headers, Body&& body, Response& response) {
	headers, body;
	response << "Hello, World!";
}

void GET_helix_users_extensions_list(Header_authorization&& headers, Response& response) {
	headers;
	response << "Hello, World!";
}

void GET_helix_users_extensions(Query_user_id&& queries, Header_authorization&& headers, Response& response) {
	queries, headers;
	response << "Hello, World!";
}

void PUT_helix_users_extensions(Header_authorization_contentXlength&& headers, Body&& body, Response& response) {
	headers, body;
	response << "Hello, World!";
}

void GET_helix_videos(Query_after_before_first_game_id_id_language_period_sort_type_user_id&& queries, Header_clientXid&& headers, Response& response) {
	queries, headers;
	response << "Hello, World!";
}

void POST_extensions__clientId_auth_secret(xstring&& clientId, Header_clientXid_contentXlength_contentXtype&& headers, Body&& body, Response& response) {
	clientId, headers, body;
	response << "Hello, World!";
}

void GET_extensions__clientId_auth_secret(xstring&& clientId, Header_authorization_clientXid&& headers, Response& response) {
	clientId, headers;
	response << "Hello, World!";
}

void DELETE_extensions__clientId_auth_secret(xstring&& clientId, Header_clientXid&& headers, Response& response) {
	clientId, headers;
	response << "Hello, World!";
}

void GET_json(Header_origin&& headers, Response& response) {
	std::stringstream ss;
	std::unordered_map<std::string, std::string> object;
	object.insert({ "message", "Hello, World!" });
	WriteJson(ss, object);
	response.WriteHeader("Access-Control-Allow-Credentials", "true");
	response.WriteHeader("Access-Control-Allow-Origin", headers.Origin);
	response.WriteHeader("Content-Type", "application/json");
	response << ss.str();
}

void GET_text(Header_origin&& headers, Response& response) {
	response.WriteHeader("Access-Control-Allow-Credentials", "true");
	response.WriteHeader("Access-Control-Allow-Origin", headers.Origin);
	response.WriteHeader("Content-Type", "text/plain");
	response << "Hello, World!";
}

// Then, include it for the Dispatch function.
#define DISPATCH
#include "StaticApp.inl"

int main(int argc, char* argv[]) {
	char const* prog = strrchr(argv[0], '\\');
	if(prog) {
		++prog;
	} else {
		prog = argv[0];
	}
	StaticHttpServer server;
	if(argc == 3) {
		server.ConfigureSecurity(argv[1], argv[2]);
	} else if(argc != 1) {
		std::cout << "usage: " << prog << " [certificateChainFile privateKeyFile]" << std::endl;
		return 2;
	}
	server.Listen(6006);
	return 0;
}
