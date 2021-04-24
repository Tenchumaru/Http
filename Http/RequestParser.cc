#include "pch.h"
#include "RequestParser.h"

bool RequestParser::ValidateNext(std::string const& s) {
	return Uri::Create(s, uri);
}

bool RequestParser::ValidateLast(std::string const& s) {
	return ValidateVersion(s);
}

bool RequestParser::HandleMessage() {
	Request rv;
	rv.verb.swap(first);
	std::swap(rv.uri, uri);
	rv.version = ((last[5] - '0') << 4) | (last[7] - '0');
	rv.headers.swap(headers);
	rv.data.swap(data);
	HttpParser::Reset();
	return requestHandler(rv);
}
