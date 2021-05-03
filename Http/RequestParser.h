#pragma once

#include "HttpParser.h"
#include "Request.h"
#include "Uri.h"

class RequestParser : public HttpParser {
public:
	RequestParser() = default;
	RequestParser(RequestParser const&) = delete;
	RequestParser(RequestParser&&) = default;
	RequestParser& operator=(RequestParser const&) = delete;
	RequestParser& operator=(RequestParser&&) = default;
	~RequestParser() = default;
	bool ComposeRequest(Request& request);

private:
	Uri uri;

	bool ValidateNext(std::string const& s) override;
	bool ValidateLast(std::string const& s) override;
};
