#include "stdafx.h"
#include "../Json/Json.h"
#include "../Http/Http.h"
#include "../Http/HttpServer.h"
#include "../Http/ClosableResponse.h"
#include "App.inl"

void GET_helix_users(Query_id_login&& queries, Header_authorization&& headers, Response& response) {
	 queries, headers;
	response << "Hello, World!";
}

void GET_helix_users_follows(Query_after_first_from_id_to_id&& queries, Header_clientXid&& headers, Response& response) {
	 queries, headers;
	response << "Hello, World!";
}

void PUT_helix_users(Header_authorization_contentXlength&& headers, Response& response) {
	 headers;
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

void PUT_helix_users_extensions(Header_authorization_contentXlength&& headers, Response& response) {
	 headers;
	response << "Hello, World!";
}

void GET_helix_videos(Query_after_before_first_game_id_id_language_period_sort_type_user_id&& queries, Header_clientXid&& headers, Response& response) {
	 queries, headers;
	response << "Hello, World!";
}

void POST_extensions__clientId_auth_secret(xstring&& clientId, Header_clientXid_contentXlength_contentXtype&& headers, Response& response) {
	 clientId, headers;
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

int main() {
	HttpServer server;
	server.Listen(6006);
	return 0;
}
