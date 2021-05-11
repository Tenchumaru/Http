#include "pch.h"
#include "RequestParser.h"

bool RequestParser::ComposeRequest(Request& request) {
	if (isComplete) {
		request.verb.swap(first);
		std::swap(request.uri, uri);
		request.version = ((last[5] - '0') << 4) | (last[7] - '0');
		request.headers.swap(headers);
		HttpParser::Reset();
		return true;
	}
	return false;
}

bool RequestParser::ValidateNext(std::string const& s) {
	return Uri::Create(s, uri);
}

bool RequestParser::ValidateLast(std::string const& s) {
	return ValidateVersion(s);
}

std::streamsize RequestParser::ValidateDataSizeHeaders() {
	auto contentLength_ = HttpParser::ValidateDataSizeHeaders();
	if (contentLength_ == 0 && (first == "POST" || first == "PUT")) {
		throw Exception(StatusLines::LengthRequired);
	}
	return contentLength_;
}
