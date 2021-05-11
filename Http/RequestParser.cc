#include "pch.h"
#include "RequestParser.h"

using namespace std::literals;

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
	if (last[5] != '1' || last[7] != '0') {
		auto it = headers.find("Expect"s);
		if (it != headers.cend()) {
			if (_stricmp(it->second.c_str(), "100-continue")) {
				throw Exception(StatusLines::ExpectationFailed);
			} else if (contentLength_ == 0) {
				throw Exception(StatusLines::BadRequest, "continue expectation but no content length");
			}
		}
	}
	return contentLength_;
}
