#include "pch.h"
#include "Http.h"

using namespace std::literals;

bool AtEndOfPath(char ch) {
	return std::isspace(ch, std::locale()) || ch == '?' || ch == '#';
}

bool CollectParameter(char const*& p, size_t index, char const*& pn, char const*& qn) {
	pn = qn = p + index;
	while (*qn != '?' && *qn != '/' && *qn != ' ' && *qn != '\r' && *qn != '\n') {
		++qn;
	}
	p = qn - index - 1;
	return true;
}

void FourExEx(Response& response, StatusLines::StatusLine const& statusLine, char const* message /*= nullptr*/) {
	response.WriteStatus(statusLine);
	if (message && *message) {
		response.WriteHeader("Content-Length"s, std::to_string(strlen(message)));
		response << message;
	}
}

void FourHundred(Response& response, char const* message) {
	response.WriteStatus(StatusLines::BadRequest);
	if (message && *message) {
		response.WriteHeader("Content-Length"s, std::to_string(strlen(message)));
		response << message;
	}
}

void FourZeroFour(Response& response) {
	response.WriteStatus(StatusLines::NotFound);
}
