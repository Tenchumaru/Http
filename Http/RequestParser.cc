#include "stdafx.h"
#include "RequestParser.h"

RequestParser::RequestParser(fn_t requestHandler) : requestHandler(requestHandler) {}

RequestParser::~RequestParser() {}

bool RequestParser::ValidateNext(std::string const& s) {
	return Uri::Create(s, uri);
}

bool RequestParser::ValidateLast(std::string const& s) {
	return ValidateVersion(s);
}

void RequestParser::HandleMessage() {
	Request rv;
	rv.verb.swap(first);
	std::swap(rv.uri, uri);
	rv.version.swap(last);
	rv.headers.swap(headers);
	rv.data.swap(data);
	HttpParser::Reset();
	requestHandler(rv);
}
